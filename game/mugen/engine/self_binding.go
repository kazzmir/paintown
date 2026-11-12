package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type SelfBindParameters struct {
	Time, Facing, CoordinateWidth int
	X, Y                          float64
}

func ExecuteSelfBindParameters(params map[string]string, env Environment, version string, root bool) error {
	kind := "BindToParent"
	if root {
		kind = "BindToRoot"
	}
	ctrl := &cns.StateController{Type: kind, Params: params, LanguageVersion: version}
	fn, _, err := compileSelfBind(ctrl, root)
	if err != nil {
		return err
	}
	return fn(ctrl, env)
}

func compileSelfBind(ctrl *cns.StateController, root bool) (ControllerFunc, bool, error) {
	args, err := compileArguments(ctrl, []argumentSpec{{"time", true, false}, {"facing", true, false}})
	if err != nil {
		return nil, true, err
	}
	if raw, present := ctrl.Params["pos"]; present {
		expressions, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
		if err != nil {
			if single, e := evaluator.ParseReferenceTupleForVersion(strings.TrimSpace(strings.TrimSuffix(strings.TrimSpace(raw), ",")), 1, ctrl.LanguageVersion); e == nil {
				expressions, err = append(single, &evaluator.Number{Value: 0}), nil
			}
		}
		if err != nil {
			return nil, true, fmt.Errorf("pos: %w", err)
		}
		for i, expr := range expressions {
			args = append(args, argument{argumentSpec{fmt.Sprintf("pos%d", i), false, true}, expr})
		}
	}
	for _, arg := range args {
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
		if arg.integer && integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		context, ok := env.(interface {
			BindSelf(bool, func() (SelfBindParameters, error)) error
		})
		if !ok {
			return fmt.Errorf("%s context is unavailable", ctrl.Type)
		}
		// Nonhelpers are native no-ops, including parameter evaluation.
		return context.BindSelf(root, func() (SelfBindParameters, error) {
			v, err := evaluateArgumentsWithBottom(args, env, true)
			if err != nil {
				return SelfBindParameters{}, err
			}
			p := SelfBindParameters{Time: 1, X: v["pos0"].Number(), Y: v["pos1"].Number()}
			if value, present := v["time"]; present {
				p.Time = int(value.Number())
			}
			if value, present := v["facing"]; present {
				p.Facing = int(value.Number())
			}
			if profile, ok := env.(interface{ LocalCoordinateWidth() int }); ok {
				p.CoordinateWidth = profile.LocalCoordinateWidth()
			}
			return p, nil
		})
	}, true, nil
}
