package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// Inherited times execute the original source again at each field's native
// evaluation point. They cannot copy the previously evaluated/coerced integer.
func compileHitTiming(ctrl *cns.StateController) ([]hitDefCue, error) {
	values := map[string]evaluator.Expression{}
	var cues []hitDefCue
	for _, field := range []struct {
		key, fallback string
		defaultValue  float64
	}{
		{"ground.hittime", "", 0}, {"ground.slidetime", "", 0},
		{"guard.hittime", "ground.hittime", 0}, {"guard.slidetime", "ground.slidetime", 0},
		{"guard.ctrltime", "guard.slidetime", 0}, {"airguard.ctrltime", "guard.ctrltime", 0},
		{"air.hittime", "", 20}, {"down.hittime", "", 20},
	} {
		expression := values[field.fallback]
		if expression == nil {
			expression = &evaluator.Number{Value: field.defaultValue}
		}
		if raw, present := ctrl.Params[field.key]; present {
			expressions, err := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
			if err != nil {
				return nil, fmt.Errorf("%s: %w", field.key, err)
			}
			expression = expressions[0]
			if integerFieldFloatLiteral(expression) {
				return nil, fmt.Errorf("%s: expected int value and read float", field.key)
			}
			if err := evaluator.ValidateKnownTriggers(expression); err != nil {
				return nil, fmt.Errorf("%s: %w", field.key, err)
			}
		}
		values[field.key] = expression
		cues = append(cues, hitIntegerCue(field.key, []evaluator.Expression{expression}))
	}
	return cues, nil
}

func hitTimingKey(key string) bool {
	switch key {
	case "ground.hittime", "ground.slidetime", "guard.hittime", "guard.slidetime", "guard.ctrltime", "airguard.ctrltime", "air.hittime", "down.hittime":
		return true
	}
	return false
}
