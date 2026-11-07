package main

import (
	"image"
	"path/filepath"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

// One connected scenario exercises compiled variable/type controllers through
// intro release, independent messages/control, KO, win pose and round reset.
// Resources are the shared stock tree; no copied assets or captures are made.
func TestAnnouncementsAndControllerRoundFlow(t *testing.T) {
	e := soundTestEngine(t)
	oldInput := input.GlobalManager
	t.Cleanup(func() { input.GlobalManager = oldInput })
	input.GlobalManager = input.NewInputManager(e.cfg)
	input.GlobalManager.Keyboard = &flowKeyboard{}
	request := match.Request{StagePath: "stages/kfm.def"}
	for slot := range request.Players {
		request.Players[slot] = match.Selection{DefPath: filepath.Join(e.dataDir, "chars/kfm/kfm.def"), Palette: 1}
	}
	f, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.currentState = f
	layout, err := parsers.Parse(strings.NewReader(`[Round]
match.wins = 2
match.maxdrawgames = 1
start.waittime = 30
round.time = 0
round.default.spr = 200,0
round.default.offset = 640,320
round.default.displaytime = 80
round1.spr = 80,0
round1.offset = 640,300
round1.displaytime = 2
round1.snd = 0,1
round.sndtime = 0
fight.spr = 80,0
fight.offset = 640,300
fight.displaytime = 4
fight.time = 6
fight.snd = 1,0
fight.sndtime = 6
ctrl.time = 0
KO.spr = 200,0
KO.offset = 640,300
KO.font = 1,0,0
KO.displaytime = -1
KO.time = 0
KO.snd = 2,0
KO.sndtime = 0
slow.time = 0
over.waittime = 20
over.hittime = 10
over.wintime = 0
over.time = 40
win.spr = 80,0
win.offset = 640,300
win.time = 3
win.displaytime = 2
`))
	if err != nil {
		t.Fatal(err)
	}
	var sections []*parsers.Section
	for _, section := range f.match.Fight.AST.Sections {
		if !strings.EqualFold(section.Name, "Round") {
			sections = append(sections, section)
		}
	}
	f.match.Fight.AST.Sections = append(sections, layout.Sections...)
	program := `[Statedef -2]
[State -2, observe round]
type=VarSet
trigger1=1
v=1
value=RoundState
[State -2, observe state]
type=VarSet
trigger1=1
v=5
value=StateNo
[State -2, observe control]
type=VarSet
trigger1=1
v=6
value=Ctrl
[Statedef 5900]
type=S
movetype=I
physics=N
[State 5900, enter]
type=ChangeState
trigger1=1
value=0
[Statedef 0]
type=S
movetype=I
physics=N
anim=0
ctrl=0
velset=0,0
[State 0, count]
type=VarAdd
trigger1=1
v=0
value=1
[State 0, runtime integer coercion]
type=VarSet
trigger1=Time=0
v=2
value=7.9+0.0
[State 0, range]
type=VarRangeSet
trigger1=Time=0
first=3
last=4
value=Var(2)
[State 0, legacy float expression]
type=FVarSet
trigger1=Time=0
v=1
value=1.0/2
[State 0, legacy float addition]
type=FVarAdd
trigger1=Time=1
v=1
value=1.0/4
[State 0, legacy float range]
type=FVarRangeSet
trigger1=Time=2
range=2,3
value=FVar(1)
[State 0, change type]
type=StateTypeSet
trigger1=Time=3
statetype=C
movetype=I
physics=N
[State 0, preserve type]
type=StateTypeSet
trigger1=Time=4
statetype=U
movetype=U
physics=U
[State 0, restore type]
type=StateTypeSet
trigger1=Time=5
statetype=S
[State 0, intro]
type=AssertSpecial
trigger1=GameTime<400
flag=intro
[State 0, self KO]
type=LifeSet
trigger1=GameTime=500 && TeamSide=2
value=0
[Statedef 5030]
type=L
movetype=H
physics=N
ctrl=0
[State 5030, grounded]
type=ChangeState
trigger1=GameTime>=501
value=5150
`
	for _, number := range []string{"5150", "170", "175", "180"} {
		kind, move := "S", "I"
		if number == "5150" {
			kind, move = "L", "H"
		}
		program += "[Statedef " + number + "]\ntype=" + kind + "\nmovetype=" + move + "\nphysics=N\nctrl=0\n[State " + number + ", hold]\ntype=Null\ntrigger1=1\n"
	}
	for _, player := range f.match.Players {
		file, err := cns.ParseNamed(strings.NewReader(program), "segment/round-flow.cns")
		if err != nil {
			t.Fatal(err)
		}
		previous := player.Character.StateFile
		file.Data, file.Size, file.Velocity, file.Movement = previous.Data, previous.Size, previous.Velocity, previous.Movement
		file.ConstantFields = previous.ConstantFields
		file.SetLanguageVersion("1.1")
		player.Character.StateFile = file
		player.Character.ChangeState(5900, 0, -1)
	}
	canvas := ebiten.NewImage(1280, 720)
	defer canvas.Deallocate()
	messageVisible := func(key string) bool { return f.hud.messages[key].visible() }
	seenDraw, reset := false, false
	for frame := 0; frame <= 570; frame++ {
		if err := e.Update(); err != nil {
			t.Fatal(frame, err)
		}
		if f.match.Round == 2 {
			if len(f.hud.messages) != 0 || f.hud.played["fight"] || f.match.Announcements.Fight >= 0 {
				t.Fatal("round reset retained presentation/control cue")
			}
			reset = true
			break
		}
		root := f.match.Players[0].Character
		if frame == 4 && root.StateType != "C" {
			t.Fatal("StateTypeSet U did not preserve the authored posture")
		}
		if frame == 6 && (root.StateType != "S" || root.IntVars[3] != 7 || root.IntVars[4] != 7 || root.FloatVars[1] != .75 || root.FloatVars[2] != .75 || root.FloatVars[3] != .75) {
			t.Fatal("compiled variable/type controllers diverged", root.StateType, root.IntVars, root.FloatVars)
		}
		if frame >= 399 && frame <= 481 {
			if messageVisible("round.default") != (frame >= 401 && frame <= 480) || messageVisible("round") != (frame == 401 || frame == 402) {
				t.Fatal("independent default/numbered Round lifetime", frame)
			}
		}
		if frame >= 469 && frame <= 480 {
			if (root.IntVars[1] == 2) != (frame >= 470) || (root.IntVars[6] != 0) != (frame >= 470) {
				t.Fatal("control-zero or first RoundState2 pass", frame, root.IntVars[1], root.IntVars[6])
			}
			if messageVisible("fight") != (frame >= 476 && frame <= 479) || f.hud.played["fight"] != (frame >= 476) {
				t.Fatal("delayed Fight graphic/sound after control", frame)
			}
		}
		if frame >= 511 && frame <= 530 && !messageVisible("finish") {
			t.Fatal("persistent KO component lost across Win transition", frame)
		}
		if frame == 511 {
			// The retained HUD observation placed this stock bitmap label at
			// [601,198..680,226), with an exclusive upper corner. Exercise
			// the same motif and fitted viewport
			// in this round flow, without restoring the old capture bank.
			label := ebiten.NewImage(1280, 720)
			f.hud.label(label, "Round", "KO", "HUD", 0, 0)
			canvas.Clear()
			fit, err := coord.HUDProject(f.match.Options.GameSpace, e.motif.Info.LocalCoord, coord.Space{Width: 1280, Height: 720}, e.cfg.Video.KeepAspect)
			if err != nil {
				t.Fatal(err)
			}
			op := &ebiten.DrawImageOptions{}
			op.GeoM.Scale(fit.ScaleX, fit.ScaleY)
			op.GeoM.Translate(fit.X, fit.Y)
			if err := f.linearSampler.DrawImage(canvas, label, op); err != nil {
				t.Fatal(err)
			}
			pixels := make([]byte, 1280*720*4)
			canvas.ReadPixels(pixels)
			label.Deallocate()
			bounds := image.Rectangle{}
			for y := 0; y < 720; y++ {
				for x := 0; x < 1280; x++ {
					i := (y*1280 + x) * 4
					if pixels[i] != 0 || pixels[i+1] != 0 || pixels[i+2] != 0 {
						bounds = bounds.Union(image.Rect(x, y, x+1, y+1))
					}
				}
			}
			if bounds != image.Rect(601, 198, 680, 226) {
				t.Fatal("stock bitmap label placement", bounds)
			}
		}
		if frame == 521 && (root.IntVars[5] != 180 || root.IntVars[6] != 0 || root.IntVars[0] != 521) {
			t.Fatal("first Win pass state/control/counter", root.IntVars)
		}
		if frame >= 520 && frame <= 530 && messageVisible("result") != (frame == 524 || frame == 525) {
			t.Fatal("Win start and expiry", frame)
		}
		if frame == 401 || frame == 470 || frame == 476 || frame == 521 || frame == 524 || frame == 526 {
			canvas.Clear()
			e.Draw(canvas)
			if f.renderError != nil || f.hud.renderError != nil {
				t.Fatal("production drawing", f.renderError, f.hud.renderError)
			}
			if frame == 476 {
				pixels := make([]byte, 1280*720*4)
				canvas.ReadPixels(pixels)
				seenDraw = pixels[3] == 255
			}
		}
	}
	if !reset || !seenDraw {
		t.Fatal("round did not reset or production drawing was not consumed", reset, seenDraw)
	}
}
