package paintown

import (
	"encoding/binary"

	"github.com/hajimehoshi/ebiten/v2"
)

// Logical inputs are sampled once per engine update. Merge keyboard/controller
// held states before computing edges so multiple devices cannot double-fire.
type inputAction int

const (
	inputLeft inputAction = iota
	inputRight
	inputUp
	inputDown
	inputJump
	inputAttack1
	inputAttack2
	inputMenuLeft
	inputMenuRight
	inputMenuUp
	inputMenuDown
	inputConfirm
	inputBack
	inputPause
	inputActionCount
)

type inputButtons [inputActionCount]bool

type logicalInput struct{ duration [inputActionCount]int }

func (input *logicalInput) advance(buttons inputButtons) {
	for action, held := range buttons {
		if held {
			input.duration[action]++
		} else {
			input.duration[action] = 0
		}
	}
}
func (input *logicalInput) held(action inputAction) bool    { return input.duration[action] > 0 }
func (input *logicalInput) pressed(action inputAction) bool { return input.duration[action] == 1 }
func (input *logicalInput) repeat(action inputAction) bool {
	ticks := input.duration[action]
	return ticks == 1 || (ticks > 25 && ticks%6 == 0)
}
func (input *logicalInput) gameplay() InputState {
	return InputState{
		HeldLeft: input.held(inputLeft), HeldRight: input.held(inputRight), HeldUp: input.held(inputUp), HeldDown: input.held(inputDown),
		HeldJump: input.held(inputJump), HeldAttack1: input.held(inputAttack1), HeldAttack2: input.held(inputAttack2),
		Left: input.pressed(inputLeft), Right: input.pressed(inputRight), Up: input.pressed(inputUp), Down: input.pressed(inputDown),
		Jump: input.pressed(inputJump), Attack1: input.pressed(inputAttack1), Attack2: input.pressed(inputAttack2),
	}
}

type MenuInput struct{ Left, Right, Up, Down, Confirm, Back bool }

func (input *logicalInput) menu() MenuInput {
	return MenuInput{Left: input.repeat(inputMenuLeft), Right: input.repeat(inputMenuRight), Up: input.repeat(inputMenuUp), Down: input.repeat(inputMenuDown), Confirm: input.pressed(inputConfirm), Back: input.pressed(inputBack)}
}

var currentInput logicalInput
var gamepadIDs []ebiten.GamepadID

// UpdateInput must be called exactly once per Engine.Update, before resuming
// gameplay/menu coroutines. Polling never waits for controller events.
func UpdateInput() {
	key := ebiten.IsKeyPressed
	left, right, up, down := key(ebiten.KeyArrowLeft), key(ebiten.KeyArrowRight), key(ebiten.KeyArrowUp), key(ebiten.KeyArrowDown)
	escape := key(ebiten.KeyEscape) || key(ebiten.KeyCapsLock)
	buttons := inputButtons{
		inputLeft: left, inputRight: right, inputUp: up, inputDown: down,
		inputJump: key(ebiten.KeySpace), inputAttack1: key(ebiten.KeyA), inputAttack2: key(ebiten.KeyS),
		inputMenuLeft: left, inputMenuRight: right, inputMenuUp: up || key(ebiten.KeyW), inputMenuDown: down || key(ebiten.KeyS),
		inputConfirm: key(ebiten.KeyEnter) || key(ebiten.KeySpace), inputBack: escape || key(ebiten.KeyBackspace), inputPause: escape,
	}
	// Merge both paths: the same physical pad may be visible through both,
	// but edges are derived only once from the combined held state.
	legacy, _ := pollLegacyController()
	native := inputButtons{}
	gamepadIDs = ebiten.AppendGamepadIDs(gamepadIDs[:0])
	for _, id := range gamepadIDs {
		if !ebiten.IsStandardGamepadLayoutAvailable(id) {
			continue
		}
		button := func(b ebiten.StandardGamepadButton) bool { return ebiten.IsStandardGamepadButtonPressed(id, b) }
		pad := controllerState{
			X:    ebiten.StandardGamepadAxisValue(id, ebiten.StandardGamepadAxisLeftStickHorizontal),
			Y:    ebiten.StandardGamepadAxisValue(id, ebiten.StandardGamepadAxisLeftStickVertical),
			Left: button(ebiten.StandardGamepadButtonLeftLeft), Right: button(ebiten.StandardGamepadButtonLeftRight),
			Up: button(ebiten.StandardGamepadButtonLeftTop), Down: button(ebiten.StandardGamepadButtonLeftBottom),
			Cross: button(ebiten.StandardGamepadButtonRightBottom), Circle: button(ebiten.StandardGamepadButtonRightRight),
			Square: button(ebiten.StandardGamepadButtonRightLeft), Triangle: button(ebiten.StandardGamepadButtonRightTop),
			Start: button(ebiten.StandardGamepadButtonCenterRight), Select: button(ebiten.StandardGamepadButtonCenterLeft), Home: button(ebiten.StandardGamepadButtonCenterCenter),
		}
		native.merge(pad.buttons())
	}
	buttons.merge(native)
	buttons.merge(legacy.buttons())
	currentInput.advance(buttons)
}

func ReadGameInput() InputState { return currentInput.gameplay() }
func ReadMenuInput() MenuInput  { return currentInput.menu() }
func PauseRequested() bool      { return currentInput.pressed(inputPause) }

func (buttons *inputButtons) merge(other inputButtons) {
	for index, held := range other {
		buttons[index] = buttons[index] || held
	}
}

const controllerDeadzone = 0.25

type controllerState struct {
	X, Y                                                 float64
	Left, Right, Up, Down                                bool
	Cross, Circle, Square, Triangle, Start, Select, Home bool
}

func (pad controllerState) buttons() inputButtons {
	left, right := pad.Left || pad.X < -controllerDeadzone, pad.Right || pad.X > controllerDeadzone
	up, down := pad.Up || pad.Y < -controllerDeadzone, pad.Down || pad.Y > controllerDeadzone
	return inputButtons{
		inputLeft: left, inputRight: right, inputUp: up, inputDown: down,
		inputMenuLeft: left, inputMenuRight: right, inputMenuUp: up, inputMenuDown: down,
		inputJump: pad.Cross, inputAttack1: pad.Square, inputAttack2: pad.Triangle || pad.Circle,
		inputConfirm: pad.Cross || pad.Start, inputBack: pad.Circle || pad.Select, inputPause: pad.Start || pad.Home,
	}
}

// The Linux joystick API enumerates buttons differently across drivers. Use
// JSIOCGBTNMAP/JSIOCGAXMAP codes, not hard-coded PS3 button indices. In
// particular, PS3 trigger axes (ABS_Z/RZ) must never count as movement.
type joystickState struct {
	axes                   [64]int16
	buttons                [512]bool
	axisCodes              [64]byte
	buttonCodes            [512]uint16
	axisCount, buttonCount int
	pending                [8]byte
	pendingBytes           int
}

func (stick *joystickState) feed(data []byte) {
	for len(data) > 0 {
		n := copy(stick.pending[stick.pendingBytes:], data)
		stick.pendingBytes += n
		data = data[n:]
		if stick.pendingBytes == len(stick.pending) {
			stick.event(stick.pending[:])
			stick.pendingBytes = 0
		}
	}
}
func (stick *joystickState) event(record []byte) {
	if len(record) != 8 {
		return
	}
	value, index := int16(binary.LittleEndian.Uint16(record[4:6])), int(record[7])
	switch record[6] &^ 0x80 { // JS_EVENT_INIT carries a normal state update.
	case 1:
		if index < stick.buttonCount {
			stick.buttons[index] = value != 0
		}
	case 2:
		if index < stick.axisCount {
			stick.axes[index] = value
		}
	}
}
func (stick *joystickState) controller() controllerState {
	var pad controllerState
	for index := 0; index < stick.axisCount; index++ {
		value := stick.axes[index]
		normalized := max(-1, min(1, float64(value)/32767))
		switch stick.axisCodes[index] {
		case 0:
			pad.X = normalized // ABS_X
		case 1:
			pad.Y = normalized // ABS_Y
		case 16:
			pad.Left, pad.Right = value < 0, value > 0 // ABS_HAT0X
		case 17:
			pad.Up, pad.Down = value < 0, value > 0 // ABS_HAT0Y
		}
	}
	for index := 0; index < stick.buttonCount; index++ {
		if !stick.buttons[index] {
			continue
		}
		switch stick.buttonCodes[index] {
		case 304:
			pad.Cross = true // BTN_SOUTH
		case 305:
			pad.Circle = true // BTN_EAST
		case 307:
			pad.Triangle = true // BTN_NORTH
		case 308:
			pad.Square = true // BTN_WEST
		case 314:
			pad.Select = true
		case 315:
			pad.Start = true
		case 316:
			pad.Home = true
		case 544:
			pad.Up = true
		case 545:
			pad.Down = true
		case 546:
			pad.Left = true
		case 547:
			pad.Right = true
		}
	}
	return pad
}
