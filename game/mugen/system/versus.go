package main

import (
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
)

// VersusState shows the VS match-up screen.
type VersusState struct {
	engine  *Engine
	ticks   int
	request match.Request
}

func NewVersusState(engine *Engine, request match.Request) *VersusState {
	return &VersusState{engine: engine, request: request}
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
		return NewFightState(s.engine, s.request)
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
		s.engine.motif.VsBG.DrawLayer(screen, 0, 0, 0, 0, s.engine.sprites)
	}

	vs := s.engine.motif.VSScreen
	// Draw VS text placeholder
	if vs.P1NameFont[0] >= 1 && vs.P1NameFont[0] < len(s.engine.motif.Fonts) {
		f := s.engine.motif.Fonts[vs.P1NameFont[0]]
		if f != nil {
			f.Draw(screen, s.request.Players[0].Name, vs.P1NamePos[0], vs.P1NamePos[1], vs.P1NameFont[1], font.AlignLeft)
		}
	}
	if vs.P2NameFont[0] >= 1 && vs.P2NameFont[0] < len(s.engine.motif.Fonts) {
		if f := s.engine.motif.Fonts[vs.P2NameFont[0]]; f != nil {
			f.Draw(screen, s.request.Players[1].Name, vs.P2NamePos[0], vs.P2NamePos[1], vs.P2NameFont[1], font.AlignRight)
		}
	}

	// Draw Layer 1 BG (foreground)
	if s.engine.motif.VsBG != nil {
		s.engine.motif.VsBG.DrawLayer(screen, 1, 0, 0, 0, s.engine.sprites)
	}
}
