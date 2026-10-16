package character

import (
	"fmt"
	"math"
	"sort"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/logger"
)

// Character represents a MUGEN entity running through the State Machine.
// It implements the Environment interface so the state machine can execute controllers against it.
type Character struct {
	RuntimeError     error
	stateInitialized bool
	updateActive     bool
	pausedThisTick   bool
	JuggleCost       int
	HitDefActive     bool
	contactSerial    uint64
	RuntimeWarnings  []string
	warningSeen      map[string]bool

	// CNS data containing states and physics globals
	StateFile  *cns.CNS
	LocalCoord coord.Space

	// AIR data containing animation definitions
	AirFile   *air.Data
	animation *air.Player
	// CMD data containing command definitions
	CmdFile *cmd.Data

	// State Machine execution engine
	SM *StateMachine

	// Position and Velocity
	X, Y, Z    float64
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
	PrevStateNo  int    // Tracks the previous state before the last ChangeState

	// Variables explicitly defined by users (var(0) - var(59), fvar(0) - fvar(39))
	IntVars   [60]int
	FloatVars [40]float64

	// System variables (sysvar(0) - sysvar(4), sysfvar(0) - sysfvar(4))
	SysVars      [5]int
	SysFloatVars [5]float64

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

	// Initial positions from stage
	StartX, StartY, StartZ float64
	StartFacing            int

	// Stage Boundaries and Screen positioning
	LeftBound, RightBound float64
	TopBound, BotBound    float64
	CameraX               float64
	ScreenWidth           int

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

	CurrentHitDef     cns.HitDef
	Specials          map[string]bool
	ActiveReversalDef map[string]string

	// Hierarchy
	Parent *Character
	Root   *Character
	Target []*Character

	// Stats
	HitCount int
	// ComboCount is the displayed combo counter, distinct from the current
	// attack's HitCount. Match combat owns its reset/credit rules.
	ComboCount int

	// Visual/Drawing State
	DrawAngle  float64
	DrawOffset []float64

	// System/Debug
	Clipboard string

	// Hierarchy
	Helpers     []*Character
	Projectiles []*Character
	Explods     []*ExplodData

	// Constants
	VictoryQuoteIndex int

	// Combat status for triggers
	MoveHit     int
	MoveContact int
	MoveGuarded int

	// HitGuard tracks which targets (by PlayerID) have already been hit by the current HitDef
	// activation, preventing the same attack from landing multiple times per swing.
	HitGuard         map[int]bool
	GetHit           cns.HitDef
	HitTime          int
	HitShakeTime     int // Separate from visual ShakeTime? No, reuse ShakeTime.
	HitFall          bool
	HitFallVelActive bool // Tracks if HitFallVel controller was triggered

	// Input latching to stabilize diagonals (input name -> ticks remaining)
	InputLatch map[string]int

	// Protection logic
	NotHitByFlags map[string]int // flag -> ticks remaining
	HitByFlags    map[string]int // flag -> ticks remaining

	// Rounds
	RoundsExisted int

	// Advanced Movement
	AirJumpCount int

	// Match state
	RoundState  int
	IsMatchOver bool
}

// NewCharacter returns a newly initialized character.
func NewCharacter(stateFile *cns.CNS, airFile *air.Data, cmdFile *cmd.Data) *Character {
	maxPower := stateFile.Data.Power
	if maxPower == 0 && len(stateFile.ConstantFields["data.power"]) == 0 {
		maxPower = 3000
	}
	return &Character{
		LocalCoord:          coord.Default(),
		StateFile:           stateFile,
		AirFile:             airFile,
		CmdFile:             cmdFile,
		SM:                  NewStateMachine(),
		Time:                0,
		StateNo:             0,
		PrevStateNo:         0,
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
		InputLatch:          make(map[string]int),
		HitGuard:            make(map[int]bool),
		Life:                stateFile.Data.Life,
		MaxLife:             stateFile.Data.Life,
		Power:               0,
		MaxPower:            maxPower,
		AttackMul:           1.0,
		DefenceMul:          1.0,
		X:                   0.0,
		Y:                   0.0,
		Z:                   0.0,
		StartX:              0.0,
		StartY:              0.0,
		StartZ:              0.0,
		StartFacing:         1,
		LeftBound:           -1000, // Defaults
		RightBound:          1000,
		TopBound:            0,
		BotBound:            0,
		GravityActive:       false,
		CurrentHitDef:       cns.HitDef{},
		ActiveReversalDef:   make(map[string]string),
		Specials:            make(map[string]bool),
		Target:              make([]*Character, 0),
		Helpers:             make([]*Character, 0),
		Projectiles:         make([]*Character, 0),
		Explods:             make([]*ExplodData, 0),
		DrawOffset:          []float64{0, 0},
		NotHitByFlags:       make(map[string]int),
		HitByFlags:          make(map[string]int),
		RoundState:          2,
		IsMatchOver:         false,
	}
}

// Update ticks the character's state machine forward by one frame.
func (c *Character) Update() {
	if c.RuntimeError != nil {
		return
	}
	if !c.stateInitialized {
		c.ChangeState(c.StateNo, -1, -1)
		if c.RuntimeError != nil {
			return
		}
	}
	c.updateActive = true
	c.pausedThisTick = c.ShakeTime > 0
	contactAtStart := c.contactSerial
	defer func() { c.updateActive = false }()

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

	isPaused := c.pausedThisTick
	if !isPaused {
		c.PosFrozen = false
	}
	if isPaused {
		c.ShakeTime--
	} else if c.HitTime > 0 {
		c.HitTime--
	}

	if c.Commands != nil && c.CmdFile != nil {
		physicalRaw := GetRawInputs(c.PlayerID, c.Facing)
		raw := make([]string, len(physicalRaw))
		copy(raw, physicalRaw)

		// Input Latching (Phase 8): stabilize directional inputs against jitter/alternation
		if !isPaused {
			// 1. Apply existing latches for inputs that just dropped
			for in, ticks := range c.InputLatch {
				if ticks > 0 {
					found := false
					for _, r := range physicalRaw {
						if r == in {
							found = true
							break
						}
					}
					if !found {
						// Only add if not physically present this tick
						raw = append(raw, in)
						c.InputLatch[in] = ticks - 1
					}
				} else {
					delete(c.InputLatch, in)
				}
			}

			// 2. Refresh/Set latch ONLY for current PHYSICAL inputs
			for _, in := range physicalRaw {
				if strings.HasPrefix(in, "Dir") {
					c.InputLatch[in] = 1 // 1 tick buffer is safer than 2
				}
			}
		}

		sort.Strings(raw)
		c.Commands.Add(raw)

		if len(c.Commands.History) > 0 {
			var matched []string
			for name, cmdList := range c.CmdFile.Commands {
				for _, cmd := range cmdList {
					// MUGEN command matching
					if c.Commands.Match(cmd.Sequence, cmd.Time, cmd.BufferTime) {
						c.ActiveCommands = append(c.ActiveCommands, name)
						matched = append(matched, name)
						break // Found a match for this command name
					}
				}
			}
			if len(matched) > 0 {
				sort.Strings(matched)
				c.LastCommandMatched = strings.Join(matched, ", ")
			}
		}
	}

	// 3. MUGEN multi-level state processing:
	// -3, then -2, then -1, then current state level.

	// Paused passes still evaluate controllers with ignorehitpause.
	if err := c.SM.RunStatePass(c.StateFile.States, c); err != nil {
		c.RuntimeError = fmt.Errorf("player %d: %w", c.PlayerID, err)
	}
	if c.RuntimeError != nil {
		return
	}

	// 3.5 Built-in Transitions
	if !isPaused {
		c.HandleBuiltinTransitions()
	}

	// 4. Collision Detection (Phase 4)
	c.ResolveCollisions(c.Target)

	// 5. Physics and Time update
	if !isPaused {
		c.Time++
		if c.contactSerial == contactAtStart && c.ShakeTime == 0 {
			if c.MoveHit > 0 {
				c.MoveHit++
			}
			if c.MoveContact > 0 {
				c.MoveContact++
			}
			if c.MoveGuarded > 0 {
				c.MoveGuarded++
			}
		}
		c.UpdateAnimation()

		// Apply friction
		c.ApplyFriction()

		// Apply gravity
		if strings.ToUpper(c.Physics) == "A" || c.GravityActive {
			yaccel := 0.44 // Default
			if c.StateFile != nil {
				yaccel = c.StateFile.Movement.YAccel
			}
			c.VelY += yaccel
		}

		// Tick down protection flags
		for k, v := range c.NotHitByFlags {
			if v > 0 {
				c.NotHitByFlags[k] = v - 1
			} else {
				delete(c.NotHitByFlags, k)
			}
		}
		for k, v := range c.HitByFlags {
			if v > 0 {
				c.HitByFlags[k] = v - 1
			} else {
				delete(c.HitByFlags, k)
			}
		}

		if !c.PosFrozen {
			// VelX is in CHARACTER-LOCAL space (positive = toward opponent).
			// Multiply by Facing to get world-space displacement.
			c.X += c.VelX * float64(c.Facing)
			c.Y += c.VelY

			// Clamp bounds
			if c.X < c.LeftBound {
				c.X = c.LeftBound
			}
			if c.X > c.RightBound {
				c.X = c.RightBound
			}

			// Screen boundaries
			if c.ScreenWidth > 0 {
				edge := float64(c.ScreenWidth) / 2.0
				if c.X < c.CameraX-edge {
					c.X = c.CameraX - edge
				}
				if c.X > c.CameraX+edge {
					c.X = c.CameraX + edge
				}
			}

			// Push collision
			c.ResolvePushCollision()

			// Auto-turn:
			// Continuously face opponent. Now that VelX is negated upon flip
			// (preserving absolute momentum), this won't cause mid-air oscillation.
			if !c.Specials["noautoturn"] && len(c.Target) > 0 {
				opponent := c.Target[0]
				newFacing := c.Facing
				opponentX, _ := c.PositionOf(opponent)
				if opponentX > c.X {
					newFacing = 1
				} else if opponentX < c.X {
					newFacing = -1
				}
				if newFacing != c.Facing {
					c.Facing = newFacing
					c.VelX = -c.VelX // Preserve absolute momentum across the flip
				}
			}
		}

		// Standard MUGEN Grounding
		// Standard MUGEN Grounding: Only for normal movement (Jump)
		if c.Y >= 0 && c.VelY >= 0 && strings.ToUpper(c.Physics) == "A" && c.MoveType != "H" {
			c.Y = 0
			c.VelY = 0
			c.ChangeState(52, -1, -1)
		}
	}

	// 6. Update Helpers, Projectiles, Explods
	for _, h := range c.Helpers {
		h.Update()
	}
	for _, p := range c.Projectiles {
		p.Update()
	}
	var activeExplods []*ExplodData
	for _, e := range c.Explods {
		e.CurrentTime++
		if e.Time == -1 || e.CurrentTime < e.Time {
			activeExplods = append(activeExplods, e)
		}
	}
	c.Explods = activeExplods

	// Diagnostic: detect infinite fall
	if c.Y > 5000 && !c.PosFrozen {
		fmt.Printf("[WARNING] Tick %d: Extreme Y position detected in state %d!\n", c.Time, c.StateNo)
	}
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
	if p := c.animationClock(); p != nil {
		return p.AnimTime()
	}
	return 0
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

// GetCtrl returns whether the character currently has control (can input commands).
func (c *Character) GetCtrl() bool {
	return c.Ctrl
}

func (c *Character) AddPosition(x, y float64) {

	c.X += x
	c.Y += y
}

func (c *Character) SetPosition(x, y float64) {
	c.X = x
	c.Y = y
}

// CurrentElement returns the current AIR animation element for this character.
// Used by collision detection to read Clsn1 (hitboxes) and Clsn2 (hurtboxes).
func (c *Character) CurrentElement() *air.Element {
	if c.AirFile == nil {
		return nil
	}
	anim, ok := c.AirFile.Actions[c.Anim]
	if !ok || len(anim.Elements) == 0 {
		return nil
	}
	idx := c.AnimElem
	if idx < 0 || idx >= len(anim.Elements) {
		idx = 0
	}
	return &anim.Elements[idx]
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
	if active && c.StateFile != nil {
		c.VelY += c.StateFile.Movement.YAccel
	}
}

func (c *Character) VelMul(x, y float64) {
	c.VelX *= x
	c.VelY *= y
}

func (c *Character) PosFreeze(active bool) {
	c.PosFrozen = active
}

func (c *Character) HitDef(params map[string]string) {
	c.HitDefActive = true
	// Parse and store the new HitDef
	c.CurrentHitDef = cns.ParseHitDef(params)
	// Entering a HitDef makes the character an attacker
	c.MoveType = "A"
}

func (c *Character) Helper(params map[string]string) {
	// Helper(id, name, pos, pos_type, facing, stathero, ownpal, keyctrl, anim)
	stateno := 0
	if v, ok := params["stateno"]; ok {
		stateno, _ = strconv.Atoi(v)
	}

	// Create a new character as a helper
	// It shares the same files but has its own state machine
	h := NewCharacter(c.StateFile, c.AirFile, c.CmdFile)
	h.LocalCoord = c.LocalCoord
	h.Parent = c
	h.Root = c.Root
	if h.Root == nil {
		h.Root = c
	}
	h.PlayerID = c.PlayerID
	h.Facing = c.Facing

	// Default pos is (0,0) relative to parent axis
	h.X = c.X
	h.Y = c.Y

	if v, ok := params["pos"]; ok {
		pos := cns.ParseFloatList(v)
		if len(pos) >= 1 {
			h.X += pos[0] * float64(c.Facing)
		}
		if len(pos) >= 2 {
			h.Y += pos[1]
		}
	}

	h.ChangeState(stateno, 0, -1)
	c.Helpers = append(c.Helpers, h)
}

func (c *Character) Projectile(params map[string]string) {
	// Projectile(id, anim, hitdef, pos, vel, ...)
	anim := 0
	if v, ok := params["anim"]; ok {
		anim, _ = strconv.Atoi(v)
	}

	p := NewCharacter(c.StateFile, c.AirFile, c.CmdFile)
	p.LocalCoord = c.LocalCoord
	p.Parent = c
	p.Root = c.Root
	if p.Root == nil {
		p.Root = c
	}
	p.PlayerID = c.PlayerID
	p.Facing = c.Facing
	p.Anim = anim
	p.X = c.X
	p.Y = c.Y

	if v, ok := params["pos"]; ok {
		pos := cns.ParseFloatList(v)
		if len(pos) >= 1 {
			p.X += pos[0] * float64(c.Facing)
		}
		if len(pos) >= 2 {
			p.Y += pos[1]
		}
	}

	if v, ok := params["vel"]; ok {
		vel := cns.ParseFloatList(v)
		if len(vel) >= 1 {
			// VelX is in local space. DO NOT multiply by Facing here because
			// the physics update (which projectiles share) will multiply VelX by Facing.
			p.VelX = vel[0]
		}
		if len(vel) >= 2 {
			p.VelY = vel[1]
		}
	}

	// Projectiles are like constant-motion helpers with a specific anim
	c.Projectiles = append(c.Projectiles, p)
}

func (c *Character) AssertSpecial(flag string) {
	c.Specials[strings.ToLower(flag)] = true
}

func (c *Character) Explod(params map[string]string) {
	id := 0
	if v, ok := params["id"]; ok {
		id, _ = strconv.Atoi(v)
	}
	anim := 0
	if v, ok := params["anim"]; ok {
		anim, _ = strconv.Atoi(v)
	}

	e := &ExplodData{
		ID:     id,
		Anim:   anim,
		X:      c.X,
		Y:      c.Y,
		Facing: c.Facing,
		Time:   -1, // Default infinite until animation ends (simplified here)
	}

	if v, ok := params["pos"]; ok {
		pos := cns.ParseFloatList(v)
		if len(pos) >= 1 {
			e.X += pos[0] * float64(c.Facing)
		}
		if len(pos) >= 2 {
			e.Y += pos[1]
		}
	}

	c.Explods = append(c.Explods, e)
}

func (c *Character) RemoveExplod(id int) {
	var remaining []*ExplodData
	for _, e := range c.Explods {
		if id != -1 && e.ID != id {
			remaining = append(remaining, e)
		}
	}
	c.Explods = remaining
}

func (c *Character) AfterImage(params map[string]string) {
	// Stub
}

func (c *Character) AfterImageTime(time int) {
	// Stub
}

func (c *Character) EnvShake(params map[string]string) {
	// Stub: Trigger screen shake in stage/game
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
	c.MoveHit = 0
	c.MoveContact = 0
	c.MoveGuarded = 0
}

func (c *Character) HitAdd(val int) {
	c.ComboCount += val
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

func (c *Character) DestroySelf() {
	// Logic: mark for deletion from the game world
}
func (c *Character) VarRandom(id int, min, max int) {
	// Stub: set variable to random value
}

func (c *Character) FallEnvShake(params map[string]string) {
	// Logic: shake screen on fall impact
	// Generic EnvShake for now
	c.EnvShake(params)
}

func (c *Character) GameMakeAnim(params map[string]string) {
	// Stub: system animations (dust, sparks)
}

func (c *Character) MakeDust(params map[string]string) {
	// Stub: dust particles
}

func (c *Character) VarRangeSet(min, max int, val int) {
	for i := min; i <= max; i++ {
		if i >= 0 && i < len(c.IntVars) {
			c.IntVars[i] = val
		}
	}
}

func (c *Character) FVarRangeSet(min, max int, val float64) {
	for i := min; i <= max; i++ {
		if i >= 0 && i < len(c.FloatVars) {
			c.FloatVars[i] = val
		}
	}
}

func (c *Character) VictoryQuote(quote int) {
	if c.Parent != nil {
		return
	}
	if quote < 0 || quote > 99 {
		quote = -1
	}
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

func (c *Character) GetHitShakeOver() bool {
	return c.ShakeTime <= 0
}

func (c *Character) GetHitOver() bool {
	return c.HitTime <= 0
}

func (c *Character) GetHitFall() bool {
	return c.HitFall
}

func (c *Character) SetHitFall(fall bool) {
	c.HitFall = fall
}

func (c *Character) SetHitFallDamage(damage int) {
	// Stub
}

func (c *Character) SetHitFallVel(active bool) {
	c.HitFallVelActive = active
	if active {
		c.VelX = c.GetHitVar("fall.xvel")
		c.VelY = c.GetHitVar("fall.yvel")
	}
}

func (c *Character) GetPrevStateNo() int {
	return c.PrevStateNo
}

func (c *Character) GetRoundsExisted() int {
	return c.RoundsExisted
}

func (c *Character) HitBy(params map[string]string) {
	time := 1
	if t, ok := params["time"]; ok {
		fmt.Sscanf(t, "%d", &time)
	}
	if v, ok := params["value"]; ok {
		c.HitByFlags[v] = time
	}
	if v, ok := params["value2"]; ok {
		c.HitByFlags[v] = time
	}
}

func (c *Character) NotHitBy(params map[string]string) {
	time := 1
	if t, ok := params["time"]; ok {
		fmt.Sscanf(t, "%d", &time)
	}
	if v, ok := params["value"]; ok {
		c.NotHitByFlags[v] = time
	}
	if v, ok := params["value2"]; ok {
		c.NotHitByFlags[v] = time
	}
}

func (c *Character) GetInGuardDist() bool {
	// Guard distance is defined in [Size] in CNS
	guardDist := 100.0 // Default
	if c.StateFile != nil {
		guardDist = float64(c.StateFile.Size.AttackDist)
	}

	for _, target := range c.Target {
		if strings.ToUpper(target.MoveType) == "A" {
			// Calculate relative distance
			targetX, _ := c.PositionOf(target)
			relDist := (targetX - c.X) * float64(c.Facing)
			if relDist > 0 && relDist < guardDist {
				return true
			}
		}
	}
	return false
}

func (c *Character) GetMatchOver() bool {
	return c.IsMatchOver
}

func (c *Character) GetRoundNo() int {
	return 1
}

func (c *Character) GetRoundState() int {
	return c.RoundState
}

func (c *Character) GetCNSMovement() *cns.Movement {
	if c.StateFile != nil {
		return &c.StateFile.Movement
	}
	return nil
}

func (c *Character) HasAnim(animNo int) bool {
	if c.AirFile == nil {
		return false
	}
	_, exists := c.AirFile.Actions[animNo]
	return exists
}

func (c *Character) GetHitVar(name string) float64 {
	val := 0.0
	switch strings.ToLower(name) {
	case "animtype":
		// Light=0, Medium=1, Hard=2, Back=3, Up=4, DiagUp=5
		switch strings.ToLower(c.GetHit.AnimType) {
		case "light":
			val = 0
		case "medium":
			val = 1
		case "hard":
			val = 2
		case "back":
			val = 3
		case "up":
			val = 4
		case "diagup":
			val = 5
		}
	case "air.animtype":
		switch strings.ToLower(c.GetHit.AirAnimType) {
		case "light":
			val = 0
		case "medium":
			val = 1
		case "hard":
			val = 2
		case "back":
			val = 3
		case "up":
			val = 4
		case "diagup":
			val = 5
		}
	case "groundtype":
		// None=0, Stand=1, Trip=2
		switch strings.ToLower(c.GetHit.GroundType) {
		case "none":
			val = 0
		case "high", "low": // Stand
			val = 1
		case "trip":
			val = 2
		default:
			val = 1
		}
	case "xvel":
		if c.HitFall || c.StateType == "A" {
			if len(c.GetHit.AirVel) > 0 {
				val = c.GetHit.AirVel[0]
			}
		} else if len(c.GetHit.GroundVel) > 0 {
			val = c.GetHit.GroundVel[0]
		}
	case "yvel":
		if c.HitFall || c.StateType == "A" {
			if len(c.GetHit.AirVel) > 1 {
				val = c.GetHit.AirVel[1]
			} else if len(c.GetHit.GroundVel) > 1 {
				val = c.GetHit.GroundVel[1]
			}
		} else if len(c.GetHit.GroundVel) > 1 {
			val = c.GetHit.GroundVel[1]
		}
	case "yaccel":
		if c.GetHit.YAccel != 0.0 {
			val = c.GetHit.YAccel
		} else {
			val = 0.44 // MUGEN default
		}
	case "fall":
		if c.HitFall {
			val = 1
		} else {
			val = float64(c.GetHit.Fall)
		}
	case "fall.damage":
		val = float64(c.GetHit.FallDamage)
	case "fall.xvel":
		if len(c.GetHit.FallVel) > 0 {
			val = c.GetHit.FallVel[0]
		}
	case "fall.yvel":
		if len(c.GetHit.FallVel) > 1 {
			val = c.GetHit.FallVel[1]
		} else {
			val = -4.5
		}
	case "fall.recover":
		if c.GetHit.FallRecover {
			val = 1
		}
	case "hittime":
		if c.StateType == "A" {
			val = float64(c.GetHit.AirHitTime)
		} else {
			val = float64(c.GetHit.GroundHitTime)
		}
	case "slidetime":
		val = float64(c.GetHit.GroundSlide)
	case "ctrltime":
		// This is usually GroundHitTime? MUGEN docs say it's same as hittime.
		if c.StateType == "A" {
			val = float64(c.GetHit.AirHitTime)
		} else {
			val = float64(c.GetHit.GroundHitTime)
		}
	}
	return val
}

// GetTargetPos returns the opponent's position in this player's local units.
func (c *Character) GetTargetPos() (float64, float64) {
	if len(c.Target) > 0 {
		return c.PositionOf(c.Target[0])
	}
	return c.X, c.Y // No target: return self (distance = 0)
}

func (c *Character) IsPaused() bool {
	if c.updateActive {
		return c.pausedThisTick || c.ShakeTime > 0
	}
	return c.ShakeTime > 0
}

func (c *Character) GetPersistence(key string) int {
	return c.PersistenceCounters[key]
}

func (c *Character) SetPersistence(key string, val int) {
	c.PersistenceCounters[key] = val
}

func (c *Character) GetSysVar(idx int) int {
	if idx >= 0 && idx < 5 {
		return c.SysVars[idx]
	}
	return 0
}

func (c *Character) SetSysVar(idx, val int) {
	if idx >= 0 && idx < 5 {
		c.SysVars[idx] = val
	}
}

func (c *Character) GetSysFVar(idx int) float64 {
	if idx >= 0 && idx < 5 {
		return c.SysFloatVars[idx]
	}
	return 0
}

func (c *Character) SetSysFVar(idx int, val float64) {
	if idx >= 0 && idx < 5 {
		c.SysFloatVars[idx] = val
	}
}

func (c *Character) GetCNSVelocity() *cns.Velocity {
	if c.StateFile != nil {
		return &c.StateFile.Velocity
	}
	return &cns.Velocity{}
}

func (c *Character) GetStateno() int {
	return c.StateNo
}

func (c *Character) GetStateFile() *cns.CNS {
	return c.StateFile
}
func (c *Character) HandleBuiltinTransitions() {
	if c.StateNo == 5110 {
		liedownTime := 60 // Default MUGEN
		if c.StateFile != nil && c.StateFile.Data.LiedownTime > 0 {
			liedownTime = c.StateFile.Data.LiedownTime
		}

		if c.GetLife() <= 0 && c.GetAnimTime() == 0 {
			c.ChangeState(5150, -1, -1)
			return
		}

		if c.Time >= liedownTime {
			c.ChangeState(5120, -1, -1)
			return
		}
	}

	// Jumping Sequence (Phase 10)
	if c.StateNo == 40 && c.GetAnimTime() == 0 {
		c.ChangeState(50, -1, -1)
		return
	}
	if c.StateNo == 50 && c.VelY > 0 {
		c.ChangeState(51, -1, -1)
		return
	}
	if (c.StateNo == 50 || c.StateNo == 51) && c.Y >= 0 && c.VelY >= 0 {
		c.ChangeState(52, -1, -1)
		return
	}
	if c.StateNo == 52 && (c.GetAnimTime() == 0 || c.Time >= 3) {
		c.ChangeState(0, -1, -1)
		return
	}

	// Guarding Transitions (States 120-132)
	if !c.Specials["noguard"] && c.GetInGuardDist() && c.Command("holdback") {
		if c.StateNo < 120 || c.StateNo > 155 {
			c.ChangeState(120, -1, -1)
			return
		}
	}

	// Guard Maintenance/Recovery
	if c.StateNo >= 120 && c.StateNo <= 140 {
		if !c.Command("holdback") || !c.GetInGuardDist() {
			c.ChangeState(140, -1, -1) // Guard End
			return
		}

		// State-specific Guard transitions
		if c.StateNo == 120 { // Guard Start
			if c.StateType == "S" {
				c.ChangeState(130, -1, -1)
			} else if c.StateType == "C" {
				c.ChangeState(131, -1, -1)
			} else {
				c.ChangeState(132, -1, -1)
			}
			return
		}
	}

	if c.StateFile == nil {
		return
	}

	st := strings.ToUpper(c.StateType)
	phys := strings.ToUpper(c.Physics)

	if st == "S" && phys == "S" {
		if c.Command("holdup") && c.StateNo != 40 {
			// Jump Start
			// Set sysvar(1) for direction: 0=neutral, 1=fwd, -1=back
			dir := 0
			if c.Command("holdfwd") {
				dir = 1
			} else if c.Command("holdback") {
				dir = -1
			}
			c.SetSysVar(1, dir)
			c.ChangeState(40, -1, -1)
		} else if c.Command("holddown") && c.StateNo != 10 && c.StateNo != 11 {
			c.ChangeState(10, -1, -1)
		} else if c.Ctrl && c.Command("holdfwd") && c.StateNo != 20 && c.StateNo != 100 {
			c.ChangeState(20, -1, -1)
		} else if c.Ctrl && c.Command("holdback") && c.StateNo != 20 && c.StateNo != 105 {
			c.ChangeState(20, -1, -1)
		} else if c.StateNo == 20 && c.Ctrl && !c.Command("holdfwd") && !c.Command("holdback") {
			// Walk -> Stand
			c.ChangeState(0, -1, -1)
		}
	} else if st == "C" && phys == "C" {
		if !c.Command("holddown") && c.StateNo != 12 {
			c.ChangeState(12, -1, -1) // Crouch to Stand
		}
	}

	// State-specific maintenance
	if c.StateNo == 140 { // Guard End
		if c.Time >= 20 { // Fixed recovery time for now
			if c.StateType == "S" {
				c.ChangeState(0, 1, -1)
			} else if c.StateType == "C" {
				c.ChangeState(11, 1, -1)
			}
		}
	}

	if c.StateNo >= 150 && c.StateNo <= 155 {
		if c.GetHitOver() {
			if c.StateType == "S" {
				c.ChangeState(130, -1, -1)
			} else if c.StateType == "C" {
				c.ChangeState(131, -1, -1)
			} else {
				c.ChangeState(132, -1, -1)
			}
			return
		}
	}

	// Ground Recovery (5200, 5201)
	if c.HitFall && st == "L" && c.Command("recovery") {
		canRecover := c.GetHitVar("fall.recover") != 0
		if canRecover {
			c.ChangeState(5200, -1, -1)
			return
		}
	}

	// Trip Sequence (5070)
	if c.GetHitVar("fall") != 0 && strings.EqualFold(c.GetHit.GroundType, "Trip") {
		if c.StateNo < 5070 || c.StateNo > 5071 {
			c.ChangeState(5070, -1, -1)
			return
		}
	}

	// Air Recovery (5040, 5210)
	if c.HitFall && st == "A" && c.Command("recovery") {
		canRecover := c.GetHitVar("fall.recover") != 0
		if canRecover {
			// Air recovery states
			if c.HasAnim(5210) {
				c.ChangeState(5210, -1, -1)
			} else {
				c.ChangeState(5040, -1, -1)
			}
			return
		}
	}

	// State-specific maintenance (System)
	if c.StateNo == 100 { // RUN_FWD
		if !c.Command("holdfwd") {
			c.ChangeState(0, -1, -1)
		}
	} else if c.StateNo == 105 { // RUN_BACK
		if !c.Command("holdback") {
			c.ChangeState(0, -1, -1)
		}
	}

	// Air Jump Bookkeeping (State 45)
	if c.StateNo == 45 && c.Time == 0 {
		c.AirJumpCount++
	}

	// Reset AirJumpCount on landing
	if st == "S" || st == "C" {
		c.AirJumpCount = 0
	}

	// Match End Transitions (Phase 11)
	if c.GetRoundState() == 3 { // Match Finished
		if c.GetLife() <= 0 {
			if c.StateNo != 170 && c.StateNo != 5150 {
				c.ChangeState(170, -1, -1) // Lose
			}
		} else if c.StateNo != 180 {
			c.ChangeState(180, -1, -1) // Win
		}
	}

	// Intro Transitions (Phase 11)
	if c.GetRoundState() == 0 && c.StateNo != 190 {
		c.ChangeState(190, -1, -1)
	} else if c.GetRoundState() == 1 && c.StateNo != 191 {
		c.ChangeState(191, -1, -1)
	}

	// Vertical Runs (110, 115) - Very simplified
	if st == "S" && phys == "S" {
		if c.Command("holdup") && c.Command("holdfwd") && c.StateNo != 40 {
			// Some characters have a run-up or high jump
			// c.ChangeState(110, -1, -1)
		}
	}
}

func (c *Character) ApplyFriction() {
	if c.StateFile == nil {
		return
	}
	phys := strings.ToUpper(c.Physics)
	if phys == "S" {
		c.VelX *= c.StateFile.Movement.StandFriction
	} else if phys == "C" {
		c.VelX *= c.StateFile.Movement.CrouchFriction
	}
}

func (c *Character) ResolvePushCollision() {
	if c.StateFile == nil {
		return
	}
	// Only push if on ground and not in hit state
	st := strings.ToUpper(c.StateType)
	if st != "S" && st != "C" {
		return
	}

	for _, other := range c.Target {
		othSt := strings.ToUpper(other.StateType)
		if (othSt == "S" || othSt == "C") && !other.PosFrozen {
			otherX, _ := c.PositionOf(other)
			dist := otherX - c.X
			front, back := c.bodyWidths()
			otherFront, otherBack := other.bodyWidths()
			leftWidth, rightWidth := back, front
			if c.Facing < 0 {
				leftWidth, rightWidth = front, back
			}
			otherLeft, otherRight := otherBack, otherFront
			if other.Facing < 0 {
				otherLeft, otherRight = otherFront, otherBack
			}
			scale := c.ScaleFrom(other)
			pushDist := rightWidth + otherLeft*scale
			if dist < 0 {
				pushDist = leftWidth + otherRight*scale
			}
			absDist := math.Abs(dist)
			if absDist < pushDist {
				direction := 1.0
				if dist < 0 || (dist == 0 && c.PlayerID > other.PlayerID) {
					direction = -1
				}
				delta := (pushDist - absDist) / 2 * direction
				c.X -= delta
				other.X += delta / scale
			}
		}
	}
}

func (c *Character) ChangeState(stateNo int, ctrl int, anim int) {
	overrides := engine.EntryOverrides{HasCtrl: ctrl != -1, Ctrl: ctrl != 0, HasAnim: anim != -1, Anim: anim}
	c.changeState(stateNo, overrides)
}

// Transition preserves explicit negative ctrl/anim values without conflating
// them with the legacy ChangeState -1 omission sentinel. State-owner restoration
// for SelfState is completed with the custom-state ownership work.
func (c *Character) Transition(stateNo int, overrides engine.EntryOverrides, self bool) error {
	c.changeState(stateNo, overrides)
	return c.RuntimeError
}

func (c *Character) ParentEnvironment() engine.Environment {
	if c.Parent == nil {
		return nil
	}
	return c.Parent
}

func (c *Character) GetDefenceMultiplier() float64 {
	base := 100
	if c.StateFile != nil && c.StateFile.Data.Defence > 0 {
		base = c.StateFile.Data.Defence
	}
	return float64(base) / 100 * c.DefenceMul
}

func (c *Character) changeState(stateNo int, overrides engine.EntryOverrides) {
	if c.RuntimeError != nil {
		return
	}
	if c.StateFile == nil {
		c.RuntimeError = fmt.Errorf("state %d: no state file", stateNo)
		return
	}
	next, found := c.StateFile.States[stateNo]
	if !found || stateNo < 0 {
		c.RuntimeError = fmt.Errorf("state %d is not defined for player %d", stateNo, c.PlayerID)
		return
	}
	if err := c.SM.CompileState(next); err != nil {
		c.RuntimeError = err
		return
	}
	c.LastStateChange = fmt.Sprintf("%d -> %d (%s)", c.StateNo, stateNo, next.Name)
	c.PrevStateNo = c.StateNo
	c.StateNo = stateNo
	c.Time = 0
	c.HitFallVelActive = false
	c.PosFrozen = false
	for key := range c.PersistenceCounters {
		if strings.HasPrefix(key, "pos") {
			delete(c.PersistenceCounters, key)
		}
	}
	if err := c.SM.EnterState(next, c, overrides); err != nil {
		c.RuntimeError = err
		return
	}
	c.stateInitialized = true
}

func (c *Character) BeginStateEntry(hitdefPersist, movehitPersist, hitcountPersist bool) {
	if !hitdefPersist {
		c.CurrentHitDef = cns.HitDef{}
		c.HitDefActive = false
		c.HitGuard = make(map[int]bool)
	}
	if !movehitPersist {
		c.MoveHit = 0
		c.MoveContact = 0
		c.MoveGuarded = 0
	}
	if !hitcountPersist {
		c.HitCount = 0
	}
}

func (c *Character) EvaluationWarning(message string) {
	if c.warningSeen == nil {
		c.warningSeen = make(map[string]bool)
	}
	key := fmt.Sprintf("%d:%s", c.StateNo, message)
	if c.warningSeen[key] || len(c.RuntimeWarnings) >= 256 {
		return
	}
	c.warningSeen[key] = true
	warning := fmt.Sprintf("player %d state %d: %s", c.PlayerID, c.StateNo, message)
	c.RuntimeWarnings = append(c.RuntimeWarnings, warning)
	logger.LogUnresolved(warning)
}
func (c *Character) SetJuggleCost(cost int) { c.JuggleCost = cost }
func (c *Character) FaceOpponent() {
	if len(c.Target) == 0 {
		return
	}
	facing := c.Facing
	x, _ := c.PositionOf(c.Target[0])
	if x > c.X {
		facing = 1
	} else if x < c.X {
		facing = -1
	}
	if facing != c.Facing {
		c.Facing = facing
		c.VelX = -c.VelX
	}
}

func (c *Character) ChangeAnim(animNo int) {
	if err := c.ChangeAnimation(animNo, 1); err != nil {
		c.RuntimeError = err
	}
}

func (c *Character) UpdateAnimation() {
	if p := c.animationClock(); p != nil {
		p.Update()
		c.syncAnimation()
		if p.Error != nil {
			c.RuntimeError = p.Error
		}
	}
}

func (c *Character) ResolveCollisions(others []*Character) {
	// 1. Check character's own HitBoxes
	if c.MoveType == "A" {
		for _, other := range others {
			if other == c || other.Life <= 0 {
				continue
			}

			if CheckCollision(c, other) {
				c.HandleHit(other)
			}
		}
	}

	// 2. Check Projectiles' HitBoxes
	var activeProjectiles []*Character
	for _, p := range c.Projectiles {
		hit := false
		for _, other := range others {
			if other == c || other.Life <= 0 {
				continue
			}
			if CheckCollision(p, other) {
				p.HandleHit(other)
				hit = true
				break
			}
		}
		// Projectiles typically disappear on hit (unless specified otherwise)
		if !hit {
			activeProjectiles = append(activeProjectiles, p)
		}
	}
	c.Projectiles = activeProjectiles
}

func (c *Character) HandleHit(other *Character) {
	if !c.HitDefActive {
		return
	}
	// Prevent the same target from being hit multiple times by the same HitDef swing
	if c.HitGuard == nil {
		c.HitGuard = make(map[int]bool)
	}
	if c.HitGuard[other.PlayerID] {
		return // Already hit this target this swing
	}
	c.HitGuard[other.PlayerID] = true

	// 1. Mark attacker results
	c.MoveHit = 1
	c.MoveContact = 1
	c.MoveGuarded = 0
	c.contactSerial++

	// Apply HitPause to attacker
	if len(c.CurrentHitDef.PauseTime) > 0 {
		c.ShakeTime = c.CurrentHitDef.PauseTime[0]
	}

	// 2. Mark defender results
	other.ProcessHit(c, c.CurrentHitDef)
}

func (c *Character) ProcessHit(attacker *Character, hit cns.HitDef) {
	c.GetHit = hit // Copy by value to avoid stack pointer issues

	// Life reduction
	damage := 0
	if len(hit.Damage) > 0 {
		damage = hit.Damage[0]
	}
	// Apply attack/defence multipliers
	finalDamage := float64(damage) * attacker.AttackMul / c.DefenceMul
	c.Life -= int(finalDamage)
	if c.Life < 0 {
		c.Life = 0
	}

	// Hit Pause/Shake
	if len(hit.PauseTime) > 1 {
		c.ShakeTime = hit.PauseTime[1]
	} else if len(hit.PauseTime) > 0 {
		c.ShakeTime = hit.PauseTime[0]
	}

	// Knockback Velocity
	// Negative X = push defender backward (away from attacker).
	if c.StateType == "A" || hit.Fall == 1 {
		// Airborne hit or forced fall
		if len(hit.AirVel) > 0 {
			c.VelX = -hit.AirVel[0]
		} else if len(hit.GroundVel) > 0 {
			c.VelX = -hit.GroundVel[0]
		}

		if len(hit.AirVel) > 1 {
			c.VelY = hit.AirVel[1]
		} else if len(hit.GroundVel) > 1 {
			c.VelY = hit.GroundVel[1]
		}
	} else {
		// Grounded hit
		if len(hit.GroundVel) > 0 {
			c.VelX = -hit.GroundVel[0]
		}
		if len(hit.GroundVel) > 1 {
			c.VelY = hit.GroundVel[1]
		}
	}

	// Turn to face attacker
	attackerX, _ := c.PositionOf(attacker)
	if attackerX > c.X {
		c.Facing = 1
	} else if attackerX < c.X {
		c.Facing = -1
	}

	// Set HitTime duration based on state
	if c.StateType == "A" || hit.Fall == 1 {
		if hit.AirHitTime > 0 {
			c.HitTime = hit.AirHitTime
		} else {
			c.HitTime = 20 // Reasonable default if air.hittime missing
		}
	} else {
		c.HitTime = hit.GroundHitTime
	}

	// Track if this hit causes a fall
	c.HitFall = hit.Fall == 1 || c.StateType == "A"

	// Guarding vs Normal Hit
	if c.StateNo >= 120 && c.StateNo <= 155 {
		// Guard Hit States (150-155)
		if c.StateType == "S" {
			c.ChangeState(150, 0, -1)
		} else if c.StateType == "C" {
			c.ChangeState(152, 0, -1)
		} else {
			c.ChangeState(154, 0, -1)
		}
	} else {
		// Transition to Hit State (5000 is standard MUGEN "hit" state)
		c.ChangeState(5000, 0, -1)
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
			case "F", "holdfwd":
				mapped = "DirForward"
			case "B", "holdback":
				mapped = "DirBack"
			case "U", "holdup":
				mapped = "DirUp"
			case "D", "holddown":
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

type ExplodData struct {
	ID          int
	Anim        int
	X, Y        float64
	PosType     string
	Facing      int
	VelX, VelY  float64
	Time        int // Ticks remaining (-1 for infinite)
	CurrentTime int
}
