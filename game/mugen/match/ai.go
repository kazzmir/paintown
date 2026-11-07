package match

import "github.com/kazzmir/paintown/game/mugen/character"

// BasicAIInput is an initial single-opponent policy using the same logical
// command path as humans. It is deterministic; it is not Elecbyte AI parity.
func (m *Match) BasicAIInput(playerID int) []string {
	if playerID < 1 || playerID > 4 || m.Phase != Active {
		return nil
	}
	p := m.ActivePlayers()[playerID-1]
	if p == nil {
		return nil
	}
	c := p.Character
	var other *character.Character
	for _, enemy := range c.Opponents {
		if enemy.IsAlive() {
			other = enemy
			break
		}
	}
	if other == nil {
		return nil
	}
	if !c.Ctrl {
		return nil
	}
	distance, _ := c.GetOpponentBodyDistance()
	if other.MoveType == "A" && distance < float64(c.LocalCoordinateWidth())*0.3 {
		return []string{"DirBack"}
	}
	if distance > float64(c.LocalCoordinateWidth())*0.09 {
		return []string{"DirForward"}
	}
	period := max(18, 60-c.AILevel*4)
	if m.Tick%period < 2 {
		return []string{"ButtonA"}
	}
	return nil
}
