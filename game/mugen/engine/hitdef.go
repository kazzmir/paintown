package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// Sound parameters are compiled here while the remaining legacy HitDef schema
// is progressively replaced. Never mutate the shared controller's source map.
func compileHitDef(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	type cue struct {
		key  string
		own  bool
		args []argument
	}
	var cues []cue
	for _, key := range []string{"hitsound", "guardsound"} {
		raw, present := ctrl.Params[key]
		if !present {
			continue
		}
		raw = strings.TrimSpace(raw)
		own := false
		if len(raw) > 0 {
			switch raw[0] {
			case 'S', 's':
				own = true
				raw = raw[1:]
			case 'F', 'f':
				raw = raw[1:]
			}
		}
		expressions, err := evaluator.ParseReferenceTuple(raw, 2)
		if err != nil {
			// The beta accepts a group alone and supplies item zero.
			if single, singleErr := evaluator.ParseReferenceTuple(raw, 1); singleErr == nil {
				expressions, err = append(single, &evaluator.Number{Value: 0}), nil
			}
		}
		if err != nil {
			return nil, true, fmt.Errorf("%s: %w", key, err)
		}
		entry := cue{key: key, own: own}
		for i, expr := range expressions {
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			entry.args = append(entry.args, argument{argumentSpec: argumentSpec{name: fmt.Sprint(i), integer: true, required: true}, expr: expr})
		}
		cues = append(cues, entry)
	}
	return func(_ *cns.StateController, env Environment) error {
		params := make(map[string]string, len(ctrl.Params))
		for key, value := range ctrl.Params {
			params[key] = value
		}
		for _, cue := range cues {
			values, err := evaluateArguments(cue.args, env)
			if err != nil {
				return fmt.Errorf("%s: %w", cue.key, err)
			}
			prefix := ""
			if cue.own {
				prefix = "S"
			}
			params[cue.key] = fmt.Sprintf("%s%d,%d", prefix, int(values["0"].Number()), int(values["1"].Number()))
		}
		env.HitDef(params)
		return nil
	}, true, nil
}
