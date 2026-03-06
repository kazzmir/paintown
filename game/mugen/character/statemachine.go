package character

import (
	"fmt"
	"math"
	"regexp"
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
	GetCtrl() bool
	ChangeState(stateNo int, ctrl int, anim int) // ctrl/anim -1 means no change
	ChangeAnim(animNo int)
	Command(name string) bool
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
	sm.controllers["selfstate"] = handleChangeState // SelfState is same as ChangeState for now unless we handle helper/parent logic
	sm.controllers["changeanim"] = handleChangeAnim
	sm.controllers["velset"] = handleVelSet
	sm.controllers["veladd"] = handleVelAdd
	sm.controllers["posset"] = handlePosSet
	sm.controllers["posadd"] = handlePosAdd
	sm.controllers["varset"] = handleVarSet
	sm.controllers["varadd"] = handleVarAdd
	sm.controllers["fvarset"] = handleFVarSet
	sm.controllers["fvaradd"] = handleFVarAdd
}

// ExecuteState executes all valid controllers within a given state definition.
// Returns true if a state transition (ChangeState/SelfState) occurred.
func (sm *StateMachine) ExecuteState(state *cns.StateDef, env Environment) (bool, error) {
	for i := range state.Controllers {
		ctrl := &state.Controllers[i]
		matched := sm.evaluateTriggers(ctrl, env)

		if matched {
			fmt.Printf("Tick %d: Activating %s (%s) in state %d\n", env.GetTime(), ctrl.Type, ctrl.Name, env.GetStateNo())
			handler, ok := sm.controllers[strings.ToLower(ctrl.Type)]
			if !ok {
				// Unsupported controller type
				continue
			}
			err := handler(ctrl, env)
			if err != nil {
				return false, fmt.Errorf("error executing %s: %v", ctrl.Type, err)
			}

			// If the controller type is ChangeState or SelfState, we aborted the rest of the controllers
			// in this state block in the C++ version.
			lowerType := strings.ToLower(ctrl.Type)
			if lowerType == "changestate" || lowerType == "selfstate" {
				return true, nil
			}
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
		switch strings.ToLower(name) {
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
		case "movehit":
			return 0 // Placeholder
		case "movecontact":
			return 0 // Placeholder
		case "power":
			return 1000 // Placeholder
		case "s": // Standing / Stand physics
			return 1
		case "c": // Crouching / Crouch physics
			return 2
		case "a": // Aerial / Attack movetype
			return 3
		case "l": // Lying
			return 4
		case "i": // Idle movetype
			return 5
		case "h": // Hit movetype
			return 6
		case "n": // None physics
			return 7
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

		parts := strings.SplitN(trigStr, "=", 2)
		if len(parts) < 2 {
			continue
		}
		key := strings.ToLower(strings.TrimSpace(parts[0]))
		val := strings.TrimSpace(parts[1])

		expr, ok := sm.cache[val]
		if !ok {
			var err error
			expr, err = evaluator.Parse(val)
			if err != nil {
				continue
			}
			sm.cache[val] = expr
		}

		var groupID int
		if strings.HasPrefix(key, "triggerall") {
			groupID = -1
		} else if strings.HasPrefix(key, "trigger") {
			idStr := strings.TrimPrefix(key, "trigger")
			fmt.Sscanf(idStr, "%d", &groupID)
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
				return false
			}
		}
	}

	// 2. At least one group must be true.
	// A group is true if ALL its expressions are true.
	hasNumberedGroup := false
	for _, id := range groupIDs {
		if id <= 0 {
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
	var val int
	fmt.Sscanf(valStr, "%d", &val)

	ctrlFlag := -1
	if cStr, ok := ctrl.Params["ctrl"]; ok {
		fmt.Sscanf(cStr, "%d", &ctrlFlag)
	}

	animOverride := -1
	if aStr, ok := ctrl.Params["anim"]; ok {
		fmt.Sscanf(aStr, "%d", &animOverride)
	}

	env.ChangeState(val, ctrlFlag, animOverride)
	return nil
}

func handleChangeAnim(ctrl *cns.StateController, env Environment) error {
	valStr, ok := ctrl.Params["value"]
	if !ok {
		return fmt.Errorf("missing value in ChangeAnim")
	}
	var val int
	fmt.Sscanf(valStr, "%d", &val)
	env.ChangeAnim(val)
	return nil
}

func handleVelSet(ctrl *cns.StateController, env Environment) error {
	if xStr, ok := ctrl.Params["x"]; ok {
		var x float64
		fmt.Sscanf(xStr, "%f", &x)
		env.SetVelocity(x, 0) // Simplify for stub: y is unaffected unless specified, but we don't have GetVelocity yet
	}
	if yStr, ok := ctrl.Params["y"]; ok {
		var y float64
		fmt.Sscanf(yStr, "%f", &y)
		env.SetVelocity(0, y) // Simplify for stub
	}
	// For actual VelSet, X and Y can be specified independently while preserving the other.
	return nil
}

func handleVelAdd(ctrl *cns.StateController, env Environment) error {
	var x, y float64
	if xStr, ok := ctrl.Params["x"]; ok {
		fmt.Sscanf(xStr, "%f", &x)
	}
	if yStr, ok := ctrl.Params["y"]; ok {
		fmt.Sscanf(yStr, "%f", &y)
	}
	env.AddVelocity(x, y)
	return nil
}

func handlePosSet(ctrl *cns.StateController, env Environment) error {
	var x, y float64
	if xStr, ok := ctrl.Params["x"]; ok {
		fmt.Sscanf(xStr, "%f", &x)
		env.SetPosition(x, 0)
	}
	if yStr, ok := ctrl.Params["y"]; ok {
		fmt.Sscanf(yStr, "%f", &y)
		env.SetPosition(0, y)
	}
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
		reKey := regexp.MustCompile(`var\((\d+)\)`)
		reVal := regexp.MustCompile(`\((\d+)\)`)
		for k, v := range ctrl.Params {
			k = strings.ToLower(k)
			if match := reKey.FindStringSubmatch(k); match != nil {
				fmt.Sscanf(match[1], "%d", &idx)
				valStr = v
				found = true
				break
			}
			if k == "var" {
				if match := reVal.FindStringSubmatch(v); match != nil {
					fmt.Sscanf(match[1], "%d", &idx)
					parts := strings.SplitN(v, "=", 2)
					if len(parts) == 2 {
						valStr = strings.TrimSpace(parts[1])
						found = true
						break
					}
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
	env.AddPosition(x, y)
	return nil
}
