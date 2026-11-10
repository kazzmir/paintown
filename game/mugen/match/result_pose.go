package match

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/character"
)

// AdvanceResultPose keeps the completed fight alive behind a Win/Survival
// results overlay. It deliberately does not run result commitment, the round
// timer or the next-round scheduler a second time.
func (m *Match) AdvanceResultPose() error {
	if m.Phase != Finished {
		return fmt.Errorf("result pose requires a completed match")
	}
	m.SimulationSkipped = false
	if err := m.Effects.AdvanceWithPause(m.Timeline.Paused(), m.Timeline.SuperPaused()); err != nil {
		return fmt.Errorf("result effects: %w", err)
	}
	if !m.Timeline.BackgroundFrozen() {
		m.Stage.Update()
	}
	stageScale, _ := m.Stage.StageInfo.LocalCoord.ScaleTo(m.Options.GameSpace)
	m.Viewport.GroundY = m.Stage.GroundLevel() * stageScale
	for _, player := range m.Actors() {
		c := player.Character
		c.Ctrl, c.HitDefActive, c.RoundDamageDisabled = false, false, true
		scale, _ := m.Options.GameSpace.ScaleTo(player.LocalCoord)
		c.CameraX = m.CameraX * scale
	}
	if err := character.TickCharacters(m.activeRoots()); err != nil {
		return fmt.Errorf("result pose tick %d: %w", m.Tick, err)
	}
	if err := m.Effects.Flush(); err != nil {
		return fmt.Errorf("result effects: %w", err)
	}
	m.updateCamera()
	m.Tick++
	return nil
}
