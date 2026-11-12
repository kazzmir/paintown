package engine

import (
	"fmt"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
	"strings"
)

// Transparency is an actor's frame-local override of its current AIR element.
// Default evaluates its alpha expressions but leaves an earlier override intact.
type Transparency struct {
	Enabled             bool
	Mode                string
	Source, Destination int
}

func compileTrans(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	mode := "default"
	if raw, present := ctrl.Params["trans"]; present {
		mode = strings.ToLower(strings.TrimSpace(raw))
	}
	switch mode {
	case "default", "none", "add", "add1", "addalpha", "sub":
	default:
		return nil, true, fmt.Errorf("invalid trans type %q", ctrl.Params["trans"])
	}
	var args []argument
	if raw, present := ctrl.Params["alpha"]; present {
		tuple, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
		if err != nil {
			tuple, err = evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
		}
		if err != nil {
			return nil, true, fmt.Errorf("alpha: %w", err)
		}
		for i, expr := range tuple {
			key := fmt.Sprintf("alpha%d", i)
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			if integerFieldFloatLiteral(expr) {
				return nil, true, fmt.Errorf("%s: expected int value and read float", key)
			}
			args = append(args, argument{argumentSpec{key, true, false}, expr})
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		if mode == "default" {
			return nil
		}
		p := Transparency{Enabled: true, Mode: mode, Source: 256, Destination: 256}
		if mode == "add1" {
			p.Destination = 128
		}
		if mode == "addalpha" {
			p.Destination = 0
		}
		if mode == "add" || mode == "add1" || mode == "addalpha" {
			p.Mode = "alpha"
			if v, ok := values["alpha0"]; ok {
				p.Source = max(0, min(256, int(v.Number())))
			}
			if v, ok := values["alpha1"]; ok {
				p.Destination = max(0, min(256, int(v.Number())))
			}
		}
		context, ok := env.(interface{ SetTransparency(Transparency) })
		if !ok {
			return fmt.Errorf("Trans actor context is unavailable")
		}
		context.SetTransparency(p)
		return nil
	}, true, nil
}

func ExecuteTransParameters(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "Trans", Params: params, LanguageVersion: version}
	handler, _, err := compileTrans(ctrl)
	if err != nil {
		return err
	}
	return handler(ctrl, env)
}
