package input

import (
	"fmt"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/inpututil"
	"github.com/kazzmir/paintown/game/mugen/config"
)

type Action int

const (
	ActionJump Action = iota
	ActionCrouch
	ActionLeft
	ActionRight
	ActionA
	ActionB
	ActionC
	ActionX
	ActionY
	ActionZ
	ActionStart
	ActionEscape
	ActionUp
	ActionDown
	ActionLeftUI
	ActionRightUI
	ActionEnter
	ActionSpace
)

func (a Action) String() string {
	switch a {
	case ActionJump:
		return "Jump"
	case ActionCrouch:
		return "Crouch"
	case ActionLeft:
		return "Left"
	case ActionRight:
		return "Right"
	case ActionA:
		return "A"
	case ActionB:
		return "B"
	case ActionC:
		return "C"
	case ActionX:
		return "X"
	case ActionY:
		return "Y"
	case ActionZ:
		return "Z"
	case ActionStart:
		return "Start"
	case ActionEscape:
		return "Escape"
	case ActionUp:
		return "Up"
	case ActionDown:
		return "Down"
	case ActionLeftUI:
		return "LeftUI"
	case ActionRightUI:
		return "RightUI"
	case ActionEnter:
		return "Enter"
	case ActionSpace:
		return "Space"
	}
	return "Unknown"
}

type InputManager struct {
	Keyboard   KeyboardSource
	P1Keyboard config.Keys
	P2Keyboard config.Keys
	P1Joystick config.Keys
	P2Joystick config.Keys

	// Specific for Ebiten Gamepad support
	GamepadIDs []ebiten.GamepadID

	LastKeyEvent    string
	LastActionEvent string
}

// KeyboardSource is scoped to one input manager, allowing deterministic tests
// and replays without changing the operating system's keyboard state.
type KeyboardSource interface {
	IsPressed(ebiten.Key) bool
	IsJustPressed(ebiten.Key) bool
}

type physicalKeyboard struct{}

func (physicalKeyboard) IsPressed(key ebiten.Key) bool     { return ebiten.IsKeyPressed(key) }
func (physicalKeyboard) IsJustPressed(key ebiten.Key) bool { return inpututil.IsKeyJustPressed(key) }

func (im *InputManager) keyboard() KeyboardSource {
	if im.Keyboard != nil {
		return im.Keyboard
	}
	return physicalKeyboard{}
}

func NewInputManager(cfg *config.MugenConfig) *InputManager {
	return &InputManager{
		P1Keyboard: cfg.Input.Player1Keyboard,
		P2Keyboard: cfg.Input.Player2Keyboard,
		P1Joystick: cfg.Input.Player1Joystick,
		P2Joystick: cfg.Input.Player2Joystick,
	}
}

func (im *InputManager) Update() {
	im.GamepadIDs = ebiten.AppendGamepadIDs(im.GamepadIDs[:0])

	// Reset action log each frame
	im.LastActionEvent = ""

	// Update raw key events (store only the latest for status line)
	source := im.keyboard()
	for k := ebiten.Key(0); k <= ebiten.KeyMax; k++ {
		if source.IsJustPressed(k) {
			im.LastKeyEvent = fmt.Sprintf("%v(%d)", k, int(k))
		}
	}
}

func (im *InputManager) IsPressed(player int, action Action) bool {
	keys := im.P1Keyboard
	if player == 2 {
		keys = im.P2Keyboard
	}

	key := im.getActionKey(keys, action)
	if key >= 0 && key <= ebiten.KeyMax && im.keyboard().IsPressed(key) {
		sep := ""
		if im.LastActionEvent != "" {
			sep = ", "
		}
		im.LastActionEvent += fmt.Sprintf("%s%s", sep, action.String())
		return true
	}

	// Check Gamepad IDs if lenient joystick support is needed later
	return false
}

func (im *InputManager) IsJustPressed(player int, action Action) bool {
	keys := im.P1Keyboard
	if player == 2 {
		keys = im.P2Keyboard
	}

	key := im.getActionKey(keys, action)
	if key >= 0 && key <= ebiten.KeyMax && im.keyboard().IsJustPressed(key) {
		return true
	}

	return false
}

func (im *InputManager) getActionKey(keys config.Keys, action Action) ebiten.Key {
	var name string
	var fallback ebiten.Key
	switch action {
	case ActionJump, ActionUp:
		name, fallback = "jump", ebiten.KeyArrowUp
	case ActionCrouch, ActionDown:
		name, fallback = "crouch", ebiten.KeyArrowDown
	case ActionLeft, ActionLeftUI:
		name, fallback = "left", ebiten.KeyArrowLeft
	case ActionRight, ActionRightUI:
		name, fallback = "right", ebiten.KeyArrowRight
	case ActionA:
		name, fallback = "a", ebiten.KeyZ
	case ActionB:
		name, fallback = "b", ebiten.KeyX
	case ActionC:
		name, fallback = "c", ebiten.KeyC
	case ActionX:
		name, fallback = "x", ebiten.KeyA
	case ActionY:
		name, fallback = "y", ebiten.KeyS
	case ActionZ:
		name, fallback = "z", ebiten.KeyD
	case ActionStart, ActionEnter:
		name, fallback = "start", ebiten.KeyEnter
	case ActionEscape:
		name, fallback = "escape", ebiten.KeyEscape
	case ActionSpace:
		return ebiten.KeySpace
	default:
		return config.NoKey
	}
	if key, configured := keys.Binding(name); configured {
		return key
	}
	return fallback
}

// Global instance for convenience
var GlobalManager *InputManager
