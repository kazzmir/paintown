package match

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/character"
)

// NewResultScene starts a separate, noncombat presentation from the loaded
// banks. The beta restores human actors, removes enemy context and the HUD,
// and enters 170/180 with RoundState 0; it does not resume the defeated actors.
// The committed match and its actor/score/carry state remain unchanged.
func (m *Match) NewResultScene(win bool, gameTime int32, seed int64) (*Match, error) {
	request := m.Request
	request.RoundStart, request.MatchNo, request.Campaign = 1, 1, 0
	request.Carry, request.RandomState = [2][]*RoundCarry{}, nil
	var cached [2][]*character.Player
	for side := range request.Teams {
		cached[side] = m.SidePlayers(side)
		request.Teams[side] = TeamSelection{Mode: Single}
		if len(cached[side]) == 2 {
			request.Teams[side].Mode = Simul
		}
		for member := range cached[side] {
			selected, _ := m.SelectionForPlayer(side + 1 + 2*member)
			selected.AILevel = 0
			request.Teams[side].Members = append(request.Teams[side].Members, selected)
		}
	}
	options := m.Options
	options.GameTime, options.RandomSeed = gameTime, seed
	options.Input, options.RosterInput, options.DisabledPlayers = nil, nil, [4]bool{}
	scene, err := newMatch(request, options, cached, true)
	if err != nil {
		return nil, err
	}
	scene.Phase = Finished
	state := 170
	if win {
		state = 180
	}
	actors := scene.Actors()
	for index, p := range actors {
		c := p.Character
		stageScale, _ := scene.Stage.StageInfo.LocalCoord.ScaleTo(p.LocalCoord)
		c.X, c.Y, c.Z, c.Facing = scene.Stage.Camera.StartX*stageScale, 0, 0, 1
		if len(actors) == 2 {
			c.X += float64(2*index-1) * 50 * float64(p.LocalCoord.Width) / 320
			c.Facing = 1 - 2*index
		}
		c.StartX, c.StartY, c.StartZ, c.StartFacing = c.X, c.Y, c.Z, c.Facing
		c.VelX, c.VelY = 0, 0
		if err := c.SetPaletteMapping([2]int{1, 1}, [2]int{1, c.PaletteNo}); err != nil {
			return nil, err
		}
		c.ChangeState(state, 0, -1)
		if c.RuntimeError != nil {
			return nil, c.RuntimeError
		}
	}
	return scene, nil
}

// AdvanceResultPose advances presentation without committing results, running
// the round timer or starting another round.
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
