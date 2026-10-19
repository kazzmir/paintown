package engine

import (
	"fmt"
	"strconv"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/combat"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type hitPowerPlan struct {
	damage, get, give []evaluator.Expression
}

func compileHitPower(ctrl *cns.StateController) (*hitPowerPlan, error) {
	read := func(key string) ([]evaluator.Expression, error) {
		raw, present := ctrl.Params[key]
		if !present {
			return nil, nil
		}
		expressions, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
		if err != nil {
			expressions, err = evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
		}
		if err != nil {
			return nil, fmt.Errorf("%s: %w", key, err)
		}
		for _, expression := range expressions {
			if integerFieldFloatLiteral(expression) {
				return nil, fmt.Errorf("%s: expected int value and read float", key)
			}
			if err := evaluator.ValidateKnownTriggers(expression); err != nil {
				return nil, fmt.Errorf("%s: %w", key, err)
			}
		}
		return expressions, nil
	}
	plan := &hitPowerPlan{}
	for _, field := range []struct {
		name string
		to   *[]evaluator.Expression
	}{{"damage", &plan.damage}, {"getpower", &plan.get}, {"givepower", &plan.give}} {
		expressions, err := read(field.name)
		if err != nil {
			return nil, err
		}
		*field.to = expressions
	}
	for len(plan.damage) < 2 {
		plan.damage = append(plan.damage, &evaluator.Number{})
	}
	return plan, nil
}

func hitIntegerCue(key string, expressions []evaluator.Expression) hitDefCue {
	cue := hitDefCue{key: key}
	for index, expression := range expressions {
		cue.args = append(cue.args, argument{argumentSpec: argumentSpec{name: strconv.Itoa(index), integer: true, required: true}, expr: expression})
	}
	return cue
}

func (p *hitPowerPlan) cues(env Environment) (hitDefCue, []hitDefCue) {
	rules := combat.DefaultPowerRules()
	if context, ok := env.(interface{ GetHitPowerRules() combat.PowerRules }); ok {
		rules = context.GetHitPowerRules()
	}
	pair := func(explicit []evaluator.Expression, factor float64) []evaluator.Expression {
		// Retain source expressions: inherited values execute again, rather
		// than reading the already coerced/stored damage or hit-power value.
		expressions := append([]evaluator.Expression(nil), explicit...)
		if len(expressions) == 0 {
			expressions = append(expressions, &evaluator.Binary{Left: p.damage[0], Operator: "*", Right: &evaluator.Number{Value: factor, IsFloat: true}})
		}
		if len(expressions) == 1 {
			expressions = append(expressions, &evaluator.Binary{Left: expressions[0], Operator: "/", Right: &evaluator.Number{Value: 2}})
		}
		return expressions
	}
	return hitIntegerCue("damage", p.damage), []hitDefCue{
		hitIntegerCue("getpower", pair(p.get, rules.Attack)),
		hitIntegerCue("givepower", pair(p.give, rules.GetHit)),
	}
}
