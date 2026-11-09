package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type TargetDropParameters struct {
	ExcludeID int
	KeepOne   bool
}
type TargetLifeParameters struct {
	ID, Value      int
	Kill, Absolute bool
}
type TargetVelocityParameters struct {
	ID              int
	X, Y            float64
	HasX, HasY, Add bool
}

// Admission precedes expressions: no targets means no parameter evaluation;
// an unmatched ID suppresses the remaining arguments. Keep ID first in specs.
func evaluateTargetArguments(args []argument, env Environment, zeroBottom bool) (map[string]evaluator.Value, error) {
	context, selectsTargets := env.(interface{ HasTargets(int) bool })
	if selectsTargets && !context.HasTargets(-1) {
		return nil, nil
	}
	if selectsTargets && len(args) > 0 && args[0].name == "id" {
		selection, err := evaluateArgumentsWithBottom(args[:1], env, zeroBottom)
		if err != nil {
			return nil, err
		}
		if !context.HasTargets(int(selection["id"].Number())) {
			return nil, nil
		}
		values, err := evaluateArgumentsWithBottom(args[1:], env, zeroBottom)
		if err != nil {
			return nil, err
		}
		values["id"] = selection["id"]
		return values, nil
	}
	return evaluateArgumentsWithBottom(args, env, zeroBottom)
}

func targetHandler(ctrl *cns.StateController, specs []argumentSpec, apply func(Environment, map[string]evaluator.Value) error) (ControllerFunc, bool, error) {
	args, err := compileArguments(ctrl, specs)
	if err != nil {
		return nil, true, err
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateTargetArguments(args, env, true)
		if err != nil || values == nil {
			return err
		}
		return apply(env, values)
	}, true, nil
}

func compileTarget(ctrl *cns.StateController, name string) (ControllerFunc, bool, error) {
	idOf := func(v map[string]evaluator.Value) int {
		if value, ok := v["id"]; ok {
			return int(value.Number())
		}
		return -1
	}
	switch name {
	case "targetdrop":
		return targetHandler(ctrl, []argumentSpec{{"excludeid", true, false}, {"keepone", true, false}}, func(env Environment, v map[string]evaluator.Value) error {
			context, ok := env.(interface {
				DropTargets(TargetDropParameters) error
			})
			if !ok {
				return fmt.Errorf("TargetDrop context is unavailable")
			}
			p := TargetDropParameters{ExcludeID: -1, KeepOne: true}
			if value, ok := v["excludeid"]; ok {
				p.ExcludeID = int(value.Number())
			}
			if value, ok := v["keepone"]; ok {
				p.KeepOne = value.Bool()
			}
			return context.DropTargets(p)
		})
	case "targetfacing", "targetpoweradd", "targetstate":
		return targetHandler(ctrl, []argumentSpec{{"id", true, false}, {"value", true, name != "targetfacing"}}, func(env Environment, v map[string]evaluator.Value) error {
			id, value := idOf(v), int(v["value"].Number())
			if name == "targetfacing" {
				env.TargetFacing(id, value)
			} else if name == "targetstate" {
				env.TargetState(id, value)
			} else {
				env.TargetPowerAdd(id, value)
			}
			return nil
		})
	case "targetlifeadd":
		return targetHandler(ctrl, []argumentSpec{{"id", true, false}, {"value", true, true}, {"kill", true, false}, {"absolute", true, false}}, func(env Environment, v map[string]evaluator.Value) error {
			context, ok := env.(interface {
				AddTargetLife(TargetLifeParameters) error
			})
			if !ok {
				return fmt.Errorf("TargetLifeAdd context is unavailable")
			}
			p := TargetLifeParameters{ID: idOf(v), Value: int(v["value"].Number()), Kill: true}
			if value, ok := v["kill"]; ok {
				p.Kill = value.Bool()
			}
			if value, ok := v["absolute"]; ok {
				p.Absolute = value.Bool()
			}
			return context.AddTargetLife(p)
		})
	case "targetvelset", "targetveladd":
		return targetHandler(ctrl, []argumentSpec{{"id", true, false}, {"x", false, false}, {"y", false, false}}, func(env Environment, v map[string]evaluator.Value) error {
			context, ok := env.(interface {
				SetTargetVelocity(TargetVelocityParameters) error
			})
			if !ok {
				return fmt.Errorf("Target velocity context is unavailable")
			}
			x, hasX := v["x"]
			y, hasY := v["y"]
			return context.SetTargetVelocity(TargetVelocityParameters{ID: idOf(v), X: x.Number(), Y: y.Number(), HasX: hasX, HasY: hasY, Add: name == "targetveladd"})
		})
	}
	return nil, false, nil
}
