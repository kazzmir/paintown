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
	request.Teams[1].Members = append(request.Teams[1].Members, request.Teams[1].Members...)
	options.RoundTime = 40
	turns, err := New(request, options)
	if err != nil {
		t.Fatal(err)
	}
	freeze, err := cns.ParseNamed(strings.NewReader(`[Statedef -2]
[State -2, freeze display clock]
type=AssertSpecial
trigger1=RoundNo!=2
flag=timerfreeze
`), "connected-turns.cns")
	if err != nil {
		t.Fatal(err)
	}
	for _, team := range turns.Teams {
		for _, p := range team.Members {
			p.Character.StateFile.States[-2] = freeze.States[-2]
		}
	}
	advanceUntil(t, turns, Active, 650)
	if len(turns.World.Roots) != 2 || turns.World.HomeTeam != 2 {
		t.Fatal("turns benched member entered world or wrong home side")
	}
	first, loser := turns.Players[0].Character, turns.Players[1].Character
	if first.MaxLife != 1000 || loser.MaxLife != 500 || loser.Life != 500 {
		t.Fatal("unequal Turns roster health", first.MaxLife, loser.MaxLife, loser.Life)
	}

	first.SetLife(400)
	first.SetPower(900)
	rootIDs := [2]int32{first.EntityID, loser.EntityID}
	bench := turns.Teams[0].Members[1].Character
	loser.SetLife(0)
	loser.ChangeState(5150, 0, -1)
	advanceUntil(t, turns, Over, 400)
	if turns.Players[1].Character != loser || turns.Teams[1].Active != 0 {
		t.Fatal("turns replaced outgoing member during result presentation")
	}
	for !turns.winPoseStarted {
		if err := turns.Update(); err != nil {
			t.Fatal(err)
		}
	}
	if first.Life != 798 || turns.TimeLeft != 40 {
		t.Fatal("native Turns recovery must precede win-pose CNS", first.Life, turns.TimeLeft)
	}
	advanceUntil(t, turns, PreIntro, 400)
	if turns.Teams[1].Active != 1 || turns.Teams[0].Active != 0 || turns.Players[1].Character == loser || bench.Time != 0 {
		t.Fatal("turns roster did not advance only the loser")
	}
	if turns.Players[0].Character.Life != 798 || turns.Players[0].Character.GetPower() != 900 || turns.Players[0].Character.RoundsExisted != 1 || turns.Players[1].Character.RoundsExisted != 0 {
		t.Fatal("turns winner or new entrant retained wrong state")
	}
	for side, p := range turns.Players {
		if p.Character.EntityID != rootIDs[side] {
			t.Fatal("turns changed persistent player slot identity")
		}
	}
	// Continue the same roster through a shorter-time win, then reverse the
	// winner against an already damaged entrant. These original-beta values
	// span time, starting opponent health, max-life scaling and carry/reset.
	win := func(side, life, elapsed, want int, next Phase) {
		t.Helper()
		advanceUntil(t, turns, Active, 650)
		for turns.elapsedFightTicks < elapsed {
			if err := turns.Update(); err != nil {
				t.Fatal(err)
			}
		}
		winner, victim := turns.Players[side].Character, turns.Players[1-side].Character
		winner.SetLife(life)
		victim.SetLife(0)
		victim.ChangeState(5150, 0, -1)
		advanceUntil(t, turns, Over, 400)
		for !turns.winPoseStarted {
			if err := turns.Update(); err != nil {
				t.Fatal(err)
			}
		}
		if winner.Life != want {
			t.Fatal("native Turns win-pose recovery", turns.Round, winner.Life, want)
		}
		advanceUntil(t, turns, next, 400)
	}
	win(0, 400, 1200, 744, PreIntro)
	if turns.Players[1].Character.MaxLife != 500 || turns.Players[1].Character.Life != 500 {
		t.Fatal("incoming health changed with remaining roster")
	}
	win(1, 100, 180, 275, PreIntro)
	if turns.Teams[0].Active != 1 || turns.Players[1].Character.Life != 275 {
		t.Fatal("reversed winner did not carry recovered health")
	}
	win(0, 900, 180, 1000, PreIntro)
	// A completed roster has no next round and grants no recovery.
	win(0, 400, 30, 400, Finished)
	if turns.Winner != 0 || turns.Round != 5 || !turns.Teams[1].exhausted || turns.Teams[0].Active != 1 {
		t.Fatal("turns failed to terminate on roster exhaustion")
	}
	// A complete mixed-team bout follows the original beta's alternating
	// results: Simul gets two rounds against four Turns members, whose max
	// life remains750. Two defeated Turns entrants must not end the match.
	request.Teams[0].Mode = Simul
	options.Team1Vs2Life = 150
	mixed, err := New(request, options)
	if err != nil {
		t.Fatal(err)
	}
	for _, team := range mixed.Teams {
		for _, p := range team.Members {
			p.Character.StateFile.States[-2] = freeze.States[-2]
		}
	}
	advanceUntil(t, mixed, Active, 650)
	for _, p := range mixed.SidePlayers(0) {
		if p.Character.MaxLife != 1000 {
			t.Fatal("mixed Simul maximum life", p.Character.MaxLife)
		}
		p.Character.SetLife(400)
	}
	if mixed.Players[1].Character.MaxLife != 750 {
		t.Fatal("mixed Turns normalization", mixed.Players[1].Character.MaxLife)
	}
	defeat := func(side int, next Phase) {
		t.Helper()
		advanceUntil(t, mixed, Active, 650)
		for mixed.elapsedFightTicks < 60 {
			if err := mixed.Update(); err != nil {
				t.Fatal(err)
			}
		}
		for _, p := range mixed.SidePlayers(side) {
			p.Character.SetLife(0)
			p.Character.ChangeState(5150, 0, -1)
		}
		advanceUntil(t, mixed, next, 650)
	}
	defeat(1, PreIntro)
	for _, p := range mixed.SidePlayers(0) {
		if p.Character.Life != 797 || p.Character.MaxLife != 1000 {
			t.Fatal("mixed winner recovery/carry", p.Character.Life, p.Character.MaxLife)
		}
	}
	defeat(0, PreIntro)
	if mixed.Round != 3 || mixed.Teams[0].exhausted || mixed.Teams[1].Active != 1 {
		t.Fatal("first Simul loss exhausted the team")
	}
	for _, p := range mixed.SidePlayers(0) {
		if p.Character.Life != 1000 {
			t.Fatal("defeated Simul member did not replenish for its second round")
		}
	}
	defeat(1, PreIntro)
	if mixed.Wins != [2]int{2, 1} || mixed.Teams[1].Active != 2 || mixed.Players[1].Character.MaxLife != 750 {
		t.Fatal("mixed match ended/renormalized after two Turns defeats")
	}
	defeat(0, Finished)
	if mixed.Winner != 1 || mixed.Round != 4 || !mixed.Teams[0].exhausted || mixed.Teams[1].exhausted {
		t.Fatal("second Simul loss did not end the mixed match")
	}

	// A Single fighter likewise retains its round allowance. Its opposing
	// four-member Turns roster receives half life, without boosting Single.
	request.Teams[0] = TeamSelection{Mode: Single, Members: request.Teams[0].Members[:1]}
	single, err := New(request, options)
	if err != nil {
		t.Fatal(err)
	}
	if single.Players[0].Character.MaxLife != 1000 || single.Players[1].Character.MaxLife != 500 {
		t.Fatal("Single/Turns life balance")
	}
	for loss := 0; loss < 2; loss++ {
		advanceUntil(t, single, Active, 650)
		single.Players[0].Character.SetLife(0)
		single.Players[0].Character.ChangeState(5150, 0, -1)
		if loss == 0 {
			advanceUntil(t, single, PreIntro, 650)
		} else {
			advanceUntil(t, single, Finished, 650)
		}
	}
	if single.Round != 2 || !single.Teams[0].exhausted || single.Teams[1].Active != 0 {
		t.Fatal("Single did not retain two rounds against Turns")
	}

	// The loading-time controls rotate only the remaining Turns roster. Follow
	// one team from its initial half swap through each reserve's elimination.
	request.Teams[0] = TeamSelection{Mode: Turns}
	for palette := 1; palette <= 4; palette++ {
		selected := request.Players[0]
		selected.Palette = palette
		request.Teams[0].Members = append(request.Teams[0].Members, selected)
	}
	held := "DirUp"
	options.RosterInput = func(playerID, facing int) []string {
		if playerID == 1 {
			return []string{held}
		}
		return nil
	}
	ordered, err := New(request, options)
	if err != nil {
		t.Fatal(err)
	}
	checkOrder := func(want [4]int, active int) {
		t.Helper()
		for index, selected := range ordered.Request.Teams[0].Members {
			if selected.Palette != want[index] {
				t.Fatal("Turns loading order", ordered.Round, index, selected.Palette, want)
			}
		}
		if ordered.Teams[0].Active != active || ordered.Request.Players[0].Palette != want[active] || ordered.Players[0].Character.RoundsExisted != 0 {
			t.Fatal("Turns entrant/selection not aligned", ordered.Teams[0].Active, ordered.Request.Players[0])
		}
	}
	checkOrder([4]int{3, 4, 1, 2}, 0)
	for index, selected := range request.Teams[0].Members {
		if selected.Palette != index+1 {
			t.Fatal("rotation mutated caller roster")
		}
	}
	orderedID := ordered.Players[0].Character.EntityID
	for loss, order := range [][4]int{{3, 1, 2, 4}, {3, 1, 4, 2}, {3, 1, 4, 2}} {
		held = []string{"DirForward", "DirBack", "DirUp"}[loss]
		advanceUntil(t, ordered, Active, 650)
		ordered.Players[0].Character.SetLife(0)
		ordered.Players[0].Character.ChangeState(5150, 0, -1)
		advanceUntil(t, ordered, PreIntro, 650)
		checkOrder(order, loss+1)
		if ordered.Players[0].Character.EntityID != orderedID || ordered.Teams[1].Active != 0 {
			t.Fatal("rotation changed root slot or opposing winner")
		}
	}
	advanceUntil(t, ordered, Active, 650)
	ordered.Players[0].Character.SetLife(0)
	ordered.Players[0].Character.ChangeState(5150, 0, -1)
	advanceUntil(t, ordered, Finished, 650)
	if ordered.Round != 4 || !ordered.Teams[0].exhausted {
		t.Fatal("rotation recycled a defeated member")
	}

	// Loading a new Survival opponent resumes the existing winner, even when
	// a direction remains held and there are still four available members.
	request.Mode, request.RoundStart = "Survival", 2
	request.Carry[0] = []*RoundCarry{{Life: 500, RoundsExisted: 0}, nil, nil, nil}
	held = "DirUp"
	resumed, err := New(request, options)
	if err != nil {
		t.Fatal(err)
	}
	if resumed.Request.Players[0].Palette != 1 || resumed.Players[0].Character.Life != 500 || resumed.Players[0].Character.RoundsExisted != 1 {
		t.Fatal("continuing Survival winner was rotated away")
	}
}
