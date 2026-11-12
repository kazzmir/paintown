package main

import (
	"fmt"
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/motif"
)

// VersusState presents the selected match before loading its fight assets.
type VersusState struct {
	engine    *Engine
	ticks     int
	request   match.Request
	portraits [2]*CharacterPortrait
	names     [2][]string
	loadError error
	fade      *ebiten.Image
}

func NewVersusState(engine *Engine, request match.Request) *VersusState {
	s := &VersusState{engine: engine, request: request}
	for side, selected := range request.Players {
		members := request.Teams[side].Members
		if len(members) == 0 {
			members = []match.Selection{selected}
		}
		spec := engine.motif.VSScreen.P1Portrait
		if side == 1 {
			spec = engine.motif.VSScreen.P2Portrait
		}
		for index, member := range members {
			name := member.Name
			if index == 0 && member.DefPath != "" {
				portrait, err := loadCharacterPortraitLanguage(member.DefPath, engine.cfg.Config.Language, spec.Sprite)
				if err != nil {
					s.loadError = err
					return s
				}
				s.portraits[side] = portrait
				if name == "" {
					name = portrait.Name
				}
			}
			s.names[side] = append(s.names[side], name)
		}
	}
	return s
}

func (s *VersusState) Update() (State, error) {
	if s.loadError != nil {
		return s, s.loadError
	}
	s.ticks++
	if s.engine.motif.VsBG != nil {
		s.engine.motif.VsBG.Update()
	}
	if input.GlobalManager.IsJustPressed(1, input.ActionEnter) || s.ticks >= s.engine.motif.VSScreen.Time {
		if s.engine.demo != nil {
			return s.engine.nextDemoScreen()
		}
		return NewFightState(s.engine, s.request)
	}
	return s, nil
}

func (s *VersusState) Draw(screen *ebiten.Image) {
	bg := s.engine.motif.VsBG
	if bg != nil && bg.BGClearColor != nil {
		screen.Fill(bg.BGClearColor)
	} else {
		screen.Fill(color.Black)
	}
	if bg != nil {
		bg.DrawLayer(screen, 0, 0, 0, 0, s.engine.sprites)
	}
	vs := s.engine.motif.VSScreen
	s.drawSide(screen, 0, vs.P1Portrait, vs.P1NamePos, vs.P1NameFont, vs.P1NameSpacing)
	s.drawSide(screen, 1, vs.P2Portrait, vs.P2NamePos, vs.P2NameFont, vs.P2NameSpacing)
	if text := s.engine.themeText("VS Screen", "match.text", ""); text != "" && s.request.MatchNo > 0 {
		// MUGEN uses %i for the Arcade match counter.
		text = replaceMatchNumber(text, s.request.MatchNo)
		s.engine.modeLabel(screen, "VS Screen", "match", text, 0, 0, s.engine.motif.Fonts)
	}
	if bg != nil {
		bg.DrawLayer(screen, 1, 0, 0, 0, s.engine.sprites)
	}
	alpha := float32(0)
	if vs.FadeInTime > 0 && s.ticks < vs.FadeInTime {
		alpha = 1 - float32(s.ticks)/float32(vs.FadeInTime)
	}
	if vs.FadeOutTime > 0 && s.ticks > vs.Time-vs.FadeOutTime {
		alpha = max(alpha, float32(s.ticks-(vs.Time-vs.FadeOutTime))/float32(vs.FadeOutTime))
	}
	if alpha > 0 {
		if s.fade == nil {
			s.fade = ebiten.NewImage(1, 1)
			s.fade.Fill(color.Black)
		}
		op := &ebiten.DrawImageOptions{}
		op.GeoM.Scale(float64(screen.Bounds().Dx()), float64(screen.Bounds().Dy()))
		op.ColorScale.ScaleAlpha(min(1, alpha))
		screen.DrawImage(s.fade, op)
	}
}

func replaceMatchNumber(text string, number int) string {
	// Keep other authored text literal rather than treating it as a format string.
	for i := 0; i+1 < len(text); i++ {
		if text[i] == '%' && text[i+1] == 'i' {
			text = text[:i] + fmt.Sprint(number) + text[i+2:]
			break
		}
	}
	return text
}

func (s *VersusState) drawSide(screen *ebiten.Image, side int, spec motif.Portrait, position [2]int, info [3]int, spacing [2]int) {
	s.portraits[side].drawPortrait(screen, spec.Sprite, spec.Offset, spec.Scale, spec.Facing, s.engine.logicalWidth, spec.Window)
	if info[0] < 1 || info[0] >= len(s.engine.motif.Fonts) {
		return
	}
	f := s.engine.motif.Fonts[info[0]]
	if f == nil {
		return
	}
	align := font.AlignCenter
	if info[2] > 0 {
		align = font.AlignLeft
	} else if info[2] < 0 {
		align = font.AlignRight
	}
	for index, name := range s.names[side] {
		s.engine.drawMotifFont(screen, f, name, position[0]+index*spacing[0], position[1]+index*spacing[1], info[1], align, "VS Screen", fmt.Sprintf("p%d.name.font", side+1))
	}
}
