package engine

import (
	"fmt"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// Defaults retain expressions, not already evaluated numbers. Native inherited
// corner-push parameters can therefore evaluate the same source expression again.
func compileHitVectors(ctrl *cns.StateController) ([]hitDefCue, error) {
	zero := &evaluator.Number{Value: 0, IsFloat: true}
	product := func(expr evaluator.Expression, factor float64) evaluator.Expression {
		return &evaluator.Binary{Left: expr, Operator: "*", Right: &evaluator.Number{Value: factor, IsFloat: true}}
	}
	read := func(key string, fallback []evaluator.Expression) ([]evaluator.Expression, error) {
		raw, present := ctrl.Params[key]
		if !present {
			return append([]evaluator.Expression(nil), fallback...), nil
		}
		expressions, err := evaluator.ParseReferenceTupleForVersion(raw, len(fallback), ctrl.LanguageVersion)
		if err != nil && len(fallback) == 2 {
			expressions, err = evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
		}
		if err != nil {
			return nil, fmt.Errorf("%s: %w", key, err)
		}
		return append(expressions, fallback[len(expressions):]...), nil
	}
	values := map[string][]evaluator.Expression{}
	define := func(key string, fallback ...evaluator.Expression) error {
		expressions, err := read(key, fallback)
		if err == nil {
			values[key] = expressions
		}
		return err
	}
	for _, key := range []string{"ground.velocity", "air.velocity"} {
		if err := define(key, zero, zero); err != nil {
			return nil, err
		}
	}
	if err := define("guard.velocity", values["ground.velocity"][0]); err != nil {
		return nil, err
	}
	if err := define("down.velocity", values["air.velocity"]...); err != nil {
		return nil, err
	}
	if err := define("airguard.velocity", product(values["air.velocity"][0], 1.5), product(values["air.velocity"][1], .5)); err != nil {
		return nil, err
	}
	var corner evaluator.Expression = product(values["guard.velocity"][0], 1.3)
	if strings.HasPrefix(strings.ToUpper(strings.TrimSpace(ctrl.Params["attr"])), "A") {
		corner = zero
	}
	if err := define("ground.cornerpush.veloff", corner); err != nil {
		return nil, err
	}
	for _, key := range []string{"air.cornerpush.veloff", "down.cornerpush.veloff", "guard.cornerpush.veloff"} {
		if err := define(key, values["ground.cornerpush.veloff"]...); err != nil {
			return nil, err
		}
	}
	if err := define("airguard.cornerpush.veloff", values["guard.cornerpush.veloff"]...); err != nil {
		return nil, err
	}
	var cues []hitDefCue
	for _, key := range []string{"ground.velocity", "ground.cornerpush.veloff", "air.velocity", "air.cornerpush.veloff", "down.velocity", "down.cornerpush.veloff", "guard.velocity", "guard.cornerpush.veloff", "airguard.velocity", "airguard.cornerpush.veloff"} {
		cue := hitDefCue{key: key, floating: true}
		for i, expr := range values[key] {
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, fmt.Errorf("%s: %w", key, err)
			}
			cue.args = append(cue.args, argument{argumentSpec: argumentSpec{name: strconv.Itoa(i), required: true}, expr: expr})
		}
		cues = append(cues, cue)
	}
	return cues, nil
}
