package main

import (
	"bytes"
	"fmt"
	"os"
	"path/filepath"
	"regexp"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/resource"
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
	debugRuntimeLifecycle(t, e, keys, canvas)
}

// One runtime episode spans key gates, ownership/pacing, time-over, cleanup,
// multi-round score retention and corrected on-disk content reload. All sprite,
// sound, animation and command banks are shared from the supplied character.
func debugRuntimeLifecycle(t *testing.T, e *Engine, keys *flowKeyboard, canvas *ebiten.Image) {
	t.Helper()
	oldTPS, oldVsync := ebiten.TPS(), ebiten.IsVsyncEnabled()
	t.Cleanup(func() { ebiten.SetTPS(oldTPS); ebiten.SetVsyncEnabled(oldVsync) })
	directory := t.TempDir()
	fightPath, err := resource.Resolve(e.motif.Files.Fight, filepath.Dir(e.motif.Source), filepath.Join(e.dataDir, "data"), e.dataDir)
	if err != nil {
		t.Fatal(err)
	}
	fightText, err := os.ReadFile(fightPath)
	if err != nil {
		t.Fatal(err)
	}
	fightText = regexp.MustCompile(`(?im)^(\s*match\.wins\s*=\s*)\d+`).ReplaceAll(fightText, []byte("${1}2"))
	e.motif.Files.Fight = filepath.Join(directory, "fight.def")
	if err := os.WriteFile(e.motif.Files.Fight, fightText, 0600); err != nil {
		t.Fatal(err)
	}
	stock := filepath.Join(e.dataDir, "chars/kfm")
	constants, err := os.ReadFile(filepath.Join(stock, "kfm.cns"))
	if err != nil {
		t.Fatal(err)
	}
	constantsPath := filepath.Join(directory, "runtime.cns")
	if err := os.WriteFile(constantsPath, constants, 0600); err != nil {
		t.Fatal(err)
	}
	definition := fmt.Sprintf("[Info]\nname=Runtime KFM\nmugenversion=1.1\nlocalcoord=320,240\n[Files]\ncns=%s\nst=%s\nstcommon=common1.cns\ncmd=%s\nsprite=%s\nanim=%s\nsound=%s\n", constantsPath, constantsPath, filepath.Join(stock, "kfm.cmd"), filepath.Join(stock, "kfm.sff"), filepath.Join(stock, "kfm.air"), filepath.Join(stock, "kfm.snd"))
	defPath := filepath.Join(directory, "runtime.def")
	if err := os.WriteFile(defPath, []byte(definition), 0600); err != nil {
		t.Fatal(err)
	}
	request := match.Request{Mode: "Versus Mode", StagePath: "stages/kfm.def"}
	request.Players = [2]match.Selection{{DefPath: defPath, Palette: 1}, {DefPath: filepath.Join(stock, "kfm.def"), Palette: 2}}
	f, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.currentState = f
	e.cfg.Debug.AllowDebugKeys = false
	e.cfg.Input.AnyKeyUnpauses = false
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
	press := func(pressed ...ebiten.Key) { t.Helper(); step(pressed...); step() }
	active := func() {
		t.Helper()
		for n := 0; n < 700 && f.match.Phase != match.Active; n++ {
			step()
		}
		if f.match.Phase != match.Active {
			t.Fatal("runtime flow did not reach active fight")
		}
	}
	active()
	press(ebiten.KeyPause)
	left, right := f.match.Players[0].Character, f.match.Players[1].Character
	press(ebiten.KeyF2)
	if left.Life != left.MaxLife || right.Life != right.MaxLife {
		t.Fatal("disabled debug keys changed life")
	}
	press(ebiten.KeyControlLeft, ebiten.KeyD)
	press(ebiten.KeyF2)
	press(ebiten.KeyF3)
	if left.Life != 1 || right.Life != 1 || left.GetPower() != left.MaxPower || right.GetPower() != right.MaxPower {
		t.Fatal("debug-mode life/power controls did not reach characters")
	}
	press(ebiten.KeySpace)
	press(ebiten.KeyControlLeft, ebiten.KeyF2)
	if left.Life != 1 || right.Life != right.MaxLife {
		t.Fatal("Ctrl-F2 lost player ownership")
	}
	press(ebiten.KeySpace)
	press(ebiten.KeyShiftLeft, ebiten.KeyF2)
	if right.Life != 1 || left.Life != left.MaxLife {
		t.Fatal("Shift-F2 lost player ownership")
	}
	press(ebiten.KeySpace)
	press(ebiten.KeyControlLeft, ebiten.KeyDigit2)
	if right.AILevel < 1 || request.Players[1].AILevel != 0 {
		t.Fatal("AI toggle did not reach live roster or changed request template")
	}
	left.VelX, left.VelY = 3, -2
	press(ebiten.KeyControlLeft, ebiten.KeyI)
	if left.StateNo != 0 || left.VelX != 0 || left.VelY != 0 {
		t.Fatal("force stand missed the ordinary state consumer")
	}
	e.Draw(canvas)
	visible := make([]byte, 1280*720*4)
	canvas.ReadPixels(visible)
	press(ebiten.KeyControlLeft, ebiten.KeyL)
	e.Draw(canvas)
	hidden := make([]byte, len(visible))
	canvas.ReadPixels(hidden)
	if !e.hideFightBars || bytes.Equal(visible, hidden) {
		t.Fatal("debug bar visibility had no rendered consumer")
	}
	press(ebiten.KeyControlLeft, ebiten.KeyL)
	profileTPS := f.match.Options.TicksPerSecond
	press(ebiten.KeyControlLeft, ebiten.KeyS)
	if ebiten.TPS() != ebiten.SyncWithFPS || f.match.Options.TicksPerSecond != profileTPS {
		t.Fatal("fast debug pacing changed script tick units")
	}
	press(ebiten.KeyControlLeft, ebiten.KeyV)
	if ebiten.TPS() != e.ticksPerSecond() || !ebiten.IsVsyncEnabled() {
		t.Fatal("debug vsync did not restore normal scheduling")
	}
	press(ebiten.KeyF5)
	if f.match.Phase != match.PreOver || !f.match.TimeOver || f.match.TimeLeft != 0 {
		t.Fatal("debug time-over bypassed the result lifecycle")
	}
	previous, processTime := left, e.gameTime
	press(ebiten.KeyF4)
	left = f.match.Players[0].Character
	if !f.userPaused || f.match.Phase != match.PreIntro || left == previous || left.RoundsExisted != 0 || f.match.Wins != ([2]int{}) || e.gameTime != processTime {
		t.Fatal("paused round restart leaked identity/score/time")
	}
	press(ebiten.KeyPause)
	active()
	press(ebiten.KeyF1)
	// Include the documented 900-pass readiness fallback plus KO slowdown,
	// victory pose and over.time; debug KO need not resemble contact damage.
	for n := 0; n < 1800 && f.match.Round == 1; n++ {
		step()
	}
	if f.match.Round != 2 || f.match.Wins != ([2]int{1, 0}) {
		t.Fatal("debug KO did not complete the ordinary round", f.match.Round, f.match.Wins, f.match.Phase, f.match.PhaseTime, f.match.Players[0].Character.StateNo, f.match.Players[1].Character.StateNo)
	}
	press(ebiten.KeyPause)
	left = f.match.Players[0].Character
	if err := left.CreateHelper(engine.HelperParameters{StateNo: 0, ID: 77, PosType: "p1", Type: "normal"}); err != nil {
		t.Fatal(err)
	}
	helperID := f.match.World.Helpers[0].EntityID
	left.IntVars[58] = 77
	left.SetLife(9)
	press(ebiten.KeyF4)
	left = f.match.Players[0].Character
	_, oldHelperAlive := f.match.World.Registry.Lookup(helperID)
	if left.RoundsExisted != 1 || left.Life != left.MaxLife || left.IntVars[58] != 0 || oldHelperAlive || len(f.match.World.Helpers) != 0 || f.match.Wins != ([2]int{1, 0}) {
		t.Fatal("round-two restart leaked transient state/counters/score")
	}
	before, oldSprites, oldStage := f.match, f.match.Players[0].SFF, f.match.Stage
	updated := regexp.MustCompile(`(?im)^(life\s*=\s*)1000\b`).ReplaceAllString(string(constants), "${1}1100")
	if updated == string(constants) {
		t.Fatal("runtime overlay did not change the source life constant")
	}
	updated += fmt.Sprintf("\n[Statedef 5900]\ntype=S\nmovetype=I\nphysics=N\nanim=ifelse(GameTime >= %d && RoundNo=2 && RoundsExisted=1,0,5)\n[State 5900, ready]\ntype=ChangeState\ntrigger1=Time=0\nvalue=0\n", e.gameTime)
	if err := os.WriteFile(constantsPath, []byte(updated), 0600); err != nil {
		t.Fatal(err)
	}
	press(ebiten.KeyShiftLeft, ebiten.KeyF4)
	left = f.match.Players[0].Character
	if f.match == before || f.match.Stage == oldStage || f.match.Players[0].SFF == oldSprites || left.MaxLife != 1100 || left.GetAnim() != 0 || f.match.Round != 2 || left.RoundsExisted != 1 || f.match.Wins != ([2]int{1, 0}) || f.match.Players[1].Character.AILevel < 1 || !f.userPaused {
		t.Fatal("reload missed changed banks or lost the current round context")
	}
	e.Draw(canvas)
	if f.renderError != nil || f.hud.renderError != nil {
		t.Fatal("reloaded presentation retained retired resources", f.renderError, f.hud.renderError)
	}
	press(ebiten.KeyEscape)
	if _, ok := e.currentState.(*TitleState); !ok {
		t.Fatal("runtime episode did not return to title")
	}
}
