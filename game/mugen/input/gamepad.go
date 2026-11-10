package input

import (
	"fmt"
	"slices"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/config"
)

// GamepadSource scopes device reads to a manager, as KeyboardSource does.
// Controls are sampled once per tick; menu edge queries never consume them.
type GamepadSource interface {
	AppendIDs([]ebiten.GamepadID) []ebiten.GamepadID
	StandardLayout(ebiten.GamepadID) bool
	StandardButton(ebiten.GamepadID, ebiten.StandardGamepadButton) bool
	StandardAxis(ebiten.GamepadID, ebiten.StandardGamepadAxis) float64
	ButtonCount(ebiten.GamepadID) int
	Button(ebiten.GamepadID, ebiten.GamepadButton) bool
	AxisCount(ebiten.GamepadID) int
	Axis(ebiten.GamepadID, int) float64
}

type physicalGamepads struct{}

func (physicalGamepads) Name(id ebiten.GamepadID) string { return ebiten.GamepadName(id) }

func (physicalGamepads) AppendIDs(ids []ebiten.GamepadID) []ebiten.GamepadID {
	return ebiten.AppendGamepadIDs(ids)
}
func (physicalGamepads) StandardLayout(id ebiten.GamepadID) bool {
	return ebiten.IsStandardGamepadLayoutAvailable(id)
}
func (physicalGamepads) StandardButton(id ebiten.GamepadID, b ebiten.StandardGamepadButton) bool {
	return ebiten.IsStandardGamepadButtonPressed(id, b)
}
func (physicalGamepads) StandardAxis(id ebiten.GamepadID, a ebiten.StandardGamepadAxis) float64 {
	return ebiten.StandardGamepadAxisValue(id, a)
}
func (physicalGamepads) ButtonCount(id ebiten.GamepadID) int { return ebiten.GamepadButtonCount(id) }
func (physicalGamepads) Button(id ebiten.GamepadID, b ebiten.GamepadButton) bool {
	return ebiten.IsGamepadButtonPressed(id, b)
}
func (physicalGamepads) AxisCount(id ebiten.GamepadID) int { return ebiten.GamepadAxisCount(id) }
func (physicalGamepads) Axis(id ebiten.GamepadID, a int) float64 {
	return ebiten.GamepadAxisValue(id, a)
}

type gamepadSlot struct {
	id       ebiten.GamepadID
	assigned bool
}

func (im *InputManager) gamepadSource() GamepadSource {
	if im.Gamepads != nil {
		return im.Gamepads
	}
	return physicalGamepads{}
}

func (im *InputManager) GamepadInfo(player int) (id ebiten.GamepadID, name string, standard, connected bool) {
	if player < 1 || player > 2 {
		return
	}
	slot := im.padSlots[player-1]
	settings := im.P1Gamepad
	if player == 2 {
		settings = im.P2Gamepad
	}
	if !settings.Enabled || !slot.assigned {
		return
	}
	id, connected = slot.id, true
	source := im.gamepadSource()
	standard = source.StandardLayout(id)
	name = fmt.Sprintf("Gamepad %d", id)
	if named, ok := source.(interface{ Name(ebiten.GamepadID) string }); ok {
		if label := named.Name(id); label != "" {
			name = label
		}
	}
	return
}

// GamepadSnapshot is a configuration-only view of the assigned SDK device.
// Unknown raw axes retain their neutral baseline instead of assuming zero;
// standard layouts already normalize their stick axes.
type GamepadSnapshot struct {
	ID                  ebiten.GamepadID
	Connected, Standard bool
	Buttons             []config.GamepadControl
	Axes                []float64
}

func (im *InputManager) GamepadSnapshot(player int) GamepadSnapshot {
	id, _, standard, connected := im.GamepadInfo(player)
	state := GamepadSnapshot{ID: id, Connected: connected, Standard: standard}
	if !connected {
		return state
	}
	source := im.gamepadSource()
	if standard {
		for button := ebiten.StandardGamepadButton(0); button <= ebiten.StandardGamepadButtonMax; button++ {
			if source.StandardButton(id, button) {
				state.Buttons = append(state.Buttons, config.GamepadControl{Kind: config.PadStandardButton, Index: int(button)})
			}
		}
		for axis := ebiten.StandardGamepadAxis(0); axis <= ebiten.StandardGamepadAxisRightStickVertical; axis++ {
			state.Axes = append(state.Axes, source.StandardAxis(id, axis))
		}
	} else {
		for axis := 0; axis < min(source.AxisCount(id), 256); axis++ {
			state.Axes = append(state.Axes, source.Axis(id, axis))
		}
	}
	if len(state.Buttons) == 0 {
		for button := 0; button < min(source.ButtonCount(id), int(ebiten.GamepadButtonMax)+1); button++ {
			if source.Button(id, ebiten.GamepadButton(button)) {
				state.Buttons = append(state.Buttons, config.GamepadControl{Kind: config.PadButton, Index: button})
			}
		}
	}
	return state
}
func (s GamepadSnapshot) Changes(previous GamepadSnapshot, deadzone float64) []config.GamepadControl {
	if !s.Connected || !previous.Connected || s.ID != previous.ID || s.Standard != previous.Standard {
		return nil
	}
	var controls []config.GamepadControl
	for _, button := range s.Buttons {
		if !slices.Contains(previous.Buttons, button) {
			controls = append(controls, button)
		}
	}
	kind := config.PadAxis
	if s.Standard {
		kind = config.PadStandardAxis
	}
	for axis, value := range s.Axes {
		if axis >= len(previous.Axes) {
			continue
		}
		delta := value - previous.Axes[axis]
		if delta > deadzone || delta < -deadzone {
			controls = append(controls, config.GamepadControl{Kind: kind, Index: axis, Positive: delta > 0})
		}
	}
	return controls
}

func (im *InputManager) updateGamepads() {
	source := im.gamepadSource()
	im.GamepadIDs = source.AppendIDs(im.GamepadIDs[:0])
	slices.Sort(im.GamepadIDs)
	settings := [2]*config.GamepadConfig{&im.P1Gamepad, &im.P2Gamepad}
	for side, slot := range im.padSlots {
		if !settings[side].Enabled || !slices.Contains(im.GamepadIDs, slot.id) {
			im.padSlots[side] = gamepadSlot{}
		}
	}
	// A remaining controller keeps its player when another disconnects. New
	// devices fill only free, enabled seats, with deterministic initial order.
	for side := range settings {
		if !settings[side].Enabled || im.padSlots[side].assigned {
			continue
		}
		for _, id := range im.GamepadIDs {
			used := false
			for _, slot := range im.padSlots {
				used = used || slot.assigned && slot.id == id
			}
			if !used {
				im.padSlots[side] = gamepadSlot{id: id, assigned: true}
				break
			}
		}
	}
	im.padPrevious, im.keyboardPrevious = im.padHeld, im.keyboardHeld
	im.padHeld, im.keyboardHeld = [2]uint32{}, [2]uint32{}
	for side, pad := range settings {
		slot := im.padSlots[side]
		standard := slot.assigned && source.StandardLayout(slot.id)
		matches := func(control config.GamepadControl) bool {
			switch control.Kind {
			case config.PadStandardButton:
				return standard && source.StandardButton(slot.id, ebiten.StandardGamepadButton(control.Index))
			case config.PadButton:
				return control.Index >= 0 && control.Index < source.ButtonCount(slot.id) && source.Button(slot.id, ebiten.GamepadButton(control.Index))
			case config.PadStandardAxis, config.PadAxis:
				var value float64
				if control.Kind == config.PadStandardAxis {
					if !standard {
						return false
					}
					value = source.StandardAxis(slot.id, ebiten.StandardGamepadAxis(control.Index))
				} else {
					if control.Index < 0 || control.Index >= source.AxisCount(slot.id) {
						return false
					}
					value = source.Axis(slot.id, control.Index)
				}
				if control.Positive {
					return value > pad.Deadzone
				}
				return value < -pad.Deadzone
			}
			return false
		}
		for action := ActionJump; action <= ActionSpace; action++ {
			if im.matchAction(side+1, action, im.keyboard().IsPressed) {
				im.keyboardHeld[side] |= 1 << action
			}
			name, _ := actionBinding(action)
			if slot.assigned && pad.MatchBinding(name, matches) {
				im.padHeld[side] |= 1 << action
			}
		}
		if fresh := im.padHeld[side] &^ im.padPrevious[side]; fresh != 0 {
			im.LastKeyEvent = fmt.Sprintf("P%d gamepad %d", side+1, slot.id)
		}
	}
}

func (im *InputManager) gamepadPressed(player int, action Action, edge bool) bool {
	if player < 1 || player > 2 || action < ActionJump || action > ActionSpace {
		return false
	}
	mask := uint32(1) << action
	if im.padHeld[player-1]&mask == 0 {
		return false
	}
	return !edge || im.padPrevious[player-1]&mask == 0 && im.keyboardPrevious[player-1]&mask == 0
}
