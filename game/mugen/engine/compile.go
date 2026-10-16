package engine

import (
	"fmt"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type triggerProgram struct {
	all    []evaluator.Expression
	groups [][]evaluator.Expression
}
type compiledController struct {
	source   *cns.StateController
	handler  ControllerFunc
	triggers triggerProgram
}
type stateProgram struct {
	controllers []compiledController
	entry       entryProgram
	err         error
}

// CompileState validates immutable state/controller sources without evaluating
// them or touching a character. Compiled programs are shared safely across hosts.
func (sm *StateMachine) CompileState(state *cns.StateDef) error {
	_, err := sm.compileState(state)
	return err
}

func (sm *StateMachine) compileState(state *cns.StateDef) (*stateProgram, error) {
	if state == nil {
		return nil, fmt.Errorf("nil state definition")
	}
	sm.compileMu.Lock()
	defer sm.compileMu.Unlock()
	if program, ok := sm.programs[state]; ok {
		return program, program.err
	}
	program := &stateProgram{}
	sm.programs[state] = program
	if len(state.Controllers) == 0 {
		program.err = fmt.Errorf("state %d (line %d) has no controllers", state.ID, state.Line)
		return program, program.err
	}
	var entryErr error
	program.entry, entryErr = compileEntry(state)
	if entryErr != nil {
		program.err = fmt.Errorf("state %d entry (line %d): %w", state.ID, state.Line, entryErr)
		return program, program.err
	}
	for i := range state.Controllers {
		ctrl := &state.Controllers[i]
		handler, ok := sm.controllers[strings.ToLower(ctrl.Type)]
		var triggers triggerProgram
		var err error
		if !ok {
			err = fmt.Errorf("unsupported controller type %q", ctrl.Type)
		} else {
			err = validateUniversal(ctrl)
			if err == nil {
				triggers, err = sm.compileTriggers(ctrl)
			}
		}
		if err != nil {
			program.err = controllerError(state, i, ctrl, err)
			return program, program.err
		}
		program.controllers = append(program.controllers, compiledController{source: ctrl, handler: handler, triggers: triggers})
	}
	return program, nil
}

func validateUniversal(ctrl *cns.StateController) error {
	for _, field := range []struct{ key, raw string }{{"persistent", ctrl.PersistentSource}, {"ignorehitpause", ctrl.IgnoreHitPauseSource}} {
		key, raw := field.key, field.raw
		if raw == "" {
			continue
		}
		n, err := strconv.ParseInt(strings.TrimSpace(raw), 10, 32)
		if err != nil || key == "persistent" && n < 0 {
			return fmt.Errorf("%s must be an integer literal: %q", key, raw)
		}
	}
	if ctrl.Persistent < 0 {
		return fmt.Errorf("negative persistent value")
	}
	return nil
}

func controllerError(state *cns.StateDef, index int, ctrl *cns.StateController, err error) error {
	return fmt.Errorf("state %d controller %d %q (%s, line %d): %w", state.ID, index, ctrl.Name, ctrl.Type, ctrl.Line, err)
}

func (sm *StateMachine) compileTriggers(ctrl *cns.StateController) (triggerProgram, error) {
	program := triggerProgram{}
	groups := make(map[int][]evaluator.Expression)
	for _, source := range ctrl.Triggers {
		key, raw, ok := strings.Cut(source, "=")
		if !ok {
			return program, fmt.Errorf("invalid trigger %q", source)
		}
		key = strings.ToLower(strings.TrimSpace(key))
		raw = strings.TrimSpace(raw)
		index := -1
		if key != "triggerall" {
			if !strings.HasPrefix(key, "trigger") {
				return program, fmt.Errorf("invalid trigger key %q", key)
			}
			n, err := strconv.Atoi(strings.TrimPrefix(key, "trigger"))
			if err != nil || n < 1 {
				return program, fmt.Errorf("invalid trigger key %q", key)
			}
			index = n
		}
		expr, found := sm.cache[raw]
		if !found {
			var err error
			expr, err = evaluator.Parse(raw)
			if err != nil {
				return program, fmt.Errorf("%s = %s: %w", key, raw, err)
			}
			sm.cache[raw] = expr
		}
		if index < 0 {
			program.all = append(program.all, expr)
		} else {
			groups[index] = append(groups[index], expr)
		}
	}
	if len(groups[1]) == 0 {
		return program, fmt.Errorf("controller requires trigger1")
	}
	for i := 1; len(groups[i]) > 0; i++ {
		program.groups = append(program.groups, groups[i])
	}
	return program, nil
}

func (p *triggerProgram) match(env Environment) bool {
	for _, expr := range p.all {
		if !evaluator.EvaluateValue(expr, env).Bool() {
			return false
		}
	}
	for _, group := range p.groups {
		matched := true
		for _, expr := range group {
			if !evaluator.EvaluateValue(expr, env).Bool() {
				matched = false
				break
			}
		}
		if matched {
			return true
		}
	}
	return false
}
