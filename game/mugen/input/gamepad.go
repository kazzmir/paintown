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

func (im *InputManager) updateGamepads() {
	source := im.Gamepads
	if source == nil {
		source = physicalGamepads{}
	}
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
