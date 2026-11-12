package main

import (
	"fmt"
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
	"image/color"
	"math"
	"strings"
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
	canvas                        *ebiten.Image
}

func NewInputConfigState(engine *Engine, back State) *InputConfigState {
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
	if s.canvas == nil {
		s.canvas = ebiten.NewImage(400, 280)
	}
	s.canvas.Fill(color.RGBA{12, 18, 32, 255})
	im := input.GlobalManager
	kind := "KEYBOARD"
	if s.gamepad {
		kind = "GAMEPAD"
	}
	ebitenutil.DebugPrintAt(s.canvas, fmt.Sprintf("PLAYER %d %s | Tab: player | F2: device", s.player, kind), 8, 4)
	var rows []string
	if s.gamepad {
		pad := s.pad()
		enabled := "Disabled"
		if pad.Enabled {
			enabled = "Enabled"
		}
		rows = []string{"Gamepad: " + enabled, fmt.Sprintf("Deadzone: %.2f", pad.Deadzone)}
		_, name, standard, connected := im.GamepadInfo(s.player)
		detail := "No controller assigned"
		if connected {
			layout := "raw"
			if standard {
				layout = "standard"
			}
			detail = name + " (" + layout + ")"
		}
		ebitenutil.DebugPrintAt(s.canvas, detail, 8, 22)
	} else {
		enabled := "Enabled"
		if im.KeyboardDisabled[s.player-1] {
			enabled = "Disabled"
		}
		rows = []string{"Keyboard: " + enabled}
	}
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
		rows = append(rows, fmt.Sprintf("%-7s %s", action.String(), value))
	}
	top := max(0, s.cursor-10)
	for index := top; index < min(top+11, len(rows)); index++ {
		marker := "  "
		if index == s.cursor {
			marker = "> "
		}
		ebitenutil.DebugPrintAt(s.canvas, marker+rows[index], 8, 48+(index-top)*16)
	}
	hint := "Enter replace | Insert add | Delete disable"
	if s.waiting {
		hint = "Release controls, then press one. Esc cancels"
	}
	if s.settle {
		hint = "Release controls to continue"
	}
	ebitenutil.DebugPrintAt(s.canvas, hint+"\nSession changes only | Esc: back", 8, 244)
	drawUtilityCanvas(screen, s.canvas)
}

func drawUtilityCanvas(screen, canvas *ebiten.Image) {
	screen.Fill(color.RGBA{4, 8, 16, 255})
	scale := min(float64(screen.Bounds().Dx())/float64(canvas.Bounds().Dx()), float64(screen.Bounds().Dy())/float64(canvas.Bounds().Dy()))
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Scale(scale, scale)
	op.GeoM.Translate(float64(screen.Bounds().Min.X)+(float64(screen.Bounds().Dx())-float64(canvas.Bounds().Dx())*scale)/2, float64(screen.Bounds().Min.Y)+(float64(screen.Bounds().Dy())-float64(canvas.Bounds().Dy())*scale)/2)
	screen.DrawImage(canvas, op)
}
