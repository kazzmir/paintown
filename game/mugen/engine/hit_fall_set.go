package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileHitFallSet(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	var args []argument
	// The beta ignores the documented xvel/yvel fields, including their
	// expression side effects and malformed input. Only value is compiled.
	for _, field := range []argumentSpec{{"value", true, false}} {
		raw, present := ctrl.Params[field.name]
		if !present {
			continue
		}
		expressions, err := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
		if err != nil {
			return nil, true, fmt.Errorf("%s: %w", field.name, err)
		}
		expr := expressions[0]
		if field.integer && integerFieldFloatLiteral(expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", field.name)
		}
		if err := evaluator.ValidateKnownTriggers(expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", field.name, err)
		}
		args = append(args, argument{argumentSpec: field, expr: expr})
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		value := int32(-1)
		if supplied, present := values["value"]; present {
			value = int32(supplied.Number())
		}
		context, ok := env.(interface{ SetHitFallValue(int32) })
		if !ok {
			return fmt.Errorf("HitFallSet context is unavailable")
		}
		context.SetHitFallValue(value)
		return nil
	}, true, nil
}
