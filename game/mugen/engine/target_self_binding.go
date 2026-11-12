package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

type TargetSelfBindParameters struct {
	Time, CoordinateWidth int
	X, Y                  float64
	HasX, HasY            bool
	Point                 byte
}

func ExecuteTargetSelfBindParameters(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "BindToTarget", Params: params, LanguageVersion: version}
	fn, _, err := compileTargetSelfBind(ctrl)
	if err != nil {
		return err
	}
	return fn(ctrl, env)
}

func compileTargetSelfBind(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	id, err := compileArguments(ctrl, []argumentSpec{{"id", true, false}})
	if err != nil {
		return nil, true, err
	}
	args, err := compileArguments(ctrl, []argumentSpec{{"time", true, false}})
	if err != nil {
		return nil, true, err
	}
	point := byte('f')
	components := 0
	if raw, present := ctrl.Params["pos"]; present {
		expressions, parseErr := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
		if parseErr != nil {
			// The literal point consumes the suffix after two numeric fields.
			// Beta checks only its first letter, including a trailing suffix.
			parts := parsers.SplitList(raw)
			for end := 2; end < len(parts); end++ {
				pair, pairErr := evaluator.ParseReferenceTupleForVersion(strings.Join(parts[:end], ","), 2, ctrl.LanguageVersion)
				if pairErr != nil {
					continue
				}
				literal := strings.ToLower(strings.TrimSpace(strings.Join(parts[end:], ",")))
				if literal == "" || !strings.ContainsRune("fhm", rune(literal[0])) {
					return nil, true, fmt.Errorf("pos: invalid bind point %q", literal)
				}
				point, expressions, parseErr = literal[0], pair, nil
				break
			}
		}
		if parseErr != nil {
			// Omitted Y stays unspecified; it does not become zero.
			expressions, parseErr = evaluator.ParseReferenceTupleForVersion(strings.TrimSpace(strings.TrimSuffix(strings.TrimSpace(raw), ",")), 1, ctrl.LanguageVersion)
		}
		if parseErr != nil {
			return nil, true, fmt.Errorf("pos: %w", parseErr)
		}
		components = len(expressions)
		for i, expr := range expressions {
			args = append(args, argument{argumentSpec{fmt.Sprintf("pos%d", i), false, true}, expr})
		}
	}
	for _, arg := range append(append([]argument(nil), id...), args...) {
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
		if arg.integer && integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		context, ok := env.(interface {
			BindCallerToTarget(int, func() (TargetSelfBindParameters, error)) error
		})
		if !ok {
			return fmt.Errorf("BindToTarget context is unavailable")
		}
		v, err := evaluateArgumentsWithBottom(id, env, true)
		if err != nil {
			return err
		}
		targetID := -1
		if value, present := v["id"]; present {
			targetID = int(value.Number())
		}
		return context.BindCallerToTarget(targetID, func() (TargetSelfBindParameters, error) {
			v, err := evaluateArgumentsWithBottom(args, env, true)
			if err != nil {
				return TargetSelfBindParameters{}, err
			}
			p := TargetSelfBindParameters{Time: 1, Point: point, HasX: components > 0, HasY: components > 1, X: v["pos0"].Number(), Y: v["pos1"].Number()}
			if value, present := v["time"]; present {
				p.Time = int(value.Number())
			}
			if profile, ok := env.(interface{ LocalCoordinateWidth() int }); ok {
				p.CoordinateWidth = profile.LocalCoordinateWidth()
			}
			return p, nil
		})
	}, true, nil
}
