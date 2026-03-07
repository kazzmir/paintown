package character

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/input"
)

// Character represents a MUGEN entity running through the State Machine.
// It implements the Environment interface so the state machine can execute controllers against it.
type Character struct {
	// CNS data containing states and physics globals
	StateFile *cns.CNS

	// AIR data containing animation definitions
	AirFile *air.Data
	// CMD data containing command definitions
	CmdFile *cmd.Data

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
	// Buffered commands for the current tick
	ActiveCommands []string

	// Player ID (1 or 2)
	PlayerID int

	// Hit pause/shake time
	ShakeTime int

	// Facing direction (1 for Right, -1 for Left)
	Facing int

	// Controller persistence counters (key is controller name or ID)
	PersistenceCounters map[string]int

	// Debug info for status line
	LastCommandMatched string
	LastStateChange    string

	// Stats
	Life       int
	MaxLife    int
	Power      int
	MaxPower   int
	AttackMul  float64
	DefenceMul float64

	// Physics/Logic flags
	GravityActive bool
	PosFrozen     bool

	// Visuals
	SprPriority int

	CurrentHitDef     map[string]string
	Specials          map[string]bool
	ActiveReversalDef map[string]string

	// Hierarchy
	Parent *Character
	Root   *Character
	Target []*Character

	// Stats
	HitCount int

	// Visual/Drawing State
	DrawAngle  float64
	DrawOffset []float64

	// System/Debug
	Clipboard string

	// Hierarchy
	Helpers []*Character

	// Constants
	VictoryQuoteIndex int

	// Combat status for triggers
	MoveHit     int
	MoveContact int
	MoveGuarded int
}

// NewCharacter returns a newly initialized character.
func NewCharacter(stateFile *cns.CNS, airFile *air.Data, cmdFile *cmd.Data) *Character {
	return &Character{
		StateFile:           stateFile,
		AirFile:             airFile,
		CmdFile:             cmdFile,
		SM:                  NewStateMachine(),
		Time:                0,
		StateNo:             0,
		Anim:                0,
		AnimTime:            0,
		AnimElem:            0,
		AnimElemTime:        0,
		StateType:           "S",
		Physics:             "S",
		MoveType:            "I",
		Ctrl:                true,
		PlayerID:            1, // Default to P1
		Facing:              1, // Default to Right
		PersistenceCounters: make(map[string]int),
		ActiveCommands:      make([]string, 0),
		Life:                stateFile.Data.Life,
		MaxLife:             stateFile.Data.Life,
		Power:               0,
		MaxPower:            3000,
		AttackMul:           1.0,
		DefenceMul:          1.0,
		GravityActive:       false,
		CurrentHitDef:       make(map[string]string),
		ActiveReversalDef:   make(map[string]string),
		Specials:            make(map[string]bool),
		Target:              make([]*Character, 0),
		Helpers:             make([]*Character, 0),
		DrawOffset:          []float64{0, 0},
	}
}

// Update ticks the character's state machine forward by one frame.
func (c *Character) Update() {
	/*
		if c.Time%60 == 0 {
			fmt.Printf("Character.Update: Tick %d, State %d\n", c.Time, c.StateNo)
		}
	*/

	// 1. Buffer commands once for the start of the tick
	c.ActiveCommands = make([]string, 0)
	c.LastCommandMatched = "" // Clear stale match
	// Clear frame-local specials
	c.Specials = make(map[string]bool)
	c.ActiveReversalDef = make(map[string]string)
	c.DrawAngle = 0
	c.DrawOffset = []float64{0, 0}
	// Clear stale debug info globally so we only see current frame actions.
	if input.GlobalManager != nil {
		input.GlobalManager.LastKeyEvent = ""
		input.GlobalManager.LastActionEvent = ""
	}

	if c.Commands != nil && c.CmdFile != nil {
		// ADDED: Actually record the raw inputs for this tick into the history
		// PASS c.Facing to handle relative directions (F/B)
		raw := GetRawInputs(c.PlayerID, c.Facing)
		c.Commands.Add(raw)

		if len(c.Commands.History) > 0 {
			var matched []string
			for name, cmdList := range c.CmdFile.Commands {
				for _, cmd := range cmdList {
					// MUGEN command matching
					if c.Commands.Match(cmd.Sequence, cmd.Time) {
						c.ActiveCommands = append(c.ActiveCommands, name)
						matched = append(matched, name)
						break // Found a match for this command name
					}
				}
			}
			if len(matched) > 0 {
				c.LastCommandMatched = strings.Join(matched, ", ")
			}
		}
	}

	// 2. Hit Pause Logic
	isPaused := c.ShakeTime > 0
	if isPaused {
		c.ShakeTime--
	}

	// 3. MUGEN multi-level state processing:
	// -3, then -2, then -1, then current state level.

	// Reset ChangeState counter to prevent infinite loops
	changeCount := 0

	processState := func(stateNo int) bool {
		/*
			if sd, ok := c.StateFile.States[stateNo]; ok {
				fmt.Printf("Tick %d: Processing state %d (%s)\n", c.Time, stateNo, sd.Name)
			} else {
				fmt.Printf("Tick %d: Processing state %d\n", c.Time, stateNo)
			}
		*/
		if changeCount > 100 {
			fmt.Printf("Tick %d: Recursion limit reached!\n", c.Time)
			return true // Halt
		}
		if stateDef, ok := c.StateFile.States[stateNo]; ok {
			// Diagnostic: is AnimTime 0?
			if c.GetAnimTime() == 0 {
				fmt.Printf("[Tick %d] State %d: AnimTime=0 reached\n", c.GetTime(), stateNo)
			}

			changed, err := c.SM.ExecuteState(stateDef, c)
			if err != nil {
				fmt.Printf("Tick %d: Error in state %d: %v\n", c.Time, stateNo, err)
			}
			if changed {
				// c.LastStateChange already set in ChangeState
				changeCount++
				return true
			}
		}
		return false
	}

	// Negative states processing order
	processState(-3)
	processState(-2)
	processState(-1)

	// Process current state (and any new states we transition into)
	for i := 0; i < 10; i++ { // Limit to 10 transitions per tick
		if !processState(c.StateNo) {
			break
		}
	}

	// 5. Physics and Time update (MUGEN accuracy states these happen AFTER state processing)
	if !isPaused {
		c.Time++
		c.AnimTime++
		c.UpdateAnimation()

		// Apply gravity for air physics (or if explicitly active)
		if strings.ToUpper(c.Physics) == "A" || c.GravityActive {
			yaccel := 0.44 // Default
			if c.StateFile != nil {
				yaccel = c.StateFile.Movement.YAccel
			}
			c.VelY += yaccel
		}

		if !c.PosFrozen {
			c.X += c.VelX
			c.Y += c.VelY
		}

		// Standard MUGEN Grounding: if player touches the ground in an AIR state, transition to Land (52)
		// Only trigger Landing if we are moving downwards and were previously in air physics
		if c.Y >= 0 && c.VelY >= 0 && strings.ToUpper(c.Physics) == "A" {
			c.Y = 0
			c.VelY = 0
			// State 52 is standard Landing state in MUGEN common1.cns
			c.ChangeState(52, -1, -1)
		}
	}

	/*
		if c.Time%60 == 0 {
			fmt.Printf("Tick %d: State=%d, Pos=(%.1f, %.1f)\n", c.Time, c.StateNo, c.X, c.Y)
		}
	*/
}

// --- Environment Interface Implementation ---

func (c *Character) GetTime() int {
	return c.Time
}

func (c *Character) GetStateNo() int {
	return c.StateNo
}

func (c *Character) GetFacing() int {
	return c.Facing
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

	// MUGEN animationTime() returns -(remaining_duration) or similar.
	// Actually, the common usage is AnimTime = 0 check.
	// We'll return (total_remaining_ticks).
	total := 0
	for i := c.AnimElem; i < len(anim.Elements); i++ {
		time := anim.Elements[i].Time
		if time == -1 {
			return -1 // Infinite duration
		}
		if i == c.AnimElem {
			// Remaining for current element
			total += time - c.AnimElemTime
		} else {
			total += time
		}
	}
	// MUGEN Accuracy: AnimTime = 0 means the last tick of the last frame.
	// total is the number of ticks including the current one.
	// So 1 tick remaining (current tick) should return 0.
	return -(total - 1)
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

func (c *Character) Turn() {
	c.Facing = -c.Facing
}

func (c *Character) SetCtrl(ctrl bool) {
	c.Ctrl = ctrl
}

func (c *Character) SetStateType(stype string) {
	c.StateType = strings.ToUpper(stype)
}

func (c *Character) SetPhysics(physics string) {
	c.Physics = strings.ToUpper(physics)
}

func (c *Character) SetMoveType(mtype string) {
	c.MoveType = strings.ToUpper(mtype)
}

func (c *Character) SetPower(power int) {
	c.Power = power
	if c.Power > c.MaxPower {
		c.Power = c.MaxPower
	}
	if c.Power < 0 {
		c.Power = 0
	}
}

func (c *Character) AddPower(power int) {
	c.SetPower(c.Power + power)
}

func (c *Character) GetPower() int {
	return c.Power
}

func (c *Character) SetLife(life int) {
	c.Life = life
	if c.Life > c.MaxLife {
		c.Life = c.MaxLife
	}
	if c.Life < 0 {
		c.Life = 0
	}
}

func (c *Character) AddLife(life int) {
	c.SetLife(c.Life + life)
}

func (c *Character) GetLife() int {
	return c.Life
}

func (c *Character) SetAttackMul(mul float64) {
	c.AttackMul = mul
}

func (c *Character) SetDefenceMul(mul float64) {
	c.DefenceMul = mul
}

func (c *Character) SetSprPriority(priority int) {
	c.SprPriority = priority
}

func (c *Character) SetGravity(active bool) {
	c.GravityActive = active
}

func (c *Character) VelMul(x, y float64) {
	c.VelX *= x
	c.VelY *= y
}

func (c *Character) PosFreeze(active bool) {
	c.PosFrozen = active
}

func (c *Character) HitDef(params map[string]string) {
	c.CurrentHitDef = params
}

func (c *Character) Projectile(params map[string]string) {
	// Stub for now
}

func (c *Character) AssertSpecial(flag string) {
	c.Specials[strings.ToLower(flag)] = true
}

func (c *Character) Explod(params map[string]string) {
	// Stub for now
}

func (c *Character) RemoveExplod(id int) {
	// Stub for now
}

func (c *Character) AfterImage(params map[string]string) {
	// Stub
}

func (c *Character) AfterImageTime(time int) {
	// Stub
}

func (c *Character) EnvShake(params map[string]string) {
	// Stub
}

func (c *Character) EnvColor(params map[string]string) {
	// Stub
}

func (c *Character) ScreenBound(params map[string]string) {
	// Stub
}

func (c *Character) Width(params map[string]string) {
	// Stub
}

func (c *Character) Trans(params map[string]string) {
	// Stub
}

func (c *Character) BindToParent(params map[string]string) {
	// Logic: set position relative to parent axis
}

func (c *Character) BindToRoot(params map[string]string) {
	// Logic: set position relative to root axis
}

func (c *Character) BindToTarget(params map[string]string) {
	// Logic: set position relative to target axis
}

func (c *Character) TargetBind(params map[string]string) {
	// Logic: set target position relative to self axis
}

func (c *Character) TargetDrop(id int) {
	// Logic: remove target from list
}

func (c *Character) TargetFacing(id, facing int) {
	// Logic: set target facing
}

func (c *Character) TargetLifeAdd(id, val int, kill bool) {
	// Logic: add life to target
}

func (c *Character) TargetPowerAdd(id, val int) {
	// Logic: add power to target
}

func (c *Character) TargetState(id, state int) {
	// Logic: change target state
}

func (c *Character) TargetVelAdd(x, y float64, id int) {
	// Logic: add velocity to target
}

func (c *Character) TargetVelSet(x, y float64, id int) {
	// Logic: set velocity for target
}

func (c *Character) ReversalDef(params map[string]string) {
	c.ActiveReversalDef = params
}

func (c *Character) MoveHitReset() {
	// Logic: reset hit flags
}

func (c *Character) HitAdd(val int) {
	c.HitCount += val
}

func (c *Character) ChangeAnim2(animNo int) {
	// Logic: use P1's AIR file for P2
}

func (c *Character) SelfState(stateNo int, ctrl, anim int) {
	// Logic: change back to own state file
	c.ChangeState(stateNo, ctrl, anim)
}

func (c *Character) PalFX(params map[string]string) {
	// Stub: palette effects
}

func (c *Character) AllPalFX(params map[string]string) {
	// Stub: global palette effects
}

func (c *Character) BGPalFX(params map[string]string) {
	// Stub: background palette effects
}

func (c *Character) RemapPal(params map[string]string) {
	// Stub: palette remapping
}

func (c *Character) AngleAdd(angle float64) {
	c.DrawAngle += angle
}

func (c *Character) AngleDraw(angle float64) {
	// Logic: set drawing angle for the current frame
	c.DrawAngle = angle
}

func (c *Character) AngleMul(mul float64) {
	c.DrawAngle *= mul
}

func (c *Character) AngleSet(angle float64) {
	c.DrawAngle = angle
}

func (c *Character) Offset(x, y float64) {
	c.DrawOffset = []float64{x, y}
}

func (c *Character) SndPan(channel int, pan int) {
	// Stub: sound panning
}

func (c *Character) StopSnd(channel int) {
	// Stub: stop sound
}

func (c *Character) AppendToClipboard(text string) {
	c.Clipboard += "\n" + text
}

func (c *Character) ClearClipboard() {
	c.Clipboard = ""
}

func (c *Character) DisplayToClipboard(text string) {
	c.Clipboard = text
}

func (c *Character) ForceFeedback(params map[string]string) {
	// Stub: Rumble/Haptics
}

func (c *Character) SuperPause(params map[string]string) {
	// Logic: handle global time freeze and darken
}

func (c *Character) Helper(params map[string]string) {
	// Logic: create a new character instance as a child
}

func (c *Character) DestroySelf() {
	// Logic: mark for deletion from the game world
}

func (c *Character) FallEnvShake(params map[string]string) {
	// Logic: shake screen on fall impact
}

func (c *Character) GameMakeAnim(params map[string]string) {
	// Stub: system animations (dust, sparks)
}

func (c *Character) MakeDust(params map[string]string) {
	// Stub: dust particles
}

func (c *Character) VarRandom(id int, min, max int) {
	// Stub: set variable to random value
}

func (c *Character) VarRangeSet(min, max int, val int) {
	for i := min; i <= max && i < len(c.IntVars); i++ {
		c.IntVars[i] = val
	}
}

func (c *Character) FVarRangeSet(min, max int, val float64) {
	for i := min; i <= max && i < len(c.FloatVars); i++ {
		c.FloatVars[i] = val
	}
}

func (c *Character) VictoryQuote(quote int) {
	c.VictoryQuoteIndex = quote
}

func (c *Character) GetMoveHit() int {
	return c.MoveHit
}

func (c *Character) GetMoveContact() int {
	return c.MoveContact
}

func (c *Character) GetMoveGuarded() int {
	return c.MoveGuarded
}

func (c *Character) GetCtrl() bool {
	return c.Ctrl
}

func (c *Character) IsPaused() bool {
	return c.ShakeTime > 0
}

func (c *Character) GetPersistence(key string) int {
	return c.PersistenceCounters[key]
}

func (c *Character) SetPersistence(key string, val int) {
	c.PersistenceCounters[key] = val
}

func (c *Character) GetStateFile() *cns.CNS {
	return c.StateFile
}

func (c *Character) ChangeState(stateNo int, ctrl int, anim int) {
	if c.StateNo != stateNo {
		stateName := ""
		if sd, ok := c.StateFile.States[stateNo]; ok {
			stateName = sd.Name
		}
		if stateName != "" {
			c.LastStateChange = fmt.Sprintf("%d -> %d (%s)", c.StateNo, stateNo, stateName)
		} else {
			c.LastStateChange = fmt.Sprintf("%d -> %d", c.StateNo, stateNo)
		}
	}
	c.StateNo = stateNo
	c.Time = 0 // Entering a new state resets Time to 0.

	// MUGEN Accuracy: Reseting persistence counters for all POSITIVE states.
	// Negative states (-3, -2, -1) maintain their persistence across state changes.
	for k := range c.PersistenceCounters {
		if strings.HasPrefix(k, "pos") {
			delete(c.PersistenceCounters, k)
		}
	}

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

	/*
		if oldStateNo != stateNo {
			fmt.Printf("Tick %d: ChangeState %d -> %d, Ctrl=%v, Anim=%d\n", c.Time, oldStateNo, stateNo, c.Ctrl, c.Anim)
		}
	*/
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
				if c.AnimElem+1 < len(anim.Elements) {
					c.AnimElem++
					c.AnimElemTime = 0
				} else {
					// End of animation reached. Default to looping back to the beginning or LoopStart
					loopTo := 0
					if anim.LoopStart != -1 {
						loopTo = anim.LoopStart
					}
					c.AnimElem = loopTo
					c.AnimElemTime = 0
				}
			}
		}
	}
}

func (c *Character) Command(name string) bool {
	// 1. Check matched sequences from c.ActiveCommands (computed in Update)
	for _, active := range c.ActiveCommands {
		if active == name {
			return true
		}
	}

	// 2. Fallback: Check current raw input buffer for simple button/direction names
	if c.Commands != nil && len(c.Commands.History) > 0 {
		last := cbLast(c.Commands.History)
		for _, p := range last {
			// p is standardized spelled-out name: "DirForward", "ButtonA", etc.
			// name is the command being checked (e.g. from a trigger).

			// Map name to spelled out version
			mapped := ""
			switch name {
			case "F":
				mapped = "DirForward"
			case "B":
				mapped = "DirBack"
			case "U":
				mapped = "DirUp"
			case "D":
				mapped = "DirDown"
			case "a":
				mapped = "ButtonA"
			case "b":
				mapped = "ButtonB"
			case "c":
				mapped = "ButtonC"
			case "x":
				mapped = "ButtonX"
			case "y":
				mapped = "ButtonY"
			case "z":
				mapped = "ButtonZ"
			case "s":
				mapped = "ButtonStart"
			default:
				// Lenient check for lowercase directions often used in older scripts
				switch strings.ToLower(name) {
				case "f":
					mapped = "DirForward"
				case "b":
					mapped = "DirBack"
				case "u":
					mapped = "DirUp"
				case "d":
					mapped = "DirDown"
				}
			}

			if p == mapped {
				return true
			}
		}
	}

	return false
}

func cbLast(history [][]string) []string {
	if len(history) == 0 {
		return nil
	}
	return history[len(history)-1]
}
