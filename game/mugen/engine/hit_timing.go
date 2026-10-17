package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// Inherited times execute the original source again at each field's native
// evaluation point. They cannot copy the previously evaluated/coerced integer.
func compileHitGroundTiming(ctrl *cns.StateController) ([]hitDefCue, error) {
	values := map[string]evaluator.Expression{}
	var cues []hitDefCue
	for _, field := range []struct{ key, fallback string }{
		{"ground.hittime", ""}, {"ground.slidetime", ""},
		{"guard.hittime", "ground.hittime"}, {"guard.slidetime", "ground.slidetime"},
		{"guard.ctrltime", "guard.slidetime"}, {"airguard.ctrltime", "guard.ctrltime"},
	} {
		expression := values[field.fallback]
		if expression == nil {
			expression = &evaluator.Number{}
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

func hitGroundTimingKey(key string) bool {
	switch key {
	case "ground.hittime", "ground.slidetime", "guard.hittime", "guard.slidetime", "guard.ctrltime", "airguard.ctrltime":
		return true
	}
	return false
}
