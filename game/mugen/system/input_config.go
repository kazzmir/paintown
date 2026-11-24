package main

import (
	"fmt"
	"math"
	"reflect"
	"strings"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
)

var inputConfigActions = []input.Action{input.ActionJump, input.ActionCrouch, input.ActionLeft, input.ActionRight, input.ActionA, input.ActionB, input.ActionC, input.ActionX, input.ActionY, input.ActionZ, input.ActionStart, input.ActionEscape}

const inputCaptureTimeout = 4 * time.Second

// Session edits use YAML's immutable binding setters and never rewrite a user
// file. Raw navigation survives disabled input and rebinding menu controls.
type InputConfigState struct {
	engine                        *Engine
	back                          State
	player, cursor                int
	gamepad                       bool
	waiting, appendBinding, armed bool
	settle                        bool
	baseline                      input.GamepadSnapshot
	background                    *ebiten.Image
	utility, dialog               utilityMenu
	confirmDefaults, defaultsYes  bool
	rawCapture, testing, testSeen bool
	testBaseline                  input.GamepadSnapshot
	now                           func() time.Time
	captureDeadline, messageUntil time.Time
	message                       string
	savedInput                    *optionSnapshot
	confirmExit                   bool
	exitChoice                    int
}

func NewInputConfigState(engine *Engine, back State) *InputConfigState {
	if engine.initialOptions == nil {
		engine.initialOptions = engine.snapshotOptions()
	}
	return &InputConfigState{engine: engine, back: back, player: 1, now: time.Now, savedInput: engine.snapshotOptions()}
}

func (s *InputConfigState) captureState() input.GamepadSnapshot {
	state := input.GlobalManager.GamepadSnapshot(s.player)
	if s.rawCapture {
		return state.Raw()
	}
	return state
}

func (s *InputConfigState) notify(message string) {
	s.message, s.messageUntil = message, s.now().Add(3*time.Second)
}

func (s *InputConfigState) startTest() {
	s.testing, s.testSeen = true, false
	s.testBaseline = input.GlobalManager.GamepadSnapshot(s.player).Raw()
}

func (s *InputConfigState) save() bool {
	// The manager owns live edits; persist a value snapshot without changing
	// the session Load/Save or startup-reset snapshots.
	cfg := *s.engine.cfg
	live := s.engine.snapshotOptions()
	cfg.Input.Player1Keyboard, cfg.Input.Player2Keyboard = live.keys[0], live.keys[1]
	cfg.Input.Gamepads, cfg.Input.KeyboardDisabled = live.pads, live.keyboardDisabled
	backup, err := cfg.SaveInputConfig()
	if err != nil {
		fmt.Printf("Input configuration save failed: %v\n", err)
		s.notify("Save failed; see console")
		return false
	} else if backup != "" {
		s.notify("Saved inputs; backup made")
	} else {
		s.notify("Saved inputs to mugen.yaml")
	}
	s.savedInput = live
	return true
}

func (s *InputConfigState) inputsChanged() bool {
	live, saved := s.engine.snapshotOptions(), s.savedInput
	return live.keyboardDisabled != saved.keyboardDisabled || !reflect.DeepEqual(live.keys, saved.keys) || !reflect.DeepEqual(live.pads, saved.pads)
}

func (s *InputConfigState) leave() State {
	if s.inputsChanged() {
		s.confirmExit, s.exitChoice = true, 0
		return s
	}
	return s.back
}

func (s *InputConfigState) offset() int {
	if s.gamepad {
		return 2
	}
	return 1
}
func (s *InputConfigState) pad() config.GamepadConfig {
	if s.player == 2 {
		return input.GlobalManager.P2Gamepad
	}
	return input.GlobalManager.P1Gamepad
}
func (s *InputConfigState) savePad(pad config.GamepadConfig) {
	if s.player == 1 {
		input.GlobalManager.P1Gamepad = pad
	} else {
		input.GlobalManager.P2Gamepad = pad
	}
	s.engine.cfg.Input.Gamepads[s.player-1] = pad
}
func (s *InputConfigState) writeKeyboard(keys ...ebiten.Key) {
	im := input.GlobalManager
	action := inputConfigActions[s.cursor-s.offset()]
	if s.appendBinding {
		keys = append(im.KeyboardBindings(s.player, action), keys...)
	}
	if s.player == 1 {
		im.P1Keyboard.SetBindings(action.String(), keys...)
		s.engine.cfg.Input.Player1Keyboard = im.P1Keyboard
	} else {
		im.P2Keyboard.SetBindings(action.String(), keys...)
		s.engine.cfg.Input.Player2Keyboard = im.P2Keyboard
	}
}
func (s *InputConfigState) writeGamepad(controls ...config.GamepadControl) {
	pad := s.pad()
	action := inputConfigActions[s.cursor-s.offset()].String()
	if s.appendBinding {
		previous, _ := pad.Bindings(action)
		controls = append(previous, controls...)
	}
	pad.SetBindings(action, controls...)
	s.savePad(pad)
}
func (s *InputConfigState) Update() (State, error) {
	im := input.GlobalManager
	if s.engine.motif.OptionBG != nil {
		s.engine.motif.OptionBG.Update()
	}
	if s.confirmExit {
		if im.KeyJustPressed(ebiten.KeyEscape) {
			s.confirmExit = false
			return s, nil
		}
		if im.KeyJustPressed(ebiten.KeyArrowLeft) || im.KeyJustPressed(ebiten.KeyArrowUp) || im.IsJustPressed(1, input.ActionLeftUI) || im.IsJustPressed(1, input.ActionUp) {
			s.exitChoice = (s.exitChoice + 2) % 3
		}
		if im.KeyJustPressed(ebiten.KeyArrowRight) || im.KeyJustPressed(ebiten.KeyArrowDown) || im.IsJustPressed(1, input.ActionRightUI) || im.IsJustPressed(1, input.ActionDown) {
			s.exitChoice = (s.exitChoice + 1) % 3
		}
		if im.KeyJustPressed(ebiten.KeyEnter) || im.IsJustPressed(1, input.ActionEnter) {
			s.confirmExit = false
			switch s.exitChoice {
			case 0:
				if s.save() {
					return s.back, nil
				}
			case 1:
				return s.back, nil
			}
			s.settle = true
		}
		return s, nil
	}
	if s.testing {
		if im.KeyJustPressed(ebiten.KeyEscape) || im.KeyJustPressed(ebiten.KeyF4) {
			s.testing, s.settle = false, true
			s.baseline = s.captureState()
			return s, nil
		}
		if im.KeyJustPressed(ebiten.KeyTab) {
			s.player = 3 - s.player
			s.startTest()
		}
		if im.KeyJustPressed(ebiten.KeyF2) {
			s.gamepad, s.cursor = !s.gamepad, 0
			s.startTest()
		}
		if s.gamepad {
			current := im.GamepadSnapshot(s.player)
			state := current.Raw()
			if state.ID != s.testBaseline.ID || state.Connected != s.testBaseline.Connected {
				s.testBaseline, s.testSeen = state, false
			}
			s.testSeen = s.testSeen || len(current.Buttons) > 0 || len(state.Changes(s.testBaseline, .05)) > 0
		} else {
			for key := ebiten.Key(0); key <= ebiten.KeyMax; key++ {
				s.testSeen = s.testSeen || im.KeyPressed(key)
			}
		}
		return s, nil
	}
	if s.confirmDefaults {
		if im.KeyJustPressed(ebiten.KeyArrowLeft) || im.KeyJustPressed(ebiten.KeyArrowRight) {
			s.defaultsYes = !s.defaultsYes
		}
		if im.KeyJustPressed(ebiten.KeyEscape) {
			s.confirmDefaults = false
		}
		if im.KeyJustPressed(ebiten.KeyEnter) {
			if s.defaultsYes {
				initial, side := s.engine.initialOptions, s.player-1
				if side == 0 {
					im.P1Keyboard, im.P1Gamepad = initial.keys[side], initial.pads[side]
					s.engine.cfg.Input.Player1Keyboard = initial.keys[side]
				} else {
					im.P2Keyboard, im.P2Gamepad = initial.keys[side], initial.pads[side]
					s.engine.cfg.Input.Player2Keyboard = initial.keys[side]
				}
				im.KeyboardDisabled[side] = initial.keyboardDisabled[side]
				s.engine.cfg.Input.KeyboardDisabled = im.KeyboardDisabled
				s.engine.cfg.Input.Gamepads[side] = initial.pads[side]
				s.cursor = 0
			}
			s.confirmDefaults, s.settle = false, true
		}
		return s, nil
	}
	if im.KeyJustPressed(ebiten.KeyEscape) {
		if s.waiting {
			s.waiting = false
			return s, nil
		}
		return s.leave(), nil
	}
	if s.settle {
		if s.controlsReleased() {
			s.settle = false
		}
		return s, nil
	}
	if !s.waiting && im.IsJustPressed(1, input.ActionEscape) {
		return s.leave(), nil
	}
	if s.waiting {
		if !s.now().Before(s.captureDeadline) {
			s.waiting, s.armed = false, false
			s.notify("Timed out; binding unchanged")
			return s, nil
		}
		state := s.captureState()
		if !s.armed {
			s.armed = s.controlsReleased()
			if s.armed {
				s.baseline = state
			}
			return s, nil
		}
		if s.gamepad {
			if !state.Connected || !s.baseline.Connected || state.ID != s.baseline.ID || state.Standard != s.baseline.Standard {
				s.armed = false
				return s, nil
			}
			if controls := state.Changes(s.baseline, s.pad().Deadzone); len(controls) > 0 {
				s.writeGamepad(controls[0])
				s.waiting = false
				s.settle = true
			}
		} else {
			for key := ebiten.Key(0); key <= ebiten.KeyMax; key++ {
				if im.KeyJustPressed(key) {
					s.writeKeyboard(key)
					s.waiting = false
					s.settle = true
					break
				}
			}
		}
		return s, nil
	}
	if im.KeyJustPressed(ebiten.KeyF4) {
		s.startTest()
		return s, nil
	}
	if im.KeyJustPressed(ebiten.KeyF5) {
		s.save()
		return s, nil
	}
	if s.gamepad && im.KeyJustPressed(ebiten.KeyF6) {
		s.rawCapture = !s.rawCapture
		return s, nil
	}
	if im.KeyJustPressed(ebiten.KeyF3) {
		s.confirmDefaults, s.defaultsYes = true, false
		return s, nil
	}
	if im.KeyJustPressed(ebiten.KeyTab) {
		s.player = 3 - s.player
	}
	if im.KeyJustPressed(ebiten.KeyF2) {
		s.gamepad = !s.gamepad
		s.cursor = 0
	}
	actionEnd := len(inputConfigActions) + s.offset()
	count := actionEnd + 2 // Test Inputs and Save Inputs are also selectable.
	if im.KeyJustPressed(ebiten.KeyArrowUp) || im.IsJustPressed(1, input.ActionUp) {
		s.cursor = (s.cursor + count - 1) % count
	}
	if im.KeyJustPressed(ebiten.KeyArrowDown) || im.IsJustPressed(1, input.ActionDown) {
		s.cursor = (s.cursor + 1) % count
	}
	confirm := im.KeyJustPressed(ebiten.KeyEnter) || im.KeyJustPressed(ebiten.KeyNumpadEnter) || im.IsJustPressed(1, input.ActionEnter)
	left := im.KeyJustPressed(ebiten.KeyArrowLeft) || im.IsJustPressed(1, input.ActionLeftUI)
	right := im.KeyJustPressed(ebiten.KeyArrowRight) || im.IsJustPressed(1, input.ActionRightUI)
	if s.cursor == 0 {
		if confirm || left || right {
			if s.gamepad {
				pad := s.pad()
				pad.Enabled = !pad.Enabled
				s.savePad(pad)
			} else {
				im.KeyboardDisabled[s.player-1] = !im.KeyboardDisabled[s.player-1]
				s.engine.cfg.Input.KeyboardDisabled = im.KeyboardDisabled
			}
			s.settle = true
		}
	} else if s.gamepad && s.cursor == 1 {
		delta := 0.
		if left {
			delta -= .05
		}
		if right {
			delta += .05
		}
		if delta != 0 {
			pad := s.pad()
			pad.Deadzone = max(0, min(.95, math.Round((pad.Deadzone+delta)*20)/20))
			s.savePad(pad)
			s.settle = true
		}
	} else if s.cursor >= actionEnd {
		if confirm {
			if s.cursor == actionEnd {
				s.startTest()
			} else {
				s.save()
			}
		}
	} else if im.KeyJustPressed(ebiten.KeyDelete) {
		s.appendBinding = false
		if s.gamepad {
			s.writeGamepad()
		} else {
			s.writeKeyboard()
		}
		s.settle = true
	} else if confirm || im.KeyJustPressed(ebiten.KeyInsert) {
		s.waiting, s.armed = true, false
		s.captureDeadline = s.now().Add(inputCaptureTimeout)
		s.message = ""
		s.appendBinding = im.KeyJustPressed(ebiten.KeyInsert)
	}
	return s, nil
}
func (s *InputConfigState) controlsReleased() bool {
	im := input.GlobalManager
	for key := ebiten.Key(0); key <= ebiten.KeyMax; key++ {
		if im.KeyPressed(key) {
			return false
		}
	}
	if !s.gamepad {
		return true
	}
	state := s.captureState()
	if !state.Connected {
		return true
	}
	if len(state.Buttons) > 0 {
		return false
	}
	if state.Standard {
		for _, value := range state.Axes {
			if math.Abs(value) > s.pad().Deadzone {
				return false
			}
		}
		return true
	}
	if s.waiting && !s.armed {
		return true
	}
	return len(state.Changes(s.baseline, s.pad().Deadzone)) == 0
}
func (s *InputConfigState) Draw(screen *ebiten.Image) {
	e := s.engine
	if s.background == nil {
		s.background = ebiten.NewImage(e.logicalWidth, e.logicalHeight)
	}
	heading := "INPUT CONFIGURATION"
	if s.testing {
		heading = "INPUT TEST"
	}
	content := e.drawOptionsBackground(screen, s.background, heading)
	im := input.GlobalManager
	kind := "Keyboard"
	if s.gamepad {
		kind = "Gamepad"
	}
	rows := []utilityRow{{fmt.Sprintf("Player %d", s.player), kind}, {"Tab: player", "F2: device"}}
	if s.gamepad {
		pad := s.pad()
		enabled := "Disabled"
		if pad.Enabled {
			enabled = "Enabled"
		}
		_, name, standard, connected := im.GamepadInfo(s.player)
		detail := "No controller assigned"
		if connected {
			layout := "raw"
			if standard {
				layout = "standard"
			}
			detail = name + " (" + layout + ")"
		}
		capture := "Auto"
		if s.rawCapture {
			capture = "Raw"
		}
		rows = append(rows, utilityRow{detail, ""}, utilityRow{"F6 capture", capture}, utilityRow{},
			utilityRow{"Gamepad", enabled}, utilityRow{"Deadzone", fmt.Sprintf("%.2f", pad.Deadzone)})
	} else {
		enabled := "Enabled"
		if im.KeyboardDisabled[s.player-1] {
			enabled = "Disabled"
		}
		rows = append(rows, utilityRow{}, utilityRow{"Keyboard", enabled})
	}
	panel := screen.SubImage(content).(*ebiten.Image)
	if s.confirmExit {
		s.dialog.draw(e, panel, []utilityRow{{"Save changed inputs?", ""}, {},
			{"Save", "YAML + backup"}, {"Session only", "Keep changes"}, {"Back", "Continue editing"}, {},
			{"Arrows: choose", "Enter: confirm"}, {"Esc: continue editing", ""}}, s.exitChoice+2, "", panel.Bounds().Dy()/2)
		return
	}
	if s.confirmDefaults {
		choice := "No"
		if s.defaultsYes {
			choice = "Yes"
		}
		s.dialog.draw(e, panel, []utilityRow{{fmt.Sprintf("Restore P%d startup bindings?", s.player), choice},
			{"Left/right: choose", "Enter: confirm"}}, 0, "", panel.Bounds().Dy()/2)
		return
	}
	if s.testing {
		rows = append(rows, s.testRows()...)
		s.utility.draw(e, panel, rows, -1, "", 4)
		return
	}
	cursor := len(rows) - s.offset() + s.cursor
	for _, action := range inputConfigActions {
		var names []string
		if s.gamepad {
			controls, _ := s.pad().Bindings(action.String())
			for _, control := range controls {
				names = append(names, control.String())
			}
		} else {
			for _, key := range im.KeyboardBindings(s.player, action) {
				names = append(names, key.String())
			}
		}
		value := strings.Join(names, ", ")
		if len(names) == 0 {
			value = "disabled"
		}
		rows = append(rows, utilityRow{action.String(), value})
	}
	rows = append(rows, utilityRow{"Test Inputs", "Enter / F4"}, utilityRow{"Save Inputs", "Enter / F5"})
	hint := utilityRow{"Enter: replace", "Insert: add"}
	if s.waiting {
		seconds := max(0, int(math.Ceil(s.captureDeadline.Sub(s.now()).Seconds())))
		prompt := "Press input"
		if !s.armed {
			prompt = "Release controls"
		}
		hint = utilityRow{fmt.Sprintf("%s (%ds)", prompt, seconds), "Esc: cancel"}
	}
	if s.settle {
		hint = utilityRow{"Release controls to continue", ""}
	}
	if s.message != "" && s.now().Before(s.messageUntil) {
		hint = utilityRow{s.message, ""}
	}
	rows = append(rows, utilityRow{}, hint, utilityRow{"Delete: disable", "F3: reset"}, utilityRow{"F5 saves to YAML", "Esc: back"})
	// Fit below the authored heading, using the same small utility lettering as
	// Options. Every binding stays visible, including on a small game window.
	s.utility.draw(e, panel, rows, cursor, "", 4)
}

func compactInputNames(names []string) string {
	if len(names) == 0 {
		return "--"
	}
	for count := len(names); count > 1; count-- {
		if len(strings.Join(names[:count], ", ")) <= 28 {
			if count < len(names) {
				return strings.Join(names[:count], ", ") + fmt.Sprintf(" +%d", len(names)-count)
			}
			return strings.Join(names, ", ")
		}
	}
	return names[0]
}

// Diagnostics read the same device-specific held actions as gameplay. Raw
// signals remain separate so a keyboard or a bad legacy map cannot hide the
// difference between a detected device and one actually supplying input.
func (s *InputConfigState) testRows() []utilityRow {
	im := input.GlobalManager
	signal := "Waiting for input"
	if s.testSeen {
		signal = "Input observed"
	}
	rows := []utilityRow{{"Device signal", signal}}
	if s.gamepad {
		state := im.GamepadSnapshot(s.player)
		var buttons, axes, standard []string
		for _, control := range state.RawButtons {
			buttons = append(buttons, fmt.Sprint(control.Index))
		}
		for _, control := range state.Buttons {
			if control.Kind == config.PadStandardButton {
				standard = append(standard, control.String())
			}
		}
		for _, control := range state.Raw().Changes(s.testBaseline, .05) {
			if control.Kind == config.PadAxis {
				axes = append(axes, fmt.Sprintf("%d:%+.2f", control.Index, state.RawAxes[control.Index]))
			}
		}
		rows = append(rows, utilityRow{fmt.Sprintf("Raw: %d buttons, %d axes", state.RawButtonCount, len(state.RawAxes)), ""},
			utilityRow{"Buttons", compactInputNames(buttons)}, utilityRow{"Axes", compactInputNames(axes)}, utilityRow{"Standard", compactInputNames(standard)})
	} else {
		var keys []string
		for key := ebiten.Key(0); key <= ebiten.KeyMax; key++ {
			if im.KeyPressed(key) {
				keys = append(keys, key.String())
			}
		}
		rows = append(rows, utilityRow{"Keys", compactInputNames(keys)})
	}
	for _, action := range inputConfigActions {
		pressed := im.KeyboardActionPressed(s.player, action)
		if s.gamepad {
			pressed = im.GamepadActionPressed(s.player, action)
		}
		value := "--"
		if pressed {
			value = "PRESSED"
		}
		rows = append(rows, utilityRow{action.String(), value})
	}
	return append(rows, utilityRow{}, utilityRow{"Test only; bindings unchanged", ""}, utilityRow{"F4 / Esc: finish test", ""})
}
