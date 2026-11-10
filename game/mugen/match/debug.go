package match

// SetAILevel changes only this match's copied roster. Campaign templates keep
// their ownership, while round resets/rematches retain the live debug choice.
func (m *Match) SetAILevel(playerID, level int) bool {
	if playerID < 1 || playerID > 4 || level < 0 || level > 8 {
		return false
	}
	players := m.ActivePlayers()
	p := players[playerID-1]
	if p == nil {
		return false
	}
	side, member := (playerID-1)%2, 0
	if playerID > 2 {
		member = 1
	} else if m.Teams[side].Mode == Turns {
		member = m.Teams[side].Active
	}
	m.Request.Teams[side].Members[member].AILevel = level
	if playerID <= 2 {
		m.Request.Players[side].AILevel = level
	}
	root := p.Character
	root.AILevel = level
	delete(m.aiBrains, root.EntityID)
	for _, helper := range m.World.Helpers {
		owner := helper
		for owner.Parent != nil {
			owner = owner.Parent
		}
		if owner == root {
			helper.AILevel = level
		}
	}
	return true
}

// ForceTimeOver uses the ordinary result pipeline, including team life fractions,
// announcements, pose entry and result commitment. It cannot end a second round
// while the previous result is already being displayed.
func (m *Match) ForceTimeOver() bool {
	if m.Phase != Active {
		return false
	}
	m.finishRound()
	return true
}
