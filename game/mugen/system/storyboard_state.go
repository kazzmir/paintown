package main

import (
	"fmt"
	"image/color"
	"os"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/sff"
	sb "github.com/kazzmir/paintown/game/mugen/storyboard"
)

// StoryboardState plays a storyboard .def file and then transitions to nextState.
type StoryboardState struct {
	engine     *Engine
	storyboard string // absolute path to the .def file
	nextState  func() State

	player *sb.Player
	loaded bool
	failed bool
}

func (s *StoryboardState) load() {
	s.loaded = true

	if s.storyboard == "" {
		s.failed = true
		return
	}

	f, err := os.Open(s.storyboard)
	if err != nil {
		fmt.Printf("Storyboard: failed to open %s: %v\n", s.storyboard, err)
		s.failed = true
		return
	}
	defer f.Close()

	sbDef, err := parsers.ParseStoryboard(f)
	if err != nil {
		fmt.Printf("Storyboard: failed to parse %s: %v\n", s.storyboard, err)
		s.failed = true
		return
	}

	sbDir := filepath.Dir(s.storyboard)

	// Load the sprite file for this storyboard
	var charSFF *sff.SFF
	if sbDef.SprFile != "" {
		sprPath := filepath.Join(sbDir, sbDef.SprFile)
		sprFile, err := os.Open(sprPath)
		if err == nil {
			charSFF, _ = sff.Parse(sprFile)
			sprFile.Close()
		}
	}

	// Build sprite image map and raw sprite map from the SFF
	spriteMap := make(map[string]*background.SpriteImages)
	rawSprites := make(map[string]*sff.Sprite)
	if charSFF != nil {
		for i := range charSFF.Sprites {
			sp := &charSFF.Sprites[i]
			if sp.Image == nil {
				continue
			}
			key := fmt.Sprintf("%d-%d", sp.GroupNumber, sp.ImageNumber)
			eImg := imageToEbiten(sp.Image)
			if eImg != nil {
				spriteMap[key] = &background.SpriteImages{Masked: eImg, Opaque: eImg}
				rawSprites[key] = sp
			}
		}
	}

	// Also re-open the .def to parse embedded AIR actions
	f2, err := os.Open(s.storyboard)
	if err != nil {
		s.failed = true
		return
	}
	defer f2.Close()

	ast, err := parsers.Parse(f2)
	if err != nil {
		s.failed = true
		return
	}
	airData, _ := air.ParseFromAST(ast)

	// Build storyboard.Storyboard from parsed scenes
	story := &sb.Storyboard{
		StartScene: sbDef.StartScene,
		SpriteMap:  spriteMap,
		RawSprites: rawSprites,
	}

	lastClearColor := color.RGBA{0, 0, 0, 255}
	lastClearColorSet := false
	lastDefaultPosition := [2]float64{0, 0}

	for _, sceneDef := range sbDef.Scenes {
		scene := &sb.Scene{
			EndTime:      sceneDef.EndTime,
			SpriteImages: spriteMap,
			RawSprites:   rawSprites,
			Fader: sb.FadeTool{
				FadeInTime:  sceneDef.FadeInTime,
				FadeOutTime: sceneDef.FadeOutTime,
				FadeInColor: color.RGBA{
					R: uint8(sceneDef.FadeInColor[0]),
					G: uint8(sceneDef.FadeInColor[1]),
					B: uint8(sceneDef.FadeInColor[2]),
					A: 255,
				},
				FadeOutColor: color.RGBA{
					R: uint8(sceneDef.FadeOutColor[0]),
					G: uint8(sceneDef.FadeOutColor[1]),
					B: uint8(sceneDef.FadeOutColor[2]),
					A: 255,
				},
			},
		}

		if sceneDef.ClearColorSet {
			lastClearColor = color.RGBA{
				R: uint8(sceneDef.ClearColor[0]),
				G: uint8(sceneDef.ClearColor[1]),
				B: uint8(sceneDef.ClearColor[2]),
				A: 255,
			}
			lastClearColorSet = true
		}
		scene.ClearColor = lastClearColor
		scene.ClearColorSet = lastClearColorSet

		if sceneDef.DefaultPosSet {
			lastDefaultPosition = [2]float64{
				float64(sceneDef.DefaultPosition[0]),
				float64(sceneDef.DefaultPosition[1]),
			}
		}
		scene.DefaultPosition = lastDefaultPosition

		// Build layers from layer definitions
		for _, layerDef := range sceneDef.Layers {
			if layerDef.AnimNo < 0 || airData == nil {
				continue
			}
			action, ok := airData.Actions[layerDef.AnimNo]
			if !ok {
				continue
			}
			layer := &sb.Layer{
				Offset:     [2]float64{float64(layerDef.OffsetX), float64(layerDef.OffsetY)},
				StartTime:  layerDef.StartTime,
				Sprites:    spriteMap,
				RawSprites: rawSprites,
				Player: &air.Player{
					Action: action,
				},
			}
			scene.Layers = append(scene.Layers, layer)
		}

		story.Scenes = append(story.Scenes, scene)
	}

	if len(story.Scenes) == 0 {
		s.failed = true
		return
	}

	s.player = sb.NewPlayer(story)
}

func (s *StoryboardState) Update() (State, error) {
	if !s.loaded {
		s.load()
	}

	if s.failed || s.player == nil {
		return s.nextState(), nil
	}

	skip := input.GlobalManager.IsJustPressed(1, input.ActionEnter) ||
		input.GlobalManager.IsJustPressed(1, input.ActionEscape) ||
		input.GlobalManager.IsJustPressed(1, input.ActionSpace)

		// this should be skip to next scene not done (unless on the last sceene)
	done := s.player.Update(sb.InputState{Skip: skip})
	if done {
		return s.nextState(), nil
	}
	return s, nil
}

func (s *StoryboardState) Draw(screen *ebiten.Image) {
	screen.Fill(color.Black)
	if s.player != nil {
		s.player.Draw(screen)
	}
}
