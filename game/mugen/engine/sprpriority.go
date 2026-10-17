package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileSprPriority(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	args, err := compileArguments(ctrl, []argumentSpec{{"value", true, true}})
	if err != nil {
		return nil, true, err
	}
	if integerFieldFloatLiteral(args[0].expr) {
		return nil, true, fmt.Errorf("value: expected int value and read float")
	}
	if err := evaluator.ValidateKnownTriggers(args[0].expr); err != nil {
		return nil, true, fmt.Errorf("value: %w", err)
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		// Native accepts the measured +/-99 values beyond the documented
		// -5..5 range. Preserve ordering rather than clamping them.
		env.SetSprPriority(int(values["value"].Number()))
		return nil
	}, true, nil
}
