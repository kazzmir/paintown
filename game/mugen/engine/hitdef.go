package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// Sound, spark, custom-state and selection parameters compile here while the
// remaining legacy schema is replaced. Never mutate the shared source map.
func compileHitDef(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	type cue struct {
		key  string
		own  bool
		args []argument
	}
	var cues []cue
	priorityClass := "Hit"
	for _, key := range []string{"hitsound", "guardsound", "sparkno", "guard.sparkno", "sparkxy", "id", "p1stateno", "p2stateno", "p2getp1state", "p2facing", "hitonce", "priority", "numhits"} {
		raw, present := ctrl.Params[key]
		if !present {
			continue
		}
		raw = strings.TrimSpace(raw)
		if key == "priority" {
			// Try the complete scalar first: a redirect can itself contain a
			// comma. Only the final enum separator belongs to priority.
			if _, err := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion); err != nil {
				separator := strings.LastIndex(raw, ",")
				if separator < 0 {
					return nil, true, fmt.Errorf("priority: %w", err)
				}
				kind := strings.ToUpper(strings.TrimSpace(raw[separator+1:]))
				switch {
				case strings.HasPrefix(kind, "H"):
					priorityClass = "Hit"
				case strings.HasPrefix(kind, "M"):
					priorityClass = "Miss"
				case strings.HasPrefix(kind, "D"):
					priorityClass = "Dodge"
				default:
					return nil, true, fmt.Errorf("priority: invalid class %q", raw[separator+1:])
				}
				raw = strings.TrimSpace(raw[:separator])
			}
		}
		own := false
		resource := key == "hitsound" || key == "guardsound" || key == "sparkno" || key == "guard.sparkno"
		if resource && len(raw) > 0 {
			switch raw[0] {
			case 'S', 's':
				own = true
				raw = raw[1:]
			case 'F', 'f':
				raw = raw[1:]
			}
		}
		count := 2
		if key != "hitsound" && key != "guardsound" && key != "sparkxy" {
			count = 1
		}
		expressions, err := evaluator.ParseReferenceTupleForVersion(raw, count, ctrl.LanguageVersion)
		if err != nil && count == 2 {
			// The beta accepts a group alone and supplies item zero.
			if single, singleErr := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion); singleErr == nil {
				expressions, err = append(single, &evaluator.Number{Value: 0}), nil
			}
		}
		if err != nil {
			return nil, true, fmt.Errorf("%s: %w", key, err)
		}
		entry := cue{key: key, own: own}
		for i, expr := range expressions {
			if (key == "p1stateno" || key == "p2stateno" || key == "p2getp1state" || key == "p2facing" || key == "hitonce" || key == "priority" || key == "numhits") && integerFieldFloatLiteral(expr) {
				return nil, true, fmt.Errorf("%s: expected int value and read float", key)
			}
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			entry.args = append(entry.args, argument{argumentSpec: argumentSpec{name: fmt.Sprint(i), integer: true, required: true}, expr: expr})
		}
		cues = append(cues, entry)
	}
	if raw, present := ctrl.Params["affectteam"]; present {
		team := strings.ToUpper(strings.TrimSpace(raw))
		if team == "" || !strings.ContainsAny(team[:1], "EFB") {
			return nil, true, fmt.Errorf("invalid affectteam %q", raw)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		params := make(map[string]string, len(ctrl.Params))
		for key, value := range ctrl.Params {
			params[key] = value
		}
		for _, cue := range cues {
			values, err := evaluateArgumentsWithBottom(cue.args, env, cue.key == "hitonce" || cue.key == "priority" || cue.key == "numhits")
			if err != nil {
				return fmt.Errorf("%s: %w", cue.key, err)
			}
			prefix := ""
			if cue.own {
				prefix = "S"
			}
			params[cue.key] = fmt.Sprintf("%s%d", prefix, int(values["0"].Number()))
			if len(cue.args) == 2 {
				params[cue.key] += fmt.Sprintf(",%d", int(values["1"].Number()))
			}
			if cue.key == "priority" {
				params[cue.key] += "," + priorityClass
			}
		}
		env.HitDef(params)
		return nil
	}, true, nil
}
