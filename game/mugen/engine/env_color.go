package engine

import (
	"fmt"
	"image/color"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type EnvironmentColorParameters struct {
	Time  int
	Color color.NRGBA
	Under bool
}

func compileEnvironmentColor(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	var args []argument
	if raw, ok := ctrl.Params["value"]; ok {
		tuple, err := evaluator.ParseReferenceTupleForVersion(raw, 3, ctrl.LanguageVersion)
		if err != nil {
			return nil, true, fmt.Errorf("value: %w", err)
		}
		for i, expr := range tuple {
			args = append(args, argument{argumentSpec{fmt.Sprintf("value%d", i), true, false}, expr})
		}
	}
	// Native evaluates RGB before time and under, including inactive requests.
	for _, key := range []string{"time", "under"} {
		if raw, ok := ctrl.Params[key]; ok {
			tuple, err := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
			if err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			args = append(args, argument{argumentSpec{key, true, false}, tuple[0]})
		}
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
		p := EnvironmentColorParameters{Time: 1}
		rgb := [3]uint32{255, 255, 255}
		for i := range rgb {
			if v, ok := values[fmt.Sprintf("value%d", i)]; ok {
				rgb[i] = uint32(int64(v.Number()))
			}
		}
		// Unmasked integer packing lets values carry/sign-extend into later channels.
		packed := rgb[0] | rgb[1]<<8 | rgb[2]<<16
		p.Color = color.NRGBA{R: uint8(packed), G: uint8(packed >> 8), B: uint8(packed >> 16), A: 255}
		if v, ok := values["time"]; ok {
			p.Time = int(v.Number())
		}
		if v, ok := values["under"]; ok {
			p.Under = v.Bool()
		}
		context, ok := env.(interface {
			SetEnvironmentColor(EnvironmentColorParameters) error
		})
		if !ok {
			return fmt.Errorf("EnvColor global timeline context is unavailable")
		}
		return context.SetEnvironmentColor(p)
	}, true, nil
}

func ExecuteEnvironmentColor(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "EnvColor", Params: params, LanguageVersion: version}
	handler, _, err := compileEnvironmentColor(ctrl)
	if err != nil {
		return err
	}
	return handler(ctrl, env)
}

func (t *Timeline) SetEnvironmentColor(p EnvironmentColorParameters) { t.EnvironmentColor = p }

func (t *Timeline) AdvanceEnvironmentColor() {
	if t.EnvironmentColor.Time > 0 {
		t.EnvironmentColor.Time--
	}
}
