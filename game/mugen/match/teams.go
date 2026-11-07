package match

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/character"
)

type TeamMode string

const (
	Single TeamMode = "single"
	Simul  TeamMode = "simul"
	Turns  TeamMode = "turns"
)

type TeamSelection struct {
	Mode    TeamMode
	Members []Selection
}

type Team struct {
	Mode      TeamMode
	Members   []*character.Player
	Active    int
	next      int
	entered   map[*character.Player]bool
	exhausted bool
}

func normalizeTeams(request Request) (Request, error) {
	for side := range request.Teams {
		team := &request.Teams[side]
		if len(team.Members) == 0 {
			team.Members = []Selection{request.Players[side]}
		}
		if team.Mode == "" {
			team.Mode = Single
		}
		count := len(team.Members)
		if (team.Mode == Single && count != 1) || (team.Mode == Simul && count != 2) || (team.Mode == Turns && (count < 1 || count > 4)) {
			return request, fmt.Errorf("team %d: %s requires one, two, or one-to-four members respectively", side+1, team.Mode)
		}
		if team.Mode != Single && team.Mode != Simul && team.Mode != Turns {
			return request, fmt.Errorf("team %d: unknown team mode %q", side+1, team.Mode)
		}
		team.Members = append([]Selection(nil), team.Members...)
		request.Players[side] = team.Members[0]
	}
	return request, nil
}

// ActivePlayers preserves beta player numbering: P1/P2 are the active leaders;
// P3/P4 are their simul partners. Benched turns members never tick or collide.
func (m *Match) ActivePlayers() []*character.Player {
	players := []*character.Player{m.Players[0], m.Players[1]}
	for side := range m.Teams {
		if team := &m.Teams[side]; team.Mode == Simul && len(team.Members) > 1 {
			players = append(players, team.Members[1])
		} else {
			players = append(players, nil)
		}
	}
	// Keep each partner's P3/P4 slot even when only the opposite team is simul.
	return players
}

func (m *Match) Actors() []*character.Player {
	var players []*character.Player
	for _, p := range m.ActivePlayers() {
		if p != nil {
			players = append(players, p)
		}
	}
	return players
}

func (m *Match) activeRoots() []*character.Character {
	var roots []*character.Character
	for _, p := range m.Actors() {
		roots = append(roots, p.Character)
	}
	return roots
}

func (m *Match) SelectionForPlayer(playerID int) (Selection, bool) {
	if playerID < 1 || playerID > 4 {
		return Selection{}, false
	}
	side, index := (playerID-1)%2, 0
	if playerID > 2 {
		index = 1
	} else if m.Teams[side].Mode == Turns {
		index = m.Teams[side].Active
	}
	if index >= len(m.Request.Teams[side].Members) {
		return Selection{}, false
	}
	return m.Request.Teams[side].Members[index], true
}

func (m *Match) SidePlayers(side int) []*character.Player {
	if side < 0 || side > 1 {
		return nil
	}
	players := []*character.Player{m.Players[side]}
	if m.Teams[side].Mode == Simul {
		players = append(players, m.Teams[side].Members[1])
	}
	return players
}

func (m *Match) hasTurns() bool { return m.Teams[0].Mode == Turns || m.Teams[1].Mode == Turns }

func (m *Match) teamKO(side int) bool {
	allDead := true
	for _, p := range m.SidePlayers(side) {
		dead := !p.Character.IsAlive()
		if dead && m.Options.TeamLoseOnKO {
			return true
		}
		allDead = allDead && dead
	}
	return allDead
}

func (m *Match) teamLife(side int) (life, maximum int64) {
	for _, p := range m.SidePlayers(side) {
		life += int64(max(0, p.Character.Life))
		maximum += int64(max(1, p.Character.MaxLife))
	}
	return
}

// Turns consumes losing rosters at committed round results, while their
// outgoing actors remain available for the complete result/win-pose phase.
func (m *Match) commitTeamResult() {
	if !m.hasTurns() {
		return
	}
	for side := range m.Teams {
		team := &m.Teams[side]
		if m.Winner == side {
			continue
		}
		if team.Mode == Turns && team.Active+1 < len(team.Members) {
			team.next = team.Active + 1
		} else {
			team.exhausted = true
		}
	}
	m.matchDone = m.Teams[0].exhausted || m.Teams[1].exhausted
}

func homeTeam(mode string) int {
	mode = strings.ToLower(mode)
	if strings.Contains(mode, "arcade") || strings.Contains(mode, "survival") {
		return 2
	}
	return 1
}
