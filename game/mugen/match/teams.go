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
	Mode        TeamMode
	Members     []Selection
	StartMember int // Survival resumes the original Turns roster without reviving losses.
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
		if team.StartMember < 0 || team.StartMember >= count || (team.Mode != Turns && team.StartMember != 0) {
			return request, fmt.Errorf("team %d: invalid starting member %d for %s roster", side+1, team.StartMember, team.Mode)
		}
		if len(request.Carry[side]) > count {
			return request, fmt.Errorf("team %d: round carry exceeds roster size", side+1)
		}
		team.Members = append([]Selection(nil), team.Members...)
		request.Carry[side] = append([]*RoundCarry(nil), request.Carry[side]...)
		request.Players[side] = team.Members[team.StartMember]
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

func rotateTail[T any](values []T, start, offset int) {
	count := len(values) - start
	if count < 2 {
		return
	}
	offset = (offset%count + count) % count
	tail := append([]T(nil), values[start:]...)
	copy(values[start:], tail[offset:])
	copy(values[start+count-offset:], tail[:offset])
}

// The beta readme defines this loading-time control. Eliminated members stay
// before next; a continuing Survival winner never receives another rotation.
func (m *Match) rotateNextMembers(initial bool) {
	read := m.Options.RosterInput
	if read == nil {
		read = m.Options.Input
	}
	if read == nil {
		return
	}
	for side := range m.Teams {
		team := &m.Teams[side]
		start, count := team.next, len(team.Members)-team.next
		if team.Mode != Turns || count < 2 || (!initial && start == team.Active) {
			continue
		}
		if initial && start < len(m.Request.Carry[side]) && m.Request.Carry[side][start] != nil {
			continue
		}
		if m.Request.Teams[side].Members[start].AILevel > 0 {
			continue
		}
		facing := m.Stage.PlayerInfo.P1Facing
		if side == 1 {
			facing = m.Stage.PlayerInfo.P2Facing
		}
		if facing == 0 {
			facing = 1 - side*2
		}
		forward, back, up := false, false, false
		for _, key := range read(side+1, facing) {
			switch key {
			case "DirForward":
				forward = true
			case "DirBack":
				back = true
			case "DirUp":
				up = true
			}
		}
		offset := 0
		if up && count == 4 {
			offset = 2
		} else if forward != back {
			if forward {
				offset = 1
			} else {
				offset = count - 1
			}
		}
		if offset == 0 {
			continue
		}
		rotateTail(team.Members, start, offset)
		rotateTail(m.Request.Teams[side].Members, start, offset)
		if len(m.Request.Carry[side]) == len(team.Members) {
			rotateTail(m.Request.Carry[side], start, offset)
		}
	}
}

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
		} else if team.Mode == Turns || m.Wins[1-side] >= m.teamRoundLimit(side) {
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

func (m *Match) isSurvival() bool {
	return strings.Contains(strings.ToLower(m.Request.Mode), "survival")
}

// A Turns member has one round; Single/Simul retain the fight definition's
// round allowance even against Turns. Survival opponents are single bouts.
func (m *Match) teamRoundLimit(side int) int {
	if m.isSurvival() && m.Teams[side].Mode != Turns {
		return 1
	}
	return m.roundAllowance(side)
}

// Health balancing precedes Survival's one-loss/infinite-opponent overrides.
func (m *Match) roundAllowance(side int) int {
	if m.Teams[side].Mode == Turns {
		return len(m.Teams[side].Members)
	}
	return max(1, m.Setting("Round", "match.wins", 2))
}

// The supplied 1.1b1's recovery routine (419350, caller 415262) first
// truncates a per-thousand bonus, then multiplies by maximum life. Its time
// window is always 100 counts, independent of the configured round limit.
func (m *Match) recoverWinner() {
	survival := m.isSurvival() && m.Winner == 0
	if m.Winner < 0 || (!m.hasTurns() && !survival) || (m.matchDone && !survival) {
		return
	}
	base, bonus := 125, float32(275)
	if survival {
		// Survival spreads the base across partners/reserves and reduces
		// the time bonus for the team's first member (native 4246a0).
		count := 1 // Simul's two active fighters form one round group.
		if m.Teams[m.Winner].Mode == Turns {
			count = len(m.Teams[m.Winner].Members)
		}
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
		// A defeated Simul partner receives life too, retaining Alive0 during
		// the result and returning with that amount on the next round/bout.
		c.SetLife(min(c.MaxLife, c.Life+c.MaxLife*permille/1000))
	}
}

// The beta compares concurrent members per required opponent defeat, with
// Team.1VS2Life weighting Simul's strength. Only the stronger team's maximum
// life is reduced (native4245ac..424643). Original roster/round allowances
// remain in force after members are eliminated; it never boosts Single life.
func (m *Match) rosterLife(side, life int) int {
	var strength [2]float32
	for team := range m.Teams {
		count := 1
		if m.Teams[team].Mode == Simul {
			count = 2
		}
		strength[team] = float32(float64(count) / float64(m.roundAllowance(1-team)))
		if m.Teams[team].Mode == Simul {
			strength[team] = float32(float64(strength[team]) * float64(m.Options.Team1Vs2Life) / 200)
		}
	}
	ratio := float32(strength[0] / strength[1])
	factor := float32(1)
	if ratio > 1 {
		if side == 0 {
			factor = float32(1 / float64(ratio))
		}
	} else if side == 1 {
		factor = ratio
	}
	if m.isSurvival() && side == 0 && m.Teams[side].Mode == Turns {
		// Native4246a0 reduces reserve teams by10% for each extra member;
		// Simul keeps its ordinary1-vs-2 normalization instead.
		factor = float32(1 - float64(len(m.Teams[side].Members)-1)*.1)
	}
	// The native consumer424b9b..424bae applies Life% in the same expression
	// and rounds the resulting maximum, rather than truncating each factor.
	return max(1, int(float64(life)*float64(factor)*float64(m.Options.LifePercent)/100+.5))
}
