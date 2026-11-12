package engine

import (
	"fmt"
	"math"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileEnvironmentShake(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	var args []argument
	for _, key := range []string{"time", "ampl", "phase", "freq"} {
		if raw, ok := ctrl.Params[key]; ok {
			tuple, err := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
			if err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			expr := tuple[0]
			if (key == "time" || key == "ampl") && integerFieldFloatLiteral(expr) {
				return nil, true, fmt.Errorf("%s: expected int value and read float", key)
			}
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			// Runtime amplitudes retain fractions despite rejecting float literals.
			args = append(args, argument{argumentSpec{key, key == "time", false}, expr})
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		p := ShakeParameters{Amplitude: -4, Frequency: 60}
		if v, ok := values["time"]; ok {
			p.Time = int(v.Number())
		}
		if v, ok := values["ampl"]; ok {
			p.Amplitude = float32(v.Number())
		}
		if v, ok := values["freq"]; ok {
			p.Frequency = float32(math.Abs(v.Number()))
		}
		// Tiny nonzero magnitudes survive; only exact zero has this fallback.
		if p.Frequency == 0 {
			p.Frequency = .001
		}
		if v, ok := values["phase"]; ok {
			p.Phase = float32(v.Number())
		} else if p.Frequency >= 90 {
			p.Phase = 90
		}
		context, ok := env.(interface{ SetEnvironmentShake(ShakeParameters) error })
		if !ok {
			return fmt.Errorf("EnvShake global timeline context is unavailable")
		}
		return context.SetEnvironmentShake(p)
	}, true, nil
}

func ExecuteEnvironmentShake(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "EnvShake", Params: params, LanguageVersion: version}
	handler, _, err := compileEnvironmentShake(ctrl)
	if err != nil {
		return err
	}
	return handler(ctrl, env)
}

// RequestEnvironmentShake replaces this frame's pending request. Nonpositive
// durations discard that pending request while preserving an active shake.
func (t *Timeline) RequestEnvironmentShake(p ShakeParameters) {
	t.pendingShake = nil
	t.RequestShake(p)
}
