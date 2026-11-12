package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type PaletteFXScope uint8

const (
	PaletteFXLocal PaletteFXScope = iota
	PaletteFXAll
	PaletteFXBackground
)

type PaletteFXParameters struct {
	Scope               PaletteFXScope
	Time, Color, Period int
	Add, Mul, SinAdd    [3]int
	Invert              bool
}

func compilePaletteFX(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	var args []argument
	for _, key := range []string{"time", "color", "invertall"} {
		if raw, present := ctrl.Params[key]; present {
			expr, err := evaluator.ParseReferenceForVersion(raw, ctrl.LanguageVersion)
			if err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			args = append(args, argument{argumentSpec{key, true, false}, expr})
		}
	}
	for _, key := range []string{"add", "mul", "sinadd"} {
		if raw, present := ctrl.Params[key]; present {
			count := 3
			if key == "sinadd" {
				count = 4
			}
			tuple, err := evaluator.ParseReferenceTupleForVersion(raw, count, ctrl.LanguageVersion)
			if err != nil && key == "sinadd" {
				tuple, err = evaluator.ParseReferenceTupleForVersion(raw, 3, ctrl.LanguageVersion)
			}
			if err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			for i, expr := range tuple {
				args = append(args, argument{argumentSpec{fmt.Sprintf("%s%d", key, i), true, false}, expr})
			}
		}
	}
	byName := map[string]argument{}
	for _, arg := range args {
		if integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
		byName[arg.name] = arg
	}
	args = nil
	for _, key := range []string{"time", "color", "add0", "add1", "add2", "mul0", "mul1", "mul2", "sinadd0", "sinadd1", "sinadd2", "sinadd3", "invertall"} {
		if arg, present := byName[key]; present {
			args = append(args, arg)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		p := PaletteFXParameters{Color: 256, Mul: [3]int{256, 256, 256}}
		switch strings.ToLower(ctrl.Type) {
		case "allpalfx":
			p.Scope = PaletteFXAll
		case "bgpalfx":
			p.Scope = PaletteFXBackground
		}
		if v, ok := values["time"]; ok {
			p.Time = int(v.Number())
		}
		if v, ok := values["color"]; ok {
			p.Color = int(v.Number())
		}
		if v, ok := values["invertall"]; ok {
			p.Invert = v.Bool()
		}
		if v, ok := values["sinadd3"]; ok {
			p.Period = int(v.Number())
		}
		for i := 0; i < 3; i++ {
			if v, ok := values[fmt.Sprintf("add%d", i)]; ok {
				p.Add[i] = int(v.Number())
			}
			if v, ok := values[fmt.Sprintf("mul%d", i)]; ok {
				p.Mul[i] = int(v.Number())
			}
			if v, ok := values[fmt.Sprintf("sinadd%d", i)]; ok {
				p.SinAdd[i] = int(v.Number())
			}
		}
		context, ok := env.(interface {
			SetPaletteEffect(PaletteFXParameters) error
		})
		if !ok {
			return fmt.Errorf("%s actor context is unavailable", ctrl.Type)
		}
		return context.SetPaletteEffect(p)
	}, true, nil
}

func ExecutePaletteFXParameters(params map[string]string, env Environment, version string) error {
	return ExecutePaletteFXController("PalFX", params, env, version)
}

func ExecutePaletteFXController(kind string, params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: kind, Params: params, LanguageVersion: version}
	handler, _, err := compilePaletteFX(ctrl)
	if err != nil {
		return err
	}
	return handler(ctrl, env)
}
