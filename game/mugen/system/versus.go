package main

import (
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
)

// VersusState shows the VS match-up screen.
type VersusState struct {
	engine *Engine
	ticks  int
}

func NewVersusState(engine *Engine) *VersusState {
	return &VersusState{engine: engine}
}

func (s *VersusState) Update() (State, error) {
	s.ticks++

	// Update backgrounds
	if s.engine.motif.VsBG != nil {
		s.engine.motif.VsBG.Update()
	}

	vsTime := s.engine.motif.VSScreen.Time
	if vsTime <= 0 {
		vsTime = 150
	}

	// Skip on button press
	if input.GlobalManager.IsJustPressed(1, input.ActionEnter) || s.ticks >= vsTime {
		// TODO: Transition to the actual fight
		return NewTitleState(s.engine), nil
	}
	return s, nil
}

func (s *VersusState) Draw(screen *ebiten.Image) {
	// Clear
	if s.engine.motif.VsBG != nil && s.engine.motif.VsBG.BGClearColor != nil {
		screen.Fill(s.engine.motif.VsBG.BGClearColor)
	} else {
		screen.Fill(color.Black)
	}

	// Draw Layer 0 BG
	if s.engine.motif.VsBG != nil {
		for _, el := range s.engine.motif.VsBG.Elements {
			if el.GetLayerNo() == 0 {
				el.Draw(screen, 0, 0, s.engine.sprites)
			}
		}
	}

	vs := s.engine.motif.VSScreen
	// Draw VS text placeholder
	if vs.P1NameFont[0] >= 1 && vs.P1NameFont[0] < len(s.engine.motif.Fonts) {
		f := s.engine.motif.Fonts[vs.P1NameFont[0]]
		if f != nil {
			f.Draw(screen, "Player 1", vs.P1NamePos[0], vs.P1NamePos[1], vs.P1NameFont[1], font.AlignLeft)
			f.Draw(screen, "Player 2", vs.P2NamePos[0], vs.P2NamePos[1], vs.P2NameFont[1], font.AlignRight)
		}
	}

	// Draw Layer 1 BG (foreground)
	if s.engine.motif.VsBG != nil {
		for _, el := range s.engine.motif.VsBG.Elements {
			if el.GetLayerNo() == 1 {
				el.Draw(screen, 0, 0, s.engine.sprites)
			}
		}
	}
}
