package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileHitFallVectors(ctrl *cns.StateController) ([]hitDefCue, error) {
	var cues []hitDefCue
	for _, field := range []struct {
		key          string
		defaultValue float64
		optional     bool
	}{
		{"fall.xvelocity", 0, true},
		{"fall.yvelocity", -4.5, false},
		{"yaccel", 0.35, false},
	} {
		raw, present := ctrl.Params[field.key]
		if !present && field.optional {
			// An omitted X preserves velocity; explicit zero must set it.
			continue
		}
		var expression evaluator.Expression = &evaluator.Number{Value: field.defaultValue, IsFloat: true}
		if present {
			expressions, err := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
			if err != nil {
				return nil, fmt.Errorf("%s: %w", field.key, err)
			}
			expression = expressions[0]
			if err := evaluator.ValidateKnownTriggers(expression); err != nil {
				return nil, fmt.Errorf("%s: %w", field.key, err)
			}
		}
		cues = append(cues, hitDefCue{key: field.key, floating: true, coordinateDefault: !present,
			args: []argument{{argumentSpec: argumentSpec{name: "0", required: true}, expr: expression}}})
	}
	return cues, nil
}
