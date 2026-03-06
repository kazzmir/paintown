package character

import (
	"fmt"
	"os"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
)

// Character represents a MUGEN entity running through the State Machine.
// It implements the Environment interface so the state machine can execute controllers against it.
type Character struct {
	// CNS data containing states and physics globals
	StateFile *cns.CNS

	// AIR data containing animation definitions
	AirFile *air.Data

	// State Machine execution engine
	SM *StateMachine

	// Position and Velocity
	X, Y       float64
	VelX, VelY float64

	// internal state variables
	Time         int
	StateNo      int
	Anim         int
	AnimTime     int    // Total ticks since animation started
	AnimElem     int    // Current frame index (0-based internally, MUGEN uses 1-based in triggers)
	AnimElemTime int    // Ticks passed in current element
	StateType    string // S, C, A, L
	Physics      string // S, C, A, N
	MoveType     string // I, A, H
	Ctrl         bool   // If character is in control

	// Variables explicitly defined by users (var(0) - var(59), fvar(0) - fvar(39))
	IntVars   [60]int
	FloatVars [40]float64

	// Command buffer reference
	Commands *CommandBuffer

	// Player ID (1 or 2)
	PlayerID int
}

// NewCharacter returns a newly initialized character.
func NewCharacter(stateFile *cns.CNS, airFile *air.Data) *Character {
	return &Character{
		StateFile:    stateFile,
		AirFile:      airFile,
		SM:           NewStateMachine(),
		Time:         0,
		StateNo:      0,
		Anim:         0,
		AnimTime:     0,
		AnimElem:     0,
		AnimElemTime: 0,
		StateType:    "S",
		Physics:      "S",
		MoveType:     "I",
		Ctrl:         true,
		PlayerID:     1, // Default to P1
	}
}

// Update ticks the character's state machine forward by one frame.
func (c *Character) Update() {
	if c.Time%60 == 0 {
		fmt.Printf("Character.Update: Tick %d, State %d\n", c.Time, c.StateNo)
	}

	// MUGEN multi-level state processing:
	// 1. Process negative states -3, -2, -1
	// 2. Process current state
	// If any ChangeState occurs, the rest of THAT state level is skipped.

	// Placeholder for active commands (will be integrated with commands.go)
	// For now, continue using whatever is currently in the environment

	// Reset ChangeState counter to prevent infinite loops
	changeCount := 0

	processState := func(stateNo int) bool {
		if changeCount > 100 {
			fmt.Printf("Tick %d: Recursion limit reached!\n", c.Time)
			return true // Halt
		}
		if stateDef, ok := c.StateFile.States[stateNo]; ok {
			changed, err := c.SM.ExecuteState(stateDef, c)
			if err != nil {
				fmt.Printf("Tick %d: Error in state %d: %v\n", c.Time, stateNo, err)
			}
			if changed {
				fmt.Printf("Tick %d: State changed to %d (via %d)\n", c.Time, c.StateNo, stateNo)
				os.Stdout.Sync()
				changeCount++
				return true
			}
		}
		return false
	}

	// 1. Pass through special states -3, -2, -1
	processState(-3)
	processState(-2)
	processState(-1)

	// 2. Process current state (and any new states we transition into)
	for i := 0; i < 10; i++ { // Limit to 10 transitions per tick to avoid infinite loops but allow rapid setup
		if !processState(c.StateNo) {
			break
		}
	}

	// Logic for time increment (if not changed state)
	c.Time++
	c.AnimTime++
	c.UpdateAnimation()

	// 3. Physics calculation (apply Velocity based on Physics type)
	c.X += c.VelX
	c.Y += c.VelY

	if c.Time%60 == 0 {
		fmt.Printf("Tick %d: State=%d, Pos=(%.1f, %.1f)\n", c.Time, c.StateNo, c.X, c.Y)
	}
}

// --- Environment Interface Implementation ---

func (c *Character) GetTime() int {
	return c.Time
}

func (c *Character) GetStateNo() int {
	return c.StateNo
}

func (c *Character) GetAnim() int {
	return c.Anim
}

func (c *Character) GetAnimElem() int {
	return c.AnimElem + 1 // MUGEN uses 1-based indexing for AnimElem
}

func (c *Character) GetAnimTime() int {
	if c.AirFile == nil {
		return 0
	}
	anim, ok := c.AirFile.Actions[c.Anim]
	if !ok || len(anim.Elements) == 0 {
		return 0
	}

	if c.AnimElem >= len(anim.Elements) {
		return 0
	}

	currElem := anim.Elements[c.AnimElem]
	if currElem.Time == -1 {
		return -1
	}

	// MUGEN animationTime() logic
	total := 0
	for i := c.AnimElem + 1; i < len(anim.Elements); i++ {
		total += anim.Elements[i].Time
	}
	remainingInCurr := anim.Elements[c.AnimElem].Time - c.AnimElemTime
	if remainingInCurr < 0 {
		remainingInCurr = 0
	}
	return total + remainingInCurr
}

func (c *Character) GetVar(idx int) int {
	if idx >= 0 && idx < 60 {
		return c.IntVars[idx]
	}
	return 0
}

func (c *Character) SetVar(idx, val int) {
	if idx >= 0 && idx < 60 {
		c.IntVars[idx] = val
	}
}

func (c *Character) GetFVar(idx int) float64 {
	if idx >= 0 && idx < 40 {
		return c.FloatVars[idx]
	}
	return 0
}

func (c *Character) SetFVar(idx int, val float64) {
	if idx >= 0 && idx < 40 {
		c.FloatVars[idx] = val
	}
}

func (c *Character) GetStateType() string {
	return c.StateType
}

func (c *Character) GetPhysics() string {
	return c.Physics
}

func (c *Character) GetMoveType() string {
	return c.MoveType
}

func (c *Character) AddVelocity(x, y float64) {
	c.VelX += x
	c.VelY += y
}

func (c *Character) SetVelocity(x, y float64) {
	c.VelX = x
	c.VelY = y
}

func (c *Character) AddPosition(x, y float64) {
	c.X += x
	c.Y += y
}

func (c *Character) SetPosition(x, y float64) {
	c.X = x
	c.Y = y
}

func (c *Character) GetPos() (float64, float64) {
	return c.X, c.Y
}

func (c *Character) GetVel() (float64, float64) {
	return c.VelX, c.VelY
}

func (c *Character) GetCtrl() bool {
	return c.Ctrl
}

func (c *Character) ChangeState(stateNo int, ctrl int, anim int) {
	oldStateNo := c.StateNo
	c.StateNo = stateNo
	c.Time = 0 // Entering a new state resets Time to 0.

	// In MUGEN, jumping to a state automatically sets Anim, Physics, StateType based on [Statedef]
	if nextState, ok := c.StateFile.States[stateNo]; ok {
		if nextState.Type != "" {
			c.StateType = nextState.Type
		}
		if nextState.Physics != "" && strings.ToLower(nextState.Physics) != "u" {
			c.Physics = nextState.Physics
		}
		if nextState.MoveType != "" && strings.ToLower(nextState.MoveType) != "u" {
			c.MoveType = nextState.MoveType
		}

		// Update Ctrl flag.
		// 1. Priority: ChangeState(..., ctrl, ...) parameter
		if ctrl != -1 {
			c.Ctrl = ctrl != 0
		} else {
			// 2. Priority: [Statedef] ctrl parameter
			if nextState.Ctrl != -1 {
				c.Ctrl = nextState.Ctrl != 0
			}
			// 3. Fallback: remains unchanged (MUGEN behavior)
		}

		// Update Anim
		// 1. Priority: ChangeState(..., ..., anim) parameter
		if anim != -1 {
			c.ChangeAnim(anim)
		} else {
			// 2. Priority: [Statedef] anim parameter
			if nextState.Anim != -1 {
				c.ChangeAnim(nextState.Anim)
			}
		}
	}

	if oldStateNo != stateNo {
		fmt.Printf("Tick %d: ChangeState %d -> %d, Ctrl=%v, Anim=%d\n", c.Time, oldStateNo, stateNo, c.Ctrl, c.Anim)
	}
}

func (c *Character) ChangeAnim(animNo int) {
	c.Anim = animNo
	c.AnimTime = 0
	c.AnimElem = 0
	c.AnimElemTime = 0
}

func (c *Character) UpdateAnimation() {
	if c.AirFile == nil {
		return
	}
	anim, ok := c.AirFile.Actions[c.Anim]
	if !ok || len(anim.Elements) == 0 {
		return
	}

	if c.AnimElem < len(anim.Elements) {
		currElem := anim.Elements[c.AnimElem]
		if currElem.Time != -1 {
			c.AnimElemTime++
			if c.AnimElemTime >= currElem.Time {
				c.AnimElemTime = 0
				if c.AnimElem+1 < len(anim.Elements) {
					c.AnimElem++
				} else {
					// End of animation reached
					if anim.LoopStart != -1 {
						// Loop back to the LoopStart index
						c.AnimElem = anim.LoopStart
					} else {
						// Stay at the last frame and let AnimElemTime continue to accumulate,
						// which keeps GetAnimTime() at 0
						c.AnimElemTime = currElem.Time - 1
					}
				}
			}
		}
	}
}

func (c *Character) Command(name string) bool {
	if c.Commands == nil || len(c.Commands.History) == 0 {
		return false
	}

	last := c.Commands.History[len(c.Commands.History)-1]
	parts := strings.Split(last, ",")
	found := false
	for _, p := range parts {
		if strings.TrimSpace(p) == name {
			found = true
			break
		}
	}

	if !found {
		return false
	}

	// MUGEN edge-triggering: for buttons, it's only a match if it wasn't held in the previous tick.
	// Directional inputs (holdfwd, etc.) are level-triggered and should return true as long as held.
	if IsButton(name) {
		if len(c.Commands.History) > 1 {
			prev := c.Commands.History[len(c.Commands.History)-2]
			prevParts := strings.Split(prev, ",")
			for _, p := range prevParts {
				if strings.TrimSpace(p) == name {
					// Still held from previous tick, not a new "press"
					return false
				}
			}
		}
	}

	return true
}
