package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileHitFall(ctrl *cns.StateController) ([]hitDefCue, error) {
	values := map[string]evaluator.Expression{}
	var cues []hitDefCue
	for _, field := range []struct {
		key, fallback string
		defaultValue  float64
	}{
		{"fall", "", 0}, {"air.fall", "fall", 0},
		{"fall.recover", "", 1}, {"fall.recovertime", "", 4},
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
		// air.fall inherits the source, executing its side effects again.
		values[field.key] = expression
		cues = append(cues, hitIntegerCue(field.key, []evaluator.Expression{expression}))
	}
	return cues, nil
}

func hitFallKey(key string) bool {
	switch key {
	case "fall", "air.fall", "fall.recover", "fall.recovertime":
		return true
	}
	return false
}
