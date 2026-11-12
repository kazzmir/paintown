package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// WidthParameters holds front/back offsets in the executing state's units.
type WidthParameters struct {
	Edge, Player [2]float64
}

func ExecuteWidthParameters(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "Width", Params: params, LanguageVersion: version}
	fn, _, err := compileWidth(ctrl)
	if err != nil {
		return err
	}
	return fn(ctrl, env)
}

func compileWidth(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	_, edge := ctrl.Params["edge"]
	_, player := ctrl.Params["player"]
	shorthand := !edge && !player
	fields := []string{"edge", "player"}
	if shorthand {
		fields = []string{"value"}
	}
	var args []argument
	for _, field := range fields {
		raw, present := ctrl.Params[field]
		if !present {
			continue
		}
		expressions, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
		if err != nil {
			if single, singleErr := evaluator.ParseReferenceTupleForVersion(strings.TrimSpace(strings.TrimSuffix(strings.TrimSpace(raw), ",")), 1, ctrl.LanguageVersion); singleErr == nil {
				expressions, err = append(single, &evaluator.Number{Value: 0}), nil
			}
		}
		if err != nil {
			return nil, true, fmt.Errorf("%s: %w", field, err)
		}
		for index, expression := range expressions {
			name := fmt.Sprintf("%s%d", field, index)
			if integerFieldFloatLiteral(expression) {
				return nil, true, fmt.Errorf("%s: expected int value and read float", name)
			}
			if err := evaluator.ValidateKnownTriggers(expression); err != nil {
				return nil, true, fmt.Errorf("%s: %w", name, err)
			}
			args = append(args, argument{argumentSpec{name, true, false}, expression})
		}
	}
	if len(args) == 0 {
		return nil, true, fmt.Errorf("Width requires value, edge or player")
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		context, ok := env.(interface{ SetWidth(WidthParameters) error })
		if !ok {
			return fmt.Errorf("Width context is unavailable")
		}
		vector := func(name string) [2]float64 {
			return [2]float64{values[name+"0"].Number(), values[name+"1"].Number()}
		}
		p := WidthParameters{Edge: vector("edge"), Player: vector("player")}
		if shorthand {
			p.Edge, p.Player = vector("value"), vector("value")
		}
		return context.SetWidth(p)
	}, true, nil
}
