package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type dustEnvironment interface {
	DustClock() (int32, error)
	DustPositionAxis(int, float64) (float64, error)
	CreateDust([2]float64) error
}

func compileDust(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	var spacing []argument
	var positions [2][]argument
	_, second := ctrl.Params["pos2"]
	for _, key := range []string{"spacing", "pos", "pos2"} {
		raw, present := ctrl.Params[key]
		if !present {
			continue
		}
		count := 2
		if key == "spacing" {
			count = 1
		}
		expressions, err := evaluator.ParseReferenceTupleForVersion(raw, count, ctrl.LanguageVersion)
		if err != nil && key != "spacing" {
			// One axis may have a trailing comma; two axes may not. Parse
			// expressions normally to preserve function and redirect commas.
			single := strings.TrimSuffix(strings.TrimSpace(raw), ",")
			expressions, err = evaluator.ParseReferenceTupleForVersion(single, 1, ctrl.LanguageVersion)
		}
		if err != nil {
			return nil, true, fmt.Errorf("%s: %w", key, err)
		}
		for axis, expr := range expressions {
			integer := key != "pos2"
			if integer && integerFieldFloatLiteral(expr) {
				return nil, true, fmt.Errorf("%s: expected int value and read float", key)
			}
			if _, quoted := expr.(*evaluator.StringExpr); quoted {
				return nil, true, fmt.Errorf("%s: expected numeric expression", key)
			}
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			arg := argument{argumentSpec{fmt.Sprintf("%s%d", key, axis), integer, false}, expr}
			if key == "spacing" {
				spacing = append(spacing, arg)
			} else {
				index := 0
				if key == "pos2" {
					index = 1
				}
				positions[index] = append(positions[index], arg)
			}
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		context, ok := env.(dustEnvironment)
		if !ok {
			return fmt.Errorf("MakeDust entity/effect context is unavailable")
		}
		values, err := evaluateArgumentsWithBottom(spacing, env, true)
		if err != nil {
			return err
		}
		interval := 3
		if value, present := values["spacing0"]; present {
			interval = int(value.Number())
		}
		if interval < 1 {
			if warnings, ok := env.(interface{ EvaluationWarning(string) }); ok {
				warnings.EvaluationWarning("MakeDust spacing below 1 uses 1")
			}
			interval = 1
		}
		clock, err := context.DustClock()
		if err != nil {
			return err
		}
		if clock%int32(interval) != 0 {
			return nil
		}
		for index, axes := range positions {
			if index == 1 && !second {
				break
			}
			var position [2]float64
			for axis := range position {
				value := 0.0
				if axis < len(axes) {
					arg := axes[axis]
					values, err := evaluateArgumentsWithBottom([]argument{arg}, env, true)
					if err != nil {
						return err
					}
					value = values[arg.name].Number()
				}
				// Each expression precedes its own jitter draw.
				position[axis], err = context.DustPositionAxis(axis, value)
				if err != nil {
					return err
				}
			}
			// Failed admission is a no-op; the second cloud still evaluates.
			if err := context.CreateDust(position); err != nil {
				return err
			}
		}
		return nil
	}, true, nil
}

func ExecuteDust(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "MakeDust", Params: params, LanguageVersion: version}
	handler, _, err := compileDust(ctrl)
	if err != nil {
		return err
	}
	return handler(ctrl, env)
}
