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
	if strings.Contains(mode, "arcade") || strings.Contains(mode, "survival") || strings.Contains(mode, "co-op") {
		return 2
	}
	return 1
}

// The supplied 1.1b1's recovery routine (419350, caller 415262) first
// truncates a per-thousand bonus, then multiplies by maximum life. Its time
// window is always 100 counts, independent of the configured round limit.
func (m *Match) recoverWinner() {
	survival := strings.Contains(strings.ToLower(m.Request.Mode), "survival") && m.Winner == 0
	if m.Winner < 0 || (!m.hasTurns() && !survival) || (m.matchDone && !survival) {
		return
	}
	base, bonus := 125, float32(275)
	if survival {
		// Survival spreads the base across partners/reserves and reduces
		// the time bonus for the team's first member (native 4246a0).
		count := len(m.Teams[m.Winner].Members)
		if count > 1 {
			base /= count
			bonus = float32(float64(bonus) * (float64(count) - .5) / float64(count))
		}
	}
	remaining := float32(max(0, (100-float64(m.elapsedFightTicks)/float64(max(1, m.Setting("Time", "framespercount", 60))))/100))
	// Preserve the beta's float32 intermediate and its initial opponent
	// health adjustment; current/KO life would erase that distinction.
	timed := float32(bonus * remaining)
	factor := 1 + float64(float32(.6))*(float64(m.startingLife[1-m.Winner])-1)
	permille := int(float64(base) + float64(timed)*factor)
	for _, p := range m.SidePlayers(m.Winner) {
		c := p.Character
		if c.IsAlive() {
			c.SetLife(min(c.MaxLife, c.Life+c.MaxLife*permille/1000))
		}
	}
}

// The beta balances the total reserve health of unequal Turns rosters: a
// four-member team facing two members gets half life per entrant. The original
// configured roster sizes remain in force after members are eliminated.
func (m *Match) rosterLife(side, life int) int {
	team := m.Teams[side]
	if team.Mode != Turns || m.Teams[1-side].Mode != Turns || len(team.Members) == 0 {
		return max(1, life)
	}
	own, other := len(team.Members), len(m.Teams[1-side].Members)
	if other > 0 && other < own {
		life = life * other / own
	}
	return max(1, life)
}
