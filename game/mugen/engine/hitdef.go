package engine

import (
	"fmt"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type hitDefCue struct {
	key               string
	own, floating     bool
	coordinateDefault bool
	args              []argument
}

// This order is measured by the tagged full-field and vector programs. Source
// declaration order does not determine parameter expression side effects.
var hitDefEvaluationOrder = []string{
	"id", "kill", "guard.kill", "fall.kill", "hitonce", "damage", "getpower", "givepower", "pausetime", "numhits",
	"hitsound", "guardsound", "priority", "p1stateno", "p2stateno", "p2getp1state",
	"p1sprpriority", "p2sprpriority", "forcestand", "forcenofall", "fall", "fall.damage", "fall.xvelocity", "fall.yvelocity", "fall.recover", "fall.recovertime",
	"sparkno", "guard.sparkno", "sparkxy", "p1facing", "p1getp2facing", "p2facing",
	"ground.hittime", "ground.slidetime", "ground.velocity", "ground.cornerpush.veloff", "air.hittime", "air.fall", "air.velocity", "air.cornerpush.veloff",
	"down.bounce", "down.hittime", "down.velocity", "down.cornerpush.veloff", "guard.velocity", "guard.hittime", "guard.slidetime", "guard.ctrltime", "guard.dist", "guard.pausetime", "guard.cornerpush.veloff",
	"airguard.velocity", "airguard.ctrltime", "airguard.cornerpush.veloff", "yaccel",
}

// Sound, spark, custom-state and selection parameters compile here while the
// remaining legacy schema is replaced. Never mutate the shared source map.
func compileHitDef(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	power, err := compileHitPower(ctrl)
	if err != nil {
		return nil, true, err
	}
	pauses, err := compileHitPauses(ctrl)
	if err != nil {
		return nil, true, err
	}
	timing, err := compileHitTiming(ctrl)
	if err != nil {
		return nil, true, err
	}
	fall, err := compileHitFall(ctrl)
	if err != nil {
		return nil, true, err
	}
	physics, err := compileHitFallVectors(ctrl)
	if err != nil {
		return nil, true, err
	}
	var cues []hitDefCue
	priorityClass := "Hit"
	for _, key := range []string{"hitsound", "guardsound", "sparkno", "guard.sparkno", "sparkxy", "id", "p1stateno", "p2stateno", "p2getp1state", "p1facing", "p1getp2facing", "p2facing", "hitonce", "priority", "numhits", "p1sprpriority", "p2sprpriority", "forcenofall", "forcestand", "kill", "guard.kill", "fall.kill", "fall.damage", "down.bounce", "guard.dist"} {
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
		entry := hitDefCue{key: key, own: own}
		strictInteger := false
		switch key {
		case "p1stateno", "p2stateno", "p2getp1state", "p1facing", "p1getp2facing", "p2facing", "hitonce", "priority", "numhits", "p1sprpriority", "p2sprpriority", "forcenofall", "forcestand", "kill", "guard.kill", "fall.kill", "fall.damage", "down.bounce", "guard.dist":
			strictInteger = true
		}
		for i, expr := range expressions {
			if strictInteger && integerFieldFloatLiteral(expr) {
				return nil, true, fmt.Errorf("%s: expected int value and read float", key)
			}
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			entry.args = append(entry.args, argument{argumentSpec: argumentSpec{name: fmt.Sprint(i), integer: !entry.floating, required: true}, expr: expr})
		}
		cues = append(cues, entry)
	}
	vectors, err := compileHitVectors(ctrl)
	if err != nil {
		return nil, true, err
	}
	cues = append(cues, vectors...)
	cues = append(cues, pauses...)
	cues = append(cues, timing...)
	cues = append(cues, fall...)
	cues = append(cues, physics...)
	byKey := make(map[string]hitDefCue, len(cues))
	for _, cue := range cues {
		known := false
		for _, key := range hitDefEvaluationOrder {
			known = known || key == cue.key
		}
		if !known {
			return nil, true, fmt.Errorf("unrouted HitDef parameter %q", cue.key)
		}
		byKey[cue.key] = cue
	}
	literals, err := compileHitDefLiterals(ctrl)
	if err != nil {
		return nil, true, err
	}
	return func(_ *cns.StateController, env Environment) error {
		params := make(map[string]string, len(ctrl.Params))
		for key, value := range ctrl.Params {
			params[key] = value
		}
		for key, value := range literals {
			params[key] = value
		}
		damage, transfers := power.cues(env)
		for _, key := range hitDefEvaluationOrder {
			cue, present := byKey[key]
			switch key {
			case "damage":
				cue, present = damage, true
			case "getpower":
				cue, present = transfers[0], true
			case "givepower":
				cue, present = transfers[1], true
			}
			if !present {
				continue
			}
			values, err := evaluateArgumentsWithBottom(cue.args, env, cue.key == "damage" || cue.key == "getpower" || cue.key == "givepower" || cue.key == "pausetime" || cue.key == "guard.pausetime" || cue.key == "hitonce" || cue.key == "priority" || cue.key == "numhits" || cue.key == "p1sprpriority" || cue.key == "p2sprpriority" || cue.key == "p1facing" || cue.key == "p1getp2facing" || cue.key == "forcenofall" || cue.key == "forcestand" || cue.key == "kill" || cue.key == "guard.kill" || cue.key == "fall.kill" || cue.key == "fall.damage" || cue.key == "down.bounce" || cue.key == "guard.dist" || hitTimingKey(cue.key) || hitFallKey(cue.key) || cue.floating)
			if err != nil {
				return fmt.Errorf("%s: %w", cue.key, err)
			}
			if cue.coordinateDefault {
				width := 320
				if context, ok := env.(interface{ LocalCoordinateWidth() int }); ok && context.LocalCoordinateWidth() > 0 {
					width = context.LocalCoordinateWidth()
				}
				values["0"] = evaluator.Float(float32(values["0"].Number()) * float32(width) / 320)
			}
			prefix := ""
			if cue.own {
				prefix = "S"
			}
			format := func(index string) string {
				if cue.floating {
					return strconv.FormatFloat(values[index].Number(), 'g', -1, 32)
				}
				number := int(values[index].Number())
				if cue.key == "pausetime" || cue.key == "guard.pausetime" || hitTimingKey(cue.key) {
					number = max(0, number)
				}
				if cue.key == "fall.recovertime" && number < 0 {
					number = 4
				}
				return strconv.Itoa(number)
			}
			params[cue.key] = prefix + format("0")
			if len(cue.args) == 2 {
				params[cue.key] += "," + format("1")
			}
			if cue.key == "priority" {
				params[cue.key] += "," + priorityClass
			}
		}
		env.HitDef(params)
		return nil
	}, true, nil
}
