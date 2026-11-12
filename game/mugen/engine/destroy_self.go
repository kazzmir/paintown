package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type DestroySelfParameters struct {
	Recursive, RemoveExplods bool
}

func compileDestroySelf(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	args, err := compileArguments(ctrl, []argumentSpec{{"recursive", true, false}, {"removeexplods", true, false}})
	if err != nil {
		return nil, true, err
	}
	for _, arg := range args {
		if integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		p := DestroySelfParameters{Recursive: values["recursive"].Bool(), RemoveExplods: values["removeexplods"].Bool()}
		if context, ok := env.(interface {
			DestroyHelper(DestroySelfParameters) error
		}); ok {
			return context.DestroyHelper(p)
		}
		if p.Recursive || p.RemoveExplods {
			return fmt.Errorf("DestroySelf context is unavailable")
		}
		env.DestroySelf()
		return nil
	}, true, nil
}
