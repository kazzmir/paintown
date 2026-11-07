package main

import (
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/cns"
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
	commands, err := cmd.Parse(strings.NewReader(`[Command]
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
	for n := 0; n < 6; n++ {
		step("DirBack")
	}
	step()
	step("ButtonX", "ButtonY")
	for n := 0; n < 4; n++ {
		step()
	}
	step("DirDown")
	step("DirDown", "DirForward")
	step("DirForward")
	step("ButtonX")
	for n := 0; n < 4; n++ {
		step()
	}
	step("DirForward")
	step("ButtonA")
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
	for side := range request.Players {
		request.Players[side].AILevel = 8
	}
	e.pendingMatchSeed = 16807
	f, err = NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.currentState = f
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
