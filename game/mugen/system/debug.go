package main

import (
	"fmt"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/resource"
)

func (s *FightState) debugActors() []*character.Character {
	actors := make([]*character.Character, 0, 2+len(s.match.World.Helpers))
	for _, player := range s.match.Actors() {
		actors = append(actors, player.Character)
	}
	return append(actors, s.match.World.Helpers...)
}

func (s *FightState) updateDebugSelection() {
	allowed := s.engine.cfg.Debug.Debug || s.engine.cfg.Debug.AllowDebugMode
	if allowed && input.GlobalManager != nil && input.GlobalManager.DebugCyclePressed() {
		s.debugSelection++
		if s.debugSelection >= len(s.debugActors()) {
			s.debugSelection = -1
		}
	}
}

func (s *FightState) drawDebugClipboard(screen *ebiten.Image) error {
	if s.debugSelection < 0 || !s.engine.demoShowsDebug() {
		return nil
	}
	actors := s.debugActors()
	if s.debugSelection >= len(actors) {
		s.debugSelection = 0
	}
	if s.debugFont == nil {
		path, err := resource.Resolve("font/f-4x6.fnt", s.engine.dataDir)
		if err != nil {
			return fmt.Errorf("debug clipboard font: %w", err)
		}
		s.debugFont, err = font.LoadFont(path)
		if err != nil {
			return fmt.Errorf("debug clipboard font: %w", err)
		}
	}
	actor := actors[s.debugSelection]
	lineHeight := max(1, s.debugFont.Height+s.debugFont.SpacingY)
	header := fmt.Sprintf("Player %d ID %d State %d Time %d Ctrl %t", actor.InputSlot, actor.EntityID, actor.StateNo, actor.Time, actor.Ctrl)
	s.debugFont.Draw(screen, header, 10, lineHeight+2, 0, font.AlignLeft)
	text := strings.ReplaceAll(strings.ReplaceAll(actor.Clipboard, "\r\n", "\n"), "\t", "    ")
	lines := strings.Split(strings.TrimSuffix(text, "\n"), "\n")
	limit := max(1, (screen.Bounds().Dy()-2*lineHeight-4)/lineHeight)
	if len(lines) > limit {
		lines = lines[len(lines)-limit:]
	}
	baseline := screen.Bounds().Dy() - 2 - (len(lines)-1)*lineHeight
	for _, line := range lines {
		s.debugFont.Draw(screen, line, 10, baseline, 0, font.AlignLeft)
		baseline += lineHeight
	}
	return nil
}
