package match

type roundRestartSnapshot struct {
	round, draws int
	wins         [2]int
	active       [2]int
	exhausted    [2]bool
	members      [2][]RoundCarry
}

// Capture before state5900 entry, so restarting does not apply entry-expression
// side effects twice. Benched members and the uncommitted score stay included.
func (m *Match) captureRoundRestart() {
	snapshot := &roundRestartSnapshot{round: m.Round, wins: m.Wins, draws: m.Draws}
	for side, team := range m.Teams {
		snapshot.active[side], snapshot.exhausted[side] = team.Active, team.exhausted
		for _, p := range team.Members {
			snapshot.members[side] = append(snapshot.members[side], *SnapshotRound(p.Character))
		}
	}
	m.roundRestart = snapshot
}

// RestartRound retires transient actors/effects through normal reset, restoring
// this round's initial carry and score without counting another round. Process
// GameTime and the match RNG continue; assets and campaign templates are reused.
func (m *Match) RestartRound() error {
	snapshot := m.roundRestart
	if snapshot == nil {
		return nil
	}
	m.Round, m.Wins, m.Draws = snapshot.round, snapshot.wins, snapshot.draws
	m.matchDone = false
	for side := range m.Teams {
		m.Teams[side].next = snapshot.active[side]
		m.Teams[side].exhausted = snapshot.exhausted[side]
		for member, player := range m.Teams[side].Members {
			carry, c := snapshot.members[side][member], player.Character
			c.SetLife(carry.Life)
			c.SetPower(carry.Power)
			c.RoundsExisted, c.IntVars, c.FloatVars = carry.RoundsExisted, carry.IntVars, carry.FloatVars
		}
	}
	m.restarting = snapshot
	defer func() { m.restarting = nil }()
	m.Stage.ResetBackground()
	return m.resetRound()
}

// Reload reconstructs stage/fight/character banks but keeps this round's roster,
// starting carry, score and live ownership. The old match remains owned by the
// caller until the replacement loads; no archive is unlinked here.
func (m *Match) Reload() (*Match, error) {
	snapshot := m.roundRestart
	if snapshot == nil {
		return nil, nil
	}
	request, options := m.Request, m.Options
	options.GameTime = m.Timeline.GameTime
	request.RoundStart = snapshot.round
	random := m.World.RandomState()
	request.RandomState = &random
	for side := range request.Teams {
		request.Teams[side].StartMember = snapshot.active[side]
		request.Carry[side] = nil
		for _, entry := range snapshot.members[side] {
			carry := entry
			// New's campaign carry counts a newly loaded round. Reload stays
			// in the existing round and must publish the same counter at entry.
			carry.RoundsExisted--
			request.Carry[side] = append(request.Carry[side], &carry)
		}
	}
	rosterInput := options.RosterInput
	options.RosterInput = func(int, int) []string { return nil } // no second loading-time roster rotation
	fresh, err := New(request, options)
	if err != nil {
		return nil, err
	}
	fresh.Options.RosterInput = rosterInput
	fresh.Wins, fresh.Draws = snapshot.wins, snapshot.draws
	fresh.roundRestart.wins, fresh.roundRestart.draws = snapshot.wins, snapshot.draws
	for side := range fresh.Teams {
		fresh.Teams[side].exhausted = snapshot.exhausted[side]
	}
	return fresh, nil
}

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
