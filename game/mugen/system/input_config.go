package main

import (
	"fmt"
	"math"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
)

var inputConfigActions = []input.Action{input.ActionJump, input.ActionCrouch, input.ActionLeft, input.ActionRight, input.ActionA, input.ActionB, input.ActionC, input.ActionX, input.ActionY, input.ActionZ, input.ActionStart, input.ActionEscape}

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
}

func NewInputConfigState(engine *Engine, back State) *InputConfigState {
	if engine.initialOptions == nil {
		engine.initialOptions = engine.snapshotOptions()
	}
	return &InputConfigState{engine: engine, back: back, player: 1}
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
		return s.back, nil
	}
	if s.settle {
		if s.controlsReleased() {
			s.settle = false
		}
		return s, nil
	}
	if !s.waiting && im.IsJustPressed(1, input.ActionEscape) {
		return s.back, nil
	}
	if s.waiting {
		state := im.GamepadSnapshot(s.player)
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
	count := len(inputConfigActions) + s.offset()
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
	state := im.GamepadSnapshot(s.player)
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
	content := e.drawOptionsBackground(screen, s.background, "INPUT CONFIGURATION")
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
		rows = append(rows, utilityRow{detail, ""}, utilityRow{},
			utilityRow{"Gamepad", enabled}, utilityRow{"Deadzone", fmt.Sprintf("%.2f", pad.Deadzone)})
	} else {
		enabled := "Enabled"
		if im.KeyboardDisabled[s.player-1] {
			enabled = "Disabled"
		}
		rows = append(rows, utilityRow{}, utilityRow{"Keyboard", enabled})
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
	hint := utilityRow{"Enter: replace", "Insert: add"}
	if s.waiting {
		hint = utilityRow{"Release controls, then press one", "Esc: cancel"}
	}
	if s.settle {
		hint = utilityRow{"Release controls to continue", ""}
	}
	rows = append(rows, utilityRow{}, hint, utilityRow{"Delete: disable", "F3: reset"}, utilityRow{"Session only", "Esc: back"})
	// Fit below the authored heading, using the same small utility lettering as
	// Options. Every binding stays visible, including on a small game window.
	panel := screen.SubImage(content).(*ebiten.Image)
	s.utility.draw(e, panel, rows, cursor, "", 4)
	if s.confirmDefaults {
		choice := "No"
		if s.defaultsYes {
			choice = "Yes"
		}
		s.dialog.draw(e, panel, []utilityRow{{fmt.Sprintf("Restore P%d startup bindings?", s.player), choice}}, 0, "", panel.Bounds().Dy()/2)
	}
}
