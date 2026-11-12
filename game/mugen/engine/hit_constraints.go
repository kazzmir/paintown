package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileHitConstraints(ctrl *cns.StateController) ([]hitDefCue, error) {
	read := func(key string, limit int) ([]evaluator.Expression, error) {
		raw, present := ctrl.Params[key]
		if !present {
			return nil, nil
		}
		var expressions []evaluator.Expression
		var err error
		for count := limit; count > 0; count-- {
			expressions, err = evaluator.ParseReferenceTupleForVersion(raw, count, ctrl.LanguageVersion)
			if err == nil {
				break
			}
		}
		if err != nil {
			return nil, fmt.Errorf("%s: %w", key, err)
		}
		for _, expr := range expressions {
			if integerFieldFloatLiteral(expr) {
				return nil, fmt.Errorf("%s: expected int value and read float", key)
			}
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, fmt.Errorf("%s: %w", key, err)
			}
		}
		return expressions, nil
	}
	var cues []hitDefCue
	if _, snap := ctrl.Params["snap"]; snap {
		expressions, err := read("snap", 3)
		if err != nil {
			return nil, err
		}
		duration := evaluator.Expression(&evaluator.Number{Value: 0})
		if len(expressions) == 3 {
			duration = expressions[2]
			expressions = expressions[:2]
		}
		// The beta replaces both bounds with the snap source, evaluating
		// its X/Y expressions twice. Authored min/max fields are ignored.
		cues = append(cues, hitIntegerCue("mindist", expressions), hitIntegerCue("maxdist", expressions), hitIntegerCue("snap", []evaluator.Expression{duration}))
	} else {
		for _, key := range []string{"mindist", "maxdist"} {
			expressions, err := read(key, 2)
			if err != nil {
				return nil, err
			}
			if expressions != nil {
				cues = append(cues, hitIntegerCue(key, expressions))
			}
		}
	}
	return cues, nil
}

func hitConstraintKey(key string) bool {
	return key == "mindist" || key == "maxdist" || key == "snap"
}

func hitConstraintDuration(params map[string]string, value string) {
	// Keep the public source key counted once while separating its internal
	// duration from the evaluated bounds sent to the runtime parser.
	delete(params, "snap")
	params["snap.time"] = value
}
