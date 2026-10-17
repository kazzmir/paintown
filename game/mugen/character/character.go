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
	"github.com/kazzmir/paintown/game/mugen/combat"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/logger"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/snd"
	"github.com/kazzmir/paintown/game/mugen/sound"
)

// Character represents a MUGEN entity running through the State Machine.
// It implements the Environment interface so the state machine can execute controllers against it.
type Character struct {
	Name, AuthorName string
	RuntimeError     error
	stateInitialized bool
	updateActive     bool
	pausedThisTick   bool
	worldFrozen      bool
	pauseCommands    map[string]bool
	pauseActor       *engine.PauseActor

	JuggleCost            int
	HitDefActive          bool
	stateSerial           uint64
	statePassBudget       engine.StatePassBudget
	hitTargets            map[*Character]hitTarget
	targetOrder           []*Character
	hitGeneration         uint64
	superDefenceMul       float64
	SuperTargetDefenceMul float64
	pendingHitDamage      int
	hasPendingHitDamage   bool
	RuntimeWarnings       []string
	warningSeen           map[string]bool

	// CNS data containing states and physics globals
	StateFile           *cns.CNS
	stateOwner          *Character
	pendingStateEntry   *cns.StateDef
	pendingMissingEntry bool
	allowMissingState   bool
	binding             *actorBinding
	boundThisTick       bool
	bindingReported     bool
	tickMoveType        string
	pendingHitFacing    int
	getHitFacing        int // selected hit heading, independent of deferred physical turns
	pendingHitYOffset   float64
	cornerPushVelocity  float64 // world impulse in common engine units
	LocalCoord          coord.Space
	GameCoord           coord.Space
	Viewport            *coord.Viewport
	Timeline            *engine.Timeline
	Palettes            *palette.State

	// AIR data containing animation definitions
	AirFile           *air.Data
	animationFile     *air.Data
	animation         *air.Player
	contactFrame      *air.Element
	contactFrameReady bool
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
	// InputSource supplies facing-relative logical input for this entity's tick.
	// A nil source retains the physical input adapter for existing applications.
	InputSource func(playerID, facing int) []string
	// Buffered commands for the current tick
	ActiveCommands []string

	// InputSlot selects a keyboard/controller; EntityID is script-visible ID.
	InputSlot                                int
	EntityID                                 int32
	TeamSide                                 int
	HelperID                                 int
	HelperKeyControl, PlayerHelper           bool
	HelperName                               string
	helperBirthTick                          int32
	helperBirthKnown                         bool
	helperPauseMoveTime, helperSuperMoveTime int
	World                                    *World
	destroyed                                bool
	isProjectile                             bool

	// Attacker pause suppresses CNS/time; receiver shake does not.
	ShakeTime    int
	HitPauseTime int
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
	ScreenLeftMargin      float64
	ScreenRightMargin     float64
	screenBoundsDisabled  bool
	cameraDisabled        [2]bool
	frameWidths           engine.WidthParameters

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
	PowerRules *combat.PowerRules
	AttackMul  float64
	DefenceMul float64

	// Physics/Logic flags
	GravityActive bool
	PosFrozen     bool
	PushDisabled  bool

	// Visuals
	SprPriority int

	CurrentHitDef     cns.HitDef
	GuardDistance     float64
	Specials          map[string]bool
	ActiveReversalDef map[string]string
	SoundQueue        *sound.Queue
	AudioOwner        uint64
	OwnSounds         *snd.SoundBank
	CommonSounds      *snd.SoundBank

	// Hierarchy
	Parent    *Character
	Root      *Character
	Opponents []*Character

	// Stats
	HitCount     int
	UniqHitCount int
	// ComboCount is the displayed combo counter, distinct from the current
	// attack's HitCount. Match combat owns its reset/credit rules.
	ComboCount int

	// Visual/Drawing State
	DrawAngle  float64
	DrawOffset []float64

	// System/Debug
	Clipboard string

	// Hierarchy
	Helpers        []*Character
	Projectiles    []*Character
	Explods        []*ExplodData
	koSoundStarted bool
	koSoundAge     int
	EffectQueue    *effect.Queue
	CommonEffects  effect.Source

	// Constants
	VictoryQuoteIndex int

	// Combat status for triggers
	MoveHit     int
	MoveContact int
	MoveGuarded int

	// HitGuard tracks world EntityIDs already hit by the current HitDef.
	// activation, preventing the same attack from landing multiple times per swing.
	HitGuard         map[int]bool
	GetHit           cns.HitDef
	HitTime          int
	HitFall          bool
	GetHitDown       bool
	GetHitAir        bool
	GetHitGuarded    bool
	GetHitDamage     int
	HitFallVelActive bool // Tracks if HitFallVel controller was triggered

	// Input latching to stabilize diagonals (input name -> ticks remaining)
	InputLatch map[string]int

	// Protection logic
	HitFilters   [2]combat.HitFilter
	HitOverrides [8]combat.HitOverride

	// Rounds
	RoundsExisted int

	// Advanced Movement
	AirJumpCount int

	// Match state
	RoundState  int
	RoundNo     int
	PaletteNo   int
	AILevel     int
	IsMatchOver bool
}

// NewCharacter returns a newly initialized character.
func NewCharacter(stateFile *cns.CNS, airFile *air.Data, cmdFile *cmd.Data) *Character {
	queue := &sound.Queue{}
	maxPower := stateFile.Data.Power
	if maxPower == 0 && len(stateFile.ConstantFields["data.power"]) == 0 {
		maxPower = 3000
	}
	return &Character{
		Timeline:              &engine.Timeline{},
		pauseActor:            engine.NewPauseActor(),
		superDefenceMul:       1,
		SuperTargetDefenceMul: 1.5,
		SoundQueue:            queue,
		AudioOwner:            queue.NewOwner(),
		EffectQueue:           &effect.Queue{},
		LocalCoord:            coord.Default(),
		GameCoord:             coord.Default(),
		StateFile:             stateFile,
		AirFile:               airFile,
		CmdFile:               cmdFile,
		SM:                    NewStateMachine(),
		Time:                  0,
		StateNo:               0,
		PrevStateNo:           0,
		Anim:                  0,
		AnimTime:              0,
		AnimElem:              0,
		AnimElemTime:          0,
		StateType:             "S",
		Physics:               "S",
		MoveType:              "I",
		Ctrl:                  true,
		InputSlot:             1, // Default to P1
		EntityID:              -1,
		Facing:                1, // Default to Right
		PersistenceCounters:   make(map[string]int),
		ActiveCommands:        make([]string, 0),
		InputLatch:            make(map[string]int),
		HitGuard:              make(map[int]bool),
		Life:                  stateFile.Data.Life,
		MaxLife:               stateFile.Data.Life,
		Power:                 0,
		MaxPower:              maxPower,
		GuardDistance:         float64(stateFile.Size.AttackDist),
		AttackMul:             1.0,
		DefenceMul:            1.0,
		X:                     0.0,
		Y:                     0.0,
		Z:                     0.0,
		StartX:                0.0,
		StartY:                0.0,
		StartZ:                0.0,
		StartFacing:           1,
		LeftBound:             -1000, // Defaults
		RightBound:            1000,
		TopBound:              0,
		BotBound:              0,
		GravityActive:         false,
		CurrentHitDef:         cns.HitDef{},
		ActiveReversalDef:     make(map[string]string),
		Specials:              make(map[string]bool),
		Opponents:             make([]*Character, 0),
		Helpers:               make([]*Character, 0),
		Projectiles:           make([]*Character, 0),
		Explods:               make([]*ExplodData, 0),
		DrawOffset:            []float64{0, 0},
		HitTime:               -1,
		RoundState:            2,
		RoundNo:               1,
		PaletteNo:             1,
		IsMatchOver:           false,
	}
}

// Update advances an isolated entity through the same tick adapter as matches.
// Multi-player callers must pass all roots to TickCharacters once per tick.
func (c *Character) Update() {
	if c.destroyed {
		return
	}
	if err := TickCharacters([]*Character{c}); err != nil {
		c.RuntimeError = err
	}
}

func (c *Character) startTick() bool {
	c.boundThisTick = false
	c.tickMoveType = c.MoveType
	if c.Viewport != nil && c.Viewport.Space.Width > 0 {
		scale := float64(c.LocalCoordinateWidth()) / float64(c.Viewport.Space.Width)
		c.CameraX = c.Viewport.CameraX * scale
		c.ScreenWidth = c.LocalCoordinateWidth()
	}
	if c.RuntimeError != nil {
		return false
	}
	c.worldFrozen = c.Timeline != nil && c.Timeline.Frozen(c.pauseActor)
	if c.Parent != nil && !c.isProjectile && c.Timeline != nil && c.Timeline.Paused() {
		allowance := &c.helperPauseMoveTime
		if c.Timeline.SuperPaused() {
			allowance = &c.helperSuperMoveTime
		}
		if *allowance > 0 {
			*allowance--
			c.worldFrozen = false
		}
	}
	if c.worldFrozen {
		c.updateActive = true
		return true
	}
	if !c.stateInitialized {
		c.ChangeState(c.StateNo, -1, -1)
		if c.RuntimeError != nil {
			return false
		}
	}
	c.updateActive = true
	if c.HitPauseTime > 0 {
		c.HitPauseTime--
	}
	c.pausedThisTick = c.HitPauseTime > 0
	if c.ShakeTime > 0 {
		c.ShakeTime--
	}
	if c.ShakeTime == 0 && c.HitTime >= 0 {
		c.HitTime--
	}
	return true
}

func (c *Character) runStateTick() bool {
	if c.worldFrozen {
		c.bufferPausedCommands()
		return true
	}
	c.prepareBinding()
	c.pausedThisTick = c.HitPauseTime > 0
	isPaused := c.IsPaused()
	// Clear frame-local specials
	c.Specials = make(map[string]bool)
	c.PushDisabled = false
	c.screenBoundsDisabled = c.Parent != nil && !c.PlayerHelper
	c.cameraDisabled = [2]bool{c.screenBoundsDisabled, c.screenBoundsDisabled}
	c.frameWidths = engine.WidthParameters{}
	c.ActiveReversalDef = make(map[string]string)
	c.DrawAngle = 0
	c.DrawOffset = []float64{0, 0}
	// Clear stale debug info globally so we only see current frame actions.
	if input.GlobalManager != nil {
		input.GlobalManager.LastKeyEvent = ""
		input.GlobalManager.LastActionEvent = ""
	}

	if !isPaused {
		c.PosFrozen = false
	}

	c.updateCommands(isPaused)
	for name := range c.pauseCommands {
		if !c.Command(name) {
			c.ActiveCommands = append(c.ActiveCommands, name)
		}
	}
	if len(c.pauseCommands) > 0 {
		sort.Strings(c.ActiveCommands)
		c.LastCommandMatched = strings.Join(c.ActiveCommands, ", ")
		c.pauseCommands = nil
	}

	// 3. MUGEN multi-level state processing:
	// -3, then -2, then -1, then current state level.

	// Paused passes still evaluate controllers with ignorehitpause.
	c.statePassBudget = engine.StatePassBudget{}
	if err := c.SM.RunStatePassWithBudget(c.StateFile.States, c, &c.statePassBudget); err != nil {
		c.RuntimeError = fmt.Errorf("player %d: %w", c.InputSlot, err)
	}
	if c.RuntimeError != nil {
		c.updateActive = false
		return false
	}

	// Engine transitions enter their positive state in this same tick.
	generation := c.stateSerial
	if !isPaused {
		c.HandleBuiltinTransitions()
	}
	if c.RuntimeError == nil && generation != c.stateSerial {
		c.RuntimeError = c.SM.RunPositivePassWithBudget(c.StateFile.States, c, &c.statePassBudget)
	}
	if c.RuntimeError != nil {
		c.updateActive = false
		return false
	}
	return true
}

func (c *Character) finishTick() {
	defer func() {
		if c.RuntimeError == nil {
			if c.pendingHitFacing != 0 {
				if c.Facing != c.pendingHitFacing {
					c.Turn()
				}
				c.pendingHitFacing = 0
			}
			c.publishBindings()
		}
		c.updateActive = false
	}()
	if c.RuntimeError != nil {
		return
	}
	isPaused := c.pausedThisTick

	// 5. Physics and Time update
	if !isPaused && !c.worldFrozen {
		// The down-hit snap survives authored CNS position writes, then is
		// consumed before the first movement step. GetHitVar observes zero next tick.
		c.Y += c.pendingHitYOffset
		c.pendingHitYOffset = 0
		c.advanceCornerPush()
		for {
			if !c.PosFrozen && !c.boundThisTick {
				// VelX is in CHARACTER-LOCAL space (positive = toward opponent).
				// Multiply by Facing to get world-space displacement.
				c.X += c.VelX * float64(c.Facing)
				c.Y += c.VelY

				// Engine turning belongs to the common idle/crouch/walk states;
				// custom states retain their facing. Preserve world momentum on flip.
				if (c.StateNo == 0 || c.StateNo == 11 || c.StateNo == 20) && !c.Specials["noautoturn"] && len(c.Opponents) > 0 {
					opponent := c.Opponents[0]
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

			// Native integrates position before applying the physics forces.
			c.ApplyFriction()
			if strings.EqualFold(c.Physics, "A") || c.GravityActive {
				yaccel := 0.44
				if c.StateFile != nil {
					yaccel = c.StateFile.Movement.YAccel
				}
				c.VelY += yaccel
			}
			if c.Y >= 0 && c.VelY >= 0 && strings.EqualFold(c.Physics, "A") && c.StateNo != 105 {
				c.ChangeState(52, -1, -1)
				if c.RuntimeError != nil {
					return
				}
				c.RuntimeError = c.SM.RunPositivePassWithBudget(c.StateFile.States, c, &c.statePassBudget)
				if c.RuntimeError != nil {
					return
				}
				// Landing retains position/velocity and immediately resumes CNS and
				// physics. Negative states and final clocks still run once per tick.
				continue
			}
			break
		}
		c.constrainPosition()
		// Contacts at the next tick use this completed simulation pose, before
		// the animation clock selects the next CNS/display element.
		c.contactFrame, c.contactFrameReady = c.CurrentElement(), true
		c.Time++
		if c.MoveHit > 0 {
			c.MoveHit++
		}
		if c.MoveContact > 0 {
			c.MoveContact++
		}
		if c.MoveGuarded > 0 {
			c.MoveGuarded++
		}
		c.UpdateAnimation()
		c.ageBinding()
	} else {
		// ignorehitpause controllers can still change an action while its
		// clock is frozen, so publish their resulting pose too.
		c.contactFrame, c.contactFrameReady = c.CurrentElement(), true
	}

	// 6. Update Helpers, Projectiles, Explods
	for _, p := range c.Projectiles {
		p.Update()
	}

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
	c.X = x + c.cameraPositionX()
	c.Y = y
}

// CurrentElement returns the current AIR animation element for this character.
// Used by collision detection to read Clsn1 (hitboxes) and Clsn2 (hurtboxes).
func (c *Character) CurrentElement() *air.Element {
	frames := c.AnimationFile()
	if frames == nil {
		return nil
	}
	anim, ok := frames.Actions[c.Anim]
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
	return c.X - c.cameraPositionX(), c.Y
}

func (c *Character) GetVel() (float64, float64) {
	return c.VelX, c.VelY
}

func (c *Character) Turn() {
	c.Facing = -c.Facing
	c.VelX = -c.VelX
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
	next := strings.ToUpper(mtype)
	if c.MoveType == "H" && next != "H" {
		c.hitGeneration++
		c.superDefenceMul = 1
	}
	c.MoveType = next
}

func (c *Character) SetPower(power int) {
	c = c.rootPlayer()
	c.Power = power
	if c.Power > c.MaxPower {
		c.Power = c.MaxPower
	}
	if c.Power < 0 {
		c.Power = 0
	}
}

func (c *Character) AddPower(power int) {
	c.SetPower(c.GetPower() + power)
}

func (c *Character) GetPower() int {
	return c.rootPlayer().Power
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
	c.HitGuard = make(map[int]bool)
	// Parse and store the new HitDef
	c.CurrentHitDef = cns.ParseHitDef(params)
	if _, specified := params["sparkno"]; !specified {
		c.CurrentHitDef.SparkNo = c.StateFile.Data.SparkNo
	}
	if _, specified := params["guard.sparkno"]; !specified {
		c.CurrentHitDef.GuardSparkNo = c.StateFile.Data.GuardSparkNo
	}
	if _, specified := params["guard.dist"]; specified {
		c.GuardDistance = c.CurrentHitDef.GuardDist
	}
	c.CurrentHitDef.GuardDist = c.GuardDistance
	if _, specified := params["yaccel"]; !specified {
		c.CurrentHitDef.YAccel = float64(float32(0.35) * float32(c.LocalCoordinateWidth()) / 320)
	}
	// Entering a HitDef makes the character an attacker
	c.SetMoveType("A")
}

func (c *Character) SetAttackDistance(distance int) {
	c.GuardDistance = float64(distance)
	c.CurrentHitDef.GuardDist = c.GuardDistance
}

func (c *Character) SetPlayerPush(enabled bool) { c.PushDisabled = !enabled }

func (c *Character) Helper(params map[string]string) {
	version := ""
	if owner := c.currentStateOwner(); owner.StateFile != nil && owner.StateFile.States[c.StateNo] != nil {
		version = owner.StateFile.States[c.StateNo].LanguageVersion
	}
	if err := engine.ExecuteHelperParameters(params, c.stateEnvironment(), version); err != nil {
		c.RuntimeError = fmt.Errorf("Helper: %w", err)
	}
}

func (c *Character) Projectile(params map[string]string) {
	// Projectile(id, anim, hitdef, pos, vel, ...)
	anim := 0
	if v, ok := params["anim"]; ok {
		anim, _ = strconv.Atoi(v)
	}

	p := NewCharacter(c.StateFile, c.AirFile, c.CmdFile)
	p.isProjectile = true
	p.World = c.World
	p.BindSound(c.SoundQueue, c.OwnSounds, c.CommonSounds)
	p.BindEffects(c.EffectQueue, c.CommonEffects)
	p.LocalCoord = c.LocalCoord
	p.GameCoord = c.GameCoord
	p.Viewport = c.Viewport
	p.Timeline = c.Timeline
	p.SuperTargetDefenceMul = c.SuperTargetDefenceMul
	p.Palettes = c.Palettes
	if c.Palettes != nil && params["ownpal"] == "1" {
		p.Palettes = c.Palettes.Clone()
	}
	p.Parent = c
	p.Root = c.Root
	if p.Root == nil {
		p.Root = c
	}
	p.InputSlot = c.InputSlot
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
	version := ""
	if owner := c.currentStateOwner(); owner.StateFile != nil && owner.StateFile.States[c.StateNo] != nil {
		version = owner.StateFile.States[c.StateNo].LanguageVersion
	}
	if err := engine.ExecuteExplodParameters(params, c.stateEnvironment(), version); err != nil {
		c.RuntimeError = fmt.Errorf("Explod: %w", err)
	}
}

func (c *Character) RemoveExplod(id int) {
	var remaining []*ExplodData
	for _, e := range c.Explods {
		if id >= 0 && e.ID != id {
			remaining = append(remaining, e)
		} else if e.Visual != nil {
			e.Visual.Active = false
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
	version := ""
	if owner := c.currentStateOwner(); owner.StateFile != nil && owner.StateFile.States[c.StateNo] != nil {
		version = owner.StateFile.States[c.StateNo].LanguageVersion
	}
	if err := engine.ExecuteScreenBoundParameters(params, c.stateEnvironment(), version); err != nil {
		c.RuntimeError = fmt.Errorf("ScreenBound: %w", err)
	}
}

func (c *Character) Width(params map[string]string) {
	version := ""
	if owner := c.currentStateOwner(); owner.StateFile != nil && owner.StateFile.States[c.StateNo] != nil {
		version = owner.StateFile.States[c.StateNo].LanguageVersion
	}
	if err := engine.ExecuteWidthParameters(params, c.stateEnvironment(), version); err != nil {
		c.RuntimeError = fmt.Errorf("Width: %w", err)
	}
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
	p := engine.TargetBindParameters{Time: 1, ID: -1}
	if value, present := params["time"]; present {
		p.Time, _ = strconv.Atoi(value)
	}
	if value, present := params["id"]; present {
		p.ID, _ = strconv.Atoi(value)
	}
	if pos := cns.ParseFloatList(params["pos"]); len(pos) > 0 {
		p.X = pos[0]
		if len(pos) > 1 {
			p.Y = pos[1]
		}
	}
	if err := c.BindTargets(p); err != nil {
		c.RuntimeError = err
	}
}

func (c *Character) TargetDrop(id int) {
	if err := c.DropTargets(engine.TargetDropParameters{ExcludeID: id, KeepOne: true}); err != nil {
		c.RuntimeError = err
	}
}

func (c *Character) TargetFacing(id, facing int) {
	if facing == 0 {
		return
	}
	direction := c.Facing
	if facing < 0 {
		direction = -direction
	}
	for _, target := range c.acquiredTargets(id, true) {
		if target.Facing != direction {
			target.Facing = direction
			target.VelX = -target.VelX
		}
	}
}

func (c *Character) TargetLifeAdd(id, val int, kill bool) {
	if err := c.AddTargetLife(engine.TargetLifeParameters{ID: id, Value: val, Kill: kill}); err != nil {
		c.RuntimeError = err
	}
}

func (c *Character) TargetPowerAdd(id, val int) {
	for _, target := range c.acquiredTargets(id, true) {
		target.AddPower(val)
	}
}

func (c *Character) TargetState(id, state int) {
	for _, target := range c.acquiredTargets(id, true) {
		target.stateOwner = c.currentStateOwner()
		target.changeStateWithEntry(max(0, state), engine.EntryOverrides{}, true)
		if target.RuntimeError != nil {
			c.RuntimeError = target.RuntimeError
			return
		}
	}
}

func (c *Character) TargetVelAdd(x, y float64, id int) {
	if err := c.SetTargetVelocity(engine.TargetVelocityParameters{ID: id, X: x, Y: y, HasX: true, HasY: true, Add: true}); err != nil {
		c.RuntimeError = err
	}
}

func (c *Character) TargetVelSet(x, y float64, id int) {
	if err := c.SetTargetVelocity(engine.TargetVelocityParameters{ID: id, X: x, Y: y, HasX: true, HasY: true}); err != nil {
		c.RuntimeError = err
	}
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
	if err := c.ChangeAnimation2(animNo, 1); err != nil {
		c.RuntimeError = err
	}
}

func (c *Character) SelfState(stateNo int, ctrl, anim int) {
	c.stateOwner = nil
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

func (c *Character) SetPaletteMapping(source, dest [2]int) error {
	if c.Palettes == nil {
		c.EvaluationWarning("RemapPal: palette archive unavailable")
		return nil
	}
	if err := c.Palettes.Remap(palette.ID(source), palette.ID(dest)); err != nil {
		c.EvaluationWarning("RemapPal: " + err.Error())
	}
	return nil
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
	c.RuntimeError = fmt.Errorf("SuperPause requires compiled parameters")
}

func (c *Character) DestroySelf() {
	if err := c.DestroyHelper(engine.DestroySelfParameters{}); err != nil {
		c.RuntimeError = err
	}
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
	return c.HitTime < 0
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
		if !c.GetHit.FallXUnchanged {
			c.VelX = c.GetHitVar("fall.xvel")
		}
		c.VelY = c.GetHitVar("fall.yvel")
	}
}

func (c *Character) GetPrevStateNo() int {
	return c.PrevStateNo
}

func (c *Character) GetRoundsExisted() int {
	return c.RoundsExisted
}

func (c *Character) GetInGuardDist() bool {
	for _, target := range c.Opponents {
		if strings.ToUpper(target.MoveType) == "A" {
			// Range extends in the attacker's facing. The defender may face
			// away and vertical separation does not affect this query.
			targetX, _ := c.PositionOf(target)
			relDist := (c.X - targetX) * float64(target.Facing)
			if relDist >= 0 && relDist < target.GuardDistance*c.ScaleFrom(target) {
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
	return c.RoundNo
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
	frames := c.AnimationFile()
	if frames == nil {
		return false
	}
	_, exists := frames.Actions[animNo]
	return exists
}

func (c *Character) HasSelfAnim(animNo int) bool {
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
		if c.GetHitGuarded && !c.GetHitAir {
			val = c.GetHit.GuardVel
		} else if c.GetHitGuarded {
			if len(c.GetHit.AirGuardVel) > 0 {
				val = c.GetHit.AirGuardVel[0]
			}
		} else if c.GetHitDown {
			if len(c.GetHit.DownVel) > 0 {
				val = c.GetHit.DownVel[0]
			}
		} else if c.GetHitAir {
			if len(c.GetHit.AirVel) > 0 {
				val = c.GetHit.AirVel[0]
			}
		} else if len(c.GetHit.GroundVel) > 0 {
			val = c.GetHit.GroundVel[0]
		}
		if c.getHitFacing != 0 {
			// GetHitVar X is world-oriented, in receiver-local units. Keep
			// its direction independent of subsequent turns/owner retirement.
			val *= float64(-c.getHitFacing)
		}
	case "yoff":
		val = c.pendingHitYOffset
	case "yvel":
		if c.GetHitGuarded {
			if c.GetHitAir && len(c.GetHit.AirGuardVel) > 1 {
				val = c.GetHit.AirGuardVel[1]
			}
		} else if c.GetHitDown {
			if len(c.GetHit.DownVel) > 1 {
				val = c.GetHit.DownVel[1]
			}
		} else if c.GetHitAir {
			if len(c.GetHit.AirVel) > 1 {
				val = c.GetHit.AirVel[1]
			}
		} else if len(c.GetHit.GroundVel) > 1 {
			val = c.GetHit.GroundVel[1]
		}
	case "yaccel":
		val = c.GetHit.YAccel
	case "fall":
		if c.HitFall {
			val = 1
		}
	case "fall.damage":
		val = float64(c.GetHit.FallDamage)
	case "fall.xvel":
		if c.GetHit.FallXUnchanged {
			width := c.GameCoord.Width
			if width <= 0 {
				width = 320
			}
			val = float64(float32(-math.MaxFloat32) * (float32(c.LocalCoordinateWidth()) / float32(width)))
		} else if len(c.GetHit.FallVel) > 0 {
			val = c.GetHit.FallVel[0]
		}
	case "fall.yvel":
		if len(c.GetHit.FallVel) > 1 {
			val = c.GetHit.FallVel[1]
		} else {
			val = -4.5 * float64(c.LocalCoordinateWidth()) / 320
		}
	case "fall.recover":
		if c.GetHit.FallRecover {
			val = 1
		}
	case "hittime":
		val = float64(c.HitTime)
	case "damage":
		val = float64(c.GetHitDamage)
	case "guarded":
		if c.GetHitGuarded {
			val = 1
		}
	case "isbound":
		if c.bindingReported {
			val = 1
		}
	case "hitshaketime":
		val = float64(c.ShakeTime)
	case "slidetime":
		if c.GetHitGuarded {
			if !c.GetHitAir {
				val = float64(c.GetHit.GuardSlide)
			}
		} else {
			val = float64(c.GetHit.GroundSlide)
		}
	case "ctrltime":
		if c.GetHitGuarded {
			val = float64(c.GetHit.GuardCtrlTime)
			if c.GetHitAir {
				val = float64(c.GetHit.AirGuardCtrlTime)
			}
		}
	}
	return val
}

// GetTargetPos returns the opponent's screen-centered X and floor-relative Y
// in this player's local units, matching GetPos for P2Dist subtraction.
func (c *Character) GetTargetPos() (float64, float64) {
	if len(c.Opponents) > 0 {
		x, y := c.PositionOf(c.Opponents[0])
		return x - c.cameraPositionX(), y
	}
	return c.GetPos() // No target: return self (distance = 0)
}

func (c *Character) IsPaused() bool {
	if c.updateActive {
		return c.pausedThisTick || c.HitPauseTime > 0
	}
	return c.HitPauseTime > 0
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

	// Jump and landing controllers belong to the character/common CNS. Physics
	// A supplies the automatic landing entry independently of the state number.

	// Guarding Transitions (States 120-132)
	if c.Ctrl && !c.Specials["noguard"] && c.GetInGuardDist() && c.Command("holdback") {
		if c.StateNo < 120 || c.StateNo > 155 {
			c.ChangeState(120, -1, -1)
			return
		}
	}

	// Common CNS owns guard-start/hold/hit transitions. State 140's
	// animation-end return is explicitly an engine rule in common1.cns.
	if c.StateNo == 140 && c.GetAnimTime() == 0 {
		state := 0
		if c.StateType == "C" {
			state = 11
		} else if c.StateType == "A" {
			state = 51
		}
		c.ChangeState(state, -1, -1)
		return
	}

	if c.StateFile == nil {
		return
	}

	st := strings.ToUpper(c.StateType)
	phys := strings.ToUpper(c.Physics)

	if st == "S" && phys == "S" {
		if c.Ctrl && c.Command("holdup") && c.StateNo != 40 {
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
		} else if c.Ctrl && c.Command("holddown") && c.StateNo != 10 && c.StateNo != 11 {
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
		if c.Ctrl && !c.Command("holddown") && c.StateNo != 12 {
			c.ChangeState(12, -1, -1) // Crouch to Stand
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

	// Run/backhop exits are authored by CNS as well; state 105 is exempt from
	// automatic Physics A landing so its own controller can select state 106.

	// Air Jump Bookkeeping (State 45)
	if c.StateNo == 45 && c.Time == 0 {
		c.AirJumpCount++
	}

	// Reset AirJumpCount on landing
	if st == "S" || st == "C" {
		c.AirJumpCount = 0
	}

	// Intro/round-over entry is owned by the match. Re-entering 191 or 180
	// here every tick would restart scripted intros and victory pose deciders.

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
	if c.StateFile == nil || c.PushDisabled || !c.hasPushBox() {
		return
	}
	// Native body push needs hurtboxes on both actors, including hit states;
	// its horizontal extents still come from CNS size rather than Clsn2 width.
	st := strings.ToUpper(c.StateType)
	if st != "S" && st != "C" {
		return
	}

	for _, other := range c.Opponents {
		if other.PushDisabled || !other.hasPushBox() {
			continue
		}
		othSt := strings.ToUpper(other.StateType)
		if (othSt == "S" || othSt == "C") && !other.PosFrozen {
			otherX, _ := c.PositionOf(other)
			dist := otherX - c.X
			front, back := c.pushWidths()
			otherFront, otherBack := other.pushWidths()
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
				if dist < 0 || (dist == 0 && c.InputSlot > other.InputSlot) {
					direction = -1
				}
				delta := (pushDist - absDist) / 2 * direction
				spacing := 10 * float64(c.LocalCoordinateWidth()) / 320
				if absDist < spacing {
					// Native near-axis overlap correction has additional spacing
					// that falls linearly to zero at ten 240p units.
					delta += (spacing - absDist) / 2 * direction
				}
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
// them with the legacy ChangeState -1 omission sentinel.
func (c *Character) Transition(stateNo int, overrides engine.EntryOverrides, self bool) error {
	if self {
		c.stateOwner = nil
	}
	c.changeState(stateNo, overrides)
	return c.RuntimeError
}

func (c *Character) ParentEnvironment() engine.Environment {
	if c.Parent == nil || c.Parent.destroyed {
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
	c.changeStateWithEntry(stateNo, overrides, false)
}

func (c *Character) changeStateWithEntry(stateNo int, overrides engine.EntryOverrides, external bool) {
	if c.RuntimeError != nil {
		return
	}
	owner := c.currentStateOwner()
	if owner.StateFile == nil {
		c.RuntimeError = fmt.Errorf("state %d: no state file", stateNo)
		return
	}
	next, found := owner.StateFile.States[stateNo]
	if (!found && !external) || stateNo < 0 {
		c.RuntimeError = fmt.Errorf("state %d is not defined for player %d", stateNo, c.InputSlot)
		return
	}
	name := "undefined"
	if next != nil {
		name = next.Name
		if err := c.SM.CompileState(next); err != nil {
			c.RuntimeError = err
			return
		}
	} else {
		c.EvaluationWarning(fmt.Sprintf("external transition: state %d is missing from the state owner", stateNo))
	}
	c.LastStateChange = fmt.Sprintf("%d -> %d (%s)", c.StateNo, stateNo, name)
	c.PrevStateNo = c.StateNo
	c.StateNo = stateNo
	c.stateSerial++
	c.Time = 0
	c.HitFallVelActive = false
	c.PosFrozen = false
	c.pendingStateEntry = nil
	c.pendingMissingEntry = false
	c.allowMissingState = !found
	for key := range c.PersistenceCounters {
		if strings.HasPrefix(key, "pos") {
			delete(c.PersistenceCounters, key)
		}
	}
	c.stateInitialized = true
	if external {
		c.pendingStateEntry = next
		c.pendingMissingEntry = !found
		return
	}
	if err := c.SM.EnterState(next, c.stateEnvironment(), overrides); err != nil {
		c.RuntimeError = err
		return
	}
}

func (c *Character) BeginStateEntry(hitdefPersist, movehitPersist, hitcountPersist bool) {
	if !hitdefPersist {
		c.GuardDistance = float64(c.StateFile.Size.AttackDist)
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
		c.UniqHitCount = 0
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
	warning := fmt.Sprintf("player %d state %d: %s", c.InputSlot, c.StateNo, message)
	c.RuntimeWarnings = append(c.RuntimeWarnings, warning)
	logger.LogUnresolved(warning)
}
func (c *Character) SetJuggleCost(cost int) { c.JuggleCost = cost }
func (c *Character) FaceOpponent() {
	if len(c.Opponents) == 0 {
		return
	}
	facing := c.Facing
	x, _ := c.PositionOf(c.Opponents[0])
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
	hit := c.CurrentHitDef
	hit.Attr = c.attributesForHit(hit)
	if other.excludesCustomHit(hit) {
		return
	}
	if err := c.ensureWorld(); err != nil {
		c.RuntimeError = err
		return
	}
	if other.World == nil {
		if err := c.World.AddRoots(other); err != nil {
			c.RuntimeError = err
			return
		}
	}
	// Prevent the same target from being hit multiple times by the same HitDef swing
	if c.HitGuard == nil {
		c.HitGuard = make(map[int]bool)
	}
	if c.HitGuard[int(other.EntityID)] || (hit.HitOnce && len(c.HitGuard) > 0) {
		return // Already hit this target this swing
	}
	c.HitGuard[int(other.EntityID)] = true

	// 1. Mark attacker results
	guarded := other.guardsHit(hit)
	c.MoveHit = 1
	c.MoveContact = 1
	c.MoveGuarded = 0
	if guarded {
		c.MoveHit, c.MoveGuarded = 0, 1
	}

	// Apply HitPause to attacker
	pause := c.CurrentHitDef.PauseTime
	if guarded {
		pause = c.CurrentHitDef.GuardPauseTime
	}
	if len(pause) > 0 {
		c.HitPauseTime = pause[0]
	}

	// 2. Mark defender results
	other.ProcessHit(c, c.CurrentHitDef)
}

func (c *Character) ProcessHit(attacker *Character, hit cns.HitDef) {
	hit.Attr = attacker.attributesForHit(hit)
	if c.excludesCustomHit(hit) {
		return
	}
	damage := c.prepareContact(attacker, hit, c.guardsHit(hit))
	c.applyHitDamage(damage)
}

func (c *Character) applyHitDamage(damage int) {
	// This variable exposes pending damage during entry/state execution only.
	c.GetHitDamage = 0
	if c.MoveType != "H" {
		return
	}
	c.SetLife(c.Life - damage)
}

// HitOverride suppresses guarding; guard flags then select eligible postures.
// Guard end (140) remains eligible even after control has been relinquished.
func (c *Character) guardsHit(hit cns.HitDef) bool {
	if _, overridden := c.matchingOverride(hit.Attr); overridden || c.StateNo < 120 || c.StateNo > 155 {
		return false
	}
	flag := strings.ToUpper(hit.GuardFlag)
	switch c.StateType {
	case "S":
		return strings.ContainsAny(flag, "HM")
	case "C":
		return strings.ContainsAny(flag, "LM")
	case "A":
		return strings.Contains(flag, "A")
	}
	return false
}

func (c *Character) prepareContact(attacker *Character, hit cns.HitDef, guarded bool) int {
	c.binding = nil
	if !guarded {
		if err := c.interruptTargets(); err != nil {
			c.RuntimeError = err
			return 0
		}
	}
	// Contact priorities apply even when guarded or overridden. Subsequent
	// own/custom state entry can replace either priority through its Statedef.
	attacker.SprPriority, c.SprPriority = 1, 0
	if hit.P1SprPriority != nil {
		attacker.SprPriority = *hit.P1SprPriority
	}
	if hit.P2SprPriority != nil {
		c.SprPriority = *hit.P2SprPriority
	}
	for _, e := range c.Explods {
		if e.RemoveOnGetHit && e.Visual != nil {
			e.Visual.Active = false
		}
	}
	if err := c.contactEffect(attacker, hit, guarded); err != nil {
		c.RuntimeError = fmt.Errorf("player %d contact effect: %w", c.InputSlot, err)
		return 0
	}
	if err := c.contactSound(attacker, hit, guarded); err != nil {
		c.RuntimeError = fmt.Errorf("player %d contact sound: %w", c.InputSlot, err)
		return 0
	}
	override, hasOverride := c.matchingOverride(hit.Attr)
	c.creditHitPower(attacker, hit, guarded)
	// Contact chooses the attack direction now, but publishes the attacker's
	// turn after its CNS/physics, like the receiver's deferred P2Facing turn.
	// Entry expressions and Time=0 controllers still observe the old heading.
	hitFacing := attacker.Facing
	if !guarded && !hasOverride {
		if hit.P1GetP2Facing != 0 {
			hitFacing = c.Facing
			if hit.P1GetP2Facing < 0 {
				hitFacing = -hitFacing
			}
		} else if hit.P1Facing < 0 {
			hitFacing = -hitFacing
		}
		if hit.P1GetP2Facing != 0 || hit.P1Facing < 0 {
			attacker.pendingHitFacing = hitFacing
		}
	}
	airHit := c.StateType == "A" || (hasOverride && override.ForceAir)
	forceStand := len(hit.GroundVel) > 1 && hit.GroundVel[1] != 0
	if hit.ForceStand != nil {
		forceStand = *hit.ForceStand
	}
	if !guarded && c.StateType == "C" && forceStand {
		c.SetStateType("S")
	}
	c.GetHitDown = c.StateType == "L" && !airHit
	c.GetHitAir = airHit
	c.GetHitGuarded = guarded
	originalHit := hit
	hit = c.hitInLocalSpace(attacker, hit)
	c.getHitFacing = hitFacing
	c.GetHit = hit // Copy by value to avoid stack pointer issues
	c.recordCornerPush(attacker, originalHit)
	c.pendingHitYOffset = 0
	if c.GetHitDown && len(hit.DownVel) > 1 && hit.DownVel[1] != 0 {
		// Beta uses a fixed 240p offset, independently of character trip/width
		// constants, converted into this receiver's local coordinate profile.
		c.pendingHitYOffset = float64(float32(15) * float32(c.LocalCoordinateWidth()) / 320)
	}
	if !guarded && (hit.P2Facing == 1 || hit.P2Facing == -1) {
		c.pendingHitFacing = -hit.P2Facing * hitFacing
	}
	c.HitPauseTime = 0
	c.pausedThisTick = false

	// Life reduction
	damage := 0
	if len(hit.Damage) > 0 {
		damage = hit.Damage[0]
	}
	if guarded {
		damage = 0
		if len(hit.Damage) > 1 {
			damage = hit.Damage[1]
		}
	}
	// Apply attack/defence multipliers
	// A new get-hit sequence clears the prior idle-state defence adjustment.
	// Later H-to-H contacts retain a multiplier established while being hit.
	if c.MoveType != "H" {
		c.DefenceMul = 1
	}
	superDefence := c.superDefenceMul
	if superDefence <= 0 {
		superDefence = 1
	}
	// Native adds one half before truncating; negative damage therefore has
	// a different boundary from an away-from-zero rounding operation.
	finalDamage := int(float64(damage)*attacker.AttackMul*c.DefenceMul/superDefence + .5)
	c.GetHitDamage = finalDamage
	if c.hasPendingHitDamage {
		c.GetHitDamage += c.pendingHitDamage
	}

	// Hit Pause/Shake
	pause := hit.PauseTime
	if guarded {
		pause = hit.GuardPauseTime
	}
	c.ShakeTime = 0
	if len(pause) > 1 {
		c.ShakeTime = pause[1]
	}

	// Knockback is stored in GetHit and applied by the get-hit state's
	// HitVelSet, rather than inventing an immediate opposite-sign velocity.

	// Set HitTime duration based on state
	if guarded {
		c.HitTime = hit.GuardHitTime
		if airHit {
			c.HitTime = -1
		}
	} else if airHit {
		if hit.AirHitTime > 0 {
			c.HitTime = hit.AirHitTime
		} else {
			c.HitTime = 20 // Reasonable default if air.hittime missing
		}
	} else {
		c.HitTime = hit.GroundHitTime
	}

	// A subsequent unguarded hit retains an existing get-hit fall unless
	// ForceNoFall clears it. A hit's own fall setting still takes precedence.
	fall := hit.Fall == 1
	if airHit {
		fall = hit.AirFall == 1
	}
	c.HitFall = fall || (!guarded && c.MoveType == "H" && c.HitFall && !hit.ForceNoFall)

	// Guarding vs Normal Hit
	// A fresh contact restores the receiver's own state definitions, including
	// contacts received while executing another player's custom state.
	c.stateOwner = nil
	if hasOverride {
		standard := 5000
		if c.StateType == "C" {
			standard = 5010
		} else if c.StateType == "L" {
			standard = 5080
		}
		if airHit {
			standard = 5020
		}
		c.ChangeState(standard, 0, -1)
		c.ChangeState(override.StateNo, 0, -1)
	} else if guarded {
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
		state := 5000
		if c.StateType == "C" {
			state = 5010
		} else if c.StateType == "L" {
			state = 5080
		} else if airHit {
			state = 5020
		}
		// Standard numbers and H/no-control publish at contact. Entry waits
		// until this receiver's positive pass, after earlier actors finish.
		// A custom destination below supersedes the standard entry entirely.
		c.changeStateWithEntry(state, engine.EntryOverrides{}, true)
		c.SetMoveType("H")
		c.SetCtrl(false)
	}
	if !guarded && !hasOverride {
		if hit.P1StateNo != nil && *hit.P1StateNo >= 0 {
			attacker.changeStateWithEntry(*hit.P1StateNo, engine.EntryOverrides{}, true)
		}
		if hit.P2StateNo != nil && *hit.P2StateNo >= 0 {
			if hit.P2GetP1State {
				c.stateOwner = attacker.currentStateOwner()
			}
			c.changeStateWithEntry(*hit.P2StateNo, engine.EntryOverrides{}, true)
		}
	}
	if c.MoveType == "H" && !hasOverride {
		if hit.HitOnce {
			if err := attacker.DropTargets(engine.TargetDropParameters{ExcludeID: -1}); err != nil {
				c.RuntimeError = err
				return 0
			}
		}
		attacker.acquireTarget(c, hit.ID)
	}
	return finalDamage
}

func (c *Character) Command(name string) bool {
	if c.Parent != nil && !c.isProjectile && !c.HelperKeyControl {
		return false
	}
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
	ID                                            int
	Anim                                          int
	X, Y                                          float64
	PosType                                       string
	Facing                                        int
	VelX, VelY                                    float64
	Time                                          int // Ticks remaining (-1 for infinite)
	CurrentTime                                   int
	Visual                                        *effect.Instance
	Fresh, IgnoreHitPause, RemoveOnGetHit         bool
	AccelX, AccelY, ScaleToGame, OffsetX, OffsetY float64
	Anchor                                        *Character
	BindingTime                                   int
}

func (c *Character) ageDefenses() {
	if c.IsPaused() {
		return
	}
	// Tick down protection flags
	c.tickHitOverrides()
	for slot := range c.HitFilters {
		if c.HitFilters[slot].Time > 0 {
			c.HitFilters[slot].Time--
		}
	}
}
