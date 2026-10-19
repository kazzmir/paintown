package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type ScreenBoundParameters struct {
	Bound, MoveX, MoveY bool
}

func ExecuteScreenBoundParameters(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "ScreenBound", Params: params, LanguageVersion: version}
	fn, _, err := compileScreenBound(ctrl)
	if err != nil {
		return err
	}
	return fn(ctrl, env)
}

func compileScreenBound(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	args, err := compileArguments(ctrl, []argumentSpec{{"value", true, false}})
	if err != nil {
		return nil, true, err
	}
	if raw, present := ctrl.Params["movecamera"]; present {
		expressions, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
		if err != nil {
			if single, singleErr := evaluator.ParseReferenceTupleForVersion(strings.TrimSpace(strings.TrimSuffix(strings.TrimSpace(raw), ",")), 1, ctrl.LanguageVersion); singleErr == nil {
				expressions, err = append(single, &evaluator.Number{Value: 0}), nil
			}
		}
		if err != nil {
			return nil, true, fmt.Errorf("movecamera: %w", err)
		}
		for index, expression := range expressions {
			args = append(args, argument{argumentSpec{fmt.Sprintf("movecamera%d", index), true, false}, expression})
		}
	}
	for _, arg := range args {
		if integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		context, ok := env.(interface {
			SetScreenBound(ScreenBoundParameters) error
		})
		if !ok {
			return fmt.Errorf("ScreenBound context is unavailable")
		}
		return context.SetScreenBound(ScreenBoundParameters{Bound: values["value"].Bool(), MoveX: values["movecamera0"].Bool(), MoveY: values["movecamera1"].Bool()})
	}, true, nil
}
