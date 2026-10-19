package engine

import (
	"fmt"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileVarRandom(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	index, err := compileArguments(ctrl, []argumentSpec{{"v", true, true}})
	if err != nil {
		return nil, true, err
	}
	ranges := []argument{
		{argumentSpec{"range0", true, false}, &evaluator.Number{Value: 0}},
		{argumentSpec{"range1", true, false}, &evaluator.Number{Value: 1000}},
	}
	if raw, present := ctrl.Params["range"]; present {
		exprs, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
		if err != nil {
			if one, singleErr := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion); singleErr == nil {
				exprs, err = []evaluator.Expression{&evaluator.Number{Value: 0}, one[0]}, nil
			}
		}
		if err != nil {
			return nil, true, fmt.Errorf("range: %w", err)
		}
		for i, expr := range exprs {
			ranges[i].expr = expr
		}
	}
	for _, arg := range append(append([]argument{}, index...), ranges...) {
		if integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		v, err := evaluateArgumentsWithBottom(index, env, true)
		if err != nil {
			return err
		}
		idx := int(v["v"].Number())
		// Invalid indices stop before evaluating either range or drawing RNG.
		if idx < 0 || idx >= 60 {
			return nil
		}
		r, err := evaluateArgumentsWithBottom(ranges, env, true)
		if err != nil {
			return err
		}
		env.VarRandom(idx, int(r["range0"].Number()), int(r["range1"].Number()))
		return nil
	}, true, nil
}
