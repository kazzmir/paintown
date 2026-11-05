package engine

import (
	"fmt"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// AfterImageParameters describes one entity's recording and palette policy.
// PostBright is evaluated by the beta, but has no effect on the measured draw.
type AfterImageParameters struct {
	Time, Length, TimeGap, FrameGap, Color int
	Invert                                 bool
	Bright, Contrast, PostBright, Add      [3]int
	Mul                                    [3]float32
	Trans                                  string
}

func DefaultAfterImageParameters() AfterImageParameters {
	return AfterImageParameters{Time: 1, Length: 20, TimeGap: 1, FrameGap: 4, Color: 256,
		Bright: [3]int{30, 30, 30}, Contrast: [3]int{120, 120, 220},
		Add: [3]int{10, 10, 25}, Mul: [3]float32{.65, .65, .75}, Trans: "none"}
}

func compileAfterImage(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	var args []argument
	for _, field := range []struct {
		key      string
		arity    int
		floating bool
	}{
		{"time", 1, false}, {"length", 1, false}, {"timegap", 1, false}, {"framegap", 1, false},
		{"palcolor", 1, false}, {"palinvertall", 1, false},
		{"palbright", 3, false}, {"palcontrast", 3, false}, {"palpostbright", 3, false},
		{"paladd", 3, false}, {"palmul", 3, true},
	} {
		raw, present := ctrl.Params[field.key]
		if !present {
			continue
		}
		tuple, err := evaluator.ParseReferenceTupleForVersion(raw, field.arity, ctrl.LanguageVersion)
		if err != nil {
			return nil, true, fmt.Errorf("%s: %w", field.key, err)
		}
		for i, expr := range tuple {
			key := field.key
			if field.arity > 1 {
				key += "." + strconv.Itoa(i)
			}
			if !field.floating && integerFieldFloatLiteral(expr) {
				return nil, true, fmt.Errorf("%s: expected int value and read float", key)
			}
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			// The beta parses/validates red brightness, but does not evaluate it.
			if field.key == "palbright" && i == 0 {
				continue
			}
			args = append(args, argument{argumentSpec{key, !field.floating, false}, expr})
		}
	}
	trans := strings.ToLower(strings.TrimSpace(ctrl.Params["trans"]))
	switch trans {
	case "add", "add1", "sub":
	default:
		// Unknown/quoted literals load and select the ordinary draw path.
		trans = "none"
	}
	return func(_ *cns.StateController, env Environment) error {
		if admission, ok := env.(interface{ CanSetAfterImage() (bool, error) }); ok {
			allowed, err := admission.CanSetAfterImage()
			if err != nil || !allowed {
				return err
			}
		}
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		p := DefaultAfterImageParameters()
		for key, dst := range map[string]*int{"time": &p.Time, "length": &p.Length, "timegap": &p.TimeGap,
			"framegap": &p.FrameGap, "palcolor": &p.Color} {
			if v, present := values[key]; present {
				*dst = int(v.Number())
			}
		}
		if v, present := values["palinvertall"]; present {
			p.Invert = v.Number() != 0
		}
		for key, dst := range map[string]*[3]int{"palbright": &p.Bright, "palcontrast": &p.Contrast,
			"palpostbright": &p.PostBright, "paladd": &p.Add} {
			for i := range dst {
				if v, present := values[key+"."+strconv.Itoa(i)]; present {
					dst[i] = int(v.Number())
				}
			}
		}
		for i := range p.Mul {
			if v, present := values["palmul."+strconv.Itoa(i)]; present {
				p.Mul[i] = float32(v.Number())
			}
		}
		p.Trans = trans
		ctx, ok := env.(interface {
			SetAfterImage(AfterImageParameters) error
		})
		if !ok {
			return fmt.Errorf("AfterImage recording context is unavailable")
		}
		return ctx.SetAfterImage(p)
	}, true, nil
}

func compileAfterImageTime(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	key := "time"
	if _, present := ctrl.Params[key]; !present {
		key = "value"
	}
	params := map[string]string{}
	if raw, present := ctrl.Params[key]; present {
		params["time"] = raw
	}
	probe := &cns.StateController{Params: params, LanguageVersion: ctrl.LanguageVersion}
	args, err := compileIntegerScalar(probe, "time")
	if err != nil {
		return nil, true, err
	}
	return func(_ *cns.StateController, env Environment) error {
		if history, ok := env.(interface{ HasAfterImage() bool }); ok && !history.HasAfterImage() {
			return nil
		}
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		duration := 0
		if v, present := values["time"]; present {
			duration = int(v.Number())
		}
		ctx, ok := env.(interface{ SetAfterImageTime(int) error })
		if !ok {
			return fmt.Errorf("AfterImageTime recording context is unavailable")
		}
		return ctx.SetAfterImageTime(duration)
	}, true, nil
}

func compileIntegerScalar(ctrl *cns.StateController, key string) ([]argument, error) {
	raw, present := ctrl.Params[key]
	if !present {
		return nil, nil
	}
	tuple, err := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
	if err != nil {
		return nil, fmt.Errorf("%s: %w", key, err)
	}
	expr := tuple[0]
	if integerFieldFloatLiteral(expr) {
		return nil, fmt.Errorf("%s: expected int value and read float", key)
	}
	if err := evaluator.ValidateKnownTriggers(expr); err != nil {
		return nil, fmt.Errorf("%s: %w", key, err)
	}
	return []argument{{argumentSpec{key, true, false}, expr}}, nil
}

func ExecuteAfterImage(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "AfterImage", Params: params, LanguageVersion: version}
	handler, _, err := compileAfterImage(ctrl)
	if err != nil {
		return err
	}
	return handler(ctrl, env)
}
