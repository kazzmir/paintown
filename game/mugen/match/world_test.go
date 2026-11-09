package match

import (
	"fmt"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

func TestRoundResetRebindsRootsAndRetiresHelperIDs(t *testing.T) {
	m := stockMatch(t, 99)
	root := m.Players[0].Character
	root.Helper(map[string]string{"stateno": "0", "id": "777"})
	if root.RuntimeError != nil || len(root.Helpers) != 1 {
		t.Fatal("helper setup", root.RuntimeError)
	}
	rootID, helperID := root.EntityID, root.Helpers[0].EntityID
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	if _, ok := m.World.Registry.Lookup(helperID); ok {
		t.Fatal("round reset retained helper ID")
	}
	fresh := m.Players[0].Character
	if fresh == root || fresh.EntityID != rootID {
		t.Fatal("round state did not replace persistent root identity")
	}
	if who, ok := m.World.Registry.Lookup(rootID); !ok || who != fresh {
		t.Fatal("root lookup retained old round state")
	}
	fresh.Helper(map[string]string{"stateno": "0", "id": "777"})
	if fresh.RuntimeError != nil || len(fresh.Helpers) != 1 || fresh.Helpers[0].EntityID <= helperID {
		t.Fatal("new round reused a retired helper ID", fresh.RuntimeError)
	}
	helperPauseBindingFlow(t, m)
}

// Continue the entity lifecycle through moving-owner pauses, self binding,
// bound body contact, retirement, a hit-paused throw and round replacement. Beta
// observations use one shared asset distribution and no retained run bank.
func helperPauseBindingFlow(t *testing.T, m *Match) {
	t.Helper()
	program := `[Statedef 5900]
[State 5900, enter]
type=ChangeState
trigger1=1
value=0
[Statedef 0]
type=S
physics=N
anim=9990
ctrl=0
[State 0, begin]
type=ChangeState
trigger1=RoundState=2 && Ctrl
value=ifelse(TeamSide=1,700,701)
[Statedef 700]
type=S
movetype=I
physics=N
anim=9990
ctrl=0
velset=0,0
`
	for phase, tick := range []int{25, 45, 80} {
		expressions := []string{"Helper(55), Pos X", "Helper(55), Time", "Helper(56), Pos X", "Helper(56), Time"}
		if phase == 2 {
			expressions = []string{"Helper(55), Pos X", "EnemyNear, Pos X", "Helper(55), Time", "Pos X"}
		}
		for index, expression := range expressions {
			program += fmt.Sprintf("[State 700, snapshot %d]\ntype=VarSet\ntrigger1=Time=%d\nfv=%d\nvalue=%s\n", phase*4+index, tick, phase*4+index, expression)
		}
	}
	for side, offset := range []int{10, -10} {
		allowance := 2
		if side == 1 {
			allowance = 0
		}
		program += fmt.Sprintf(`[State 700, helper %d]
type=Helper
trigger1=Time=0
helpertype=normal
name="Bound helper"
id=%d
stateno=%d
postype=p1
pos=Const240p(%d),0
pausemovetime=%d
supermovetime=0
ownpal=1
`, side, 55+side, 710+side, offset, allowance)
	}
	program += `[State 700, pause]
type=Pause
trigger1=Time=20 || Time=40
time=6
movetime=6
[State 700, placement]
type=PosSet
trigger1=1
x=Const240p(ifelse(Time<60,Time,0))
y=0
[State 700, free camera]
type=ScreenBound
trigger1=1
value=0
movecamera=0,0
[State 700, no push]
type=PlayerPush
trigger1=1
value=0
[Statedef 701]
type=S
movetype=I
physics=N
anim=9990
ctrl=0
velset=0,0
[State 701, placement]
type=PosSet
trigger1=(EnemyNear,Time)>=60
x=Const240p(30)
y=0
[State 701, fixed camera]
type=ScreenBound
trigger1=1
value=1
movecamera=0,0
[State 701, push]
type=PlayerPush
trigger1=1
value=(EnemyNear,Time)>=60
`
	for side, offset := range []int{10, -10} {
		push := "(Root,Time)>=60"
		if side == 1 {
			push = "0"
		}
		program += fmt.Sprintf(`[Statedef %d]
type=S
movetype=I
physics=N
anim=9990
ctrl=0
velset=Const240p(3),0
[State %d, bind]
type=BindToRoot
trigger1=Time=0
time=-1
pos=Const240p(%d),0
facing=1
[State %d, free camera]
type=ScreenBound
trigger1=1
value=0
movecamera=0,0
[State %d, push]
type=PlayerPush
trigger1=1
value=%s
`, 710+side, 710+side, offset, 710+side, 710+side, push)
	}
	program += `[Statedef 720]
type=S
movetype=A
physics=N
anim=9991
ctrl=0
velset=0,0
[State 720, grab]
type=HitDef
trigger1=Time=0
attr=S,NA
hitflag=MAF
guardflag=
damage=0,0
pausetime=8,2
getpower=0,0
givepower=0,0
ground.type=High
ground.hittime=1000
ground.velocity=0,0
sparkno=-1
guard.sparkno=-1
hitsound=-1
guardsound=-1
p1stateno=722
p2stateno=730
id=1234
[Statedef 730]
type=S
movetype=H
physics=N
anim=9990
ctrl=0
velset=Const240p(3),0
[State 730, recover own states]
type=SelfState
trigger1=(EnemyNear,Time)>=12
value=740
ctrl=1
[Statedef 740]
type=S
movetype=I
physics=N
anim=9990
ctrl=1
velset=0,0
[State 740, recovered]
type=Null
trigger1=1
[Statedef 5000]
type=S
movetype=H
physics=N
anim=9990
[State 5000, wait for custom destination]
type=Null
trigger1=1
[Statedef 722]
type=S
movetype=A
physics=N
anim=9990
ctrl=0
velset=Const240p(2),0
[State 722, throw binding]
type=TargetBind
trigger1=Var(0)=0
ignorehitpause=1
time=6
pos=Const240p(30),0
`
	for phase, frame := range []int{3, 7, 10} {
		for index, expression := range []string{"Time", "Target,Time", "Target,Vel X", "(Target,Pos X)-Pos X"} {
			program += fmt.Sprintf("[State 722, throw snapshot]\ntype=VarSet\ntrigger1=Var(0)=%d\nignorehitpause=1\nfv=%d\nvalue=%s\n", frame, 12+phase*4+index, expression)
		}
	}
	program += `[State 722, elapsed passes]
type=VarAdd
trigger1=1
ignorehitpause=1
v=0
value=1
[Statedef -2]
[State -2, no push]
type=PlayerPush
trigger1=1
ignorehitpause=1
value=0
[State -2, free camera]
type=ScreenBound
trigger1=1
ignorehitpause=1
value=0
movecamera=0,0
[Statedef 750]
type=S
movetype=A
physics=N
anim=9991
ctrl=0
velset=0,0
[State 750, custom-state contact]
type=HitDef
trigger1=Time=0
attr=S,NA
hitflag=MAF
guardflag=
damage=0,0
pausetime=0,0
ground.hittime=1000
ground.velocity=0,0
sparkno=-1
guard.sparkno=-1
hitsound=-1
guardsound=-1
p1stateno=752
p2stateno=760
[Statedef 752]
type=S
movetype=I
physics=N
anim=9990
ctrl=0
velset=0,0
[State 752, hold]
type=Null
trigger1=1
[Statedef 760]
type=S
movetype=H
physics=N
anim=9990
ctrl=0
velset=0,0
[State 760, unavailable action keeps its parameter gate]
type=Explod
trigger1=Time=0
anim=9998
pos=(var(56):=99),0
[State 760, effect in the borrowed coordinate profile]
type=Explod
trigger1=Time=0
id=77
anim=9992
postype=p1
pos=Const240p(16),Const240p(-32)
vel=Const240p(2),Const240p(-1)
accel=0,Const240p(.5)
removetime=30
[State 760, helper in borrowed position units]
type=Helper
trigger1=Time=0
id=88
stateno=861
pos=12,-24
size.height=40
[State 760, another helper retains own size units]
type=Helper
trigger1=Time=3
id=89
stateno=862
pos=20,-40
size.xscale=2
size.head.pos=10,-20
[State 760, freeze the effect and receiver together]
type=Pause
trigger1=Time=3
time=4
movetime=0
[State 760, own-state recovery retains the effect's creation units]
type=SelfState
trigger1=Time=8
value=740
ctrl=1
`
	program += targetAdmissionProgram
	program += helperAdmissionProgram
	for _, phase := range []struct {
		state, slot int
		expressions []string
	}{
		{861, 40, []string{"Pos X-(Parent,Pos X)", "Pos Y-(Parent,Pos Y)", "Const(size.height)", "Const240p(320)", "Const(size.xscale)*100", "NumHelper"}},
		{862, 46, []string{"Pos X-(Parent,Pos X)", "Pos Y-(Parent,Pos Y)", "Const(size.head.pos.x)", "Const(size.head.pos.y)", "Const(size.xscale)*100", "Const240p(320)"}},
	} {
		program += fmt.Sprintf("[Statedef %d]\ntype=S\nmovetype=I\nphysics=N\nctrl=0\nanim=9990\n", phase.state)
		for index, expression := range phase.expressions {
			program += fmt.Sprintf("[State helper observation]\ntype=ParentVarSet\ntrigger1=Time=0\nv=%d\nvalue=%s\n", phase.slot+index, expression)
		}
		program += "[State helper retirement]\ntype=DestroySelf\ntrigger1=Time=25\n"
	}
	for _, p := range m.Players {
		file, err := cns.ParseNamed(strings.NewReader(program), "connected-helper-pause-bind.cns")
		if err != nil {
			t.Fatal(err)
		}
		file.SetLanguageVersion("1.1")
		old := p.Character.StateFile
		file.Data, file.Size, file.Movement, file.Velocity, file.ConstantFields = old.Data, old.Size, old.Movement, old.Velocity, old.ConstantFields
		units := float64(p.LocalCoord.Width) / 320
		box := air.Box{X1: int(-10 * units), Y1: int(-60 * units), X2: int(10 * units)}
		attack := air.Box{X1: int(-30 * units), Y1: int(-60 * units), X2: int(30 * units)}
		p.AIR = &air.Data{Actions: map[int]*air.Action{
			9990: {ID: 9990, LoopStart: -1, Elements: []air.Element{{Group: 0, Image: 0, Time: -1, Clsn2: []air.Box{box}}}},
			9991: {ID: 9991, LoopStart: -1, Elements: []air.Element{{Group: 0, Image: 0, Time: -1, Clsn1: []air.Box{attack}, Clsn2: []air.Box{box}}}},
			9992: {ID: 9992, LoopStart: -1, Elements: []air.Element{{Group: 191, Image: 0, Time: -1}}},
		}}
		p.Character.StateFile, p.CMD = file, nil
	}
	m.Stage.Camera.StartX, m.Stage.Camera.StartY = 0, 0
	m.Stage.Camera.BoundLeft, m.Stage.Camera.BoundRight, m.Stage.Camera.BoundHigh, m.Stage.Camera.BoundLow = 0, 0, 0, 0
	m.Stage.Camera.VerticalFollow = 0
	m.Stage.StageInfo.AutoTurn = false
	m.Stage.PlayerInfo.P1StartX, m.Stage.PlayerInfo.P2StartX = 0, -100
	m.Round++
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	root := m.Players[0].Character
	reach := func(time int) {
		t.Helper()
		for tick := 0; tick < 750 && (root.StateNo != 700 || root.Time <= time); tick++ {
			if err := m.Update(); err != nil {
				t.Fatal(err)
			}
		}
		if root.StateNo != 700 || root.Time <= time {
			t.Fatal("helper lifecycle did not reach the requested phase", root.StateNo, root.Time)
		}
	}
	reach(90)
	// Both pauses let the owner move. The helper's two permitted paused ticks
	// are spent during the first pause and are not replenished by the second.
	want := []float64{32, 23, 10, 21, 50, 37, 30, 35, 4, 36, 70, 0}
	for index, expected := range want {
		if root.FloatVars[index] != expected {
			t.Fatalf("native moving-owner/helper binding phase %d slot %d: got %v want %v", index/4, index%4, root.FloatVars[index], expected)
		}
	}
	find := func(id int) *character.Character {
		t.Helper()
		for _, helper := range m.World.Helpers {
			if helper.HelperID == id {
				return helper
			}
		}
		t.Fatalf("missing helper %d", id)
		return nil
	}
	first, second := find(55), find(56)
	firstID, secondID, rootID := first.EntityID, second.EntityID, root.EntityID
	if err := first.DestroyHelper(engine.DestroySelfParameters{}); err != nil {
		t.Fatal(err)
	}
	if err := m.Update(); err != nil {
		t.Fatal(err)
	}
	if _, live := m.World.Registry.Lookup(firstID); live || m.Players[1].Character.X*320/float64(m.Players[1].Character.LocalCoordinateWidth()) != 30 {
		t.Fatal("retired bound helper retained identity or body push")
	}
	// Continue into a throw. The receiver's custom-state clock runs while
	// the attacker is hit-paused, so its finite binding can expire first.
	// Beta transfers zero velocity from the paused anchor, including over
	// the receiver's authored entry velocity, and releases it without drift.
	root.X, root.Y = 0, 0
	root.ChangeState(720, -1, -1)
	target := m.Players[1].Character
	target.X, target.Y = 20*float64(target.LocalCoordinateWidth())/320, 0
	target.ChangeState(701, -1, -1)
	for tick := 0; tick < 35; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
	}
	for index, expected := range []float64{0, 3, 0, 30, 0, 7, 0, 30, 2, 10, 0, 26} {
		if root.FloatVars[12+index] != expected {
			t.Fatalf("native hit-paused throw phase %d slot %d: got %v want %v", index/4, index%4, root.FloatVars[12+index], expected)
		}
	}
	if target.StateNo != 740 || !target.Ctrl || target.VelX != 0 || target.HitPauseTime != 0 {
		t.Fatal("throw did not release into the receiver's own recovery", target.StateNo, target.Ctrl, target.VelX, target.HitPauseTime)
	}
	// Continue through absent targets, a new grab, unmatched/matched IDs,
	// TargetDrop and helper-parent writes before recovering both actors.
	root.X, target.X = 0, 20*float64(target.LocalCoordinateWidth())/320
	target.ChangeState(701, -1, -1)
	root.ChangeState(810, -1, -1)
	for tick := 0; tick < 28; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
	}
	for index, expected := range []int{0, 0, 0, 0, 0, 9999, 0, 9999, 0, 1234, 2, 2} {
		if got := root.GetVar(40 + index); got != expected {
			t.Fatalf("native target/parent admission slot %d: got %d want %d", index, got, expected)
		}
	}
	if root.GetVar(52) != 15 || root.StateNo != 740 || target.StateNo != 740 || !root.Ctrl || !target.Ctrl || root.HasTargets(-1) {
		t.Fatal("target/parent lifecycle did not recover or release its relationships", root.StateNo, target.StateNo, root.GetVar(52))
	}
	for _, helper := range m.World.Helpers {
		if helper.HelperID == 77 {
			t.Fatal("parent-write helper survived retirement")
		}
	}
	borrowedEffectRecoveryFlow(t, m)
	// Retire the earlier helpers, fill the bank, reject a request without
	// running its expressions, then reuse a vacancy and reset the round.
	for _, helper := range append([]*character.Character(nil), m.World.Helpers...) {
		if err := helper.DestroyHelper(engine.DestroySelfParameters{}); err != nil {
			t.Fatal(err)
		}
	}
	oldLimit := m.World.HelperMax
	m.World.HelperMax = 4
	root.ChangeState(820, -1, -1)
	for tick := 0; tick < 20; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
	}
	for index, expected := range []int{0, 0, 0, 0, 4, 8, 822, 12, 1, 1, root.StateFile.Size.Height, 4} {
		if root.GetVar(40+index) != expected {
			t.Fatal("helper admission/retirement phase", index, root.GetVar(40+index), expected)
		}
	}
	if root.StateNo != 740 || !root.Ctrl || len(m.World.Helpers) != 4 {
		t.Fatal("helper bank vacancy did not recover", root.StateNo, len(m.World.Helpers))
	}
	m.World.HelperMax = oldLimit
	m.Round++
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	for _, id := range []int32{firstID, secondID} {
		if _, live := m.World.Registry.Lookup(id); live {
			t.Fatal("round reset retained a bound helper", id)
		}
	}
	root = m.Players[0].Character
	if root.EntityID != rootID {
		t.Fatal("round reset lost root identity")
	}
	reach(26)
	if find(55).EntityID <= secondID || find(56).EntityID <= secondID {
		t.Fatal("new helper generation reused retired IDs")
	}
	for index, expected := range want[:4] {
		if root.FloatVars[index] != expected {
			t.Fatal("new round did not restore helper binding/pause budget", index, root.FloatVars[index], expected)
		}
	}
}

const helperAdmissionProgram = `
[Statedef 820]
type=S
movetype=I
physics=N
ctrl=0
anim=9990
[State clear observations]
type=VarRangeSet
trigger1=Time=0
first=40
last=51
value=0
[State fill bank]
type=Helper
trigger1=Time=[0,3]
id=Time+1
stateno=821
[State rejected request]
type=Helper
trigger1=Time=4
id=(var(40):=9)
stateno=(var(41):=822)
pos=(var(42):=12),0
ownpal=(var(43):=1)
[State unused expressions do not fail]
type=Helper
trigger1=Time=5
id=~(1/0)
pos=~(1/0),0
[State full count]
type=VarSet
trigger1=Time=6
v=44
value=NumHelper
[State reuse vacancy]
type=Helper
trigger1=Time=10
id=(var(45):=8)
stateno=(var(46):=822)
pos=(var(47):=12),0
keyctrl=(var(48):=1)
ownpal=(var(49):=1)
[State restored count]
type=VarSet
trigger1=Time=14
v=51
value=NumHelper
[State recover]
type=ChangeState
trigger1=Time=15
value=740
ctrl=1
[Statedef 821]
type=S
movetype=I
physics=N
anim=9990
[State retire one]
type=DestroySelf
trigger1=IsHelper(1) && Time=8
[Statedef 822]
type=S
movetype=I
physics=N
anim=9990
[State parent confirmation]
type=ParentVarSet
trigger1=Time=0
v=50
value=Const(size.height)
`

// Native skips all expressions without a target/parent. With targets, it
// evaluates ID before admitting the remaining fields, including TargetBind.
const targetAdmissionProgram = `
[Statedef 810]
type=S
movetype=A
physics=N
anim=9991
ctrl=0
velset=0,0
[State clear observations]
type=VarRangeSet
trigger1=Time=0
first=40
last=52
value=0
[State no parent]
type=ParentVarSet
trigger1=Time=0
v=(var(40):=7)
value=(var(41):=8)
[State no target]
type=TargetLifeAdd
trigger1=Time=0 || Time=10
id=(var(42):=7)
value=(var(43):=8)
[State no target binding]
type=TargetBind
trigger1=Time=0
time=(var(44):=2)
id=(var(45):=7)
pos=(var(46):=4),0
[State grab]
type=HitDef
trigger1=Time=1
attr=S,NT
hitflag=MAF
damage=0,0
pausetime=0,0
ground.type=High
ground.hittime=1000
ground.velocity=0,0
sparkno=-1
guard.sparkno=-1
hitsound=-1
guardsound=-1
getpower=0,0
givepower=0,0
id=1234
p2stateno=812
[State unmatched target]
type=TargetLifeAdd
trigger1=Time=4
id=(var(47):=9999)
value=(var(48):=8)
[State matched target]
type=TargetVelSet
trigger1=Time=5
id=(var(49):=1234)
x=Const240p(var(50):=2)
y=0
[State applied velocity in 320p]
type=VarSet
trigger1=Time=6
v=51
value=(Target,Vel X)*320/(Target,Const240p(320))
[State unmatched binding]
type=TargetBind
trigger1=Time=7
time=(var(44):=2)
id=(var(45):=9999)
pos=(var(46):=4),0
[State drop]
type=TargetDrop
trigger1=Time=9
excludeid=-1
keepone=0
[State unused target expression is not a VM failure]
type=TargetBind
trigger1=Time=11
id=1234
time=~(1/0)
pos=~(1/0),0
[State unused parent expression is not a VM failure]
type=ParentVarAdd
trigger1=Time=11
v=~(1/0)
value=~(1/0)
[State create parent writer]
type=Helper
trigger1=Time=12
id=77
stateno=814
postype=p1
[State recover]
type=ChangeState
trigger1=Time=16
value=740
ctrl=1
[Statedef 812]
type=S
movetype=H
physics=N
anim=9990
ctrl=0
[State recover]
type=SelfState
trigger1=Time=12
value=740
ctrl=1
[Statedef 814]
type=S
movetype=I
physics=N
anim=9990
[State actual parent]
type=ParentVarSet
trigger1=Time=0
v=52
value=(var(0):=13)
[State add to actual parent]
type=ParentVarAdd
trigger1=Time=0
v=52
value=var(0)-11
[State retire]
type=DestroySelf
trigger1=Time=1
`

// Continue the same entity lifecycle through a differently scaled borrowed
// state, private effect creation, a real Pause, SelfState and effect retirement.
func borrowedEffectRecoveryFlow(t *testing.T, m *Match) {
	t.Helper()
	owner, receiver := m.Players[0].Character, m.Players[1].Character
	units := float64(receiver.LocalCoordinateWidth()) / 320
	owner.X, owner.Y = 0, 0
	receiver.X, receiver.Y = 20*units, 0
	owner.ChangeState(750, 0, -1)
	receiver.ChangeState(701, 0, -1)
	var explod *character.ExplodData
	paused, recovered := 0, false
	helperRecovered := false
	wasFrozen, age, x, y := false, 0, 0.0, 0.0
	for tick := 0; tick < 70; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("borrowed effect lifecycle", err)
		}
		if explod == nil {
			for _, candidate := range receiver.Explods {
				if candidate.ID == 77 {
					explod = candidate
				}
			}
			if explod != nil {
				state, _ := receiver.ResolveState(receiver.StateNo)
				if state != owner.StateFile.States[760] || explod.CurrentTime != 0 || explod.OffsetX != 16*units*float64(receiver.Facing) || explod.OffsetY != -32*units || explod.VelX != 2*units || explod.VelY != -units || explod.AccelY != .5*units {
					t.Fatal("borrowed effect lost evaluation units", receiver.StateNo, explod)
				}
				if explod.Visual.Source.Sprites != m.Players[1].SFF || explod.Visual.Source.Animations != m.Players[1].AIR || explod.Visual.Source.Scale != float64(m.Options.GameSpace.Width)/float64(owner.LocalCoordinateWidth()) {
					t.Fatal("borrowed effect lost issuer bank or creating state's pixel scale")
				}
			}
		}
		if explod == nil {
			continue
		}
		frozen := m.Timeline.Paused() // IsPaused is the separate hit-pause query.
		if frozen && wasFrozen {
			if explod.CurrentTime != age || explod.X != x || explod.Y != y {
				t.Fatal("paused borrowed effect advanced", explod.CurrentTime, age)
			}
			paused++
		}
		wasFrozen, age, x, y = frozen, explod.CurrentTime, explod.X, explod.Y
		if receiver.StateNo == 740 && receiver.Ctrl && explod.Visual.Active {
			state, _ := receiver.ResolveState(740)
			if state != receiver.StateFile.States[740] || explod.Visual.Source.Scale != float64(m.Options.GameSpace.Width)/float64(owner.LocalCoordinateWidth()) {
				t.Fatal("SelfState changed a live effect's creation profile")
			}
			recovered = true
			helpers := 0
			for _, helper := range m.World.Helpers {
				if helper.Parent != receiver || (helper.HelperID != 88 && helper.HelperID != 89) {
					continue
				}
				helpers++
				if helper.LocalCoordinateWidth() != receiver.LocalCoordinateWidth() || helper.Palettes.Archive != m.Players[1].SFF || helper.StateFile.States[helper.StateNo] != receiver.StateFile.States[helper.StateNo] {
					t.Fatal("borrowed helper lost issuer banks/profile/states after parent recovery")
				}
			}
			helperRecovered = helperRecovered || helpers == 2
		}
	}
	for index, expected := range []int{-48, -96, 40, 1280, 100, 1, -80, -160, 10, -20, 200, 1280} {
		if receiver.GetVar(40+index) != expected {
			t.Fatal("native borrowed helper context", index, receiver.GetVar(40+index), expected)
		}
	}
	for _, helper := range m.World.Helpers {
		if helper.Parent == receiver && (helper.HelperID == 88 || helper.HelperID == 89) {
			t.Fatal("borrowed helper survived retirement")
		}
	}
	if explod == nil || paused < 2 || !recovered || !helperRecovered || explod.Visual.Active || len(receiver.Explods) != 0 || receiver.IntVars[56] != 0 {
		t.Fatal("borrowed effect/pause/recovery/retirement incomplete", explod != nil, paused, recovered, len(receiver.Explods), receiver.IntVars[56])
	}
}
