package main

import (
	"path/filepath"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

func TestCombatClipboardComboAndDebugFlow(t *testing.T) {
	e := soundTestEngine(t)
	e.cfg.Debug.Debug, e.cfg.Debug.AllowDebugMode = false, true
	oldInput := input.GlobalManager
	t.Cleanup(func() { input.GlobalManager = oldInput })
	input.GlobalManager = input.NewInputManager(e.cfg)
	keys := &flowKeyboard{}
	input.GlobalManager.Keyboard = keys
	request := match.Request{StagePath: "stages/kfm.def"}
	for slot := range request.Players {
		request.Players[slot] = match.Selection{DefPath: filepath.Join(e.dataDir, "chars/kfm/kfm.def"), Palette: 1}
	}
	f, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.currentState, f.match.Phase = f, match.Active
	layout, err := parsers.Parse(strings.NewReader("[Combo]\nteam1.pos=80,160\nteam1.start.x=80\nteam1.displaytime=20\n"))
	if err != nil {
		t.Fatal(err)
	}
	// Put the scenario override first, matching the native layout lookup.
	f.match.Fight.AST.Sections = append(layout.Sections, f.match.Fight.AST.Sections...)
	frames, err := air.Parse(strings.NewReader(`[Begin Action 0]
Clsn1: 1
Clsn1[0] = -250,-40,250,0
Clsn2: 1
Clsn2[0] = -20,-40,20,0
0,0,0,0,-1
`))
	if err != nil {
		t.Fatal(err)
	}
	program := `[Statedef 0]
type=S
movetype=A
physics=N
anim=0
ctrl=0
[State 0, ordered display]
type=DisplayToClipboard
trigger1=Time=0
text="start %d %.2f %%\n"
params=var(10):=1,1.0/4
[State 0, append without blank row]
type=AppendToClipboard
trigger1=Time=0
text="append\t%i"
params=var(10):=var(10)+1
[State 0, two contacts]
type=HitDef
trigger1=Time=0 || Time=4
attr=S,NA
damage=10
numhits=1
pausetime=2,2
ground.velocity=0,0
ground.hittime=40
sparkno=-1
hitsound=-1,0
[State 0, authored combo addition]
type=HitAdd
trigger1=MoveHit=1 && Var(11)=0
ignorehitpause=1
value=1
[State 0, latch authored addition]
type=VarSet
trigger1=MoveHit=1 && Var(11)=0
ignorehitpause=1
v=11
value=1
[State 0, contact display]
type=DisplayToClipboard
trigger1=MoveHit=1
ignorehitpause=1
text="hit=%d q=%.2f"
params=HitCount,1.0/4
[State 0, mismatch fallback]
type=AppendToClipboard
trigger1=MoveHit=1
ignorehitpause=1
text="i=%f f=%d%%"
params=7,1.25
[State 0, escaped text and general conversion]
type=AppendToClipboard
trigger1=MoveHit=1
ignorehitpause=1
text="\"quoted, \\ %g"
params=1.23456789
[State 0, clear]
type=ClearClipboard
trigger1=Time=3
[State 0, append on empty buffer]
type=AppendToClipboard
trigger1=Time=3
text="cleared"
[State 0, helper]
type=Helper
trigger1=GameTime>=12 && Var(12)=0
stateno=900
id=900
[State 0, helper once]
type=VarSet
trigger1=GameTime>=12 && Var(12)=0
v=12
value=1
[State 0, pause]
type=Pause
trigger1=GameTime=25
time=3
movetime=0
[Statedef 20]
type=S
movetype=I
physics=N
anim=0
ctrl=0
[State 20, idle]
type=Null
trigger1=1
[Statedef 5000]
type=S
movetype=H
physics=N
anim=0
ctrl=0
[State 5000, recover]
type=ChangeState
trigger1=Time>=8
value=20
[Statedef 900]
type=S
movetype=I
physics=N
anim=0
ctrl=0
[State 900, root combo]
type=HitAdd
trigger1=Time=0
value=1
[State 900, private buffer]
type=DisplayToClipboard
trigger1=Time=0
text="helper"
[State 900, retire]
type=DestroySelf
trigger1=Time=20
`
	for slot, player := range f.match.Players {
		file, err := cns.ParseNamed(strings.NewReader(program), "segment/feedback-flow.cns")
		if err != nil {
			t.Fatal(err)
		}
		prior := player.Character.StateFile
		file.Data, file.Size, file.Velocity, file.Movement = prior.Data, prior.Size, prior.Velocity, prior.Movement
		file.ConstantFields = prior.ConstantFields
		file.SetLanguageVersion("1.1")
		player.Character.StateFile, player.Character.AirFile = file, frames
		player.Character.RoundState = 2
		state := 0
		if slot == 1 {
			state = 20
		}
		player.Character.ChangeState(state, 0, -1)
	}
	root, victim := f.match.Players[0].Character, f.match.Players[1].Character
	canvas := ebiten.NewImage(1280, 720)
	defer canvas.Deallocate()
	seenFormatting, seenEmptyAppend, seenSharedCombo, seenRetainedTotal := false, false, false, false
	seenHelper, seenPause := false, false
	var pausedText string
	for frame := 0; frame < 70; frame++ {
		keys.keys = nil
		if frame == 2 || frame == 12 || frame == 13 || frame == 14 {
			keys.keys = map[ebiten.Key]bool{ebiten.KeyControlLeft: true, ebiten.KeyD: true}
		}
		if err := e.Update(); err != nil {
			t.Fatal(frame, err)
		}
		if frame == 0 && (root.Clipboard != "start 1 0.25 %\nappend\t2" || root.IntVars[10] != 2) {
			t.Fatal("ordered formatting/append", root.Clipboard, root.IntVars[10])
		}
		if strings.Contains(root.Clipboard, "i=7 f=1.250000%\n\"quoted, \\ 1.23457") {
			seenFormatting = true
		}
		if root.Clipboard == "cleared" {
			seenEmptyAppend = true
		}
		if root.ComboCount == 4 && root.HitCount == 2 && f.hud.combos[0].count == 4 {
			seenSharedCombo = true
		}
		if root.ComboCount == 0 && f.hud.combos[0].active && f.hud.combos[0].count == 4 {
			seenRetainedTotal = true
		}
		if len(f.match.World.Helpers) == 1 {
			helper := f.match.World.Helpers[0]
			if helper.Clipboard != "helper" || root.Clipboard == "helper" {
				t.Fatal("helper clipboard ownership")
			}
			seenHelper = true
		}
		if frame == 2 {
			if f.debugSelection != 0 {
				t.Fatal("Ctrl-D did not enable root debug display")
			}
			// Same-frame hidden/visible rendering checks the real consumer,
			// without retaining screenshots or depending on a native raster.
			f.debugSelection = -1
			e.Draw(canvas)
			before := make([]byte, 1280*720*4)
			canvas.ReadPixels(before)
			f.debugSelection = 0
			e.Draw(canvas)
			after := make([]byte, len(before))
			canvas.ReadPixels(after)
			different := false
			for at := 640 * 1280 * 4; at < len(before); at++ {
				different = different || before[at] != after[at]
			}
			if !different || f.debugFont == nil {
				t.Fatal("formatted clipboard did not reach production pixels")
			}
		}
		if frame == 13 && (f.debugSelection != 2 || f.debugActors()[2].Clipboard != "helper") {
			t.Fatal("debug selection did not reach the helper", f.debugSelection)
		}
		if frame == 14 && f.debugSelection != -1 {
			t.Fatal("debug selection did not cycle off")
		}
		if frame == 25 {
			seenPause, pausedText = f.match.Timeline.Paused(), root.Clipboard
		}
		if frame > 25 && frame < 29 && root.Clipboard != pausedText {
			t.Fatal("pause lost retained clipboard")
		}
		e.Draw(canvas)
		if f.renderError != nil || f.hud.renderError != nil {
			t.Fatal("feedback drawing", f.renderError, f.hud.renderError)
		}
	}
	if !seenFormatting || !seenEmptyAppend || !seenSharedCombo || !seenRetainedTotal || !seenHelper || !seenPause {
		t.Fatal("connected feedback stages missing", seenFormatting, seenEmptyAppend, seenSharedCombo, seenRetainedTotal, seenHelper, seenPause)
	}
	if victim.Life != victim.MaxLife-20 || root.ComboCount != 0 || f.hud.combos[0].active || len(f.match.World.Helpers) != 0 {
		t.Fatal("contact/recovery/expiry/retirement", victim.Life, root.ComboCount, f.hud.combos[0], len(f.match.World.Helpers))
	}
	// User pause is outside CNS Pause: complete simulation/HUD/GameTime hold,
	// but key edges continue so Scroll Lock can advance one connected pass.
	step := func(pressed ...ebiten.Key) {
		t.Helper()
		keys.previous, keys.keys = keys.keys, map[ebiten.Key]bool{}
		for _, key := range pressed {
			keys.keys[key] = true
		}
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
	}
	tick, gameTime, stateTime := f.match.Tick, e.gameTime, root.Time
	step(ebiten.KeyPause)
	step(ebiten.KeyPause)
	step()
	step(ebiten.KeyF9)
	if !f.userPaused || f.match.Tick != tick || e.gameTime != gameTime || root.Time != stateTime {
		t.Fatal("user pause advanced simulation or held Pause toggled it again")
	}
	step(ebiten.KeyScrollLock)
	step(ebiten.KeyScrollLock)
	if !f.userPaused || f.match.Tick != tick+1 || e.gameTime != gameTime+1 || root.Time != stateTime+1 {
		t.Fatal("held frame-step did not advance exactly one simulation pass")
	}
	step()
	step(ebiten.KeyPause)
	if f.userPaused || f.match.Tick != tick+2 || e.gameTime != gameTime+2 {
		t.Fatal("Pause did not resume the held fight")
	}
	e.cfg.Input.AnyKeyUnpauses = true
	step()
	step(ebiten.KeyPause)
	step()
	tick, gameTime = f.match.Tick, e.gameTime
	step(ebiten.KeyScrollLock)
	if !f.userPaused || f.match.Tick != tick+1 {
		t.Fatal("AnyKeyUnpauses swallowed frame-step")
	}
	step()
	step(ebiten.KeyF10)
	if f.userPaused || f.match.Tick != tick+2 || e.gameTime != gameTime+2 {
		t.Fatal("AnyKeyUnpauses did not resume on a fresh runtime key")
	}
}
