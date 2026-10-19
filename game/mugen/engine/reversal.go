package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/combat"
)

func compileReversalDef(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	raw, present := ctrl.Params["reversal.attr"]
	if !present {
		return nil, true, fmt.Errorf("missing required parameter reversal.attr")
	}
	raw = strings.ToUpper(strings.TrimSpace(raw))
	mask := ""
	if raw != "" && !strings.HasPrefix(raw, "\"") {
		parts := strings.Split(raw, ",")
		posture := strings.TrimSpace(parts[0])
		if _, err := combat.ParseAttributes(posture + ",NA"); err != nil {
			return nil, true, fmt.Errorf("reversal.attr: %w", err)
		}
		valid := []string{posture}
		for _, part := range parts[1:] {
			part = strings.TrimSpace(part)
			if len(part) != 2 || !strings.ContainsRune("NSHA", rune(part[0])) || !strings.ContainsRune("ATP", rune(part[1])) {
				break
			}
			valid = append(valid, part)
		}
		if len(valid) > 1 {
			mask = strings.Join(valid, ",")
		}
	}
	// The beta reads the complete HitDef schema, including unused damage/ID
	// expressions and the repeated guard-pause source defaults.
	evaluate, _, err := compileHitDefValues(ctrl)
	if err != nil {
		return nil, true, err
	}
	return func(_ *cns.StateController, env Environment) error {
		params, err := evaluate(env)
		if err != nil {
			return err
		}
		params["reversal.attr"] = mask
		env.ReversalDef(params)
		return nil
	}, true, nil
}
