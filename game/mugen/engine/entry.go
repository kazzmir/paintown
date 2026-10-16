package engine

import (
	"fmt"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

type EntryOverrides struct {
	HasCtrl bool
	Ctrl    bool
	HasAnim bool
	Anim    int
}
type EntryEnvironment interface {
	Environment
	BeginStateEntry(hitdefPersist, movehitPersist, hitcountPersist bool)
	SetJuggleCost(int)
	FaceOpponent()
}
type entryProgram struct {
	stateType, moveType, physics string
	values                       map[string][]evaluator.Expression
}

var entryOrder = []string{"anim", "velset", "ctrl", "poweradd", "juggle", "facep2", "hitdefpersist", "movehitpersist", "hitcountpersist", "sprpriority"}

func compileEntry(state *cns.StateDef) (entryProgram, error) {
	program := entryProgram{stateType: strings.ToUpper(state.Type), moveType: strings.ToUpper(state.MoveType), physics: strings.ToUpper(state.Physics), values: make(map[string][]evaluator.Expression)}
	if program.stateType == "" {
		program.stateType = "S"
	}
	if program.moveType == "" {
		program.moveType = "I"
	}
	if program.physics == "" {
		program.physics = "N"
	}
	for _, field := range []struct{ name, value, allowed string }{{"type", program.stateType, "SCALU"}, {"movetype", program.moveType, "IAHU"}, {"physics", program.physics, "SCANU"}} {
		if len(field.value) != 1 || !strings.Contains(field.allowed, field.value) {
			return program, fmt.Errorf("invalid %s %q", field.name, field.value)
		}
	}
	if state.ID < 0 {
		return program, nil
	}
	raw := make(map[string]string)
	for key, value := range state.Parameters {
		raw[key] = value
	}
	// Support callers constructing legacy StateDef values without source text.
	if state.Parameters == nil {
		if state.Anim != -1 {
			raw["anim"] = strconv.Itoa(state.Anim)
		}
		if state.Ctrl != -1 {
			raw["ctrl"] = strconv.Itoa(state.Ctrl)
		}
		if state.SprPriority != -255 {
			raw["sprpriority"] = strconv.Itoa(state.SprPriority)
		}
		if state.PowerAdd != 0 {
			raw["poweradd"] = strconv.Itoa(state.PowerAdd)
		}
		if state.Juggle != 0 {
			raw["juggle"] = strconv.Itoa(state.Juggle)
		}
		if state.FaceP2 != 0 {
			raw["facep2"] = strconv.Itoa(state.FaceP2)
		}
		if len(state.VelSet) > 0 {
			var parts []string
			for _, v := range state.VelSet {
				parts = append(parts, strconv.FormatFloat(v, 'f', -1, 64))
			}
			raw["velset"] = strings.Join(parts, ",")
		}
	}
	for _, name := range entryOrder {
		value, found := raw[name]
		if !found {
			continue
		}
		parts := parsers.SplitList(value)
		if len(parts) > 1 && name != "velset" || len(parts) > 2 {
			return program, fmt.Errorf("%s has invalid component count", name)
		}
		for _, part := range parts {
			expr, err := evaluator.ParseReference(part)
			if err != nil {
				return program, fmt.Errorf("%s = %s: %w", name, value, err)
			}
			program.values[name] = append(program.values[name], expr)
		}
	}
	return program, nil
}

// EnterState is the only entry-attribute path. ExecuteState never reapplies it
// merely because Time is still zero (including hitpause and same-tick passes).
func (sm *StateMachine) EnterState(state *cns.StateDef, env EntryEnvironment, overrides EntryOverrides) error {
	compiled, err := sm.compileState(state)
	if err != nil {
		return err
	}
	if state.ID < 0 {
		return fmt.Errorf("cannot enter special state %d", state.ID)
	}
	program := compiled.entry
	values := make(map[string][]evaluator.Value)
	for _, name := range entryOrder {
		for _, expr := range program.values[name] {
			value := evaluator.EvaluateValue(expr, env)
			if !value.IsNumber() {
				return fmt.Errorf("state %d entry %s = %s evaluated to %v", state.ID, name, expr.String(), value)
			}
			if name != "velset" {
				n, err := evaluator.CoerceInt(value, env, "entry "+name)
				if err != nil {
					return fmt.Errorf("state %d: %w", state.ID, err)
				}
				value = evaluator.Int(n)
			}
			values[name] = append(values[name], value)
		}
	}
	flag := func(name string) bool { return len(values[name]) > 0 && values[name][0].Bool() }
	env.BeginStateEntry(flag("hitdefpersist"), flag("movehitpersist"), flag("hitcountpersist"))
	if program.stateType != "U" {
		env.SetStateType(program.stateType)
	}
	if program.moveType != "U" {
		env.SetMoveType(program.moveType)
	}
	if program.physics != "U" {
		env.SetPhysics(program.physics)
	}
	// Establish facing before applying a new local velocity.
	if flag("facep2") {
		env.FaceOpponent()
	}
	for _, name := range entryOrder {
		v := values[name]
		if len(v) == 0 {
			continue
		}
		n := int(v[0].Number())
		switch name {
		case "anim":
			if !overrides.HasAnim {
				env.ChangeAnim(n)
			}
		case "ctrl":
			if !overrides.HasCtrl {
				env.SetCtrl(v[0].Bool())
			}
		case "velset":
			y := 0.0
			if len(v) > 1 {
				y = v[1].Number()
			}
			env.SetVelocity(v[0].Number(), y)
		case "poweradd":
			env.AddPower(n)
		case "juggle":
			env.SetJuggleCost(n)
		case "sprpriority":
			env.SetSprPriority(n)
		}
	}
	if overrides.HasCtrl {
		env.SetCtrl(overrides.Ctrl)
	}
	if overrides.HasAnim {
		env.ChangeAnim(overrides.Anim)
	}
	return nil
}
