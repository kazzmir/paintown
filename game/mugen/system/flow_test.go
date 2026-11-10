package main

import (
	"bytes"
	"os"
	"path/filepath"
	"slices"
	"strings"
	"testing"

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
func (p flowGamepads) ButtonCount(id ebiten.GamepadID) int { return 4 }
func (p flowGamepads) Button(id ebiten.GamepadID, b ebiten.GamepadButton) bool {
	return p[id].rawButtons[b]
}
func (p flowGamepads) AxisCount(id ebiten.GamepadID) int       { return 2 }
func (p flowGamepads) Axis(id ebiten.GamepadID, a int) float64 { return p[id].rawAxes[a] }

func TestTitleSelectionVersusFightResultRematchAndExit(t *testing.T) {
	root, err := filepath.Abs("../temp/mugen-1.1b1")
	if err != nil {
		t.Fatal(err)
	}
	override := filepath.Join(t.TempDir(), "mugen.yaml")
	if err := os.WriteFile(override, []byte(`input:
  p1:
    keyboard_enabled: true
    gamepad:
      enabled: true
      deadzone: 0.4
    joy:
      x: [west, "button:2", west]
      y: [north, not-a-control]
      z: disabled
    keyboard:
      right: [KeyArrowRight, KeyNumpad6]
      left: [KeyArrowLeft, KeyNumpad4]
      a: [KeyComma, KeyF6]
      x: [KeyL, KeyF7, L]
      start: [KeyEnter, KeyNumpadEnter]
      escape: [KeyEscape, KeyF8]
      y: [KeyF9, not-a-key]
      z: none
  p2:
    joy:
      right: "axis:0+"
      b: "button:0"
    keyboard:
      a: [KeyF, KeyNumpad1]
      b: KeyG
      z: []
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
	configText = []byte(strings.ReplaceAll(string(configText), "UseKeyboard = 1", "UseKeyboard = 0"))
	configText = append(configText, []byte("\n[Config]\nGameWidth=0\nGameHeight=0\n[Options]\nMotif=missing-later.def\nLife=1\nTime=0\n[Input]\nP1.UseKeyboard=1\nP2.UseKeyboard=1\n")...)
	if err := os.WriteFile(filepath.Join(configRoot, "data/mugen.cfg"), configText, 0600); err != nil {
		t.Fatal(err)
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
	pad1, pad2 := &flowPad{standard: true}, &flowPad{}
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
	// Rebind in the real Options/Input UI, then carry that new alternative
	// through the stock match below. Raw navigation also survives disabling P1.
	step(ebiten.KeyF1)
	bindingsMenu, ok := e.currentState.(*InputConfigState)
	if !ok {
		t.Fatalf("Options F1 did not open input configuration: %T", e.currentState)
	}
	step()
	for n := 0; n < 8; n++ {
		step(ebiten.KeyArrowDown)
		step()
	}
	step(ebiten.KeyInsert)
	step()
	step(ebiten.KeyF10)
	step()
	if !slices.Contains(input.GlobalManager.KeyboardBindings(1, input.ActionX), ebiten.KeyF10) {
		t.Fatal("input editor did not append a keyboard alternative")
	}
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
	panel := ebiten.NewImage(640, 480)
	e.Draw(panel)
	panel.Deallocate()
	step(ebiten.KeyEscape)
	step()
	if e.currentState != options {
		t.Fatal("input editor did not return to the existing Options state")
	}
	// HD motif options use their authored space, including the title font and
	// uppercase-only menu bank, rather than a 320px fixed left-hand anchor.
	optionCanvas := ebiten.NewImage(e.logicalWidth, e.logicalHeight)
	e.Draw(optionCanvas)
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
	step(ebiten.KeyEscape)
	if _, ok := e.currentState.(*TitleState); !ok {
		t.Fatal("options did not return to title")
	}
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
	if !strings.HasSuffix(v.request.Players[0].DefPath, "kfm/kfm.def") || !strings.HasSuffix(v.request.Players[1].DefPath, "kfm720/kfm720.def") || v.request.Players[1].Palette != 2 || v.request.StagePath != "stages/stage0-720.def" {
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
	pad1.buttons[ebiten.StandardGamepadButtonCenterRight] = true
	step()
	pad1.release()
	f, ok := e.currentState.(*FightState)
	if !ok {
		t.Fatalf("versus did not enter fight: %T", e.currentState)
	}
	watchGameTime(t, f)
	if f.match.Options.TicksPerSecond != int32(ebiten.TPS()) {
		t.Fatal("match did not inherit the live Game Speed scheduler")
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
				pad1.rawButtons[1] = true // Alternative edited in the Options UI.
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
