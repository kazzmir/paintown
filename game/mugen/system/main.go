package main

import (
	"context"
	"errors"
	"flag"
	"fmt"
	"log"
	"os"
	"os/signal"
	"path/filepath"
	"strings"
	"syscall"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/motif"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/resource"
)

type Engine struct {
	cfg              *config.MugenConfig
	motif            *motif.Motif
	dataDir          string
	screenWidth      int // Window width
	screenHeight     int // Window height
	logicalWidth     int // Game logic width (e.g. 320)
	logicalHeight    int // Game logic height (e.g. 240)
	currentState     State
	gameTime         int32 // Process ticks, including menus and finished-match waits.
	sprites          map[string]*background.SpriteImages
	sound            *engineSound
	random           *engine.RandomStream
	pendingMatchSeed int64
	modeRun          *modeRun
	demo             *demoRun
	demoCycles       int
	nextCampaign     uint64
	characterPacks   resource.CharacterPacks
	stop             <-chan struct{}
	focus            func() bool
	hideFightBars    bool
	showCollisions   bool
}

// Close stops streaming readers before removing this engine's expanded packs.
func (e *Engine) Close() error {
	return errors.Join(e.closeSound(), e.characterPacks.Close())
}

func (e *Engine) nextMatchSeed() int64 {
	if e.pendingMatchSeed != 0 {
		seed := e.pendingMatchSeed
		e.pendingMatchSeed = 0
		e.random = engine.NewRandomStream(seed)
		return seed
	}
	if e.random == nil {
		e.random = engine.NewRandomStream(time.Now().Unix())
	}
	return e.random.Next()
}

func (e *Engine) hasFocus() bool {
	if e.focus != nil {
		return e.focus()
	}
	return ebiten.IsFocused()
}

func (e *Engine) ticksPerSecond() int {
	base, speed := 60, 0
	if e.cfg != nil {
		if e.cfg.Config.GameSpeed > 0 {
			base = e.cfg.Config.GameSpeed
		}
		speed = e.cfg.Options.GameSpeed
	}
	return max(1, base+5*speed)
}

// Script tick rates stay in game units even when debug speed removes the
// wall-clock cap. Options changes use this same scheduler consumer.
func (e *Engine) applyScheduler() {
	if e.cfg.Debug.Speedup {
		ebiten.SetTPS(ebiten.SyncWithFPS)
		ebiten.SetVsyncEnabled(false)
	} else {
		ebiten.SetTPS(e.ticksPerSecond())
		ebiten.SetVsyncEnabled(e.cfg.Video.VRetrace)
	}
}

func (e *Engine) Update() error {
	select {
	case <-e.stop:
		return ebiten.Termination
	default:
	}
	_, wasTitle := e.currentState.(*TitleState)
	if wasTitle {
		e.modeRun = nil
	}
	if fight, ok := e.currentState.(*FightState); ok && fight.match != nil {
		fight.match.Timeline.GameTime = e.gameTime
	}
	if input.GlobalManager != nil {
		input.GlobalManager.Update()
	}
	interruptedDemo := e.demo != nil && demoInputPressed()
	if interruptedDemo {
		next, err := e.endDemo(false)
		if err != nil {
			return err
		}
		e.currentState = next
	}
	if err := e.syncMusic(); err != nil {
		return err
	}

	if e.currentState != nil && !interruptedDemo {
		next, err := e.currentState.Update()
		if err != nil {
			return err
		}
		e.currentState = next
	}
	if err := e.syncMusic(); err != nil {
		return err
	}
	if err := e.flushSound(); err != nil {
		return err
	}
	if _, title := e.currentState.(*TitleState); title && !wasTitle {
		// Title music has replaced any packed scene track above. No active
		// selection/match needs expanded files now; release them before the
		// next roster load instead of retaining every pack until process exit.
		e.modeRun = nil
		if err := e.characterPacks.Close(); err != nil {
			return err
		}
	}
	// User pause holds the complete simulation, including CNS GameTime. The
	// input adapter and audio service still run so resume/frame-step can work.
	if fight, ok := e.currentState.(*FightState); ok && fight.simulationHeld {
		return nil
	}
	e.gameTime++
	// A transition may have constructed a fresh match during this update.
	// Its first pass belongs to the next process tick, without reusing pauses.
	if fight, ok := e.currentState.(*FightState); ok && fight.match != nil {
		fight.match.Timeline.GameTime = e.gameTime
	}
	return nil
}

func (e *Engine) Draw(screen *ebiten.Image) {
	if e.currentState != nil {
		e.currentState.Draw(screen)
	} else {
		msg := fmt.Sprintf("MUGEN Engine\nData Dir: %s\nMotif: %s\nFPS: %d\nShadows: %v",
			e.dataDir, e.cfg.Options.Motif, e.cfg.Config.GameSpeed, e.cfg.Config.DrawShadows)
		ebitenutil.DebugPrint(screen, msg)
	}
}

func (e *Engine) Layout(outsideWidth, outsideHeight int) (int, int) {
	if _, fight := e.currentState.(*FightState); fight {
		return max(1, outsideWidth), max(1, outsideHeight)
	}
	if _, storyboard := e.currentState.(*StoryboardState); storyboard {
		return max(1, outsideWidth), max(1, outsideHeight)
	}
	return e.logicalWidth, e.logicalHeight
}

func run() (runErr error) {
	log.SetFlags(log.Lshortfile | log.Ldate | log.Lmicroseconds)

	dataDir := flag.String("data-dir", "data-new/mugen", "Path to the MUGEN data directory")
	quickMatch := flag.String("quick-match", "", "Start a versus match with two character directories, DEFs or ZIP[:DEF] entries, separated by a comma")
	quickStage := flag.String("stage", "", "Stage DEF for --quick-match (defaults to Debug.StartStage)")
	matchSeed := flag.Int64("match-seed", 0, "Initial match RNG seed for reproducible runs (1..2147483646; zero uses the clock)")
	flag.Parse()
	if *matchSeed < 0 || *matchSeed >= 2147483647 {
		return fmt.Errorf("match-seed must be zero or between 1 and 2147483646")
	}

	// Verify data directory
	if _, err := os.Stat(*dataDir); os.IsNotExist(err) {
		return fmt.Errorf("data directory does not exist: %s", *dataDir)
	}

	// Load configuration
	cfg, err := config.LoadConfig(*dataDir)
	if err != nil {
		return fmt.Errorf("load mugen.cfg: %w", err)
	}

	fmt.Printf("Loaded configuration from %s\n", filepath.Join(*dataDir, "data", "mugen.cfg"))
	fmt.Printf("Motif: %s\n", cfg.Options.Motif)
	fmt.Printf("GameSpeed: %d\n", cfg.Config.GameSpeed)

	width := cfg.Video.Width
	height := cfg.Video.Height
	if width == 0 {
		width = cfg.Config.GameWidth
	}
	if height == 0 {
		height = cfg.Config.GameHeight
	}

	// Initialize Input Manager
	input.GlobalManager = input.NewInputManager(cfg)

	// Load motif
	m, err := motif.LoadMotif(cfg.Options.Motif, *dataDir, cfg.Config.Language)
	if err != nil {
		return fmt.Errorf("load motif %s: %w", cfg.Options.Motif, err)
	}

	engine := &Engine{
		pendingMatchSeed: *matchSeed,
		random:           engine.NewRandomStream(time.Now().Unix()),
		cfg:              cfg,
		motif:            m,
		dataDir:          *dataDir,
		screenWidth:      width,
		screenHeight:     height,
		logicalWidth:     m.Info.LocalCoord.Width,
		logicalHeight:    m.Info.LocalCoord.Height,
		sprites:          make(map[string]*background.SpriteImages),
	}
	ctx, stop := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
	defer stop()
	engine.stop = ctx.Done()
	defer func() {
		if closeErr := engine.Close(); runErr == nil {
			runErr = closeErr
		}
	}()
	if err := engine.startSoundDevice(); err != nil {
		return err
	}

	// Keep CPU sprite views; upload only sprites/mask modes that actually draw.
	if m.SpriteData != nil {
		var textures background.SpriteCache
		for _, spr := range m.SpriteData.Sprites {
			if spr.Image != nil {
				key := fmt.Sprintf("%d-%d", spr.GroupNumber, spr.ImageNumber)
				engine.sprites[key] = textures.Pair(spr.Image, spr.ImageOpaque)
			}
		}
	}

	for i, f := range m.Fonts {
		if f != nil {
			fmt.Printf("Font %d loaded: %dx%d, %d glpyhs\n", i, f.Width, f.Height, len(f.Glyphs))
		}
	}

	// State Flow: Logo -> Intro -> Title
	titleState := NewTitleState(engine)
	introState := &StoryboardState{
		engine:     engine,
		storyboard: m.Files.IntroStoryboard,
		nextState:  func() State { return titleState },
	}
	logoState := &StoryboardState{
		engine:     engine,
		storyboard: m.Files.LogoStoryboard,
		nextState:  func() State { return introState },
	}

	engine.currentState = logoState
	if *quickMatch != "" {
		paths := parsers.SplitList(*quickMatch)
		if len(paths) != 2 {
			return fmt.Errorf("--quick-match requires two character entries")
		}
		stage := strings.TrimSpace(*quickStage)
		if stage == "" {
			stage = cfg.Debug.StartStage
		}
		if stage == "" {
			stage = "stages/kfm.def"
		}
		request := match.Request{Mode: "Versus Mode", StagePath: stage}
		for slot, path := range paths {
			request.Players[slot] = match.Selection{DefPath: strings.Trim(strings.TrimSpace(path), "\""), Palette: slot + 1}
		}
		engine.currentState, err = NewFightState(engine, request)
		if err != nil {
			return err
		}
	}

	fmt.Printf("Starting engine with resolution %dx%d\n", width, height)

	ebiten.SetWindowSize(width, height)
	ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)
	ebiten.SetWindowTitle("MUGEN Engine")

	// Set TPS based on GameSpeed config
	engine.applyScheduler()
	ebiten.SetRunnableOnUnfocused(!cfg.Misc.PauseOnDefocus)

	return ebiten.RunGame(engine)
}

func main() {
	if err := run(); err != nil {
		log.Fatal(err)
	}
}
