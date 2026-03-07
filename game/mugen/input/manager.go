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
	P1Keyboard config.Keys
	P2Keyboard config.Keys
	P1Joystick config.Keys
	P2Joystick config.Keys

	// Specific for Ebiten Gamepad support
	GamepadIDs []ebiten.GamepadID

	LastKeyEvent    string
	LastActionEvent string
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

	// Reset action log each frame
	im.LastActionEvent = ""

	// Update raw key events (store only the latest for status line)
	for k := ebiten.Key(0); k <= ebiten.KeyMax; k++ {
		if inpututil.IsKeyJustPressed(k) {
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
	if key != 0 && ebiten.IsKeyPressed(key) {
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
	if key != 0 && inpututil.IsKeyJustPressed(key) {
		return true
	}

	return false
}

func (im *InputManager) getActionKey(keys config.Keys, action Action) ebiten.Key {
	switch action {
	case ActionJump, ActionUp:
		if keys.Jump == 0 {
			return ebiten.KeyUp
		}
		return keys.Jump
	case ActionCrouch, ActionDown:
		if keys.Crouch == 0 {
			return ebiten.KeyDown
		}
		return keys.Crouch
	case ActionLeft, ActionLeftUI:
		if keys.Left == 0 {
			return ebiten.KeyLeft
		}
		return keys.Left
	case ActionRight, ActionRightUI:
		if keys.Right == 0 {
			return ebiten.KeyRight
		}
		return keys.Right
	case ActionA:
		if keys.A == 0 {
			return ebiten.KeyZ
		}
		return keys.A
	case ActionB:
		if keys.B == 0 {
			return ebiten.KeyX
		}
		return keys.B
	case ActionC:
		if keys.C == 0 {
			return ebiten.KeyC
		}
		return keys.C
	case ActionX:
		if keys.X == 0 {
			return ebiten.KeyA
		}
		return keys.X
	case ActionY:
		if keys.Y == 0 {
			return ebiten.KeyS
		}
		return keys.Y
	case ActionZ:
		if keys.Z == 0 {
			return ebiten.KeyD
		}
		return keys.Z
	case ActionStart:
		if keys.Start == 0 {
			return ebiten.KeyEnter
		}
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
