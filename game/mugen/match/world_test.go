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
// bound body contact, retirement and replacement in a fresh round. The beta
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
		p.AIR = &air.Data{Actions: map[int]*air.Action{9990: {ID: 9990, LoopStart: -1, Elements: []air.Element{{Group: 0, Image: 0, Time: -1, Clsn2: []air.Box{box}}}}}}
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
