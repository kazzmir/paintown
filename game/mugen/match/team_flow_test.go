package match

import (
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

// Connected team flow: all four roots run CNS and partner combat, one KO is
// tolerated until the team is defeated, reset retires helpers, then turns
// benches/advances losing members while retaining the winner and root slots.
func TestTeamRosterCombatAndResetFlow(t *testing.T) {
	base := stockMatch(t, 99)
	request, options := base.Request, base.Options
	request.Mode = "Team Versus"
	for side := range request.Teams {
		request.Teams[side] = TeamSelection{Mode: Simul, Members: []Selection{request.Players[side], request.Players[side]}}
	}
	m, err := New(request, options)
	if err != nil {
		t.Fatal(err)
	}
	for _, p := range m.Actors() {
		program, err := cns.ParseNamed(strings.NewReader(`[Statedef -2]
[State -2, team mode and names]
type=VarSet
trigger1=1
v=55
value=TeamMode=Simul && P3Name!="" && P4Name!=""
[State -2, population]
type=VarSet
trigger1=1
v=56
value=NumPartner+10*NumEnemy
[State -2, home]
type=VarSet
trigger1=1
v=57
value=IsHomeTeam
[State -2, partner redirect]
type=VarSet
trigger1=1
v=58
value=Partner, TeamSide
`), "connected-team.cns")
		if err != nil {
			t.Fatal(err)
		}
		program.SetLanguageVersion("1.1")
		p.Character.StateFile.States[-2] = program.States[-2]
	}
	advanceUntil(t, m, Active, 650)
	if len(m.World.Roots) != 4 {
		t.Fatal("simul roots missing", len(m.World.Roots))
	}
	var identities [4]int32
	for slot, p := range m.ActivePlayers() {
		c := p.Character
		identities[slot] = c.EntityID
		if c.InputSlot != slot+1 || c.TeamSide != slot%2+1 || c.IntVars[55] != 1 || c.IntVars[56] != 21 || c.IntVars[57] != (1-slot%2) || c.IntVars[58] != c.TeamSide {
			t.Fatal("team CNS context/numbering", slot, c.IntVars[55:59], c.InputSlot, c.TeamSide)
		}
	}
	leader, partner := m.Players[0].Character, m.Teams[0].Members[1].Character
	partner.AddPower(500)
	leader.AddPower(200)
	if partner.GetPower() != 700 || leader.GetPower() != 700 {
		t.Fatal("simul power did not share the team bus")
	}
	partner.InputSource = func(slot, facing int) []string {
		distance, _ := partner.GetOpponentBodyDistance()
		if distance > 16 {
			return []string{"DirForward"}
		}
		if m.Tick%40 < 2 {
			return []string{"ButtonX"}
		}
		return nil
	}
	sawContact := false
	for tick := 0; tick < 500; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
		sawContact = sawContact || partner.MoveContact > 0
	}
	if !sawContact || m.Players[1].Character.Life >= m.Players[1].Character.MaxLife {
		t.Fatal("simul partner did not reach actual contact/damage")
	}
	leader.SetLife(0)
	leader.ChangeState(5150, 0, -1)
	if err := m.Update(); err != nil {
		t.Fatal(err)
	}
	if m.Phase != Active {
		t.Fatal("first simul KO prematurely ended the team", m.Phase)
	}
	if err := partner.CreateHelper(engine.HelperParameters{ID: 9, StateNo: 0, Type: "normal"}); err != nil {
		t.Fatal(err)
	}
	helperID := partner.Helpers[0].EntityID
	partner.SetLife(0)
	partner.ChangeState(5150, 0, -1)
	advanceUntil(t, m, PreOver, 3)
	if m.Winner != 1 || m.TimeOver {
		t.Fatal("team defeat did not publish opponent result", m.Winner, m.TimeOver)
	}
	advanceUntil(t, m, PreIntro, 600)
	if m.Round != 2 || len(m.World.Helpers) != 0 {
		t.Fatal("simul round did not reset")
	}
	if _, exists := m.World.Registry.Lookup(helperID); exists {
		t.Fatal("old team helper survived reset")
	}
	for slot, p := range m.ActivePlayers() {
		if p.Character.EntityID != identities[slot] || p.Character.Life != p.Character.MaxLife {
			t.Fatal("simul slot/reset lost identity or life", slot)
		}
	}
	// The configured lose-on-first-KO rule still uses the same team result path.
	m.Options.TeamLoseOnKO = true
	advanceUntil(t, m, Active, 650)
	m.Players[1].Character.SetLife(0)
	m.Players[1].Character.ChangeState(5150, 0, -1)
	advanceUntil(t, m, PreOver, 3)
	if m.Winner != 0 || !m.Teams[1].Members[1].Character.IsAlive() {
		t.Fatal("Team.LoseOnKO ignored surviving partner/result")
	}

	request.Mode = "Team Arcade"
	for side := range request.Teams {
		request.Teams[side].Mode = Turns
	}
	turns, err := New(request, options)
	if err != nil {
		t.Fatal(err)
	}
	advanceUntil(t, turns, Active, 650)
	if len(turns.World.Roots) != 2 || turns.World.HomeTeam != 2 {
		t.Fatal("turns benched member entered world or wrong home side")
	}
	first, loser := turns.Players[0].Character, turns.Players[1].Character
	first.SetLife(700)
	first.SetPower(900)
	rootIDs := [2]int32{first.EntityID, loser.EntityID}
	bench := turns.Teams[0].Members[1].Character
	loser.SetLife(0)
	loser.ChangeState(5150, 0, -1)
	advanceUntil(t, turns, Over, 400)
	if turns.Players[1].Character != loser || turns.Teams[1].Active != 0 {
		t.Fatal("turns replaced outgoing member during result presentation")
	}
	advanceUntil(t, turns, PreIntro, 400)
	if turns.Teams[1].Active != 1 || turns.Teams[0].Active != 0 || turns.Players[1].Character == loser || bench.Time != 0 {
		t.Fatal("turns roster did not advance only the loser")
	}
	if turns.Players[0].Character.Life <= 700 || turns.Players[0].Character.Life > turns.Players[0].Character.MaxLife || turns.Players[0].Character.GetPower() != 900 || turns.Players[0].Character.RoundsExisted != 1 || turns.Players[1].Character.RoundsExisted != 0 {
		t.Fatal("turns winner or new entrant retained wrong state")
	}
	for side, p := range turns.Players {
		if p.Character.EntityID != rootIDs[side] {
			t.Fatal("turns changed persistent player slot identity")
		}
	}
	advanceUntil(t, turns, Active, 650)
	turns.Players[1].Character.SetLife(0)
	turns.Players[1].Character.ChangeState(5150, 0, -1)
	advanceUntil(t, turns, Finished, 650)
	if turns.Winner != 0 || turns.Round != 2 || !turns.Teams[1].exhausted || turns.Teams[0].Active != 0 {
		t.Fatal("turns failed to terminate on roster exhaustion")
	}
}
