package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type TargetBindParameters struct {
	Time, ID int
	X, Y     float64
}

func integerFieldFloatLiteral(expr evaluator.Expression) bool {
	switch value := expr.(type) {
	case *evaluator.Number:
		return value.IsFloat
	case *evaluator.Group:
		return integerFieldFloatLiteral(value.Expression)
	case *evaluator.Unary:
		if value.Operator == "+" || value.Operator == "-" {
			return integerFieldFloatLiteral(value.Expression)
		}
	}
	return false
}

func compileTargetBind(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	args, err := compileArguments(ctrl, []argumentSpec{{"time", true, false}, {"id", true, false}})
	if err != nil {
		return nil, true, err
	}
	for _, arg := range args {
		if integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
	}
	if raw, present := ctrl.Params["pos"]; present {
		raw = strings.TrimSpace(raw)
		// The beta supplies Y=0 for one component or one trailing comma,
		// but rejects an omitted first component and a third component.
		expressions, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
		if err != nil {
			if single, singleErr := evaluator.ParseReferenceTupleForVersion(strings.TrimSpace(strings.TrimSuffix(raw, ",")), 1, ctrl.LanguageVersion); singleErr == nil {
				expressions, err = append(single, &evaluator.Number{Value: 0}), nil
			}
		}
		if err != nil {
			return nil, true, fmt.Errorf("pos: %w", err)
		}
		for index, expression := range expressions {
			if err := evaluator.ValidateKnownTriggers(expression); err != nil {
				return nil, true, fmt.Errorf("pos: %w", err)
			}
			args = append(args, argument{argumentSpec{fmt.Sprintf("pos%d", index), false, true}, expression})
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		context, ok := env.(interface {
			BindTargets(TargetBindParameters) error
		})
		if !ok {
			return fmt.Errorf("TargetBind context is unavailable")
		}
		p := TargetBindParameters{Time: 1, ID: -1, X: values["pos0"].Number(), Y: values["pos1"].Number()}
		if value, present := values["time"]; present {
			p.Time = int(value.Number())
		}
		if value, present := values["id"]; present {
			p.ID = int(value.Number())
		}
		return context.BindTargets(p)
	}, true, nil
}
