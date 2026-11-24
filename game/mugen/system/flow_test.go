package main

import (
	"bytes"
	"os"
	"path/filepath"
	"slices"
	"strings"
	"testing"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/motif"
)

type flowKeyboard struct{ keys, previous map[ebiten.Key]bool }

func (k *flowKeyboard) IsPressed(key ebiten.Key) bool     { return k.keys[key] }
func (k *flowKeyboard) IsJustPressed(key ebiten.Key) bool { return k.keys[key] && !k.previous[key] }

// Replaces only the hardware boundary. Configuration, manager snapshots,
// screens, CMD input and combat still run through their real consumers.
type flowPad struct {
	standard   bool
	buttons    map[ebiten.StandardGamepadButton]bool
	axes       map[ebiten.StandardGamepadAxis]float64
	rawButtons map[ebiten.GamepadButton]bool
	rawAxes    map[int]float64
	layout     input.LegacyJoystickLayout
}

func (p *flowPad) release() {
	p.buttons = map[ebiten.StandardGamepadButton]bool{}
	p.axes = map[ebiten.StandardGamepadAxis]float64{}
	p.rawButtons = map[ebiten.GamepadButton]bool{}
	p.rawAxes = map[int]float64{}
}

type flowGamepads map[ebiten.GamepadID]*flowPad

func (p flowGamepads) AppendIDs(ids []ebiten.GamepadID) []ebiten.GamepadID {
	for id := range p {
		ids = append(ids, id)
	}
	return ids
}
func (p flowGamepads) StandardLayout(id ebiten.GamepadID) bool { return p[id].standard }
func (p flowGamepads) StandardButton(id ebiten.GamepadID, b ebiten.StandardGamepadButton) bool {
	return p[id].buttons[b]
}
func (p flowGamepads) StandardAxis(id ebiten.GamepadID, a ebiten.StandardGamepadAxis) float64 {
	return p[id].axes[a]
}
func (p flowGamepads) LegacyLayout(id ebiten.GamepadID) (input.LegacyJoystickLayout, bool) {
	layout := p[id].layout
	if layout == (input.LegacyJoystickLayout{}) {
		layout = input.LegacyJoystickLayout{Axes: 2, Buttons: 4}
	}
	return layout, true
}
func (p flowGamepads) ButtonCount(id ebiten.GamepadID) int {
	layout, _ := p.LegacyLayout(id)
	return layout.Buttons + layout.Hats*4
}
func (p flowGamepads) Button(id ebiten.GamepadID, b ebiten.GamepadButton) bool {
	return p[id].rawButtons[b]
}
func (p flowGamepads) AxisCount(id ebiten.GamepadID) int {
	layout, _ := p.LegacyLayout(id)
	return layout.Axes
}
func (p flowGamepads) Axis(id ebiten.GamepadID, a int) float64 { return p[id].rawAxes[a] }

func TestTitleSelectionVersusFightResultRematchAndExit(t *testing.T) {
	root, err := filepath.Abs("../temp/mugen-1.1b1")
	if err != nil {
		t.Fatal(err)
	}
	override := filepath.Join(t.TempDir(), "mugen.yaml")
	if err := os.WriteFile(override, []byte(`# Hand-edited configuration
video:
  fullscreen: false # keep this comment
  custom_video_hint: retained
sound:
  master_volume: 100
extension:
  nested: {marker: untouched}
input:
  p1:
    keyboard_enabled: true
    gamepad:
      enabled: true
      deadzone: 0.4
    joy:
      jump: [dpad_up, left_y-]
      crouch: [dpad_down, left_y+]
      left: [dpad_left, left_x-]
      right: [dpad_right, left_x+]
      start: start
      escape: back
      x: [west, "button:2", west]
      y: [north, not-a-control]
      z: disabled
    keyboard:
      Up: KeyArrowUp # preserve direction comment
      right: &right_keys [KeyArrowRight, KeyNumpad6]
      left: [KeyArrowLeft, KeyNumpad4]
      a: [KeyComma, KeyF6]
      x: [KeyL, KeyF7, L]
      start: [KeyEnter, KeyNumpadEnter]
      escape: [KeyEscape, KeyF8]
      y: [KeyF9, not-a-key]
      z: none
  p2:
    gamepad: # A blank override section is an editable empty mapping.
    joy:
      a: none
      right: "axis:0+"
      b: "button:0"
    keyboard:
      a: [KeyF, KeyNumpad1]
      b: KeyG
      z: []
extension_alias:
  saved_right: *right_keys
`), 0600); err != nil {
		t.Fatal(err)
	}
	// Native first-value cfg admission is independent of explicit YAML
	// overrides. Reuse the actual configuration text, without copying assets.
	configRoot := t.TempDir()
	if err := os.Mkdir(filepath.Join(configRoot, "data"), 0700); err != nil {
		t.Fatal(err)
	}
	configText, err := os.ReadFile(filepath.Join(root, "data/mugen.cfg"))
	if err != nil {
		t.Fatal(err)
	}
	configText = []byte(strings.ReplaceAll(strings.TrimPrefix(string(configText), "\ufeff"), "UseKeyboard = 1", "UseKeyboard = 0"))
	// CFG native numbering is device-dependent: the first controller has four
	// axes plus a hat; the second has two axes and no hat. Later groups must
	// not replace these explicit enable settings or numeric controls.
	configText = append([]byte("[Input]\nP1.Joystick.type=0\nP2.Joystick.type=1\n[P2 Joystick]\nJump=33\nLeft=35\nA=37\n"), configText...)
	configText = append(configText, []byte("\n[Config]\nGameWidth=0\nGameHeight=0\n[Options]\nMotif=missing-later.def\nLife=1\nTime=0\n[Input]\nP1.UseKeyboard=1\nP2.UseKeyboard=1\n")...)
	if err := os.WriteFile(filepath.Join(configRoot, "data/mugen.cfg"), configText, 0600); err != nil {
		t.Fatal(err)
	}
	legacy, err := config.LoadConfigWithOverrides(configRoot)
	if err != nil {
		t.Fatal(err)
	}
	legacyInput := input.NewInputManager(legacy)
	legacyInput.Keyboard = &flowKeyboard{}
	legacyPad1 := &flowPad{layout: input.LegacyJoystickLayout{Axes: 4, Buttons: 20, Hats: 1}}
	legacyPad2 := &flowPad{}
	legacyPad1.release()
	legacyPad2.release()
	legacyPads := flowGamepads{8: legacyPad1, 23: legacyPad2}
	legacyInput.Gamepads = legacyPads
	legacyPad1.rawButtons[0], legacyPad2.rawButtons[0] = true, true
	legacyInput.Update()
	if legacyInput.P1Gamepad.Enabled || !legacyInput.P2Gamepad.Enabled || legacyInput.IsPressed(1, input.ActionA) || !legacyInput.IsJustPressed(2, input.ActionA) {
		t.Fatal("native enable/code translation did not reach player input")
	}
	legacyInput.P1Gamepad.Enabled = true
	legacyPad1.rawButtons[20] = true // Native 9: first hat up, not SDK button 9.
	legacyPad2.rawAxes[0] = -.8      // Native 35: second device's horizontal negative.
	legacyInput.Update()
	if !legacyInput.IsPressed(1, input.ActionA) || !legacyInput.IsJustPressed(1, input.ActionUp) || !legacyInput.IsPressed(2, input.ActionLeft) || legacyInput.IsJustPressed(2, input.ActionA) {
		t.Fatal("native hat/axis/button input lost its logical ownership/edges")
	}
	legacyPad1.layout.Hats = 2
	legacyPad1.rawButtons[20], legacyPad1.rawButtons[24] = false, true
	legacyInput.Update()
	if !legacyInput.IsPressed(1, input.ActionUp) || legacyInput.IsPressed(1, input.ActionA) {
		t.Fatal("native hat aliases or reserved button offset were treated as fixed SDK indices")
	}
	legacyPad1.layout.Hats = 1
	delete(legacyPads, 8)
	legacyInput.Update()
	if legacyInput.IsPressed(1, input.ActionA) || !legacyInput.IsPressed(2, input.ActionA) {
		t.Fatal("native device ordinal shifted after disconnect")
	}
	legacyPad1.release()
	legacyPads[30] = legacyPad1
	legacyPad1.rawButtons[0] = true
	legacyInput.Update()
	if !legacyInput.IsJustPressed(1, input.ActionA) || legacyInput.IsJustPressed(2, input.ActionA) {
		t.Fatal("native reconnect did not restore the vacant device block")
	}
	cfg, err := config.LoadConfigWithOverrides(configRoot, override)
	if err != nil {
		t.Fatal(err)
	}
	if cfg.Config.GameWidth <= 0 || cfg.Config.GameHeight <= 0 || cfg.Options.Motif == "missing-later.def" {
		t.Fatal("later configuration groups replaced first fields")
	}
	if cfg.Input.KeyboardDisabled != ([2]bool{false, true}) {
		t.Fatal("first cfg keyboard-enable settings or explicit YAML override were ignored", cfg.Input.KeyboardDisabled)
	}
	bindings, _ := cfg.Input.Player1Keyboard.Bindings("x")
	if !slices.Equal(bindings, []ebiten.Key{ebiten.KeyL, ebiten.KeyF7}) || cfg.Input.Player1Keyboard.Y != ebiten.KeySemicolon {
		t.Fatal("YAML alternatives did not deduplicate or reject a malformed override atomically")
	}
	// Snapshot/setter isolation matters because managers and round adapters copy
	// Keys by value. Changing a copy must not rewrite another user's aliases.
	bindings[1] = ebiten.KeyF9
	copied := cfg.Input.Player1Keyboard
	copied.SetBinding("x", ebiten.KeyF10)
	original, _ := cfg.Input.Player1Keyboard.Bindings("x")
	replaced, _ := copied.Bindings("x")
	if !slices.Equal(original, []ebiten.Key{ebiten.KeyL, ebiten.KeyF7}) || !slices.Equal(replaced, []ebiten.Key{ebiten.KeyF10}) {
		t.Fatal("keyboard binding snapshots/setters shared mutable aliases")
	}
	padBindings, _ := cfg.Input.Gamepads[0].Bindings("x")
	padCopy := cfg.Input.Gamepads[0]
	padCopy.SetBindings("x")
	padBindings[0].Index = -1
	padOriginal, _ := cfg.Input.Gamepads[0].Bindings("x")
	padReplaced, _ := padCopy.Bindings("x")
	padY, _ := cfg.Input.Gamepads[0].Bindings("y")
	if len(padOriginal) != 2 || padOriginal[0].Index != int(ebiten.StandardGamepadButtonRightLeft) || len(padReplaced) != 0 || len(padY) != 1 || !cfg.Input.Gamepads[0].Enabled || !cfg.Input.Gamepads[1].Enabled || cfg.Input.Gamepads[0].Deadzone != 0.4 {
		t.Fatal("gamepad configuration lost replacement/isolation or malformed-list admission", padOriginal, padReplaced, padY)
	}
	m, err := motif.LoadMotif("data/mugen1/system.def", root)
	if err != nil {
		t.Fatal(err)
	}
	previous := input.GlobalManager
	t.Cleanup(func() { input.GlobalManager = previous })
	input.GlobalManager = input.NewInputManager(cfg)
	keyboard := &flowKeyboard{}
	input.GlobalManager.Keyboard = keyboard
	pad1, pad2 := &flowPad{standard: true, layout: input.LegacyJoystickLayout{Axes: 4, Buttons: 20, Hats: 1}}, &flowPad{}
	pad1.release()
	pad2.release()
	pads := flowGamepads{8: pad1, 23: pad2}
	input.GlobalManager.Gamepads = pads
	keyboard.keys = map[ebiten.Key]bool{ebiten.KeyQuote: true, ebiten.KeyY: true}
	if input.GlobalManager.IsPressed(1, input.ActionZ) || input.GlobalManager.IsJustPressed(2, input.ActionZ) {
		t.Fatal("disabled scalar/list bindings fell back to legacy controls")
	}
	keyboard.keys = map[ebiten.Key]bool{ebiten.KeyF: true, ebiten.KeyNumpad1: true}
	if input.GlobalManager.IsPressed(2, input.ActionA) {
		t.Fatal("disabled player keyboard reached logical input")
	}
	input.GlobalManager.KeyboardDisabled[1] = false
	if !input.GlobalManager.IsPressed(2, input.ActionA) {
		t.Fatal("both alternatives did not activate the same logical action")
	}
	delete(keyboard.keys, ebiten.KeyF)
	if !input.GlobalManager.IsPressed(2, input.ActionA) || !input.GlobalManager.IsJustPressed(2, input.ActionA) || input.GlobalManager.IsPressed(1, input.ActionA) {
		t.Fatal("releasing an alternative released the action or crossed player ownership")
	}
	delete(keyboard.keys, ebiten.KeyNumpad1)
	if input.GlobalManager.IsPressed(2, input.ActionA) {
		t.Fatal("logical action remained held after both alternatives released")
	}
	input.GlobalManager.KeyboardDisabled[1] = true
	e := &Engine{dataDir: root, cfg: cfg, motif: m, logicalWidth: m.Info.LocalCoord.Width, logicalHeight: m.Info.LocalCoord.Height}
	e.random = engine.NewRandomStream(1)
	t.Cleanup(func() {
		if err := e.closeSound(); err != nil {
			t.Error(err)
		}
	})
	e.currentState = NewTitleState(e)
	previousTPS := ebiten.TPS()
	t.Cleanup(func() { ebiten.SetTPS(previousTPS) })
	audible := false
	var ticks int32
	step := func(keys ...ebiten.Key) {
		t.Helper()
		keyboard.previous = keyboard.keys
		keyboard.keys = map[ebiten.Key]bool{}
		for _, key := range keys {
			keyboard.keys[key] = true
		}
		previousFight, inFight := e.currentState.(*FightState)
		var roots [2]*character.Character
		var previousStates [2]int
		if inFight {
			for slot, player := range previousFight.match.Players {
				roots[slot] = player.Character
				previousStates[slot] = player.Character.StateNo
			}
		}
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
		if inFight && e.currentState == previousFight && !previousFight.match.SimulationSkipped {
			for slot, root := range roots {
				// Stock 5900 clears user variables after the negative pass.
				if previousStates[slot] == 5900 {
					continue
				}
				if root.IntVars[59] != int(ticks) {
					t.Fatal("CNS GameTime lost the process tick", root.IntVars[59], ticks)
				}
			}
		}
		ticks++
		if e.gameTime != ticks {
			t.Fatal("process clock skipped or double-counted an update", e.gameTime, ticks)
		}
		if fight, ok := e.currentState.(*FightState); ok && fight.match.Timeline.GameTime != ticks {
			t.Fatal("match handoff lost the next process tick", fight.match.Timeline.GameTime, ticks)
		}
		pcm := make([]byte, 735*4)
		if _, err := e.sound.mixer.Read(pcm); err != nil {
			t.Fatal(err)
		}
		audible = audible || !bytes.Equal(pcm, make([]byte, len(pcm)))
	}
	title := e.currentState.(*TitleState)
	for index, item := range title.items {
		if item == "options" {
			title.selectIndex = index
			break
		}
	}
	pad1.buttons[ebiten.StandardGamepadButtonCenterRight] = true
	step()
	options, ok := e.currentState.(*OptionsState)
	if !ok {
		t.Fatalf("title did not open options: %T", e.currentState)
	}
	if !input.GlobalManager.IsJustPressed(1, input.ActionEnter) || !input.GlobalManager.IsJustPressed(1, input.ActionStart) {
		t.Fatal("controller edge was consumed by another screen/action query")
	}
	step() // Held start must not confirm again in the destination screen.
	if input.GlobalManager.IsJustPressed(1, input.ActionStart) {
		t.Fatal("held controller button generated another menu edge")
	}
	pad1.release()
	step()
	// Values stay on Options when activated; live settings then reach the
	// ordinary character/match/audio consumers used by the rest of this flow.
	difficulty := cfg.Options.Difficulty
	step(ebiten.KeyEnter)
	if e.currentState != options || cfg.Options.Difficulty == difficulty {
		t.Fatal("Options activation closed the page without changing its value")
	}
	step()
	for n := 0; n < 60; n++ {
		step(ebiten.KeyArrowRight)
	}
	step()
	if cfg.Options.Difficulty != 8 {
		t.Fatal("held utility navigation did not repeat and clamp difficulty")
	}
	for _, index := range []int{1, 2, 4, 5, 6} {
		options.cursor = index
		before := options.items[index].getValue()
		step(ebiten.KeyArrowLeft)
		step()
		if options.items[index].getValue() == before {
			t.Fatal("Options adjustment had no live consumer", options.items[index].label)
		}
	}
	options.cursor = 4
	step(ebiten.KeyArrowLeft) // Below the Max/100 display aliases: actual gain 99.
	step()
	if cfg.Sound.MasterVolume != 99 || cfg.Sound.EffectsVolume() >= .8 || cfg.Sound.MusicVolume() >= .75 {
		t.Fatal("Master Volume did not reach both audio gains")
	}
	// Continue one settings lifecycle through the native team/default actions
	// and the port's explicit session snapshot policy before editing input.
	step(ebiten.KeyF2)
	teamOptions, ok := e.currentState.(*OptionsState)
	if !ok || !teamOptions.team {
		t.Fatal("Options F2 did not open team settings")
	}
	if !cfg.Options.TeamLoseOnKO || teamOptions.items[1].displayValue() != "Team loses" {
		t.Fatal("native KO policy zero did not retain team defeat")
	}
	step()
	step(ebiten.KeyArrowRight)
	step()
	step(ebiten.KeyArrowDown)
	step()
	step(ebiten.KeyArrowRight)
	step()
	if cfg.Options.Team1Vs2Life != 160 || cfg.Options.TeamLoseOnKO {
		t.Fatal("team settings did not reach configuration")
	}
	step(ebiten.KeyArrowDown)
	step()
	step(ebiten.KeyEnter)
	step()
	step(ebiten.KeyArrowRight)
	step()
	step(ebiten.KeyEnter)
	step()
	if cfg.Options.Team1Vs2Life != 200 || !cfg.Options.TeamLoseOnKO {
		t.Fatal("team defaults did not restore native factory values")
	}
	teamOptions.cursor = 0
	for n := 0; n < 4; n++ {
		step(ebiten.KeyArrowLeft)
		step()
	}
	step(ebiten.KeyArrowDown)
	step()
	step(ebiten.KeyArrowRight)
	step()
	step(ebiten.KeyEscape)
	step()
	options.cursor = 9 // Explicitly session-only Load/Save.
	step(ebiten.KeyArrowRight)
	step()
	step(ebiten.KeyEnter)
	step()
	savedSettings := *e.sessionOptions
	options.cursor = 10
	step(ebiten.KeyEnter)
	step()
	step(ebiten.KeyEnter) // No is selected initially.
	step()
	if cfg.Options.Life != savedSettings.life {
		t.Fatal("default confirmation changed settings without Yes")
	}
	step(ebiten.KeyEnter)
	step()
	step(ebiten.KeyArrowRight)
	step()
	step(ebiten.KeyEnter)
	step()
	if cfg.Options.Difficulty != 4 || cfg.Options.Life != 100 || cfg.Options.Time != 99 || cfg.Options.GameSpeed != 0 || cfg.Sound.MasterVolume != 100 || cfg.Sound.WavVolume != 80 || cfg.Sound.BGMVolume != 75 || cfg.Options.Team1Vs2Life != 160 {
		t.Fatal("main defaults did not restore their own settings scope")
	}
	options.cursor = 9
	step(ebiten.KeyArrowLeft)
	step()
	step(ebiten.KeyEnter)
	step()
	if cfg.Options.Life != savedSettings.life || cfg.Options.Time != 80 || cfg.Sound.WavVolume != savedSettings.wav || cfg.Sound.BGMVolume != savedSettings.bgm {
		t.Fatal("session load did not restore settings/audio consumers")
	}
	// Rebind in the real Options/Input UI, then carry that new alternative
	// through the stock match below. Raw navigation also survives disabling P1.
	step(ebiten.KeyF1)
	bindingsMenu, ok := e.currentState.(*InputConfigState)
	if !ok {
		t.Fatalf("Options F1 did not open input configuration: %T", e.currentState)
	}
	// Input editing is another page of the active motif, including during
	// capture and confirmation. Keep its exposed background identical to
	// Options and use the same lettering even without title-menu fonts.
	checkInputMotif := func() {
		t.Helper()
		if width, height := e.Layout(640, 480); width != 640 || height != 480 {
			t.Fatal("input utility lettering did not use the Options window layout", width, height)
		}
		panel, backdrop := ebiten.NewImage(640, 480), ebiten.NewImage(640, 480)
		defer panel.Deallocate()
		defer backdrop.Deallocate()
		fonts := [2]int{e.motif.TitleInfo.ItemFont.Index, e.motif.TitleInfo.ItemActiveFont.Index}
		e.motif.TitleInfo.ItemFont.Index, e.motif.TitleInfo.ItemActiveFont.Index = 0, 0
		e.Draw(panel)
		items := options.items
		options.items = nil
		options.Draw(backdrop)
		options.items = items
		e.motif.TitleInfo.ItemFont.Index, e.motif.TitleInfo.ItemActiveFont.Index = fonts[0], fonts[1]
		visible, background := make([]byte, 640*480*4), make([]byte, 640*480*4)
		panel.ReadPixels(visible)
		backdrop.ReadPixels(background)
		exposed, changed := 0, 0
		for y := 80; y < 400; y++ {
			for x := 0; x < 640; x++ {
				at := (y*640 + x) * 4
				if !bytes.Equal(visible[at:at+4], background[at:at+4]) {
					changed++
					if x < 4 || x >= 636 {
						t.Fatal("input page replaced the motif background")
					}
				} else if background[at] != 0 || background[at+1] != 0 || background[at+2] != 0 {
					exposed++
				}
			}
		}
		if exposed < 1000 || changed < 1000 || bindingsMenu.utility.canvas == nil && bindingsMenu.dialog.canvas == nil {
			t.Fatal("input page lost its motif or readable utility rows", exposed, changed)
		}
	}
	checkInputMotif()
	menuNow := time.Now()
	bindingsMenu.now = func() time.Time { return menuNow }
	step()
	for n := 0; n < 8; n++ {
		step(ebiten.KeyArrowDown)
		step()
	}
	step(ebiten.KeyInsert)
	checkInputMotif()
	step()
	step(ebiten.KeyF10)
	step()
	if !slices.Contains(input.GlobalManager.KeyboardBindings(1, input.ActionX), ebiten.KeyF10) {
		t.Fatal("input editor did not append a keyboard alternative")
	}
	// Continue the same editing journey through an undetected input, timeout
	// and live keyboard test. No sleep or device/parameter matrix is needed.
	beforeTimeout := input.GlobalManager.KeyboardBindings(1, input.ActionX)
	step(ebiten.KeyEnter)
	step()
	menuNow = menuNow.Add(inputCaptureTimeout)
	step()
	if bindingsMenu.waiting || !slices.Equal(beforeTimeout, input.GlobalManager.KeyboardBindings(1, input.ActionX)) {
		t.Fatal("capture timeout changed the binding or left input capture blocked")
	}
	step(ebiten.KeyF4)
	step(ebiten.KeyF10)
	if !bindingsMenu.testing || !bindingsMenu.testSeen || !input.GlobalManager.KeyboardActionPressed(1, input.ActionX) {
		t.Fatal("input tester did not read the live keyboard binding")
	}
	nativePanel := ebiten.NewImage(640, 480)
	e.Draw(nativePanel)
	first := bindingsMenu.utility.first
	step(ebiten.KeyPageDown)
	e.Draw(nativePanel)
	if !bindingsMenu.utility.themed || bindingsMenu.utility.scale != .5 || bindingsMenu.utility.first <= first || bindingsMenu.utility.last <= bindingsMenu.utility.first {
		t.Fatal("motif-sized input diagnostics did not scroll inside the viewport")
	}
	step()
	step(ebiten.KeyPageUp)
	step()
	nativePanel.Deallocate()
	checkInputMotif()
	step(ebiten.KeyF4)
	step()
	for n := 0; n < 8; n++ {
		step(ebiten.KeyArrowUp)
		step()
	}
	step(ebiten.KeyArrowLeft)
	step()
	if !cfg.Input.KeyboardDisabled[0] {
		t.Fatal("input editor did not disable the player keyboard")
	}
	step(ebiten.KeyArrowRight)
	step()
	if cfg.Input.KeyboardDisabled[0] {
		t.Fatal("raw configuration navigation could not restore disabled input")
	}
	step(ebiten.KeyTab)
	step()
	for n := 0; n < 6; n++ {
		step(ebiten.KeyArrowDown)
		step()
	}
	step(ebiten.KeyEnter)
	step()
	step(ebiten.KeyF11)
	step()
	step(ebiten.KeyDelete)
	step()
	if bindingsMenu.player != 2 || len(input.GlobalManager.KeyboardBindings(2, input.ActionB)) != 0 {
		t.Fatal("input editor lost player ownership or disabled binding")
	}
	step(ebiten.KeyTab)
	step()
	step(ebiten.KeyF2)
	step()
	step(ebiten.KeyArrowLeft)
	step()
	step(ebiten.KeyArrowRight)
	step()
	step(ebiten.KeyArrowDown)
	step()
	step(ebiten.KeyArrowRight)
	step()
	if !cfg.Input.Gamepads[0].Enabled || cfg.Input.Gamepads[0].Deadzone != 0.45 {
		t.Fatal("gamepad pane did not apply device/deadzone settings")
	}
	for n := 0; n < 8; n++ {
		step(ebiten.KeyArrowDown)
		step()
	}
	step(ebiten.KeyInsert)
	step()
	pad1.rawButtons[1] = true
	step()
	pad1.release()
	step()
	controls, _ := cfg.Input.Gamepads[0].Bindings("x")
	if !slices.Contains(controls, config.GamepadControl{Kind: config.PadButton, Index: 1}) {
		t.Fatal("gamepad editor could not capture an unbound raw control")
	}
	// Standard-map availability must not hide raw-only axes/buttons. The
	// tester distinguishes physical signals from mapped actions and ignores
	// the keyboard while testing a controller.
	step(ebiten.KeyInsert)
	step()
	pad1.rawAxes[3] = .9
	step()
	pad1.release()
	step()
	controls, _ = cfg.Input.Gamepads[0].Bindings("x")
	if !slices.Contains(controls, config.GamepadControl{Kind: config.PadAxis, Index: 3, Positive: true}) {
		t.Fatal("standard mapping hid an unmapped raw axis from capture")
	}
	step(ebiten.KeyF4)
	pad1.rawButtons[3] = true
	step(ebiten.KeyF10)
	if !bindingsMenu.testSeen || input.GlobalManager.GamepadActionPressed(1, input.ActionX) {
		t.Fatal("controller diagnostics confused a raw signal or keyboard with its mapped action")
	}
	foundRaw := false
	for _, row := range bindingsMenu.testRows() {
		foundRaw = foundRaw || row.label == "Buttons" && row.value == "3"
	}
	if !foundRaw {
		t.Fatal("input tester hid the unbound raw button")
	}
	checkInputMotif()
	pad1.release()
	step(ebiten.KeyF4)
	step()
	step(ebiten.KeyF6)
	step(ebiten.KeyInsert)
	pad1.buttons[ebiten.StandardGamepadButtonCenterCenter] = true // Broken map reports a permanently held button.
	step()
	pad1.rawButtons[3] = true
	step()
	pad1.release()
	step()
	controls, _ = cfg.Input.Gamepads[0].Bindings("x")
	if bindingsMenu.waiting || !slices.Contains(controls, config.GamepadControl{Kind: config.PadButton, Index: 3}) {
		t.Fatal("explicit raw capture could not bypass the standard mapping")
	}
	// A nonstandard trigger may rest at -1. Capture learns that neutral value
	// and stores the positive press direction, rather than binding the rest.
	pad2.rawAxes[1] = -1
	step(ebiten.KeyTab)
	step()
	step(ebiten.KeyEnter)
	step()
	pad2.rawAxes[1] = 1
	step()
	pad2.rawAxes[1] = -1
	step()
	controls, _ = cfg.Input.Gamepads[1].Bindings("x")
	if !slices.Equal(controls, []config.GamepadControl{{Kind: config.PadAxis, Index: 1, Positive: true}}) || bindingsMenu.waiting || bindingsMenu.settle {
		t.Fatal("raw-axis capture mistook its neutral baseline for a held control", controls)
	}
	pad2.release()
	checkInputMotif()
	step(ebiten.KeyEscape)
	step()
	if !bindingsMenu.confirmExit || e.currentState != bindingsMenu {
		t.Fatal("changed inputs left the editor without a save prompt")
	}
	checkInputMotif()
	step(ebiten.KeyArrowLeft) // Back returns to editing.
	step()
	step(ebiten.KeyEnter)
	step()
	if bindingsMenu.confirmExit || e.currentState != bindingsMenu {
		t.Fatal("Back did not retain the editing session")
	}
	step(ebiten.KeyEscape)
	step()
	step(ebiten.KeyArrowRight) // Session only keeps changes live without writing.
	step()
	step(ebiten.KeyEnter)
	step()
	if e.currentState != options {
		t.Fatal("input editor did not return to the existing Options state")
	}
	options.cursor = 9
	step(ebiten.KeyArrowRight)
	step()
	step(ebiten.KeyEnter) // Save the live input edits too.
	step()
	step(ebiten.KeyF1)
	bindingsMenu = e.currentState.(*InputConfigState)
	step()
	step(ebiten.KeyF3)
	checkInputMotif()
	step()
	step(ebiten.KeyArrowRight)
	step()
	step(ebiten.KeyEnter)
	step()
	if slices.Contains(input.GlobalManager.KeyboardBindings(1, input.ActionX), ebiten.KeyF10) {
		t.Fatal("input defaults did not restore startup bindings")
	}
	step(ebiten.KeyEscape)
	step()
	step(ebiten.KeyArrowRight) // Keep the startup reset session-only too.
	step()
	step(ebiten.KeyEnter)
	step()
	options.cursor = 9
	step(ebiten.KeyArrowLeft)
	step()
	step(ebiten.KeyEnter)
	step()
	if !slices.Contains(input.GlobalManager.KeyboardBindings(1, input.ActionX), ebiten.KeyF10) {
		t.Fatal("session load lost keyboard alternatives")
	}
	if contents, err := os.ReadFile(override); err != nil || !bytes.Contains(contents, []byte("not-a-control")) || bytes.Contains(contents, []byte("F10")) {
		t.Fatal("session edits rewrote the user's YAML", err)
	}
	// Explicit persistence merges into the complete document and backs up its
	// original bytes. A rejected save must leave both editing and disk intact.
	originalYAML, _ := os.ReadFile(override)
	step(ebiten.KeyF1)
	bindingsMenu = e.currentState.(*InputConfigState)
	step()
	brokenYAML := []byte("input: [incomplete")
	if err := os.WriteFile(override, brokenYAML, 0600); err != nil {
		t.Fatal(err)
	}
	step(ebiten.KeyF5)
	if actual, _ := os.ReadFile(override); !bytes.Equal(actual, brokenYAML) || bindingsMenu.message != "Save failed; see console" {
		t.Fatal("failed input save replaced the original YAML or reported success")
	}
	if _, err := os.Stat(override + ".bak"); !os.IsNotExist(err) {
		t.Fatal("failed YAML validation replaced the backup", err)
	}
	if err := os.WriteFile(override, originalYAML, 0600); err != nil {
		t.Fatal(err)
	}
	step()
	if err := os.Mkdir(override+".bak", 0700); err != nil {
		t.Fatal(err)
	}
	step(ebiten.KeyF5)
	if actual, _ := os.ReadFile(override); !bytes.Equal(actual, originalYAML) || bindingsMenu.message != "Save failed; see console" {
		t.Fatal("backup failure replaced the original YAML or reported success")
	}
	if err := os.Remove(override + ".bak"); err != nil {
		t.Fatal(err)
	}
	step()
	step(ebiten.KeyF5)
	step()
	backup, _ := os.ReadFile(override + ".bak")
	savedYAML, _ := os.ReadFile(override)
	if !bytes.Equal(backup, originalYAML) || !bytes.Contains(savedYAML, []byte("# keep this comment")) || !bytes.Contains(savedYAML, []byte("custom_video_hint: retained")) || !bytes.Contains(savedYAML, []byte("marker: untouched")) || !bytes.Contains(savedYAML, []byte("# preserve direction comment")) || bytes.Contains(savedYAML, []byte("      Up:")) {
		t.Fatal("input save lost other configuration/comments or the complete original backup")
	}
	reloaded, err := config.LoadConfigWithOverrides(configRoot, override)
	if err != nil {
		t.Fatal(err)
	}
	persisted, _ := reloaded.Input.Player1Keyboard.Bindings("x")
	persistedPad, _ := reloaded.Input.Gamepads[0].Bindings("x")
	expectedPad, _ := cfg.Input.Gamepads[0].Bindings("x")
	if !slices.Contains(persisted, ebiten.KeyF10) || !slices.Equal(persistedPad, expectedPad) || reloaded.Sound.MasterVolume != 100 || bindingsMenu.inputsChanged() {
		t.Fatal("saved input did not round-trip or changed unrelated audio settings")
	}
	step(ebiten.KeyEscape) // F5 already saved: no second confirmation.
	step()
	if e.currentState != options || bindingsMenu.confirmExit {
		t.Fatal("explicit Save Inputs did not clear the exit confirmation")
	}
	options.cursor = 9
	step(ebiten.KeyArrowRight)
	step()
	step(ebiten.KeyEnter) // Main Options Save remains a memory snapshot.
	step()
	if actual, _ := os.ReadFile(override); !bytes.Equal(actual, savedYAML) {
		t.Fatal("main Options session save rewrote persistent configuration")
	}
	step(ebiten.KeyF1)
	bindingsMenu = e.currentState.(*InputConfigState)
	step()
	step(ebiten.KeyEscape)
	step()
	if e.currentState != options {
		t.Fatal("reopening saved inputs invented unsaved changes")
	}
	step(ebiten.KeyF1)
	bindingsMenu = e.currentState.(*InputConfigState)
	step()
	bindingsMenu.cursor = bindingsMenu.offset() + 7
	step(ebiten.KeyInsert)
	step()
	step(ebiten.KeyF11)
	step()
	step(ebiten.KeyEscape)
	step()
	step(ebiten.KeyNumpadEnter) // Save from the exit prompt, using the other Enter key.
	for n := 0; n < 3; n++ {
		step(ebiten.KeyNumpadEnter) // Held confirmation must not reopen the child.
		if e.currentState != options {
			t.Fatal("held save confirmation reopened input configuration")
		}
	}
	step()
	backup, _ = os.ReadFile(override + ".bak")
	if e.currentState != options || !bytes.Equal(backup, savedYAML) {
		t.Fatal("save-on-exit did not preserve the previous full document")
	}
	// Native-size menu lettering scrolls to the selected row; fontless themes
	// still retain the readable fallback below.
	optionCanvas := ebiten.NewImage(e.logicalWidth, e.logicalHeight)
	e.Draw(optionCanvas)
	if !options.utility.themed || options.utility.scale != 1 {
		t.Fatal("Options did not retain the title menu font's fitted size")
	}
	optionPixels := make([]byte, e.logicalWidth*e.logicalHeight*4)
	optionCanvas.ReadPixels(optionPixels)
	items := options.items
	options.items = nil
	e.Draw(optionCanvas)
	optionBase := make([]byte, len(optionPixels))
	optionCanvas.ReadPixels(optionBase)
	options.items = items
	optionCanvas.Deallocate()
	leftOption, rightOption := e.logicalWidth, -1
	for at := 0; at < len(optionPixels); at += 4 {
		if !bytes.Equal(optionPixels[at:at+4], optionBase[at:at+4]) {
			x := (at / 4) % e.logicalWidth
			leftOption = min(leftOption, x)
			rightOption = max(rightOption, x)
		}
	}
	centerOption := (leftOption + rightOption) / 2
	if rightOption < leftOption || centerOption < e.logicalWidth*4/10 || centerOption > e.logicalWidth*6/10 {
		t.Fatal("Options rows did not follow motif width", leftOption, rightOption)
	}
	// The HD pack authors its title labels as art and supplies no menu font.
	// Its settings list must remain readable and centered independently.
	fontIndices := [2]int{e.motif.TitleInfo.ItemFont.Index, e.motif.TitleInfo.ItemActiveFont.Index}
	e.motif.TitleInfo.ItemFont.Index, e.motif.TitleInfo.ItemActiveFont.Index = 0, 0
	fallback := ebiten.NewImage(e.logicalWidth, e.logicalHeight)
	e.Draw(fallback)
	fallback.ReadPixels(optionPixels)
	options.items = nil
	e.Draw(fallback)
	fallback.ReadPixels(optionBase)
	options.items = items
	fallback.Deallocate()
	changedOptions := 0
	for at := 0; at < len(optionPixels); at += 4 {
		if !bytes.Equal(optionPixels[at:at+4], optionBase[at:at+4]) {
			changedOptions++
		}
	}
	e.motif.TitleInfo.ItemFont.Index, e.motif.TitleInfo.ItemActiveFont.Index = fontIndices[0], fontIndices[1]
	if changedOptions < 1000 || options.utility.canvas == nil {
		t.Fatal("fontless screenpack left Options rows invisible", changedOptions)
	}
	for index, item := range options.items {
		if item.label == "Game Speed" {
			options.cursor = index
			break
		}
	}
	pad1.axes[ebiten.StandardGamepadAxisLeftStickHorizontal] = 0.3
	step()
	if ebiten.TPS() != previousTPS {
		t.Fatal("analog noise crossed the configured deadzone")
	}
	pad1.axes[ebiten.StandardGamepadAxisLeftStickHorizontal] = 0.8
	step()
	pad1.release()
	if ebiten.TPS() != e.ticksPerSecond() || ebiten.TPS() == previousTPS {
		t.Fatal("Game Speed option did not update the live scheduler", ebiten.TPS(), e.ticksPerSecond())
	}
	step()
	keyboardDisabled := input.GlobalManager.KeyboardDisabled[0]
	input.GlobalManager.KeyboardDisabled[0] = true
	options.cursor = len(options.items) - 2
	step(ebiten.KeyArrowDown)
	step()
	if options.cursor != len(options.items)-1 {
		t.Fatal("disabled/rebound player keys trapped Options navigation")
	}
	step(ebiten.KeyNumpadEnter)
	for n := 0; n < 20 && e.currentState == options; n++ {
		step()
	}
	if _, ok := e.currentState.(*TitleState); !ok {
		t.Fatal("options did not return to title")
	}
	input.GlobalManager.KeyboardDisabled[0] = keyboardDisabled
	for i := 0; i < 37; i++ {
		step()
	}
	pad1.buttons[ebiten.StandardGamepadButtonLeftBottom] = true
	step()
	pad1.release()
	pad1.buttons[ebiten.StandardGamepadButtonCenterRight] = true
	step()
	pad1.release()
	s, ok := e.currentState.(*SelectState)
	if !ok {
		t.Fatalf("title did not open selection: %T", e.currentState)
	}
	// The 320p KFM and 1280p KFM720 portraits must occupy the same 100px
	// authored cells in this 1280p motif, rather than shrinking the first.
	for _, portrait := range s.chars[:2] {
		canvas := ebiten.NewImage(128, 128)
		portrait.drawPortrait(canvas, [2]int{9000, 0}, [2]float64{4, 4}, [2]float64{1, 1}, 1, e.logicalWidth, nil)
		pixels := make([]byte, 128*128*4)
		canvas.ReadPixels(pixels)
		canvas.Deallocate()
		left, right := 128, 0
		for y := 0; y < 128; y++ {
			for x := 0; x < 128; x++ {
				if pixels[(y*128+x)*4+3] > 0 {
					left = min(left, x)
					right = max(right, x)
				}
			}
		}
		if right-left < 90 {
			t.Fatal("mixed-coordinate selection portrait shrank", portrait.Name, left, right)
		}
	}
	// Start both on the first stock cell. Raw controls work on a device without
	// a standard mapping; a disabled P1 action must not activate its default.
	s.p1CursorX, s.p1CursorY, s.p2CursorX, s.p2CursorY = 0, 0, 0, 0
	pad1.buttons[ebiten.StandardGamepadButtonFrontTopLeft] = true
	pad2.rawAxes[0] = 0.9
	step()
	if input.GlobalManager.IsPressed(1, input.ActionZ) || s.p1Done || s.p2Done {
		t.Fatal("disabled controller binding confirmed a selection")
	}
	pad1.release()
	pad2.release()
	step(ebiten.KeyF6)
	if s.p2Done {
		t.Fatal("P1 confirmation also confirmed P2")
	}
	pad2.rawButtons[0] = true
	step()
	pad2.release()
	if !s.choosingStage {
		t.Fatal("confirmed players did not reach stage selection")
	}
	step(ebiten.KeyNumpad6)
	step(ebiten.KeyNumpadEnter)
	v, ok := e.currentState.(*VersusState)
	if !ok {
		t.Fatalf("stage selection did not open versus: %T", e.currentState)
	}
	if !strings.HasSuffix(v.request.Players[0].DefPath, "kfm/kfm.def") || !strings.HasSuffix(v.request.Players[1].DefPath, "kfm720/kfm720.def") || v.request.Players[0].Palette != 4 || v.request.Players[1].Palette != 5 || v.request.StagePath != "stages/stage0-720.def" {
		t.Fatal("selection handoff lost metadata", v.request)
	}
	// Exercise the actual VS renderer before handing off. The former flow
	// checked only its type, allowing an empty names-only screen to pass.
	canvas := ebiten.NewImage(e.logicalWidth, e.logicalHeight)
	v.ticks = m.VSScreen.FadeInTime + 1
	v.Draw(canvas)
	withPortraits := make([]byte, e.logicalWidth*e.logicalHeight*4)
	canvas.ReadPixels(withPortraits)
	portraits := v.portraits
	v.portraits = [2]*CharacterPortrait{}
	v.Draw(canvas)
	background := make([]byte, len(withPortraits))
	canvas.ReadPixels(background)
	canvas.Deallocate()
	v.portraits = portraits
	var changed [2]int
	backgroundPixels := 0
	for y := 0; y < e.logicalHeight; y++ {
		for x := 0; x < e.logicalWidth; x++ {
			i := (y*e.logicalWidth + x) * 4
			if background[i] != 0 || background[i+1] != 0 || background[i+2] != 0 {
				backgroundPixels++
			}
			if bytes.Equal(withPortraits[i:i+4], background[i:i+4]) {
				continue
			}
			side := 0
			if x >= e.logicalWidth/2 {
				side = 1
			}
			window := m.VSScreen.P1Portrait.Window
			if side == 1 {
				window = m.VSScreen.P2Portrait.Window
			}
			if window == nil || x < window[0] || x > window[2] || y < window[1] || y > window[3] {
				t.Fatal("VS portrait escaped its authored window", x, y)
			}
			changed[side]++
		}
	}
	if backgroundPixels < 50000 || changed[0] < 1000 || changed[1] < 1000 {
		t.Fatal("VS background or portraits missing", backgroundPixels, changed)
	}
	step() // Release keyboard Start before pressing its controller alternative.
	portableStart, _ := input.GlobalManager.P1Gamepad.Bindings("start")
	nativeStart, _ := legacy.Input.Gamepads[0].Bindings("start")
	input.GlobalManager.P1Gamepad.SetBindings("start", nativeStart...)
	pad1.rawButtons[11] = true // Native 24, after four axes and one hat.
	step()
	pad1.release()
	f, ok := e.currentState.(*FightState)
	if !ok {
		t.Fatalf("versus did not enter fight: %T", e.currentState)
	}
	input.GlobalManager.P1Gamepad.SetBindings("start", portableStart...)
	watchGameTime(t, f)
	if f.match.Options.TicksPerSecond != int32(ebiten.TPS()) {
		t.Fatal("match did not inherit the live Game Speed scheduler")
	}
	if f.match.Options.LifePercent != cfg.Options.Life || f.match.Options.RoundTime != cfg.Options.Time || f.match.Players[0].Character.MaxLife != f.match.Players[0].Character.StateFile.Data.Life*cfg.Options.Life/100 {
		t.Fatal("Options life/time changes did not reach the selected match")
	}
	if f.match.Options.Team1Vs2Life != 160 || f.match.Options.TeamLoseOnKO {
		t.Fatal("team Options did not reach the match consumer")
	}
	if f.match.World.RandomSeed() != 16807 {
		t.Fatal("session seed was not handed to production match")
	}
	incoming, broken := false, false
	for i := 0; i < 600 && f.match.Phase != match.Active; i++ {
		step()
		for _, explod := range f.match.Players[0].Character.Explods {
			broken = broken || explod.Anim == 192
			if explod.Anim != 191 || explod.CurrentTime != 12 {
				continue
			}
			if explod.VelX >= -4 || explod.X >= f.match.Players[0].Character.StartX+260 {
				t.Fatal("incoming intro block ignored legacy velocity", explod.VelX, explod.X)
			}
			// Check the real stage/actor/effect composition before the kick,
			// rather than merely checking that the Explod was allocated.
			canvas := ebiten.NewImage(e.logicalWidth, e.logicalHeight)
			f.Draw(canvas)
			visible := make([]byte, e.logicalWidth*e.logicalHeight*4)
			canvas.ReadPixels(visible)
			explod.Visual.Active = false
			f.Draw(canvas)
			without := make([]byte, len(visible))
			canvas.ReadPixels(without)
			explod.Visual.Active = true
			canvas.Deallocate()
			if bytes.Equal(visible, without) {
				t.Fatal("incoming intro block was offscreen or covered before impact")
			}
			incoming = true
		}
	}
	if f.match.Phase != match.Active || !incoming || !broken {
		t.Fatal("intro flight/break/control handoff incomplete", f.match.Phase, incoming, broken)
	}
	if !audible || !f.hud.played["round1"] || !f.hud.played["fight"] {
		t.Fatal("menu/round/fight sound did not reach production PCM", audible, f.hud.played)
	}
	for _, p := range f.match.Players {
		if p.Character.OwnSounds != p.SND || p.Character.CommonSounds != f.match.Fight.CommonSounds || p.Character.SoundQueue != e.sound.queue {
			t.Fatal("match sound resources were lost")
		}
	}
	// Win one round with keyboard alternatives, then one with controller input
	// through the same stock CNS, contact, recovery and round-reset lifecycle.
	// Keep initial positions, states, life, timer and round timing intact.
	damagedRounds := map[int]bool{}
	editedX, _ := input.GlobalManager.P1Gamepad.Bindings("x")
	nativeX, _ := legacy.Input.Gamepads[0].Bindings("x")
	for tick := 0; tick < 12000 && f.match.Phase != match.Finished; tick++ {
		pad1.release()
		if f.match.Phase != match.Active {
			step()
			continue
		}
		c := f.match.Players[0].Character
		distance, _ := c.GetOpponentBodyDistance()
		if distance > 16*float64(c.LocalCoordinateWidth())/320 {
			if f.match.Round == 2 {
				pad1.axes[ebiten.StandardGamepadAxisLeftStickHorizontal] = float64(c.Facing)
				step()
			} else if c.Facing == 1 {
				step(ebiten.KeyNumpad6)
			} else {
				step(ebiten.KeyNumpad4)
			}
		} else if f.match.Tick%40 < 2 {
			if f.match.Round == 1 {
				step(ebiten.KeyF10) // Alternative edited in the Options UI.
			} else {
				input.GlobalManager.P1Gamepad.SetBindings("x", nativeX...)
				pad1.rawButtons[2] = true // Native 15 reaches the same CMD/CNS flow.
				step()
			}
		} else {
			step()
		}
		victim := f.match.Players[1].Character
		if victim.Life < victim.MaxLife {
			damagedRounds[f.match.Round] = true
		}
	}
	if !damagedRounds[1] || !damagedRounds[2] || f.match.Wins != [2]int{2, 0} {
		t.Fatal("keyboard/controller input did not win both rounds", damagedRounds, f.match.Wins)
	}
	input.GlobalManager.P1Gamepad.SetBindings("x", editedX...)
	pad1.release()
	if f.match.World.RandomSeed() != 16807 {
		t.Fatal("round reset reseeded the match")
	}
	if f.match.Phase != match.Finished {
		t.Fatal("match did not reach result")
	}
	if !f.hud.played["round2"] || !f.hud.played["KO"] {
		t.Fatal("round reset lost announcement lifecycle", f.hud.played)
	}
	for i := 0; i < 23; i++ {
		step() // Finished matches keep the process clock running.
	}
	step(ebiten.KeyEnter)
	rematch, ok := e.currentState.(*FightState)
	if !ok || rematch == f || rematch.match.Round != 1 || rematch.match.Wins != [2]int{} {
		t.Fatal("rematch did not create fresh match")
	}
	if rematch.match.Timeline == f.match.Timeline || rematch.match.Timeline.GameTime == 0 {
		t.Fatal("rematch must retain process time with a fresh simulation timeline")
	}
	watchGameTime(t, rematch)
	if rematch.match.World.RandomSeed() != 282475249 {
		t.Fatal("rematch did not advance the session RNG")
	}
	for i := 0; i < 150; i++ {
		step()
	}
	step(ebiten.KeyF8)
	if _, ok := e.currentState.(*TitleState); !ok {
		t.Fatalf("escape did not return to title: %T", e.currentState)
	}
	for i := 0; i < 41; i++ {
		step()
	}
	// Hotplug keeps a connected P2 in its seat, then fills the vacant P1 seat.
	delete(pads, 8)
	pad2.rawButtons[0] = true
	step()
	if !input.GlobalManager.IsPressed(2, input.ActionB) || input.GlobalManager.IsPressed(1, input.ActionB) {
		t.Fatal("disconnect reassigned the remaining player's controller")
	}
	pad1.release()
	pads[30] = pad1
	pad1.buttons[ebiten.StandardGamepadButtonRightLeft] = true
	step()
	if !input.GlobalManager.IsPressed(1, input.ActionX) || input.GlobalManager.IsJustPressed(2, input.ActionB) {
		t.Fatal("hotplug did not fill the free seat or repeated a held P2 edge")
	}
	pad2.release()
	pad1.release()
	step(ebiten.KeyF7)
	pad1.buttons[ebiten.StandardGamepadButtonRightLeft] = true
	step(ebiten.KeyF7)
	if !input.GlobalManager.IsPressed(1, input.ActionX) || input.GlobalManager.IsJustPressed(1, input.ActionX) {
		t.Fatal("adding a controller to a held keyboard action created a new edge")
	}
	pad1.release()
	step()
}

func TestSelectionReportsRosterFailure(t *testing.T) {
	m := &motif.Motif{}
	m.Files.Select = filepath.Join(t.TempDir(), "absent-select.def")
	s := NewSelectState(&Engine{motif: m}, "Versus Mode")
	if _, err := s.Update(); err == nil || !strings.Contains(err.Error(), "absent-select.def") {
		t.Fatal("roster failure was silently dropped", err)
	}
}
