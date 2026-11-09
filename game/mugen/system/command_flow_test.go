package main

import (
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
)

// Human charge/chord, rolling motion and strict double-tap inputs enter CNS
// states, recover, and are followed by actual stock AI combat through CMD.
func TestAuthoredCommandsAndAICombatFlow(t *testing.T) {
	e := soundTestEngine(t)
	oldInput := input.GlobalManager
	t.Cleanup(func() { input.GlobalManager = oldInput })
	input.GlobalManager = input.NewInputManager(e.cfg)
	input.GlobalManager.Keyboard = &flowKeyboard{}
	request := match.Request{Mode: "Versus Mode", StagePath: "stages/kfm.def"}
	for side := range request.Players {
		request.Players[side] = match.Selection{DefPath: filepath.Join(e.dataDir, "chars/kfm/kfm.def"), Palette: 1}
	}
	f, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.currentState = f
	f.match.Phase = match.Active
	commands, err := cmd.Parse(strings.NewReader(`[Remap]
x=a
y=a
a=
z=s
s=z
[Command]
name="charged"
command=~6$B,x+y
time=15
[Command]
name="qcf"
command=~D,DF,F,x
time=15
[Command]
name="double"
command=F,F
time=10
[Command]
name="released-x"
command=~3x
[Command]
name="disabled-a"
command=a
[Command]
name="start"
command=s
[Command]
name="x"
command=x
[Command]
name="held-x"
command=/x
buffer.time=20
[Command]
name="held-chord"
command=/x+y
buffer.time=20
[Command]
name="held-sequence"
command=/x,/z
buffer.time=3
[Command]
name="simultaneous"
command=x+z
[Command]
name="buffered-x"
command=x
buffer.time=3
[Command]
name="long-charge"
command=~300x
time=1
[Command]
name="long-charge-motion"
command=~300$B,F,z
time=30
[Command]
name="long-motion"
command=x,b
time=240
[Command]
name="long-press"
command=x
time=1
`))
	if err != nil {
		t.Fatal(err)
	}
	file, err := cns.ParseNamed(strings.NewReader(`[Statedef -1]
[State -1, charge]
type=ChangeState
triggerall=StateNo=0
trigger1=command="charged"
value=8000
[State -1, motion]
type=ChangeState
triggerall=StateNo=0
trigger1=command="qcf"
value=8001
[State -1, double]
type=ChangeState
triggerall=StateNo=0
trigger1=command="double"
value=8002
[State -1, ordinary button attack]
type=ChangeState
triggerall=StateNo=0 && Var(59)=1
trigger1=command="x"
value=8003
[State -1, planned long-charge attack]
type=ChangeState
triggerall=StateNo=0 && Var(59)=2
trigger1=command="long-charge-motion"
value=8004
[Statedef 0]
type=S
movetype=I
physics=N
anim=0
ctrl=1
velset=0,0
[State 0, idle]
type=Null
trigger1=0
[State 0, disabled button]
type=VarAdd
trigger1=command="disabled-a"
v=53
value=1
[State 0, remapped start]
type=VarAdd
trigger1=command="start"
v=54
value=1
[State 0, remapped charged release]
type=VarAdd
trigger1=command="released-x"
v=55
value=1
[State 0, long charge]
type=VarAdd
trigger1=command="long-charge"
v=40
value=1
[State 0, long charge motion]
type=VarAdd
trigger1=command="long-charge-motion"
v=41
value=1
[State 0, long motion]
type=VarAdd
trigger1=command="long-motion"
v=42
value=1
[State 0, fresh press]
type=VarAdd
trigger1=command="long-press"
v=43
value=1
[Statedef 8000]
type=S
movetype=A
physics=N
ctrl=0
[State 8000, count]
type=VarAdd
trigger1=Time=0
v=50
value=1
[State 8000, recover]
type=ChangeState
trigger1=Time>=3
value=0
[Statedef 8001]
type=S
movetype=A
physics=N
ctrl=0
[State 8001, count]
type=VarAdd
trigger1=Time=0
v=51
value=1
[State 8001, recover]
type=ChangeState
trigger1=Time>=3
value=0
[Statedef 8002]
type=S
movetype=A
physics=N
ctrl=0
[State 8002, count]
type=VarAdd
trigger1=Time=0
v=52
value=1
[State 8002, recover]
type=ChangeState
trigger1=Time>=3
value=0
[Statedef 8003]
type=S
movetype=A
physics=N
ctrl=0
[State 8003, count ordinary presses]
type=VarAdd
trigger1=Time=0
v=56
value=1
[State 8003, recover]
type=ChangeState
trigger1=Time>=3
value=0
[Statedef 8004]
type=S
movetype=A
physics=N
ctrl=0
anim=200
[State 8004, count]
type=VarAdd
trigger1=Time=0
v=44
value=1
[State 8004, contact]
type=HitDef
trigger1=Time=0
attr=S,NA
hitflag=MAF
damage=30,0
pausetime=0,0
ground.type=High
ground.hittime=10
ground.velocity=0,0
[State 8004, recover]
type=ChangeState
trigger1=Time>=12
value=0
`), "connected-commands.cns")
	if err != nil {
		t.Fatal(err)
	}
	file.SetLanguageVersion("1.1")
	root := f.match.Players[0].Character
	base := root.StateFile
	file.Data, file.Size, file.Movement = base.Data, base.Size, base.Movement
	root.StateFile, root.CmdFile = file, commands
	root.Commands = character.NewCommandBuffer(120)
	root.ChangeState(0, 1, -1)
	var inputs []string
	root.InputSource = func(int, int) []string { return inputs }
	step := func(keys ...string) {
		t.Helper()
		inputs = keys
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
	}
	// Beta x=a, a= makes authored x read physical a. Physical x does
	// nothing; disabling logical a must not prevent it from supplying x/y.
	step("ButtonX")
	step()
	if root.IntVars[50] != 0 || root.IntVars[53] != 0 {
		t.Fatal("physical x bypassed the authored button remap")
	}
	for n := 0; n < 6; n++ {
		step("DirBack")
	}
	step()
	step("ButtonA") // One physical button supplies the x+y chord.
	// Let the previous Back charge expire before pressing that same chord
	// for the next move; remapping correctly makes both commands eligible.
	for n := 0; n < 16; n++ {
		step()
	}
	step("DirDown")
	step("DirDown", "DirForward")
	step("DirForward")
	step("ButtonA")
	for n := 0; n < 4; n++ {
		step()
	}
	step("DirForward")
	step("ButtonC")
	step("DirForward")
	for n := 0; n < 4; n++ {
		step()
	}
	if root.IntVars[50] != 1 || root.IntVars[51] != 1 || root.IntVars[52] != 0 {
		t.Fatal("human command entry/recovery/strict interruption", root.IntVars[50:53], commands.Commands["qcf"], root.Commands.History, root.Commands.Match("~D,DF,F,x", 15, 15))
	}
	step("DirForward")
	step()
	step("DirForward")
	for n := 0; n < 4; n++ {
		step()
	}
	if root.IntVars[52] != 1 || root.StateNo != 0 {
		t.Fatal("strict double-tap did not enter/recover", root.IntVars[52], root.StateNo)
	}
	if err := root.CreateHelper(engine.HelperParameters{ID: 77, StateNo: 0, KeyControl: true, Type: "normal", RemapPalette: [2]int{-1, 0}}); err != nil {
		t.Fatal(err)
	}
	helper := root.Helpers[len(root.Helpers)-1]
	root.IntVars[55] = 0
	for n := 0; n < 4; n++ {
		step("ButtonA")
	}
	step()
	step("ButtonZ") // Logical Start reads physical z; physical Start reads z.
	step()
	if root.IntVars[53] != 0 || root.IntVars[54] != 1 || root.IntVars[55] != 1 || helper.IntVars[53] != 0 || helper.IntVars[54] != 1 || helper.IntVars[55] != 1 {
		t.Fatal("root/helper remap, disabled slot, Start or charged release", root.IntVars[53:56], helper.IntVars[53:56])
	}
	// Follow both actors through attack/recovery while one physical button
	// stays down. The bare x command fires once; /x remains held and the
	// explicitly buffered alias retains exactly its authored window.
	root.IntVars[59], helper.IntVars[59] = 1, 1
	for press := 1; press <= 2; press++ {
		for frame := 0; frame < 32; frame++ {
			step("ButtonA")
			for _, actor := range []*character.Character{root, helper} {
				if actor.Command("x") != (frame == 0) || !actor.Command("held-x") || !actor.Command("held-chord") || actor.Command("buffered-x") != (frame < 3) {
					t.Fatal("declared press/hold/buffer semantics were bypassed", press, frame, actor.InputSlot, actor.ActiveCommands)
				}
			}
		}
		for _, actor := range []*character.Character{root, helper} {
			if actor.IntVars[56] != press || actor.StateNo != 0 || !actor.Ctrl {
				t.Fatal("held button repeated through recovery", press, actor.IntVars[56], actor.StateNo)
			}
		}
		step()
		if root.Command("held-x") || helper.Command("held-x") || root.Command("held-chord") || helper.Command("held-chord") || !root.Command("released-x") || !helper.Command("released-x") {
			t.Fatal("held-button release did not reach the authored release command")
		}
	}
	// A held final symbol completes a sequence once and retains its authored
	// buffer, unlike a standalone hold. Rolling the second button does not
	// satisfy an ordinary simultaneous-press chord.
	root.IntVars[59], helper.IntVars[59] = 0, 0
	for frame := 0; frame < 6; frame++ {
		step("ButtonA")
	}
	for frame := 0; frame < 12; frame++ {
		step("ButtonA", "ButtonStart") // Logical z reads the physical Start key.
		for _, actor := range []*character.Character{root, helper} {
			if !actor.Command("held-chord") || actor.Command("held-sequence") != (frame < 3) || actor.Command("simultaneous") || actor.StateNo != 0 || !actor.Ctrl {
				t.Fatal("held sequence/chord completion or recovery", frame, actor.ActiveCommands, actor.StateNo, actor.Ctrl)
			}
		}
	}
	step()
	for _, actor := range []*character.Character{root, helper} {
		if actor.Command("held-x") || actor.Command("held-chord") || actor.Command("held-sequence") {
			t.Fatal("released holds survived into idle", actor.ActiveCommands)
		}
	}
	// Continue both input consumers beyond the old 120-tick cutoff. The first
	// press expires from a 240-tick motion while its 300-tick charge succeeds;
	// a later separated press completes that motion, and a short charge fails.
	for _, actor := range []*character.Character{root, helper} {
		for index := 40; index <= 43; index++ {
			actor.SetVar(index, 0)
		}
	}
	repeat := func(ticks int, keys ...string) {
		for frame := 0; frame < ticks; frame++ {
			step(keys...)
		}
	}
	repeat(330, "DirBack", "ButtonA")
	step()
	repeat(3, "DirForward")
	step("DirForward", "ButtonStart")
	step()
	step("ButtonB")
	repeat(20)
	step("ButtonA")
	repeat(132)
	step("ButtonB")
	repeat(20)
	repeat(30, "DirBack", "ButtonA")
	step()
	repeat(3, "DirForward")
	step("DirForward", "ButtonStart")
	step()
	step("ButtonB")
	repeat(20)
	for _, actor := range []*character.Character{root, helper} {
		for index, expected := range []int{1, 1, 2, 3} {
			if actor.GetVar(40+index) != expected {
				t.Fatal("native long command lifecycle", index, actor.GetVar(40+index), expected)
			}
		}
		if actor.StateNo != 0 || !actor.Ctrl || actor.Command("held-x") || actor.Command("held-chord") {
			t.Fatal("long input lifecycle did not return to controllable idle")
		}
	}
	// Continue into a fresh AI match whose only planned attack is the same
	// remapped 300-tick charge. The authored CNS must receive it, hit and
	// recover; the planner stores timed input steps rather than 300 frames.
	request.Players[0].AILevel = 8
	longAI, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.currentState = longAI
	longAI.match.Phase = match.Active
	aiRoot := longAI.match.Players[0].Character
	aiRoot.StateFile = file
	aiRoot.CmdFile = &cmd.Data{Remap: commands.Remap, Commands: map[string][]cmd.Command{"long-charge-motion": commands.Commands["long-charge-motion"]}}
	aiRoot.SetVar(59, 2)
	aiRoot.X = 0
	aiRoot.ChangeState(0, 1, -1)
	aiVictim := longAI.match.Players[1].Character
	aiVictim.X = 20
	for frame := 0; frame < 700 && !(aiRoot.GetVar(44) > 0 && aiVictim.Life < aiVictim.MaxLife && aiRoot.StateNo == 0 && aiRoot.Ctrl); frame++ {
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
	}
	if aiRoot.GetVar(44) != 1 || aiVictim.Life >= aiVictim.MaxLife || aiRoot.StateNo != 0 || !aiRoot.Ctrl {
		t.Fatal("planned long charge failed attack/contact/recovery", aiRoot.GetVar(44), aiVictim.Life, aiRoot.StateNo, aiRoot.Ctrl)
	}
	for side := range request.Players {
		request.Players[side].AILevel = 8
	}
	e.pendingMatchSeed = 16807
	f, err = NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.currentState = f
	for _, player := range f.match.Actors() {
		if len(player.Character.Commands.History) != 0 || player.Character.GetVar(40) != 0 {
			t.Fatal("fresh match retained long-command input or observations")
		}
		commands := *player.CMD
		commands.Remap = map[string]string{"x": "a", "a": "x", "y": "b", "b": "y", "z": "c", "c": "z", "s": ""}
		player.CMD, player.Character.CmdFile = &commands, &commands
	}
	sawAttack, sawSpecial, sawDamage := false, false, false
	for frame := 0; frame < 2200 && !(sawAttack && sawSpecial && sawDamage); frame++ {
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
		for _, p := range f.match.Actors() {
			sawAttack = sawAttack || p.Character.MoveType == "A"
			sawSpecial = sawSpecial || (p.Character.StateNo >= 1000 && p.Character.StateNo < 2000)
			sawDamage = sawDamage || p.Character.Life < p.Character.MaxLife
		}
	}
	if !sawAttack || !sawSpecial || !sawDamage {
		t.Fatal("authored AI commands did not reach stock attack/special/contact", sawAttack, sawSpecial, sawDamage)
	}
}
