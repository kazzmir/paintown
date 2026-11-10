package main

import (
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/input"
)

// Debug hotkeys are separate from player bindings and the AllowDebugMode gate
// for Ctrl-D. They use the ordinary character/match consumers, not a second
// combat implementation, and remain available during a user pause.
func (s *FightState) updateDebugHotkeys() error {
	im := input.GlobalManager
	if im == nil || !s.debugKeysAllowed() {
		return nil
	}
	control := im.KeyPressed(ebiten.KeyControlLeft) || im.KeyPressed(ebiten.KeyControlRight)
	shift := im.KeyPressed(ebiten.KeyShiftLeft) || im.KeyPressed(ebiten.KeyShiftRight)
	alt := im.KeyPressed(ebiten.KeyAltLeft) || im.KeyPressed(ebiten.KeyAltRight)
	if control && im.KeyJustPressed(ebiten.KeyC) {
		s.engine.showCollisions = !s.engine.showCollisions
	}
	if control && im.KeyJustPressed(ebiten.KeyL) {
		s.engine.hideFightBars = !s.engine.hideFightBars
	}
	if control && im.KeyJustPressed(ebiten.KeyS) {
		s.engine.cfg.Debug.Speedup = !s.engine.cfg.Debug.Speedup
		s.engine.applyScheduler()
	}
	if control && im.KeyJustPressed(ebiten.KeyV) {
		s.engine.cfg.Debug.Speedup = false
		s.engine.cfg.Video.VRetrace = true
		s.engine.applyScheduler()
	}
	if control && !alt {
		for slot, key := range []ebiten.Key{ebiten.KeyDigit1, ebiten.KeyDigit2, ebiten.KeyDigit3, ebiten.KeyDigit4} {
			if !im.KeyJustPressed(key) {
				continue
			}
			selected, present := s.match.SelectionForPlayer(slot + 1)
			if !present {
				continue
			}
			level := 0
			if selected.AILevel == 0 {
				level = max(1, min(8, s.engine.cfg.Options.Difficulty))
			}
			if s.match.SetAILevel(slot+1, level) && slot == 1 && s.training != nil {
				s.training.control = 2
				if level > 0 {
					s.training.control = 1
				}
			}
		}
	}
	if im.KeyJustPressed(ebiten.KeyF1) {
		side := 1
		if control {
			side = 0
		}
		s.match.Players[side].Character.SetLife(0)
	}
	if im.KeyJustPressed(ebiten.KeyF2) {
		for side, player := range s.match.Players {
			if control && side != 0 || !control && shift && side != 1 {
				continue
			}
			player.Character.SetLife(1)
		}
	}
	if im.KeyJustPressed(ebiten.KeyF3) || im.KeyJustPressed(ebiten.KeySpace) {
		for _, player := range s.match.Actors() {
			c := player.Character
			c.SetPower(c.MaxPower)
			if im.KeyJustPressed(ebiten.KeySpace) {
				c.SetLife(c.MaxLife)
			}
		}
	}
	if control && im.KeyJustPressed(ebiten.KeyI) {
		for _, player := range s.match.Actors() {
			c := player.Character
			// SelfState restores the fighter's own state bank after a throw.
			c.SelfState(0, 1, -1)
			c.VelX, c.VelY = 0, 0
			if c.RuntimeError != nil {
				return c.RuntimeError
			}
		}
	}
	if im.KeyJustPressed(ebiten.KeyF5) {
		s.match.ForceTimeOver()
	}
	if im.KeyJustPressed(ebiten.KeyF4) {
		if shift {
			fresh, err := s.match.Reload()
			if err != nil {
				return err
			}
			if fresh == nil {
				return nil
			}
			s.match.Effects.Clear()
			// New presentation caches must reference the newly loaded banks,
			// geometry and voices. Keep only session/menu choices.
			*s = FightState{engine: s.engine, match: fresh, debugSelection: s.debugSelection, userPaused: s.userPaused, training: s.training}
			s.bindMatchInputs()
		} else if err := s.match.RestartRound(); err != nil {
			return err
		}
		s.hud = NewFightHUD(s.match, s.engine)
		s.debugResetHandoff = true
	}
	return nil
}

func (s *FightState) debugKeysAllowed() bool {
	// mugen.cfg allows keys only in debug mode when AllowDebugKeys is zero.
	return s.engine.cfg.Debug.AllowDebugKeys || s.debugSelection >= 0
}
