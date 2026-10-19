package match

// BasicAIInput is an initial single-opponent policy using the same logical
// command path as humans. It is deterministic; it is not Elecbyte AI parity.
func (m *Match) BasicAIInput(playerID int) []string {
	if playerID < 1 || playerID > 2 || m.Phase != Active {
		return nil
	}
	c := m.Players[playerID-1].Character
	other := m.Players[2-playerID].Character
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
