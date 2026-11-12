package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileHitPalette(ctrl *cns.StateController) ([]hitDefCue, error) {
	var cues []hitDefCue
	for _, field := range []struct {
		key      string
		defaults []float64
	}{
		{"palfx.time", []float64{0}},
		{"palfx.add", []float64{0, 0, 0}},
		{"palfx.mul", []float64{256, 256, 256}},
	} {
		expressions := make([]evaluator.Expression, len(field.defaults))
		for i, value := range field.defaults {
			expressions[i] = &evaluator.Number{Value: value}
		}
		if raw, present := ctrl.Params[field.key]; present {
			parsed, err := evaluator.ParseReferenceTupleForVersion(raw, len(expressions), ctrl.LanguageVersion)
			if err != nil {
				return nil, fmt.Errorf("%s: %w", field.key, err)
			}
			for _, expr := range parsed {
				if integerFieldFloatLiteral(expr) {
					return nil, fmt.Errorf("%s: expected int value and read float", field.key)
				}
				if err := evaluator.ValidateKnownTriggers(expr); err != nil {
					return nil, fmt.Errorf("%s: %w", field.key, err)
				}
			}
			expressions = parsed
		}
		cues = append(cues, hitIntegerCue(field.key, expressions))
	}
	return cues, nil
}

func hitPaletteKey(key string) bool {
	return key == "palfx.time" || key == "palfx.add" || key == "palfx.mul"
}
