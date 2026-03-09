package character

import (
	"fmt"
	"math"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// Environment interface provides access to game variables and state
// This allows the state machine to query things like Time, StateNo, AnimTime, etc.
type Environment interface {
	GetTime() int
	GetAnim() int
	GetAnimTime() int
	GetVar(idx int) int
	SetVar(idx, val int)
	GetFVar(idx int) float64
	SetFVar(idx int, val float64)
	GetSysVar(idx int) int
	SetSysVar(idx, val int)
	GetSysFVar(idx int) float64
	SetSysFVar(idx int, val float64)
	GetStateType() string
	GetPhysics() string
	GetAnimElem() int
	GetMoveType() string
	AddVelocity(x, y float64)
	SetVelocity(x, y float64)
	AddPosition(x, y float64)
	SetPosition(x, y float64)
	GetPos() (float64, float64)
	GetVel() (float64, float64)
	GetStateNo() int
	GetFacing() int
	GetCtrl() bool
	ChangeState(stateNo int, ctrl int, anim int) // ctrl/anim -1 means no change
	ChangeAnim(animNo int)
	Command(name string) bool
	Turn()
	SetCtrl(ctrl bool)
	SetStateType(stype string)
	SetPhysics(physics string)
	SetMoveType(mtype string)
	SetPower(power int)
	AddPower(power int)
	GetPower() int
	SetLife(life int)
	AddLife(life int)
	GetLife() int
	SetHitFall(fall bool)
	SetHitFallDamage(damage int)
	SetHitFallVel(active bool)
	SetAttackMul(mul float64)
	SetDefenceMul(mul float64)
	SetSprPriority(priority int)
	SetGravity(active bool)
	VelMul(x, y float64)
	PosFreeze(active bool)
	HitDef(params map[string]string)
	Projectile(params map[string]string)
	AssertSpecial(flag string)
	Explod(params map[string]string)
	RemoveExplod(id int)
	AfterImage(params map[string]string)
	AfterImageTime(time int)
	EnvShake(params map[string]string)
	EnvColor(params map[string]string)
	ScreenBound(params map[string]string)
	Width(params map[string]string)
	Trans(params map[string]string)
	BindToParent(params map[string]string)
	BindToRoot(params map[string]string)
	BindToTarget(params map[string]string)
	TargetBind(params map[string]string)
	TargetDrop(id int)
	TargetFacing(id, facing int)
	TargetLifeAdd(id, val int, kill bool)
	TargetPowerAdd(id, val int)
	TargetState(id, state int)
	TargetVelAdd(x, y float64, id int)
	TargetVelSet(x, y float64, id int)
	ReversalDef(params map[string]string)
	MoveHitReset()
	HitAdd(val int)
	ChangeAnim2(animNo int)
	SelfState(stateNo int, ctrl, anim int)
	PalFX(params map[string]string)
	AllPalFX(params map[string]string)
	BGPalFX(params map[string]string)
	RemapPal(params map[string]string)
	AngleAdd(angle float64)
	AngleDraw(angle float64)
	AngleMul(mul float64)
	AngleSet(angle float64)
	Offset(x, y float64)
	SndPan(channel int, pan int)
	StopSnd(channel int)
	AppendToClipboard(text string)
	ClearClipboard()
	DisplayToClipboard(text string)
	ForceFeedback(params map[string]string)
	SuperPause(params map[string]string)
	Helper(params map[string]string)
	DestroySelf()
	FallEnvShake(params map[string]string)
	GameMakeAnim(params map[string]string)
	MakeDust(params map[string]string)
	VarRandom(id int, min, max int)
	VarRangeSet(min, max int, val int)
	FVarRangeSet(min, max int, val float64)
	VictoryQuote(quote int)
	GetMoveHit() int
	GetMoveContact() int
	GetMoveGuarded() int
	GetPrevStateNo() int
	GetInGuardDist() bool
	GetMatchOver() bool
	GetRoundNo() int
	GetRoundState() int
	GetRoundsExisted() int
	HitBy(params map[string]string)
	NotHitBy(params map[string]string)
	GetCNSVelocity() *cns.Velocity
	GetCNSMovement() *cns.Movement
	IsPaused() bool
	GetPersistence(key string) int
	SetPersistence(key string, val int)
	GetStateFile() *cns.CNS
}

// ControllerType represents a generic function that applies a StateController to the environment.
type ControllerFunc func(ctrl *cns.StateController, env Environment) error

// StateMachine manages the execution of MUGEN state blocks.
type StateMachine struct {
	controllers map[string]ControllerFunc
	cache       map[string]evaluator.Expression
}

func NewStateMachine() *StateMachine {
	sm := &StateMachine{
		controllers: make(map[string]ControllerFunc),
		cache:       make(map[string]evaluator.Expression),
	}
	sm.registerCoreControllers()
	return sm
}

// registerCoreControllers binds supported MUGEN controllers to their handlers.
func (sm *StateMachine) registerCoreControllers() {
	sm.controllers["changestate"] = handleChangeState
	sm.controllers["selfstate"] = handleSelfState
	sm.controllers["changeanim"] = handleChangeAnim
	sm.controllers["velset"] = handleVelSet
	sm.controllers["veladd"] = handleVelAdd
	sm.controllers["posset"] = handlePosSet
	sm.controllers["posadd"] = handlePosAdd
	sm.controllers["varset"] = handleVarSet
	sm.controllers["varadd"] = handleVarAdd
	sm.controllers["fvarset"] = handleFVarSet
	sm.controllers["fvaradd"] = handleFVarAdd
	sm.controllers["turn"] = handleTurn
	sm.controllers["ctrlset"] = handleCtrlSet
	sm.controllers["statetypeset"] = handleStateTypeSet
	sm.controllers["lifeadd"] = handleLifeAdd
	sm.controllers["lifeset"] = handleLifeSet
	sm.controllers["poweradd"] = handlePowerAdd
	sm.controllers["powerset"] = handlePowerSet
	sm.controllers["attackmulset"] = handleAttackMulSet
	sm.controllers["defencemulset"] = handleDefenceMulSet
	sm.controllers["sprpriority"] = handleSprPriority
	sm.controllers["gravity"] = handleGravity
	sm.controllers["velmul"] = handleVelMul
	sm.controllers["hitvelset"] = handleHitVelSet
	sm.controllers["hitfalldamage"] = handleHitFallDamage
	sm.controllers["hitfallset"] = handleHitFallSet
	sm.controllers["hitfallvel"] = handleHitFallVel
	sm.controllers["posfreeze"] = handlePosFreeze
	sm.controllers["pause"] = handlePause
	sm.controllers["playsnd"] = handlePlaySnd
	sm.controllers["hitdef"] = handleHitDef
	sm.controllers["projectile"] = handleProjectile
	sm.controllers["hitby"] = handleHitBy
	sm.controllers["nothitby"] = handleNotHitBy
	sm.controllers["assertspecial"] = handleAssertSpecial
	sm.controllers["explod"] = handleExplod
	sm.controllers["removeexplod"] = handleRemoveExplod
	sm.controllers["afterimage"] = handleAfterImage
	sm.controllers["afterimagetime"] = handleAfterImageTime
	sm.controllers["envshake"] = handleEnvShake
	sm.controllers["envcolor"] = handleEnvColor
	sm.controllers["screenbound"] = handleScreenBound
	sm.controllers["width"] = handleWidth
	sm.controllers["trans"] = handleTrans
	sm.controllers["bindtoparent"] = handleBindToParent
	sm.controllers["bindtoroot"] = handleBindToRoot
	sm.controllers["bindtotarget"] = handleBindToTarget
	sm.controllers["targetbind"] = handleTargetBind
	sm.controllers["targetdrop"] = handleTargetDrop
	sm.controllers["targetfacing"] = handleTargetFacing
	sm.controllers["targetlifeadd"] = handleTargetLifeAdd
	sm.controllers["targetpoweradd"] = handleTargetPowerAdd
	sm.controllers["targetstate"] = handleTargetState
	sm.controllers["targetveladd"] = handleTargetVelAdd
	sm.controllers["targetvelset"] = handleTargetVelSet
	sm.controllers["reversaldef"] = handleReversalDef
	sm.controllers["movehitreset"] = handleMoveHitReset
	sm.controllers["hitadd"] = handleHitAdd
	sm.controllers["changeanim2"] = handleChangeAnim2
	sm.controllers["palfx"] = handlePalFX
	sm.controllers["allpalfx"] = handleAllPalFX
	sm.controllers["bgpalfx"] = handleBGPalFX
	sm.controllers["remappal"] = handleRemapPal
	sm.controllers["angleadd"] = handleAngleAdd
	sm.controllers["angledraw"] = handleAngleDraw
	sm.controllers["anglemul"] = handleAngleMul
	sm.controllers["angleset"] = handleAngleSet
	sm.controllers["offset"] = handleOffset
	sm.controllers["sndpan"] = handleSndPan
	sm.controllers["stopsnd"] = handleStopSnd
	sm.controllers["appendtoclipboard"] = handleAppendToClipboard
	sm.controllers["clearclipboard"] = handleClearClipboard
	sm.controllers["displaytoclipboard"] = handleDisplayToClipboard
	sm.controllers["forcefeedback"] = handleForceFeedback
	sm.controllers["superpause"] = handleSuperPause
	sm.controllers["helper"] = handleHelper
	sm.controllers["destroyself"] = handleDestroySelf
	sm.controllers["fallenvshake"] = handleFallEnvShake
	sm.controllers["gamemakeanim"] = handleGameMakeAnim
	sm.controllers["makedust"] = handleMakeDust
	sm.controllers["varrandom"] = handleVarRandom
	sm.controllers["varrangeset"] = handleVarRangeSet
	sm.controllers["fvarrangeset"] = handleFVarRangeSet
	sm.controllers["victoryquote"] = handleVictoryQuote
}

// ExecuteState executes all valid controllers within a given state definition.
// Returns true if a state transition (ChangeState/SelfState) occurred.
func (sm *StateMachine) ExecuteState(state *cns.StateDef, env Environment) (bool, error) {
	isPaused := env.IsPaused()

	// Applying Statedef attributes on state entry (Time = 0)
	// This ensures attributes like MoveType or Physics don't "stick" from the previous state.
	if env.GetTime() == 0 && state.ID >= 0 {
		if state.Type != "" && strings.ToUpper(state.Type) != "U" {
			env.SetStateType(state.Type)
		}
		if state.MoveType != "" && strings.ToUpper(state.MoveType) != "U" {
			env.SetMoveType(state.MoveType)
		}
		if state.Physics != "" && strings.ToUpper(state.Physics) != "U" {
			env.SetPhysics(state.Physics)
		}
		if state.Ctrl != -1 {
			env.SetCtrl(state.Ctrl != 0)
		}
		if state.SprPriority != -255 { // Default sentinel
			env.SetSprPriority(state.SprPriority)
		}
		if state.Anim != -1 {
			env.ChangeAnim(state.Anim)
		}
		if len(state.VelSet) > 0 {
			x := state.VelSet[0]
			y := 0.0
			if len(state.VelSet) > 1 {
				y = state.VelSet[1]
			}
			env.SetVelocity(x, y)
		}
	}

	for i := range state.Controllers {
		ctrl := &state.Controllers[i]

		// 1. Hit Pause Skipping
		if isPaused && ctrl.IgnoreHitPause == 0 {
			continue
		}

		// Use a distinct prefix for negative states to prevent them being cleared during ChangeState.
		isNegative := state.ID < 0
		prefix := "pos"
		if isNegative {
			prefix = fmt.Sprintf("neg%d", state.ID)
		}

		persistKey := fmt.Sprintf("%s-%d-%s", prefix, i, ctrl.Type)
		if ctrl.Name != "" {
			persistKey = fmt.Sprintf("%s-%d-%s-%s", prefix, i, ctrl.Type, ctrl.Name)
		}

		currentPersistence := env.GetPersistence(persistKey)
		if ctrl.Persistent == 0 && currentPersistence > 0 {
			// Persistent = 0 means once-per-state entry
			continue
		}
		if ctrl.Persistent > 1 && currentPersistence >= ctrl.Persistent {
			// e.g. persistent = 5 means every 5 ticks
			// This is a simplification; MUGEN persistence is usually "every N ticks" or "once".
			// Persistent = 1 is default (every tick).
			// If persistent > 1, MUGEN interprets it as "re-run every N ticks".
		}

		matched := sm.evaluateTriggers(ctrl, env)

		if matched {
			// fmt.Printf("Tick %d: Activating %s (%s) in state %d\n", env.GetTime(), ctrl.Type, ctrl.Name, env.GetStateNo())
			handler, ok := sm.controllers[strings.ToLower(ctrl.Type)]
			if !ok {
				// Unsupported controller type
				continue
			}

			// Update persistence counter
			env.SetPersistence(persistKey, currentPersistence+1)

			err := handler(ctrl, env)
			if err != nil {
				return false, fmt.Errorf("error executing %s: %v", ctrl.Type, err)
			}

			// If the controller type is ChangeState or SelfState, we abort the rest of the controllers
			lowerType := strings.ToLower(ctrl.Type)
			if lowerType == "changestate" || lowerType == "selfstate" {
				return true, nil
			}
		} else {
			// If trigger failed, we might need to reset something?
			// Usually MUGEN persistence for > 1 works by counting executions.
		}
	}
	return false, nil
}

// evaluateTriggers evaluates the Trigger1 and TriggerAll logic for a controller.
func (sm *StateMachine) evaluateTriggers(ctrl *cns.StateController, env Environment) bool {
	evaluator.SetCustomEvaluation(func(name string, env evaluator.Environment) float64 {
		charEnv, ok := env.(Environment)
		if !ok {
			return math.MaxFloat64 // Indicate unknown if not a character environment
		}
		switch name { // name is already normalized by Evaluate
		case "time":
			return float64(charEnv.GetTime())
		case "animtime":
			return float64(charEnv.GetAnimTime())
		case "animelem":
			return float64(charEnv.GetAnimElem())
		case "anim":
			return float64(charEnv.GetAnim())
		case "pos x":
			x, _ := charEnv.GetPos()
			return x
		case "pos y":
			_, y := charEnv.GetPos()
			return y
		case "vel x":
			x, _ := charEnv.GetVel()
			return x
		case "vel y":
			_, y := charEnv.GetVel()
			return y
		case "statetype":
			// MUGEN StateType: A=1, S=2, C=3, L=4
			switch strings.ToLower(charEnv.GetStateType()) {
			case "s":
				return 2
			case "c":
				return 3
			case "a":
				return 1
			case "l":
				return 4
			default:
				return 0
			}
		case "stateno":
			return float64(charEnv.GetStateNo())
		case "ctrl":
			if charEnv.GetCtrl() {
				return 1
			}
			return 0
		case "facing":
			return float64(charEnv.GetFacing())
		case "movehit":
			return float64(charEnv.GetMoveHit())
		case "movecontact":
			return float64(charEnv.GetMoveContact())
		case "moveguarded":
			return float64(charEnv.GetMoveGuarded())
		case "power":
			return float64(charEnv.GetPower())
		case "life":
			return float64(charEnv.GetLife())
		case "alive":
			if charEnv.GetLife() > 0 {
				return 1
			}
			return 0
		case "canrecover":
			return 0 // Placeholder
		case "roundstate":
			return 2 // Fighting
		case "s": // Stand (StateType/Physics)
			return 2
		case "c": // Crouch (StateType/Physics)
			return 3
		case "a": // Air (StateType) / Attack (MoveType)
			return 1
		case "l": // Liedown (StateType)
			return 4
		case "i": // Idle (MoveType)
			return 0
		case "h": // GetHit (MoveType)
			return 5
		case "n": // None (Physics)
			return 0
		}
		return math.MaxFloat64 // Mark as unknown
	})

	if len(ctrl.Triggers) == 0 {
		return false
	}

	// Group triggers by their index
	groups := make(map[int][]evaluator.Expression)
	var groupIDs []int

	for _, trigStr := range ctrl.Triggers {
		if trigStr == "" {
			continue
		}

		// Use caching for trigger expressions
		var expr evaluator.Expression
		var err error

		if cached, ok := sm.cache[trigStr]; ok {
			expr = cached
		} else {
			// Extract the numeric index from triggers like trigger1 = ...
			parts := strings.SplitN(trigStr, "=", 2)
			if len(parts) < 2 {
				continue
			}

			rawExpr := strings.TrimSpace(parts[1])
			expr, err = evaluator.Parse(rawExpr)
			if err != nil {
				// Cache the error-state too to avoid re-parsing invalid triggers
				sm.cache[trigStr] = nil
				continue
			}
			sm.cache[trigStr] = expr
		}

		if expr == nil {
			continue
		}

		parts := strings.SplitN(trigStr, "=", 2)
		key := strings.ToLower(strings.TrimSpace(parts[0]))

		var groupID int
		if strings.HasPrefix(key, "triggerall") {
			groupID = -1
		} else if strings.HasPrefix(key, "trigger") {
			idStr := strings.TrimPrefix(key, "trigger")
			if idStr == "" {
				groupID = 1 // Default to trigger1 if just "trigger"
			} else if n, err := strconv.Atoi(idStr); err == nil {
				groupID = n
			} else {
				groupID = 1
			}
		} else {
			continue
		}

		if _, ok := groups[groupID]; !ok {
			groupIDs = append(groupIDs, groupID)
		}
		groups[groupID] = append(groups[groupID], expr)
	}

	// 1. All triggerall must be true
	if allExprs, ok := groups[-1]; ok {
		for _, expr := range allExprs {
			res := evaluator.Evaluate(expr, env)
			if res == 0 {
				// fmt.Printf("DEBUG: triggerall(%d) failed in state %d\n", i, env.GetStateNo())
				return false
			}
		}
	}

	// 2. At least one group must be true.
	// A group is true if ALL its expressions are true.
	hasNumberedGroup := false
	for _, id := range groupIDs {
		if id < 0 {
			continue
		}
		hasNumberedGroup = true
		groupMatched := true
		for _, expr := range groups[id] {
			res := evaluator.Evaluate(expr, env)
			if res == 0 {
				groupMatched = false
				break
			}
		}
		if groupMatched {
			return true
		}
	}

	// MUGEN special triggers: command = "name"
	// These are often parsed as simple attributes, but they are technically triggers.
	// Check if any trigger resembles 'command = "..."'
	for _, trigStr := range ctrl.Triggers {
		trigStr = strings.ToLower(trigStr)
		if strings.HasPrefix(trigStr, "command") && strings.Contains(trigStr, "=") {
			parts := strings.SplitN(trigStr, "=", 2)
			cmdName := strings.Trim(strings.TrimSpace(parts[1]), "\"")
			if env.Command(cmdName) {
				return true
			}
		}
	}

	// If there were no numbered groups, but all triggeralls passed, it's a match.
	if !hasNumberedGroup && len(groups[-1]) > 0 {
		return true
	}

	return false
}

// core handlers:

func handleChangeState(ctrl *cns.StateController, env Environment) error {
	valStr, ok := ctrl.Params["value"]
	if !ok {
		return fmt.Errorf("missing value in ChangeState")
	}

	// Support expressions for state value
	expr, err := evaluator.Parse(valStr)
	if err != nil {
		return fmt.Errorf("error parsing ChangeState value '%s': %v", valStr, err)
	}
	val := int(evaluator.Evaluate(expr, env))

	ctrlFlag := -1
	if cStr, ok := ctrl.Params["ctrl"]; ok {
		if cExpr, err := evaluator.Parse(cStr); err == nil {
			ctrlFlag = int(evaluator.Evaluate(cExpr, env))
		}
	}

	animOverride := -1
	if aStr, ok := ctrl.Params["anim"]; ok {
		if aExpr, err := evaluator.Parse(aStr); err == nil {
			animOverride = int(evaluator.Evaluate(aExpr, env))
		}
	}

	env.ChangeState(val, ctrlFlag, animOverride)
	return nil
}

func handleChangeAnim(ctrl *cns.StateController, env Environment) error {
	valStr, ok := ctrl.Params["value"]
	if !ok {
		return fmt.Errorf("missing value in ChangeAnim")
	}
	// Evaluate as an expression so that ifelse(...) and other MUGEN expressions work.
	// For example, State 50 uses: ifelse((vel x)=0, 41, ifelse((vel x)>0, 42, 43))
	var val int
	if expr, err := evaluator.Parse(valStr); err == nil {
		val = int(evaluator.Evaluate(expr, env))
	} else if _, err := fmt.Sscanf(valStr, "%d", &val); err != nil {
		return fmt.Errorf("invalid value in ChangeAnim: %s", valStr)
	}
	env.ChangeAnim(val)
	return nil
}

func handleVelSet(ctrl *cns.StateController, env Environment) error {
	// Read current velocity so we only override axes that are specified in params
	curX, curY := env.GetVel()
	newX, newY := curX, curY

	if xStr, ok := ctrl.Params["x"]; ok {
		if expr, err := evaluator.Parse(xStr); err == nil {
			newX = evaluator.Evaluate(expr, env)
		} else if val, err := strconv.ParseFloat(xStr, 64); err == nil {
			newX = val
		}
	}
	if yStr, ok := ctrl.Params["y"]; ok {
		if expr, err := evaluator.Parse(yStr); err == nil {
			newY = evaluator.Evaluate(expr, env)
		} else if val, err := strconv.ParseFloat(yStr, 64); err == nil {
			newY = val
		}
	}

	// Debug logging for State 40 jump velocity
	if env.GetStateNo() == 40 {
		xStrVal := ctrl.Params["x"]
		yStrVal := ctrl.Params["y"]
		fmt.Printf("[DEBUG] State 40: VelSet x_expr=%v, y_expr=%v -> newX=%v, newY=%v\n", xStrVal, yStrVal, newX, newY)
	}

	env.SetVelocity(newX, newY)
	return nil
}

func handleVelAdd(ctrl *cns.StateController, env Environment) error {
	var x, y float64
	if xStr, ok := ctrl.Params["x"]; ok {
		if expr, err := evaluator.Parse(xStr); err == nil {
			x = evaluator.Evaluate(expr, env)
		} else if val, err := strconv.ParseFloat(xStr, 64); err == nil {
			x = val
		}
	}
	if yStr, ok := ctrl.Params["y"]; ok {
		if expr, err := evaluator.Parse(yStr); err == nil {
			y = evaluator.Evaluate(expr, env)
		} else if val, err := strconv.ParseFloat(yStr, 64); err == nil {
			y = val
		}
	}
	env.AddVelocity(x, y)
	return nil
}

func handleTurn(ctrl *cns.StateController, env Environment) error {
	env.Turn()
	return nil
}

func handlePosSet(ctrl *cns.StateController, env Environment) error {
	curX, curY := env.GetPos()
	newX, newY := curX, curY
	if xStr, ok := ctrl.Params["x"]; ok {
		if expr, err := evaluator.Parse(xStr); err == nil {
			newX = evaluator.Evaluate(expr, env)
		} else if val, err := strconv.ParseFloat(xStr, 64); err == nil {
			newX = val
		}
	}
	if yStr, ok := ctrl.Params["y"]; ok {
		if expr, err := evaluator.Parse(yStr); err == nil {
			newY = evaluator.Evaluate(expr, env)
		} else if val, err := strconv.ParseFloat(yStr, 64); err == nil {
			newY = val
		}
	}
	env.SetPosition(newX, newY)
	return nil
}
func handleVarSet(ctrl *cns.StateController, env Environment) error {
	var idx int
	found := false
	valStr := ""

	if v, ok := ctrl.Params["v"]; ok {
		fmt.Sscanf(v, "%d", &idx)
		valStr = ctrl.Params["value"]
		found = true
	} else {
		// Robust search for var(X) anywhere in params
		for k, v := range ctrl.Params {
			k = strings.ToLower(k)
			// Handle both var(1) and var1
			if strings.HasPrefix(k, "var") {
				inner := strings.TrimPrefix(k, "var")
				inner = strings.Trim(inner, "()")
				if n, err := strconv.Atoi(inner); err == nil {
					idx = n
					valStr = v
					found = true
					break
				}
			} else if strings.HasPrefix(k, "sysvar") {
				inner := strings.TrimPrefix(k, "sysvar")
				inner = strings.Trim(inner, "()")
				if n, err := strconv.Atoi(inner); err == nil {
					idx = n
					valStr = v
					if expr, err := evaluator.Parse(valStr); err == nil {
						val := int(evaluator.Evaluate(expr, env))
						env.SetSysVar(idx, val)
						if idx == 1 && env.GetStateNo() == 40 {
							fmt.Printf("[DEBUG] State 40 VarSet sysvar(1) = %d\n", val)
						}
					}
					return nil
				}
			}
		}
	}

	if !found {
		return fmt.Errorf("missing v index or var(n) in VarSet (params: %v)", ctrl.Params)
	}

	// The value can be a complex expression (e.g. var(2) = command = "holdfwd")
	expr, err := evaluator.Parse(valStr)
	if err != nil {
		// Try parsing as literal int if it's not a full expression
		var val int
		if _, err := fmt.Sscanf(valStr, "%d", &val); err == nil {
			env.SetVar(idx, val)
			return nil
		}
		return fmt.Errorf("error parsing VarSet value '%s': %v", valStr, err)
	}

	val := evaluator.Evaluate(expr, env)
	env.SetVar(idx, int(val))
	return nil
}

func handleVarAdd(ctrl *cns.StateController, env Environment) error {
	vStr, ok := ctrl.Params["v"]
	if !ok {
		return fmt.Errorf("missing v index in VarAdd")
	}
	var idx int
	fmt.Sscanf(vStr, "%d", &idx)
	valStr, ok := ctrl.Params["value"]
	if !ok {
		return fmt.Errorf("missing value in VarAdd")
	}
	var val int
	fmt.Sscanf(valStr, "%d", &val)
	env.SetVar(idx, env.GetVar(idx)+val)
	return nil
}

func handleFVarSet(ctrl *cns.StateController, env Environment) error {
	vStr, ok := ctrl.Params["v"]
	if !ok {
		return fmt.Errorf("missing v index in FVarSet")
	}
	var idx int
	fmt.Sscanf(vStr, "%d", &idx)
	valStr, ok := ctrl.Params["value"]
	if !ok {
		return fmt.Errorf("missing value in FVarSet")
	}
	var val float64
	fmt.Sscanf(valStr, "%f", &val)
	env.SetFVar(idx, val)
	return nil
}

func handleFVarAdd(ctrl *cns.StateController, env Environment) error {
	vStr, ok := ctrl.Params["v"]
	if !ok {
		return fmt.Errorf("missing v index in FVarAdd")
	}
	var idx int
	fmt.Sscanf(vStr, "%d", &idx)
	valStr, ok := ctrl.Params["value"]
	if !ok {
		return fmt.Errorf("missing value in FVarAdd")
	}
	var val float64
	fmt.Sscanf(valStr, "%f", &val)
	env.SetFVar(idx, env.GetFVar(idx)+val)
	return nil
}

func handlePosAdd(ctrl *cns.StateController, env Environment) error {
	var x, y float64
	if xStr, ok := ctrl.Params["x"]; ok {
		fmt.Sscanf(xStr, "%f", &x)
	}
	if yStr, ok := ctrl.Params["y"]; ok {
		fmt.Sscanf(yStr, "%f", &y)
	}
	// PosAdd x is in character-local space (positive = forward).
	// Multiply by Facing to convert to world space, same as VelX.
	env.AddPosition(x*float64(env.GetFacing()), y)
	return nil
}

func handleCtrlSet(ctrl *cns.StateController, env Environment) error {
	if v, ok := ctrl.Params["value"]; ok {
		val, _ := strconv.Atoi(v)
		env.SetCtrl(val != 0)
	}
	return nil
}

func handleStateTypeSet(ctrl *cns.StateController, env Environment) error {
	if v, ok := ctrl.Params["statetype"]; ok {
		if strings.ToUpper(v) != "U" {
			env.SetStateType(v)
		}
	}
	if v, ok := ctrl.Params["movetype"]; ok {
		if strings.ToUpper(v) != "U" {
			env.SetMoveType(v)
		}
	}
	if v, ok := ctrl.Params["physics"]; ok {
		if strings.ToUpper(v) != "U" {
			env.SetPhysics(v)
		}
	}
	return nil
}

func handleLifeAdd(ctrl *cns.StateController, env Environment) error {
	if v, ok := ctrl.Params["value"]; ok {
		val, _ := strconv.Atoi(v)
		env.AddLife(val)
	}
	return nil
}

func handleLifeSet(ctrl *cns.StateController, env Environment) error {
	if v, ok := ctrl.Params["value"]; ok {
		val, _ := strconv.Atoi(v)
		env.SetLife(val)
	}
	return nil
}

func handlePowerAdd(ctrl *cns.StateController, env Environment) error {
	if v, ok := ctrl.Params["value"]; ok {
		val, _ := strconv.Atoi(v)
		env.AddPower(val)
	}
	return nil
}

func handlePowerSet(ctrl *cns.StateController, env Environment) error {
	if v, ok := ctrl.Params["value"]; ok {
		val, _ := strconv.Atoi(v)
		env.SetPower(val)
	}
	return nil
}

func handleAttackMulSet(ctrl *cns.StateController, env Environment) error {
	if v, ok := ctrl.Params["value"]; ok {
		val, _ := strconv.ParseFloat(v, 64)
		env.SetAttackMul(val)
	}
	return nil
}

func handleDefenceMulSet(ctrl *cns.StateController, env Environment) error {
	if v, ok := ctrl.Params["value"]; ok {
		val, _ := strconv.ParseFloat(v, 64)
		env.SetDefenceMul(val)
	}
	return nil
}

func handleSprPriority(ctrl *cns.StateController, env Environment) error {
	if v, ok := ctrl.Params["value"]; ok {
		val, _ := strconv.Atoi(v)
		env.SetSprPriority(val)
	}
	return nil
}

func handleGravity(ctrl *cns.StateController, env Environment) error {
	env.SetGravity(true)
	return nil
}

func handleVelMul(ctrl *cns.StateController, env Environment) error {
	x, y := 1.0, 1.0
	if v, ok := ctrl.Params["x"]; ok {
		if expr, err := evaluator.Parse(v); err == nil {
			x = evaluator.Evaluate(expr, env)
		} else {
			x, _ = strconv.ParseFloat(v, 64)
		}
	}
	if v, ok := ctrl.Params["y"]; ok {
		if expr, err := evaluator.Parse(v); err == nil {
			y = evaluator.Evaluate(expr, env)
		} else {
			y, _ = strconv.ParseFloat(v, 64)
		}
	}
	env.VelMul(x, y)
	return nil
}

func handleHitVelSet(ctrl *cns.StateController, env Environment) error {
	charEnv, ok := env.(evaluator.CharacterEnvironment)
	if !ok {
		return nil
	}

	curX, curY := env.GetVel()
	newX, newY := curX, curY

	if v, ok := ctrl.Params["x"]; ok {
		evalX := 0.0
		if expr, err := evaluator.Parse(v); err == nil {
			evalX = evaluator.Evaluate(expr, env)
		} else {
			evalX, _ = strconv.ParseFloat(v, 64)
		}
		if evalX != 0 {
			newX = charEnv.GetHitVar("xvel") * evalX
		}
	}

	if v, ok := ctrl.Params["y"]; ok {
		evalY := 0.0
		if expr, err := evaluator.Parse(v); err == nil {
			evalY = evaluator.Evaluate(expr, env)
		} else {
			evalY, _ = strconv.ParseFloat(v, 64)
		}
		if evalY != 0 {
			newY = charEnv.GetHitVar("yvel") * evalY
		}
	}

	env.SetVelocity(newX, newY)
	return nil
}

func handleHitFallDamage(ctrl *cns.StateController, env Environment) error {
	charEnv, ok := env.(evaluator.CharacterEnvironment)
	if !ok {
		return nil
	}
	env.AddLife(-int(charEnv.GetHitVar("fall.damage")))
	return nil
}

func handleHitFallSet(ctrl *cns.StateController, env Environment) error {
	val := 0.0
	if v, ok := ctrl.Params["value"]; ok {
		if expr, err := evaluator.Parse(v); err == nil {
			val = evaluator.Evaluate(expr, env)
		} else {
			val, _ = strconv.ParseFloat(v, 64)
		}
	}
	env.SetHitFall(val != 0)
	return nil
}

func handleHitFallVel(ctrl *cns.StateController, env Environment) error {
	env.SetHitFallVel(true)
	return nil
}

func handlePosFreeze(ctrl *cns.StateController, env Environment) error {
	val := 1
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.Atoi(v)
	}
	env.PosFreeze(val != 0)
	return nil
}

func handlePause(ctrl *cns.StateController, env Environment) error {
	// Stub
	return nil
}

func handlePlaySnd(ctrl *cns.StateController, env Environment) error {
	// Stub
	return nil
}

func handleHitDef(ctrl *cns.StateController, env Environment) error {
	env.HitDef(ctrl.Params)
	return nil
}

func handleProjectile(ctrl *cns.StateController, env Environment) error {
	env.Projectile(ctrl.Params)
	return nil
}

func handleAssertSpecial(ctrl *cns.StateController, env Environment) error {
	if v, ok := ctrl.Params["flag"]; ok {
		env.AssertSpecial(v)
	}
	if v, ok := ctrl.Params["flag2"]; ok {
		env.AssertSpecial(v)
	}
	if v, ok := ctrl.Params["flag3"]; ok {
		env.AssertSpecial(v)
	}
	return nil
}

func handleExplod(ctrl *cns.StateController, env Environment) error {
	env.Explod(ctrl.Params)
	return nil
}

func handleRemoveExplod(ctrl *cns.StateController, env Environment) error {
	id := -1
	if v, ok := ctrl.Params["id"]; ok {
		id, _ = strconv.Atoi(v)
	}
	env.RemoveExplod(id)
	return nil
}

func handleHitBy(ctrl *cns.StateController, env Environment) error {
	env.HitBy(ctrl.Params)
	return nil
}

func handleNotHitBy(ctrl *cns.StateController, env Environment) error {
	env.NotHitBy(ctrl.Params)
	return nil
}

func handleAfterImage(ctrl *cns.StateController, env Environment) error {
	env.AfterImage(ctrl.Params)
	return nil
}

func handleAfterImageTime(ctrl *cns.StateController, env Environment) error {
	if v, ok := ctrl.Params["time"]; ok {
		val, _ := strconv.Atoi(v)
		env.AfterImageTime(val)
	}
	return nil
}

func handleEnvShake(ctrl *cns.StateController, env Environment) error {
	env.EnvShake(ctrl.Params)
	return nil
}

func handleEnvColor(ctrl *cns.StateController, env Environment) error {
	env.EnvColor(ctrl.Params)
	return nil
}

func handleScreenBound(ctrl *cns.StateController, env Environment) error {
	env.ScreenBound(ctrl.Params)
	return nil
}

func handleWidth(ctrl *cns.StateController, env Environment) error {
	env.Width(ctrl.Params)
	return nil
}

func handleTrans(ctrl *cns.StateController, env Environment) error {
	env.Trans(ctrl.Params)
	return nil
}

func handleBindToParent(ctrl *cns.StateController, env Environment) error {
	env.BindToParent(ctrl.Params)
	return nil
}

func handleBindToRoot(ctrl *cns.StateController, env Environment) error {
	env.BindToRoot(ctrl.Params)
	return nil
}

func handleBindToTarget(ctrl *cns.StateController, env Environment) error {
	env.BindToTarget(ctrl.Params)
	return nil
}

func handleTargetBind(ctrl *cns.StateController, env Environment) error {
	env.TargetBind(ctrl.Params)
	return nil
}

func handleTargetDrop(ctrl *cns.StateController, env Environment) error {
	id := -1
	if v, ok := ctrl.Params["exclude"]; ok {
		id, _ = strconv.Atoi(v)
	}
	env.TargetDrop(id)
	return nil
}

func handleTargetFacing(ctrl *cns.StateController, env Environment) error {
	id := -1
	if v, ok := ctrl.Params["id"]; ok {
		id, _ = strconv.Atoi(v)
	}
	facing := 0
	if v, ok := ctrl.Params["value"]; ok {
		facing, _ = strconv.Atoi(v)
	}
	env.TargetFacing(id, facing)
	return nil
}

func handleTargetLifeAdd(ctrl *cns.StateController, env Environment) error {
	id := -1
	if v, ok := ctrl.Params["id"]; ok {
		id, _ = strconv.Atoi(v)
	}
	val := 0
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.Atoi(v)
	}
	kill := true
	if v, ok := ctrl.Params["kill"]; ok {
		kill = v != "0"
	}
	env.TargetLifeAdd(id, val, kill)
	return nil
}

func handleTargetPowerAdd(ctrl *cns.StateController, env Environment) error {
	id := -1
	if v, ok := ctrl.Params["id"]; ok {
		id, _ = strconv.Atoi(v)
	}
	val := 0
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.Atoi(v)
	}
	env.TargetPowerAdd(id, val)
	return nil
}

func handleTargetState(ctrl *cns.StateController, env Environment) error {
	id := -1
	if v, ok := ctrl.Params["id"]; ok {
		id, _ = strconv.Atoi(v)
	}
	val := 0
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.Atoi(v)
	}
	env.TargetState(id, val)
	return nil
}

func handleTargetVelAdd(ctrl *cns.StateController, env Environment) error {
	id := -1
	if v, ok := ctrl.Params["id"]; ok {
		id, _ = strconv.Atoi(v)
	}
	var x, y float64
	if xStr, ok := ctrl.Params["x"]; ok {
		x, _ = strconv.ParseFloat(xStr, 64)
	}
	if yStr, ok := ctrl.Params["y"]; ok {
		y, _ = strconv.ParseFloat(yStr, 64)
	}
	env.TargetVelAdd(x, y, id)
	return nil
}

func handleTargetVelSet(ctrl *cns.StateController, env Environment) error {
	id := -1
	if v, ok := ctrl.Params["id"]; ok {
		id, _ = strconv.Atoi(v)
	}
	var x, y float64
	if xStr, ok := ctrl.Params["x"]; ok {
		x, _ = strconv.ParseFloat(xStr, 64)
	}
	if yStr, ok := ctrl.Params["y"]; ok {
		y, _ = strconv.ParseFloat(yStr, 64)
	}
	env.TargetVelSet(x, y, id)
	return nil
}

func handleReversalDef(ctrl *cns.StateController, env Environment) error {
	env.ReversalDef(ctrl.Params)
	return nil
}

func handleMoveHitReset(ctrl *cns.StateController, env Environment) error {
	env.MoveHitReset()
	return nil
}

func handleHitAdd(ctrl *cns.StateController, env Environment) error {
	val := 1
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.Atoi(v)
	}
	env.HitAdd(val)
	return nil
}

func handleChangeAnim2(ctrl *cns.StateController, env Environment) error {
	val := 0
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.Atoi(v)
	}
	env.ChangeAnim2(val)
	return nil
}

func handleSelfState(ctrl *cns.StateController, env Environment) error {
	valStr, ok := ctrl.Params["value"]
	if !ok {
		return fmt.Errorf("missing value in SelfState")
	}

	expr, err := evaluator.Parse(valStr)
	if err != nil {
		return fmt.Errorf("error parsing SelfState value '%s': %v", valStr, err)
	}
	val := int(evaluator.Evaluate(expr, env))

	ctrlFlag := -1
	if cStr, ok := ctrl.Params["ctrl"]; ok {
		if cExpr, err := evaluator.Parse(cStr); err == nil {
			ctrlFlag = int(evaluator.Evaluate(cExpr, env))
		}
	}

	anim := -1
	if aStr, ok := ctrl.Params["anim"]; ok {
		if aExpr, err := evaluator.Parse(aStr); err == nil {
			anim = int(evaluator.Evaluate(aExpr, env))
		}
	}

	env.SelfState(val, ctrlFlag, anim)
	return nil
}

func handlePalFX(ctrl *cns.StateController, env Environment) error {
	env.PalFX(ctrl.Params)
	return nil
}

func handleAllPalFX(ctrl *cns.StateController, env Environment) error {
	env.AllPalFX(ctrl.Params)
	return nil
}

func handleBGPalFX(ctrl *cns.StateController, env Environment) error {
	env.BGPalFX(ctrl.Params)
	return nil
}

func handleRemapPal(ctrl *cns.StateController, env Environment) error {
	env.RemapPal(ctrl.Params)
	return nil
}

func handleAngleAdd(ctrl *cns.StateController, env Environment) error {
	val := 0.0
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.ParseFloat(v, 64)
	}
	env.AngleAdd(val)
	return nil
}

func handleAngleDraw(ctrl *cns.StateController, env Environment) error {
	val := 0.0
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.ParseFloat(v, 64)
	}
	env.AngleDraw(val)
	return nil
}

func handleAngleMul(ctrl *cns.StateController, env Environment) error {
	val := 1.0
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.ParseFloat(v, 64)
	}
	env.AngleMul(val)
	return nil
}

func handleAngleSet(ctrl *cns.StateController, env Environment) error {
	val := 0.0
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.ParseFloat(v, 64)
	}
	env.AngleSet(val)
	return nil
}

func handleOffset(ctrl *cns.StateController, env Environment) error {
	x, y := 0.0, 0.0
	if v, ok := ctrl.Params["x"]; ok {
		x, _ = strconv.ParseFloat(v, 64)
	}
	if v, ok := ctrl.Params["y"]; ok {
		y, _ = strconv.ParseFloat(v, 64)
	}
	env.Offset(x, y)
	return nil
}

func handleSndPan(ctrl *cns.StateController, env Environment) error {
	channel := -1
	if v, ok := ctrl.Params["channel"]; ok {
		channel, _ = strconv.Atoi(v)
	}
	pan := 0
	if v, ok := ctrl.Params["pan"]; ok {
		pan, _ = strconv.Atoi(v)
	}
	env.SndPan(channel, pan)
	return nil
}

func handleStopSnd(ctrl *cns.StateController, env Environment) error {
	channel := -1
	if v, ok := ctrl.Params["channel"]; ok {
		channel, _ = strconv.Atoi(v)
	}
	env.StopSnd(channel)
	return nil
}

func handleAppendToClipboard(ctrl *cns.StateController, env Environment) error {
	if text, ok := ctrl.Params["text"]; ok {
		env.AppendToClipboard(text)
	}
	return nil
}

func handleClearClipboard(ctrl *cns.StateController, env Environment) error {
	env.ClearClipboard()
	return nil
}

func handleDisplayToClipboard(ctrl *cns.StateController, env Environment) error {
	if text, ok := ctrl.Params["text"]; ok {
		env.DisplayToClipboard(text)
	}
	return nil
}

func handleForceFeedback(ctrl *cns.StateController, env Environment) error {
	env.ForceFeedback(ctrl.Params)
	return nil
}

func handleSuperPause(ctrl *cns.StateController, env Environment) error {
	env.SuperPause(ctrl.Params)
	return nil
}

func handleHelper(ctrl *cns.StateController, env Environment) error {
	env.Helper(ctrl.Params)
	return nil
}

func handleDestroySelf(ctrl *cns.StateController, env Environment) error {
	env.DestroySelf()
	return nil
}

func handleFallEnvShake(ctrl *cns.StateController, env Environment) error {
	env.FallEnvShake(ctrl.Params)
	return nil
}

func handleGameMakeAnim(ctrl *cns.StateController, env Environment) error {
	env.GameMakeAnim(ctrl.Params)
	return nil
}

func handleMakeDust(ctrl *cns.StateController, env Environment) error {
	env.MakeDust(ctrl.Params)
	return nil
}

func handleVarRandom(ctrl *cns.StateController, env Environment) error {
	idx := 0
	if v, ok := ctrl.Params["v"]; ok {
		idx, _ = strconv.Atoi(v)
	}
	min, max := 0, 1000
	if r, ok := ctrl.Params["range"]; ok {
		fmt.Sscanf(r, "%d,%d", &min, &max)
	}
	env.VarRandom(idx, min, max)
	return nil
}

func handleVarRangeSet(ctrl *cns.StateController, env Environment) error {
	min, max := 0, 0
	if r, ok := ctrl.Params["range"]; ok {
		fmt.Sscanf(r, "%d,%d", &min, &max)
	}
	val := 0
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.Atoi(v)
	}
	env.VarRangeSet(min, max, val)
	return nil
}

func handleFVarRangeSet(ctrl *cns.StateController, env Environment) error {
	min, max := 0, 0
	if r, ok := ctrl.Params["range"]; ok {
		fmt.Sscanf(r, "%d,%d", &min, &max)
	}
	val := 0.0
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.ParseFloat(v, 64)
	}
	env.FVarRangeSet(min, max, val)
	return nil
}

func handleVictoryQuote(ctrl *cns.StateController, env Environment) error {
	val := 0
	if v, ok := ctrl.Params["value"]; ok {
		val, _ = strconv.Atoi(v)
	}
	env.VictoryQuote(val)
	return nil
}
