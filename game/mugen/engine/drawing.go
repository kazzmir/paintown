package engine

import (
	"fmt"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type AngleDrawing struct {
	HasValue bool
	Value    float64
	Scale    [2]float32
}
type DrawingOffset struct{ X, Y *float64 }

func compileDrawing(ctrl *cns.StateController, name string) (ControllerFunc, bool, error) {
	var args []argument
	keys := []string{"value"}
	if name == "offset" {
		keys = []string{"x", "y"}
	}
	for _, key := range keys {
		if raw, ok := ctrl.Params[key]; ok {
			expr, err := evaluator.ParseReferenceForVersion(raw, ctrl.LanguageVersion)
			if err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			args = append(args, argument{argumentSpec{key, false, false}, expr})
		}
	}
	if name == "angledraw" {
		if raw, ok := ctrl.Params["scale"]; ok {
			tuple, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
			if err != nil {
				tuple, err = evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
			}
			if err != nil {
				return nil, true, fmt.Errorf("scale: %w", err)
			}
			for i, expr := range tuple {
				args = append(args, argument{argumentSpec{fmt.Sprintf("scale%d", i), false, false}, expr})
			}
		}
	}
	for _, arg := range args {
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		value := values["value"].Number()
		switch name {
		case "angleset":
			env.AngleSet(value)
		case "angleadd":
			env.AngleAdd(value)
		case "anglemul":
			env.AngleMul(value)
		case "angledraw":
			p := AngleDrawing{Scale: [2]float32{1, 1}}
			if v, ok := values["value"]; ok {
				p.HasValue, p.Value = true, v.Number()
			}
			for i, key := range []string{"scale0", "scale1"} {
				if v, ok := values[key]; ok {
					p.Scale[i] = float32(v.Number())
				}
			}
			context, ok := env.(interface{ SetAngleDrawing(AngleDrawing) })
			if !ok {
				return fmt.Errorf("AngleDraw actor context is unavailable")
			}
			context.SetAngleDrawing(p)
		case "offset":
			p := DrawingOffset{}
			if v, ok := values["x"]; ok {
				x := v.Number()
				p.X = &x
			}
			if v, ok := values["y"]; ok {
				y := v.Number()
				p.Y = &y
			}
			context, ok := env.(interface{ SetDrawingOffset(DrawingOffset) })
			if !ok {
				return fmt.Errorf("Offset actor context is unavailable")
			}
			context.SetDrawingOffset(p)
		}
		return nil
	}, true, nil
}
