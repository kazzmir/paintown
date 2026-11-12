package config

import (
	"fmt"
	"math"
	"slices"
	"strconv"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
)

type GamepadControlKind uint8

const (
	PadStandardButton GamepadControlKind = iota
	PadStandardAxis
	PadButton
	PadAxis
)

// GamepadControl names either a portable layout control or an explicit raw
// Ebitengine control. Native MUGEN joystick numbers are a separate namespace.
type GamepadControl struct {
	Kind     GamepadControlKind
	Index    int
	Positive bool
}

func (c GamepadControl) String() string {
	sign := "-"
	if c.Positive {
		sign = "+"
	}
	switch c.Kind {
	case PadStandardButton:
		for name, index := range standardGamepadButtons {
			if int(index) == c.Index {
				return name
			}
		}
	case PadStandardAxis:
		for name, index := range standardGamepadAxes {
			if int(index) == c.Index {
				return name + sign
			}
		}
	case PadButton:
		return fmt.Sprintf("button:%d", c.Index)
	case PadAxis:
		return fmt.Sprintf("axis:%d%s", c.Index, sign)
	}
	return "unknown"
}

type GamepadConfig struct {
	Enabled    bool
	Deadzone   float64
	configured uint16
	bindings   [12][]GamepadControl // immutable; setters replace, getters copy
}

func gamepadAction(action string) int {
	switch strings.ToLower(action) {
	case "jump", "up":
		return 0
	case "crouch", "down":
		return 1
	case "left":
		return 2
	case "right":
		return 3
	case "a":
		return 4
	case "b":
		return 5
	case "c":
		return 6
	case "x":
		return 7
	case "y":
		return 8
	case "z":
		return 9
	case "start":
		return 10
	case "escape":
		return 11
	}
	return -1
}

func (c *GamepadConfig) SetBindings(action string, controls ...GamepadControl) bool {
	slot := gamepadAction(action)
	if slot < 0 {
		return false
	}
	var unique []GamepadControl
	for _, control := range controls {
		if !slices.Contains(unique, control) {
			unique = append(unique, control)
		}
	}
	c.bindings[slot] = unique
	c.configured |= 1 << slot
	return true
}

func (c GamepadConfig) Bindings(action string) ([]GamepadControl, bool) {
	slot := gamepadAction(action)
	if slot < 0 || c.configured&(1<<slot) == 0 {
		return nil, false
	}
	return append([]GamepadControl(nil), c.bindings[slot]...), true
}

func (c *GamepadConfig) MatchBinding(action string, match func(GamepadControl) bool) bool {
	slot := gamepadAction(action)
	if slot < 0 {
		return false
	}
	for _, control := range c.bindings[slot] {
		if match(control) {
			return true
		}
	}
	return false
}

func ParseGamepadBindings(names []string) ([]GamepadControl, error) {
	var controls []GamepadControl
	for _, name := range names {
		name = strings.ToLower(strings.TrimSpace(name))
		if name == "" || name == "none" || name == "disabled" {
			continue
		}
		var control GamepadControl
		if index, ok := standardGamepadButtons[name]; ok {
			control = GamepadControl{Kind: PadStandardButton, Index: int(index)}
		} else {
			positive := strings.HasSuffix(name, "+")
			axisName := name
			if positive || strings.HasSuffix(name, "-") {
				axisName = name[:len(name)-1]
			}
			index, standard := standardGamepadAxes[axisName]
			switch {
			case standard && (positive || strings.HasSuffix(name, "-")):
				control = GamepadControl{Kind: PadStandardAxis, Index: int(index), Positive: positive}
			case strings.HasPrefix(name, "button:"):
				n, err := strconv.Atoi(strings.TrimPrefix(name, "button:"))
				if err != nil || n < 0 || n > int(ebiten.GamepadButtonMax) {
					return nil, fmt.Errorf("invalid raw gamepad button %q", name)
				}
				control = GamepadControl{Kind: PadButton, Index: n}
			case strings.HasPrefix(axisName, "axis:") && (positive || strings.HasSuffix(name, "-")):
				n, err := strconv.Atoi(strings.TrimPrefix(axisName, "axis:"))
				if err != nil || n < 0 || n > 255 {
					return nil, fmt.Errorf("invalid raw gamepad axis %q", name)
				}
				control = GamepadControl{Kind: PadAxis, Index: n, Positive: positive}
			default:
				return nil, fmt.Errorf("unknown gamepad control %q", name)
			}
		}
		if !slices.Contains(controls, control) {
			controls = append(controls, control)
		}
	}
	return controls, nil
}

func DefaultGamepadConfig() GamepadConfig {
	c := GamepadConfig{Deadzone: .5}
	for action, names := range map[string][]string{
		"jump": {"dpad_up", "left_y-"}, "crouch": {"dpad_down", "left_y+"},
		"left": {"dpad_left", "left_x-"}, "right": {"dpad_right", "left_x+"},
		"a": {"south"}, "b": {"east"}, "c": {"r1"}, "x": {"west"}, "y": {"north"}, "z": {"l1"},
		"start": {"start"}, "escape": {"back"},
	} {
		controls, _ := ParseGamepadBindings(names)
		c.SetBindings(action, controls...)
	}
	return c
}

func mergeGamepad(c *GamepadConfig, options *GamepadOptions, bindings map[string]KeyBinding) {
	if options != nil {
		if options.Deadzone != nil {
			value := *options.Deadzone
			if !math.IsNaN(value) && value >= 0 && value < 1 {
				c.Deadzone = value
			} else {
				fmt.Println("YAML gamepad: deadzone must be in [0,1); keeping existing value")
			}
		}
	}
	for action, names := range bindings {
		controls, err := ParseGamepadBindings(names)
		if err != nil {
			fmt.Printf("YAML gamepad %s: %v; keeping existing binding\n", action, err)
			continue
		}
		if !c.SetBindings(action, controls...) {
			fmt.Printf("YAML gamepad: unknown action %q\n", action)
		} else {
			c.Enabled = true
		}
	}
	// Invalid/legacy numeric names must not silently opt into a different
	// default mapping. An explicit device switch wins over valid bindings.
	if options != nil && options.Enabled != nil {
		c.Enabled = *options.Enabled
	}
}

var standardGamepadButtons = map[string]ebiten.StandardGamepadButton{
	"south": ebiten.StandardGamepadButtonRightBottom, "east": ebiten.StandardGamepadButtonRightRight,
	"west": ebiten.StandardGamepadButtonRightLeft, "north": ebiten.StandardGamepadButtonRightTop,
	"l1": ebiten.StandardGamepadButtonFrontTopLeft, "r1": ebiten.StandardGamepadButtonFrontTopRight,
	"l2": ebiten.StandardGamepadButtonFrontBottomLeft, "r2": ebiten.StandardGamepadButtonFrontBottomRight,
	"back": ebiten.StandardGamepadButtonCenterLeft, "start": ebiten.StandardGamepadButtonCenterRight,
	"left_stick": ebiten.StandardGamepadButtonLeftStick, "right_stick": ebiten.StandardGamepadButtonRightStick,
	"dpad_up": ebiten.StandardGamepadButtonLeftTop, "dpad_down": ebiten.StandardGamepadButtonLeftBottom,
	"dpad_left": ebiten.StandardGamepadButtonLeftLeft, "dpad_right": ebiten.StandardGamepadButtonLeftRight,

	"guide": ebiten.StandardGamepadButtonCenterCenter,
}

var standardGamepadAxes = map[string]ebiten.StandardGamepadAxis{
	"left_x": ebiten.StandardGamepadAxisLeftStickHorizontal, "left_y": ebiten.StandardGamepadAxisLeftStickVertical,
	"right_x": ebiten.StandardGamepadAxisRightStickHorizontal, "right_y": ebiten.StandardGamepadAxisRightStickVertical,
}
