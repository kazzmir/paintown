package input

import (
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

type InputManager struct {
	P1Keyboard config.Keys
	P2Keyboard config.Keys
	P1Joystick config.Keys
	P2Joystick config.Keys

	// Specific for Ebiten Gamepad support
	GamepadIDs []ebiten.GamepadID
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
	im.GamepadIDs = inpututil.AppendJustConnectedGamepadIDs(im.GamepadIDs[:0])
	im.GamepadIDs = ebiten.AppendGamepadIDs(im.GamepadIDs)
}

func (im *InputManager) IsPressed(player int, action Action) bool {
	keys := im.P1Keyboard
	if player == 2 {
		keys = im.P2Keyboard
	}

	key := im.getActionKey(keys, action)
	if key != 0 && ebiten.IsKeyPressed(key) {
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
	if key != 0 && inpututil.IsKeyJustPressed(key) {
		return true
	}

	return false
}

func (im *InputManager) getActionKey(keys config.Keys, action Action) ebiten.Key {
	switch action {
	case ActionJump, ActionUp:
		if keys.Jump == 0 && action == ActionUp {
			return ebiten.KeyUp
		}
		return keys.Jump
	case ActionCrouch, ActionDown:
		if keys.Crouch == 0 && action == ActionDown {
			return ebiten.KeyDown
		}
		return keys.Crouch
	case ActionLeft, ActionLeftUI:
		if keys.Left == 0 && action == ActionLeftUI {
			return ebiten.KeyLeft
		}
		return keys.Left
	case ActionRight, ActionRightUI:
		if keys.Right == 0 && action == ActionRightUI {
			return ebiten.KeyRight
		}
		return keys.Right
	case ActionA:
		return keys.A
	case ActionB:
		return keys.B
	case ActionC:
		return keys.C
	case ActionX:
		return keys.X
	case ActionY:
		return keys.Y
	case ActionZ:
		return keys.Z
	case ActionStart:
		return keys.Start
	case ActionEscape:
		if keys.Escape == 0 {
			return ebiten.KeyEscape
		}
		return keys.Escape
	case ActionEnter:
		return ebiten.KeyEnter
	case ActionSpace:
		return ebiten.KeySpace
	}
	return 0
}

// Global instance for convenience
var GlobalManager *InputManager
