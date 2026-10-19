package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/motif"
)

type Engine struct {
	cfg           *config.MugenConfig
	motif         *motif.Motif
	dataDir       string
	screenWidth   int // Window width
	screenHeight  int // Window height
	logicalWidth  int // Game logic width (e.g. 320)
	logicalHeight int // Game logic height (e.g. 240)
	currentState  State
	gameTime      int32 // Process ticks, including menus and finished-match waits.
	sprites       map[string]*background.SpriteImages
	sound         *engineSound
	random        *engine.RandomStream
}

func (e *Engine) nextMatchSeed() int64 {
	if e.random == nil {
		e.random = engine.NewRandomStream(time.Now().Unix())
	}
	return e.random.Next()
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

func (e *Engine) Update() error {
	if fight, ok := e.currentState.(*FightState); ok && fight.match != nil {
		fight.match.Timeline.GameTime = e.gameTime
	}
	if input.GlobalManager != nil {
		input.GlobalManager.Update()
	}

	if e.currentState != nil {
		next, err := e.currentState.Update()
		if err != nil {
			return err
		}
		e.currentState = next
	}
	if err := e.flushSound(); err != nil {
		return err
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

func main() {
	log.SetFlags(log.Lshortfile | log.Ldate | log.Lmicroseconds)

	dataDir := flag.String("data-dir", "data-new/mugen", "Path to the MUGEN data directory")
	quickMatch := flag.String("quick-match", "", "Start a versus match with two character DEF paths, separated by a comma")
	quickStage := flag.String("stage", "stages/kfm.def", "Stage DEF for --quick-match")
	flag.Parse()

	// Verify data directory
	if _, err := os.Stat(*dataDir); os.IsNotExist(err) {
		log.Fatalf("Data directory does not exist: %s", *dataDir)
	}

	// Load configuration
	cfg, err := config.LoadConfig(*dataDir)
	if err != nil {
		log.Fatalf("Failed to load mugen.cfg: %v", err)
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
	m, err := motif.LoadMotif(cfg.Options.Motif, *dataDir)
	if err != nil {
		log.Fatalf("Failed to load motif %s: %v", cfg.Options.Motif, err)
	}

	engine := &Engine{
		random:        engine.NewRandomStream(time.Now().Unix()),
		cfg:           cfg,
		motif:         m,
		dataDir:       *dataDir,
		screenWidth:   width,
		screenHeight:  height,
		logicalWidth:  m.Info.LocalCoord.Width,
		logicalHeight: m.Info.LocalCoord.Height,
		sprites:       make(map[string]*background.SpriteImages),
	}
	if err := engine.startSoundDevice(); err != nil {
		log.Fatal(err)
	}

	// Cache sprites
	if m.SpriteData != nil {
		fmt.Printf("Caching %d sprites from motif...\n", len(m.SpriteData.Sprites))
		for _, spr := range m.SpriteData.Sprites {
			if spr.Image != nil {
				key := fmt.Sprintf("%d-%d", spr.GroupNumber, spr.ImageNumber)
				si := &background.SpriteImages{
					Masked: ebiten.NewImageFromImage(spr.Image),
				}
				if spr.ImageOpaque != nil {
					si.Opaque = ebiten.NewImageFromImage(spr.ImageOpaque)
				} else {
					si.Opaque = si.Masked
				}
				engine.sprites[key] = si
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
		paths := strings.Split(*quickMatch, ",")
		if len(paths) != 2 {
			log.Fatal("--quick-match requires two character DEF paths")
		}
		request := match.Request{Mode: "Versus Mode", StagePath: *quickStage}
		for slot, path := range paths {
			request.Players[slot] = match.Selection{DefPath: filepath.Join(*dataDir, strings.TrimSpace(path)), Palette: slot + 1}
		}
		engine.currentState, err = NewFightState(engine, request)
		if err != nil {
			log.Fatal(err)
		}
	}

	fmt.Printf("Starting engine with resolution %dx%d\n", width, height)

	ebiten.SetWindowSize(width, height)
	ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)
	ebiten.SetWindowTitle("MUGEN Engine")

	// Set TPS based on GameSpeed config
	ebiten.SetTPS(engine.ticksPerSecond())

	err = ebiten.RunGame(engine)
	if closeErr := engine.closeSound(); err == nil {
		err = closeErr
	}
	if err != nil {
		log.Fatal(err)
	}
}
