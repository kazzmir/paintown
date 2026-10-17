package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileHitPauses(ctrl *cns.StateController) ([]hitDefCue, error) {
	read := func(key string, fallback []evaluator.Expression) ([]evaluator.Expression, error) {
		raw, present := ctrl.Params[key]
		if !present {
			return append([]evaluator.Expression(nil), fallback...), nil
		}
		raw = strings.TrimSpace(raw)
		expressions, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
		if err != nil {
			// The beta permits a first component alone, including its trailing
			// comma. The omitted second component retains its source default.
			expressions, err = evaluator.ParseReferenceTupleForVersion(strings.TrimSuffix(raw, ","), 1, ctrl.LanguageVersion)
		}
		if err != nil {
			return nil, fmt.Errorf("%s: %w", key, err)
		}
		for _, expression := range expressions {
			if integerFieldFloatLiteral(expression) {
				return nil, fmt.Errorf("%s: expected int value and read float", key)
			}
			if err := evaluator.ValidateKnownTriggers(expression); err != nil {
				return nil, fmt.Errorf("%s: %w", key, err)
			}
		}
		return append(expressions, fallback[len(expressions):]...), nil
	}
	pause, err := read("pausetime", []evaluator.Expression{&evaluator.Number{}, &evaluator.Number{}})
	if err != nil {
		return nil, err
	}
	// Retain source expressions: both omitted guard components execute again
	// at guard.pausetime's evaluation point, rather than copying stored ints.
	guard, err := read("guard.pausetime", pause)
	if err != nil {
		return nil, err
	}
	cues := []hitDefCue{hitIntegerCue("pausetime", pause), hitIntegerCue("guard.pausetime", guard)}
	for i := range cues {
		// Both full-order native programs observe Y before X for these
		// integer pairs, independently of source declaration order.
		cues[i].args[0], cues[i].args[1] = cues[i].args[1], cues[i].args[0]
	}
	return cues, nil
}
