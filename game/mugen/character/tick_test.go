package character

import (
	"fmt"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/combat"
	"github.com/kazzmir/paintown/game/mugen/coord"
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
life=2000
intpersistindex=0
intpersistindex=[3,4]
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
type=S
movetype=H
physics=N
anim=0
[State 5000, entry effect]
type=VarAdd
trigger1=Time=0
v=31
value=1
 =======
[State 5000
type=StateTypeSet
trigger1=AnimElem=1,[0,3] && AnimElem>=1 && Time=0
type=A
[Statedef -2]
[State -2, legacy projectile time alias]
type=VarSet
trigger1=1
ignorehitpause=1
v=29
value=ProjHitTime1 - ProjHitTime(1)
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
		c.lastProjectileEvent = &projectileEvent{id: 1, age: 5, kind: "hit"}
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
		if c.IntVars[29] != 0 || c.IntVars[30] != 2 || c.IntVars[31] != 1 || c.Time != 1 || c.Life != 975 || c.StateType != "A" {
			t.Fatalf("contact/legacy reaction phases: negative=%d entry=%d time=%d life=%d type=%s", c.IntVars[30], c.IntVars[31], c.Time, c.Life, c.StateType)
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
	crossoverFacingFlow(t)
}

// Continue combat through a locked move, landing into an immediate command,
// held walking, NoAutoTurn release and a controlled custom pose. These heading,
// animation and entry-velocity observations come from the supplied beta.
func crossoverFacingFlow(t *testing.T) {
	t.Helper()
	source := `[Data]
life=1000
[Statedef -2]
[State -2]
type=PlayerPush
trigger1=1
value=0
[State -2]
type=ScreenBound
trigger1=1
value=0
movecamera=0,0
[State -2]
type=AssertSpecial
trigger1=StateNo!=20
flag=NoWalk
[Statedef -1]
[State -1, command reads old heading]
type=VarSet
trigger1=StateNo=52 && Time=1
v=2
value=Facing
[State -1, command reads old animation]
type=VarSet
trigger1=StateNo=52 && Time=1
v=3
value=Anim
[State -1, immediate landing attack]
type=ChangeState
trigger1=StateNo=52 && Time=1
value=300
[Statedef 0]
type=S
physics=S
ctrl=1
anim=0
[State 0]
type=Null
trigger1=1
[Statedef 9000]
type=S
physics=N
ctrl=0
anim=0
[State 9000]
type=PosSet
trigger1=1
x=Const240p(-120)
y=Const240p(-100)
`
	for _, state := range []int{200, 52, 300, 20, 1000} {
		kind, ctrl, animation, next := "A", 0, 200, 52
		switch state {
		case 52:
			kind, ctrl, animation, next = "I", 1, 47, 300
		case 300:
			next = 20
		case 20:
			kind, ctrl, animation, next = "I", 1, 20, 1000
		case 1000:
			kind, ctrl, animation, next = "I", 1, 0, 9999
		}
		velocity := 0
		if state == 300 {
			velocity = 4
		}
		source += fmt.Sprintf("[Statedef %d]\ntype=S\nphysics=S\nmovetype=%s\nctrl=%d\nanim=%d\nvelset=%d,0\n", state, kind, ctrl, animation, velocity)
		if state == 20 || state == 1000 {
			source += fmt.Sprintf("[State %d]\ntype=Turn\ntrigger1=Time=0 && Facing<0\n", state)
		}
		if state == 1000 {
			source += "[State 1000]\ntype=AssertSpecial\ntrigger1=1\nflag=NoWalk\n"
		}
		if state == 20 {
			source += `[State 20]
type=AssertSpecial
trigger1=Time<10
flag=NoAutoTurn
[State 20, physical right remains right after turning]
type=VelSet
trigger1=1
x=ifelse(command="holdfwd",2,-2)
[State 20, input and native state clock]
type=VarSet
trigger1=Time=12
fv=1
value=Vel X
`
		}
		at, slot := 0, 0
		switch state {
		case 200:
			slot = -1
		case 300:
			slot = 4
		case 20:
			at, slot = 1, 6
		case 1000:
			at, slot = 12, 10
		}
		if slot >= 0 {
			for offset, expression := range []string{"Facing", "Anim"} {
				source += fmt.Sprintf("[State %d]\ntype=VarSet\ntrigger1=Time=%d\nv=%d\nvalue=%s\n", state, at, slot+offset, expression)
			}
		}
		if state == 300 {
			source += "[State 300]\ntype=VarSet\ntrigger1=Time=0\nfv=0\nvalue=Vel X\n"
		}
		if state == 20 {
			for offset, expression := range []string{"Facing", "Anim"} {
				source += fmt.Sprintf("[State 20]\ntype=VarSet\ntrigger1=Time=12\nv=%d\nvalue=%s\n", 8+offset, expression)
			}
		}
		source += fmt.Sprintf("[State %d]\ntype=ChangeState\ntrigger1=Time=20\nvalue=%d\n", state, next)
	}
	source += "[Statedef 9999]\ntype=S\nphysics=N\nctrl=0\n[State 9999]\ntype=Null\ntrigger1=1\n"
	file, err := cns.Parse(strings.NewReader(source))
	if err != nil {
		t.Fatal(err)
	}
	commands, err := cmd.Parse(strings.NewReader("[Command]\nname=\"holdfwd\"\ncommand=/$F\n[Command]\nname=\"holdback\"\ncommand=/$B\n"))
	if err != nil {
		t.Fatal(err)
	}
	frames := &air.Data{Actions: map[int]*air.Action{}}
	for _, number := range []int{0, 5, 20, 47, 200} {
		frames.Actions[number] = &air.Action{ID: number, LoopStart: -1, Elements: []air.Element{{Time: -1}}}
	}
	a, b := NewCharacter(file, frames, commands), NewCharacter(file, frames, nil)
	a.Commands = NewCommandBuffer(120)
	a.InputSlot, a.TeamSide, b.InputSlot, b.TeamSide = 1, 1, 2, 2
	b.LocalCoord = coord.Space{Width: 1280, Height: 720}
	b.X, b.Y = -480, -400
	a.Opponents, b.Opponents = []*Character{b}, []*Character{a}
	a.ChangeState(200, -1, -1)
	b.ChangeState(9000, -1, -1)
	a.InputSource = func(_ int, facing int) []string {
		if facing > 0 {
			return []string{"DirForward"}
		}
		return []string{"DirBack"}
	}
	ticks := 0
	for ; ticks < 120 && a.StateNo != 9999; ticks++ {
		if err := TickCharacters([]*Character{a, b}); err != nil {
			t.Fatal(err)
		}
	}
	want := []int{1, 47, 1, 47, -1, 200, 1, 20, -1, 5, 1, 0}
	for index, value := range want {
		if a.IntVars[index] != value {
			t.Fatalf("native crossover observation %d=%d want %d (state %d time %d)", index, a.IntVars[index], value, a.StateNo, a.Time)
		}
	}
	if a.StateNo != 9999 || a.FloatVars[0] != 4 || a.FloatVars[1] != -2 || len(a.Commands.History) != ticks {
		t.Fatal("crossover entry, physical input or clocks", a.StateNo, a.FloatVars[:2], len(a.Commands.History), ticks)
	}
	// The stage's explicit disabled-turn setting also covers command handoff.
	a.World.StageNoAutoTurn = true
	a.Facing = 1
	a.ChangeState(52, -1, -1)
	for tick := 0; tick < 2; tick++ {
		if err := TickCharacters([]*Character{a, b}); err != nil {
			t.Fatal(err)
		}
	}
	if a.StateNo != 300 || a.Facing != 1 || a.IntVars[4] != 1 {
		t.Fatal("stage-disabled turning changed command handoff", a.StateNo, a.Facing, a.IntVars[4])
	}
}
