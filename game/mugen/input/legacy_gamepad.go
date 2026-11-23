package input

import (
	"fmt"

	"github.com/hajimehoshi/ebiten/v2"
)

// LegacyJoystickLayout separates real buttons and hats. The SDK's public raw
// button count combines them; guessing that boundary misbinds hatless devices.
// Replay/device adapters can supply LegacyLayout alongside GamepadSource.
type LegacyJoystickLayout struct {
	Axes, Buttons, Hats int
}

func (p *physicalGamepads) LegacyLayout(id ebiten.GamepadID) (LegacyJoystickLayout, bool) {
	guid, name := ebiten.GamepadSDLID(id), ebiten.GamepadName(id)
	if entry, ok := p.layouts[id]; ok && entry.guid == guid && entry.name == name {
		return entry.layout, entry.known
	}
	layout, known := discoverLegacyJoystick(guid, name, ebiten.GamepadAxisCount(id), ebiten.GamepadButtonCount(id))
	if p.layouts == nil {
		p.layouts = make(map[ebiten.GamepadID]legacyLayoutEntry)
	}
	p.layouts[id] = legacyLayoutEntry{guid: guid, name: name, layout: layout, known: known}
	if !known {
		fmt.Printf("Input warning: legacy joystick layout unavailable for %q; use named/raw YAML bindings or session input configuration\n", name)
	}
	return layout, known
}

func (im *InputManager) legacyPressed(source GamepadSource, code int, deadzone float64) bool {
	if code <= 0 || code >= 128 || code%32 == 0 {
		return false
	}
	slot := im.legacySlots[code/32]
	metadata, ok := source.(interface {
		LegacyLayout(ebiten.GamepadID) (LegacyJoystickLayout, bool)
	})
	if !slot.assigned || !ok {
		return false
	}
	layout, known := metadata.LegacyLayout(slot.id)
	if !known || layout.Axes < 0 || layout.Buttons < 0 || layout.Hats < 0 {
		return false
	}
	local := code % 32
	// Native's first pair is vertical (raw axis 1), then horizontal (axis 0).
	// Later pairs retain their device order. Each polarity is a separate code.
	for axis := 0; axis < min(layout.Axes, source.AxisCount(slot.id), 16); axis++ {
		pair := axis
		if pair < 2 {
			pair ^= 1
		}
		value := source.Axis(slot.id, axis)
		if local == pair*2+1 && value < -deadzone || local == pair*2+2 && value > deadzone {
			return true
		}
	}
	// Native hats alias the first directional quartet. All reported hats still
	// reserve space before real buttons; they are not raw button zero onward.
	hatDirection := local - (layout.Axes*2 + 1)
	if layout.Hats > 0 && hatDirection >= 0 && hatDirection < 4 {
		// Native Up/Down/Left/Right versus SDK Up/Right/Down/Left.
		for hat := 0; hat < min(layout.Hats, 4); hat++ {
			button := layout.Buttons + hat*4 + [4]int{0, 2, 3, 1}[hatDirection]
			if button < source.ButtonCount(slot.id) && source.Button(slot.id, ebiten.GamepadButton(button)) {
				return true
			}
		}
		return false
	}
	button := local - (layout.Axes*2 + layout.Hats*4 + 1)
	return button >= 0 && button < layout.Buttons && button < source.ButtonCount(slot.id) && source.Button(slot.id, ebiten.GamepadButton(button))
}
