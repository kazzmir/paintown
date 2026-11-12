package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// GameAnimationParameters describes one common-resource, scene-owned cue.
// Displacement is in common game units; Position uses the state owner's units.
type GameAnimationParameters struct {
	Action       int
	Position     [2]float64
	Displacement [2]int32
	Under        bool
}

type gameAnimationEnvironment interface {
	GameAnimationDisplacement(int) ([2]int32, error)
	CreateGameAnimation(GameAnimationParameters) error
}

func compileGameAnimation(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	var base, late []argument
	for _, key := range []string{"value", "pos", "random", "under"} {
		raw, present := ctrl.Params[key]
		if !present {
			continue
		}
		count := 1
		if key == "pos" {
			count = 2
		}
		expressions, err := evaluator.ParseReferenceTupleForVersion(raw, count, ctrl.LanguageVersion)
		if err != nil && key == "pos" {
			// A lone position supplies X; native leaves Y at its zero default.
			expressions, err = evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
		}
		if err != nil {
			return nil, true, fmt.Errorf("%s: %w", key, err)
		}
		for index, expr := range expressions {
			integer := key != "pos"
			if integer && integerFieldFloatLiteral(expr) {
				return nil, true, fmt.Errorf("%s: expected int value and read float", key)
			}
			if _, quoted := expr.(*evaluator.StringExpr); quoted {
				return nil, true, fmt.Errorf("%s: expected numeric expression", key)
			}
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			name := key
			if key == "pos" {
				name = fmt.Sprintf("pos%d", index)
			}
			arg := argument{argumentSpec{name, integer, false}, expr}
			if key == "under" {
				late = append(late, arg)
			} else {
				base = append(base, arg)
			}
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		context, ok := env.(gameAnimationEnvironment)
		if !ok {
			return fmt.Errorf("GameMakeAnim entity/effect context is unavailable")
		}
		values, err := evaluateArgumentsWithBottom(base, env, true)
		if err != nil {
			return err
		}
		p := GameAnimationParameters{}
		if value, present := values["value"]; present {
			p.Action = int(value.Number())
		}
		for axis := range p.Position {
			if value, present := values[fmt.Sprintf("pos%d", axis)]; present {
				p.Position[axis] = value.Number()
			}
		}
		if value, present := values["random"]; present && value.Number() > 0 {
			p.Displacement, err = context.GameAnimationDisplacement(int(value.Number()))
			if err != nil {
				return err
			}
		}
		// Native consumes displacement draws before evaluating under, even when
		// the requested common action is unavailable. Keep these phases distinct.
		values, err = evaluateArgumentsWithBottom(late, env, true)
		if err != nil {
			return err
		}
		if value, present := values["under"]; present {
			p.Under = value.Bool()
		}
		return context.CreateGameAnimation(p)
	}, true, nil
}

func ExecuteGameAnimation(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "GameMakeAnim", Params: params, LanguageVersion: version}
	handler, _, err := compileGameAnimation(ctrl)
	if err != nil {
		return err
	}
	return handler(ctrl, env)
}
