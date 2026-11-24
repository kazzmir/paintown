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

type physicalGamepads struct {
	layouts map[ebiten.GamepadID]legacyLayoutEntry
}

type legacyLayoutEntry struct {
	guid, name string
	layout     LegacyJoystickLayout
	known      bool
}

func (physicalGamepads) Name(id ebiten.GamepadID) string { return ebiten.GamepadName(id) }

func (p *physicalGamepads) AppendIDs(ids []ebiten.GamepadID) []ebiten.GamepadID {
	ids = ebiten.AppendGamepadIDs(ids)
	for id := range p.layouts {
		if !slices.Contains(ids, id) {
			delete(p.layouts, id)
		}
	}
	return ids
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
	return &im.physicalPads
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
	RawButtons          []config.GamepadControl
	RawAxes             []float64
	RawButtonCount      int
}

func (im *InputManager) GamepadSnapshot(player int) GamepadSnapshot {
	id, _, standard, connected := im.GamepadInfo(player)
	state := GamepadSnapshot{ID: id, Connected: connected, Standard: standard}
	if !connected {
		return state
	}
	source := im.gamepadSource()
	state.RawButtonCount = min(source.ButtonCount(id), int(ebiten.GamepadButtonMax)+1)
	for button := 0; button < state.RawButtonCount; button++ {
		if source.Button(id, ebiten.GamepadButton(button)) {
			state.RawButtons = append(state.RawButtons, config.GamepadControl{Kind: config.PadButton, Index: button})
		}
	}
	for axis := 0; axis < min(source.AxisCount(id), 256); axis++ {
		state.RawAxes = append(state.RawAxes, source.Axis(id, axis))
	}
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
		state.Axes = state.RawAxes
	}
	state.Buttons = append(state.Buttons, state.RawButtons...)
	return state
}

// Raw retains the assigned device while bypassing its optional standard map.
// Capture and diagnostics must be able to see controls missing from that map.
func (s GamepadSnapshot) Raw() GamepadSnapshot {
	s.Standard, s.Buttons, s.Axes = false, s.RawButtons, s.RawAxes
	return s
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
	if s.Standard {
		// Prefer standard controls, then offer raw axes omitted by that mapping.
		raw, baseline := s.Raw(), previous.Raw()
		for _, control := range raw.Changes(baseline, deadzone) {
			if control.Kind == config.PadAxis {
				controls = append(controls, control)
			}
		}
	}
	return controls
}

// GamepadActionPressed reads the same held snapshot as gameplay, independently
// of keyboard alternatives and without consuming an edge.
func (im *InputManager) GamepadActionPressed(player int, action Action) bool {
	return im.gamepadPressed(player, action, false)
}

func (im *InputManager) updateGamepads() {
	source := im.gamepadSource()
	im.GamepadIDs = source.AppendIDs(im.GamepadIDs[:0])
	slices.Sort(im.GamepadIDs)
	for i, slot := range im.legacySlots {
		if !slices.Contains(im.GamepadIDs, slot.id) {
			im.legacySlots[i] = gamepadSlot{}
		}
	}
	for _, id := range im.GamepadIDs {
		used := false
		for _, slot := range im.legacySlots {
			used = used || slot.assigned && slot.id == id
		}
		if !used {
			for i, slot := range im.legacySlots {
				if !slot.assigned {
					im.legacySlots[i] = gamepadSlot{id: id, assigned: true}
					break
				}
			}
		}
	}
	settings := [2]*config.GamepadConfig{&im.P1Gamepad, &im.P2Gamepad}
	for side, slot := range im.padSlots {
		if !settings[side].Enabled || !slices.Contains(im.GamepadIDs, slot.id) {
			im.padSlots[side] = gamepadSlot{}
		}
		if ordinal, legacy := settings[side].LegacyDevice(); legacy && settings[side].Enabled {
			im.padSlots[side] = im.legacySlots[ordinal]
		}
	}
	for side, slot := range im.padSlots {
		if _, legacy := settings[side].LegacyDevice(); legacy || !slot.assigned {
			continue
		}
		other := 1 - side
		if _, legacy := settings[other].LegacyDevice(); legacy && im.padSlots[other].assigned && slot.id == im.padSlots[other].id {
			im.padSlots[side] = gamepadSlot{}
		}
	}
	// A remaining controller keeps its player when another disconnects. New
	// devices fill only free, enabled seats, with deterministic initial order.
	for side := range settings {
		_, legacy := settings[side].LegacyDevice()
		if !settings[side].Enabled || im.padSlots[side].assigned || legacy {
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
			if control.Kind == config.PadLegacy {
				return im.legacyPressed(source, control.Index, pad.Deadzone)
			}
			if !slot.assigned {
				return false
			}
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
			if pad.Enabled && pad.MatchBinding(name, matches) {
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
