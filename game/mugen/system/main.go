package main

import (
	"bytes"
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/audio"
	"github.com/hajimehoshi/ebiten/v2/audio/wav"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/motif"
)

const sampleRate = 44100

type Engine struct {
	cfg           *config.MugenConfig
	motif         *motif.Motif
	dataDir       string
	screenWidth   int // Window width
	screenHeight  int // Window height
	logicalWidth  int // Game logic width (e.g. 320)
	logicalHeight int // Game logic height (e.g. 240)
	currentState  State
	sprites       map[string]*background.SpriteImages
	audioCtx      *audio.Context
}

func (e *Engine) Update() error {
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

func (e *Engine) PlaySnd(group, item int) {
	if e.motif.SoundBank == nil {
		return
	}

	key := fmt.Sprintf("%d-%d", group, item)
	sound, ok := e.motif.SoundBank.Map[key]
	if !ok {
		return
	}

	d, err := wav.DecodeWithSampleRate(sampleRate, bytes.NewReader(sound.Data))
	if err != nil {
		fmt.Printf("Warning: failed to decode sound %s: %v\n", key, err)
		return
	}

	p, err := e.audioCtx.NewPlayer(d)
	if err != nil {
		fmt.Printf("Warning: failed to create audio player for %s: %v\n", key, err)
		return
	}

	p.Play()
}

func (e *Engine) Layout(outsideWidth, outsideHeight int) (int, int) {
	return e.logicalWidth, e.logicalHeight
}

func main() {
	dataDir := flag.String("data-dir", "data-new/mugen", "Path to the MUGEN data directory")
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
		width = 320
	}
	if height == 0 {
		height = 240
	}

	// Initialize Input Manager
	input.GlobalManager = input.NewInputManager(cfg)

	// Load motif
	m, err := motif.LoadMotif(cfg.Options.Motif, *dataDir)
	if err != nil {
		log.Fatalf("Failed to load motif %s: %v", cfg.Options.Motif, err)
	}

	engine := &Engine{
		cfg:           cfg,
		motif:         m,
		dataDir:       *dataDir,
		screenWidth:   width,
		screenHeight:  height,
		logicalWidth:  320, // Standard MUGEN
		logicalHeight: 240,
		sprites:       make(map[string]*background.SpriteImages),
		audioCtx:      audio.NewContext(sampleRate),
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

	fmt.Printf("Starting engine with resolution %dx%d\n", width, height)

	ebiten.SetWindowSize(width*2, height*2)
	ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)
	ebiten.SetWindowTitle("MUGEN Engine")

	// Set TPS based on GameSpeed config
	if cfg.Config.GameSpeed > 0 {
		ebiten.SetTPS(cfg.Config.GameSpeed)
	}

	if err := ebiten.RunGame(engine); err != nil {
		log.Fatal(err)
	}
}
