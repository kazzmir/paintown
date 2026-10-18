package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileHitShake(ctrl *cns.StateController, prefix string) ([]hitDefCue, error) {
	var cues []hitDefCue
	for _, field := range []struct {
		name          string
		fallback      float64
		floating      bool
		strictInteger bool
		optional      bool
	}{
		{"time", 0, false, true, false},
		// The beta rejects float literals here, but preserves runtime float
		// amplitudes for the physical shake (the fall getter returns int).
		{"ampl", -4, true, true, false},
		{"phase", 0, true, false, true},
		{"freq", 60, true, false, false},
	} {
		key := prefix + field.name
		raw, present := ctrl.Params[key]
		if !present && field.optional {
			// Phase is chosen from the evaluated frequency by the parser;
			// it must not execute the frequency expression a second time.
			continue
		}
		var expr evaluator.Expression = &evaluator.Number{Value: field.fallback, IsFloat: field.floating}
		if present {
			parsed, err := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
			if err != nil {
				return nil, fmt.Errorf("%s: %w", key, err)
			}
			expr = parsed[0]
			if field.strictInteger && integerFieldFloatLiteral(expr) {
				return nil, fmt.Errorf("%s: expected int value and read float", key)
			}
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, fmt.Errorf("%s: %w", key, err)
			}
		}
		cues = append(cues, hitDefCue{key: key, floating: field.floating, args: []argument{{argumentSpec: argumentSpec{name: "0", integer: !field.floating, required: true}, expr: expr}}})
	}
	return cues, nil
}

func hitShakeKey(key string) bool {
	return key == "envshake.time" || key == "envshake.freq" || key == "envshake.ampl" || key == "envshake.phase"
}
