package character

import (
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
)

func fixtureCharacter(t *testing.T, source string) *Character {
	t.Helper()
	data, err := cns.Parse(strings.NewReader("[Statedef 7]\nphysics=N\n" + source))
	if err != nil {
		t.Fatal(err)
	}
	data.Data.Life = 1000
	c := NewCharacter(data, nil, nil)
	c.ChangeState(7, -1, -1)
	if c.RuntimeError != nil {
		t.Fatal(c.RuntimeError)
	}
	return c
}

func TestComboAndVictoryQuoteControllers(t *testing.T) {
	c := fixtureCharacter(t, "[State 7, idle]\ntype=Null\ntrigger1=1\n")
	c.HitCount = 2
	c.HitAdd(3)
	if c.HitCount != 2 || c.ComboCount != 3 {
		t.Fatal("HitAdd changed current attack hit count")
	}
	c.VictoryQuote(100)
	if c.VictoryQuoteIndex != -1 {
		t.Fatal("invalid quote must select randomly")
	}
	c.VictoryQuote(42)
	c.Parent = &Character{}
	c.VictoryQuote(3)
	if c.VictoryQuoteIndex != 42 {
		t.Fatal("helper changed victory quote")
	}
}

func TestHitPauseBoundary(t *testing.T) {
	c := fixtureCharacter(t, `[State 7, normal]
type=VarAdd
trigger1=1
v=0
value=1
[State 7, during pause]
type=VarAdd
trigger1=1
ignorehitpause=1
v=1
value=1
`)
	c.ShakeTime = 1
	c.HitTime = 5
	c.Update()
	if c.RuntimeError != nil {
		t.Fatal(c.RuntimeError)
	}
	if c.Time != 0 || c.ShakeTime != 0 || c.HitTime != 5 || c.IntVars[0] != 0 || c.IntVars[1] != 1 {
		t.Fatalf("last paused tick: time=%d shake=%d hit=%d vars=%v", c.Time, c.ShakeTime, c.HitTime, c.IntVars[:2])
	}
	c.Update()
	if c.Time != 1 || c.HitTime != 4 || c.IntVars[0] != 1 || c.IntVars[1] != 2 {
		t.Fatalf("first unpaused tick: time=%d hit=%d vars=%v", c.Time, c.HitTime, c.IntVars[:2])
	}
}

func TestEntryOnceDuringHitPause(t *testing.T) {
	c := fixtureCharacter(t, "ctrl=0\nanim=5\npoweradd=10\n[State 7, idle]\ntype=Null\ntrigger1=1\n")
	c.ChangeState(7, 1, 9)
	if c.Power != 20 {
		t.Fatalf("power after explicit reentry=%d", c.Power)
	}
	c.ShakeTime = 2
	for i := 0; i < 3; i++ {
		c.Update()
	}
	if c.RuntimeError != nil {
		t.Fatal(c.RuntimeError)
	}
	if c.Power != 20 || c.Anim != 9 || !c.Ctrl || c.Time != 1 {
		t.Fatalf("entry repeated or override lost: power=%d anim=%d ctrl=%v time=%d", c.Power, c.Anim, c.Ctrl, c.Time)
	}
}

func TestEntryCombatPersistence(t *testing.T) {
	c := fixtureCharacter(t, "hitdefpersist=1\nmovehitpersist=1\nhitcountpersist=1\n[State 7, idle]\ntype=Null\ntrigger1=1\n[Statedef 8]\n[State 8, idle]\ntype=Null\ntrigger1=1\n")
	c.HitDefActive = true
	c.CurrentHitDef.Damage = []int{5}
	c.HitGuard[2] = true
	c.MoveHit = 9
	c.MoveContact = 8
	c.MoveGuarded = 7
	c.HitCount = 6
	c.ChangeState(7, -1, -1)
	if !c.HitDefActive || !c.HitGuard[2] || c.MoveHit != 9 || c.MoveContact != 8 || c.MoveGuarded != 7 || c.HitCount != 6 {
		t.Fatal("entry lost persisted combat state")
	}
	c.ChangeState(8, -1, -1)
	if c.HitDefActive || len(c.CurrentHitDef.Damage) != 0 || len(c.HitGuard) != 0 || c.MoveHit != 0 || c.MoveContact != 0 || c.MoveGuarded != 0 || c.HitCount != 0 {
		t.Fatal("entry kept combat state without persistence flags")
	}
}

func TestRuntimeStateErrorsStopExecution(t *testing.T) {
	c := fixtureCharacter(t, "[State 7, idle]\ntype=Null\ntrigger1=1\n")
	c.ChangeState(999, -1, -1)
	if c.RuntimeError == nil {
		t.Fatal("missing state was silently accepted")
	}
	c.Update()
	if c.Time != 0 {
		t.Fatal("character continued after runtime failure")
	}
}

func TestContactCountersRespectHitPause(t *testing.T) {
	c := fixtureCharacter(t, "movetype=A\n[State 7, idle]\ntype=Null\ntrigger1=1\n")
	c.MoveHit = 1
	c.MoveContact = 1
	c.ShakeTime = 2
	c.Update()
	c.Update()
	if c.MoveHit != 1 || c.MoveContact != 1 {
		t.Fatal("contact counters advanced during hitpause")
	}
	c.Update()
	if c.MoveHit != 2 || c.MoveContact != 2 {
		t.Fatalf("contact counters did not resume: %d/%d", c.MoveHit, c.MoveContact)
	}
	c.HitGuard[2] = true
	c.MoveHitReset()
	if c.MoveHit != 0 || c.MoveContact != 0 || !c.HitGuard[2] {
		t.Fatal("MoveHitReset altered hit eligibility or kept hit status")
	}
}

func TestPosFreezeLastsOneTick(t *testing.T) {
	c := fixtureCharacter(t, "[State 7, freeze]\ntype=PosFreeze\ntrigger1=time=0\n")
	c.VelX = 1
	c.Update()
	if c.X != 0 {
		t.Fatal("position was not frozen")
	}
	c.Update()
	if c.X != 1 {
		t.Fatalf("position stayed frozen: %g", c.X)
	}
}

func TestGravityIsOneActivation(t *testing.T) {
	c := fixtureCharacter(t, "[State 7, idle]\ntype=Null\ntrigger1=1\n")
	c.SetGravity(true)
	velocity := c.VelY
	c.Update()
	c.Update()
	if velocity != c.StateFile.Movement.YAccel || c.VelY != velocity {
		t.Fatalf("gravity persisted in N physics: %g -> %g", velocity, c.VelY)
	}
}

func TestCollisionRequiresActiveHitDef(t *testing.T) {
	c := fixtureCharacter(t, "movetype=A\n[State 7, idle]\ntype=Null\ntrigger1=1\n")
	d := fixtureCharacter(t, "[State 7, idle]\ntype=Null\ntrigger1=1\n")
	box := air.Box{X1: -10, Y1: -10, X2: 10, Y2: 10}
	c.AirFile = &air.Data{Actions: map[int]*air.Action{0: {Elements: []air.Element{{Clsn1: []air.Box{box}}}}}}
	d.AirFile = &air.Data{Actions: map[int]*air.Action{0: {Elements: []air.Element{{Clsn2: []air.Box{box}}}}}}
	if CheckCollision(c, d) {
		t.Fatal("hitboxes without a HitDef could attack")
	}
	c.HitDef(map[string]string{"attr": "S, NA", "damage": "5"})
	if !CheckCollision(c, d) {
		t.Fatal("active overlapping hitboxes did not collide")
	}
}
