package character

import (
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/combat"
)

func TestExcludedCustomHitLeavesDirectAdaptersUnchanged(t *testing.T) {
	source := "[State 7, hold]\ntype=Null\ntrigger1=1\n"
	attacker, victim := fixtureCharacter(t, source), fixtureCharacter(t, source)
	attributes, err := combat.ParseAttributes("S,NA")
	if err != nil {
		t.Fatal(err)
	}
	if err := victim.SetHitOverride(combat.HitOverride{Attributes: attributes, StateNo: 7, Time: -1}); err != nil {
		t.Fatal(err)
	}
	victim.MoveType, victim.Time, victim.Life = "H", 9, 913
	victim.GetHitDamage, victim.ShakeTime = 27, 4
	constraint := &actorBinding{anchor: attacker, remaining: 10}
	victim.binding = constraint
	hit := cns.ParseHitDef(map[string]string{"attr": "S,NA", "p1stateno": "999", "damage": "100"})
	victim.ProcessHit(attacker, hit)
	attacker.CurrentHitDef, attacker.HitDefActive, attacker.MoveContact = hit, true, 3
	attacker.HandleHit(victim)
	if victim.RuntimeError != nil || victim.StateNo != 7 || victim.Time != 9 || victim.Life != 913 || victim.GetHitDamage != 27 || victim.ShakeTime != 4 || victim.binding != constraint {
		t.Fatal("excluded direct hit changed receiver metadata", victim.RuntimeError, victim.StateNo, victim.Time, victim.Life, victim.GetHitDamage, victim.ShakeTime)
	}
	if attacker.RuntimeError != nil || attacker.MoveContact != 3 || len(attacker.HitGuard) != 0 {
		t.Fatal("excluded direct hit changed attacker contact", attacker.RuntimeError, attacker.MoveContact, attacker.HitGuard)
	}
}

func TestContactContinuationDoesNotRepeatNegativeStatesOrTimeZero(t *testing.T) {
	file, err := cns.Parse(strings.NewReader(`[Data]
life=1000
[Statedef 0]
movetype=A
physics=N
anim=0
[State 0, attack]
type=HitDef
trigger1=Time=0
attr=S,NA
damage=25
pausetime=8,8
ground.velocity=0,0
ground.hittime=11
[Statedef 5000]
movetype=H
physics=N
anim=0
[State 5000, entry effect]
type=VarAdd
trigger1=Time=0
v=31
value=1
[Statedef -2]
[State -2, frame count]
type=VarAdd
trigger1=1
ignorehitpause=1
v=30
value=1
`))
	if err != nil {
		t.Fatal(err)
	}
	box := air.Box{X1: -20, Y1: -40, X2: 20, Y2: 0}
	actions := &air.Data{Actions: map[int]*air.Action{0: {Elements: []air.Element{{Time: -1, Clsn1: []air.Box{box}, Clsn2: []air.Box{box}}}}}}
	players := []*Character{NewCharacter(file, actions, nil), NewCharacter(file, actions, nil)}
	for slot, c := range players {
		c.InputSlot = slot + 1
		c.Opponents = []*Character{players[1-slot]}
		c.ChangeState(0, -1, -1)
	}
	if err := TickCharacters(players); err != nil {
		t.Fatal(err)
	}
	for _, c := range players {
		if c.Life != 1000 {
			t.Fatal("new HitDef connected before next tick")
		}
	}
	if err := TickCharacters(players); err != nil {
		t.Fatal(err)
	}
	for _, c := range players {
		if c.IntVars[30] != 2 || c.IntVars[31] != 1 || c.Time != 1 || c.Life != 975 {
			t.Fatalf("contact duplicated/skipped phases: negative=%d entry=%d time=%d life=%d", c.IntVars[30], c.IntVars[31], c.Time, c.Life)
		}
	}
	if err := TickCharacters(players); err != nil {
		t.Fatal(err)
	}
	for _, c := range players {
		if c.IntVars[30] != 3 || c.IntVars[31] != 1 || c.Time != 2 {
			t.Fatal("hitpause repeated entry or negative pass", c.IntVars[30], c.IntVars[31], c.Time)
		}
	}
}
