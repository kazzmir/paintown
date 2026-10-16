package engine

import (
	"fmt"
	"math"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
	"github.com/kazzmir/paintown/game/mugen/sound"
)

type soundEnvironment interface {
	PlaySound(sound.PlayParameters) error
	PanSound(channel, offset int, absolute bool) error
	StopSound(channel int) error
}

func compileSound(ctrl *cns.StateController, name string) (ControllerFunc, bool, error) {
	var args []argument
	common := false
	if name == "playsnd" {
		raw, present := ctrl.Params["value"]
		if !present {
			return nil, true, fmt.Errorf("PlaySnd requires value")
		}
		raw = strings.TrimSpace(raw)
		// The beta consumes a leading F/S before parsing the expression, even
		// in Facing/FVar. Parentheses protect an identifier from this prefix.
		if len(raw) > 0 {
			switch raw[0] {
			case 'F', 'f':
				common = true
				raw = raw[1:]
			case 'S', 's':
				raw = raw[1:]
			}
		}
		tuple, err := evaluator.ParseReferenceTuple(raw, 2)
		if err != nil {
			return nil, true, fmt.Errorf("value: %w", err)
		}
		for index, expr := range tuple {
			args = append(args, argument{argumentSpec{fmt.Sprintf("value%d", index), true, true}, expr})
		}
	}
	specs := []argumentSpec{{"channel", true, name != "playsnd"}}
	if name != "stopsnd" {
		specs = append(specs, argumentSpec{"pan", true, false}, argumentSpec{"abspan", true, false})
	}
	if name == "playsnd" {
		specs = append(specs, argumentSpec{"volumescale", false, false}, argumentSpec{"freqmul", false, false}, argumentSpec{"loop", true, false}, argumentSpec{"lowpriority", true, false}, argumentSpec{"volume", true, false})
	}
	scalars, err := compileArguments(ctrl, specs)
	if err != nil {
		return nil, true, err
	}
	args = append(args, scalars...)
	_, absolute := ctrl.Params["abspan"]
	_, relativeValue := ctrl.Params["pan"]
	panKey := "abspan"
	if relativeValue || !absolute {
		panKey = "pan"
	}
	var active []argument
	for _, arg := range args {
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
		// Both offsets are accepted: pan supplies the value when present,
		// while abspan's presence still selects the absolute mode.
		if (arg.name == "pan" || arg.name == "abspan") && arg.name != panKey {
			continue
		}
		active = append(active, arg)
	}
	return func(_ *cns.StateController, env Environment) error {
		values := map[string]evaluator.Value{}
		for _, arg := range active {
			if arg.name == "volume" {
				// The beta evaluates deprecated volume (including assignments
				// and bottom) but discards its result. It never changes gain.
				evaluator.EvaluateValue(arg.expr, env)
				continue
			}
			value, err := evaluateArguments([]argument{arg}, env)
			if err != nil {
				return err
			}
			values[arg.name] = value[arg.name]
		}
		context, ok := env.(soundEnvironment)
		if !ok {
			return fmt.Errorf("sound command context is unavailable")
		}
		channel := -1
		if value, present := values["channel"]; present {
			channel = int(value.Number())
		}
		minimum := -1
		if name == "sndpan" {
			minimum = 0
		}
		if channel < minimum || channel > 31 {
			if warnings, ok := env.(evaluator.WarningEnvironment); ok {
				warnings.EvaluationWarning(fmt.Sprintf("%s channel %d outside %d..31", ctrl.Type, channel, minimum))
			}
			return nil
		}
		if name == "stopsnd" {
			return context.StopSound(channel)
		}
		pan := 0
		if value, present := values[panKey]; present {
			pan = int(value.Number())
		}
		if name == "sndpan" {
			return context.PanSound(channel, pan, absolute)
		}
		request := sound.PlayParameters{Common: common, Group: int(values["value0"].Number()), Item: int(values["value1"].Number()), Channel: channel, VolumeScale: 100, Frequency: 1, Pan: pan, Absolute: absolute}
		if value, present := values["volumescale"]; present {
			request.VolumeScale = float32(value.Number())
		}
		if value, present := values["freqmul"]; present {
			request.Frequency = float32(value.Number())
		}
		if value, present := values["loop"]; present {
			request.Loop = value.Bool()
		}
		if value, present := values["lowpriority"]; present {
			request.LowPriority = value.Bool()
		}
		if math.IsNaN(float64(request.VolumeScale)) || math.IsInf(float64(request.VolumeScale), 0) {
			if warnings, ok := env.(evaluator.WarningEnvironment); ok {
				warnings.EvaluationWarning("PlaySnd invalid volume/frequency")
			}
			return nil
		}
		if math.IsNaN(float64(request.Frequency)) || math.IsInf(float64(request.Frequency), 0) || request.Frequency <= 0 {
			if warnings, ok := env.(evaluator.WarningEnvironment); ok {
				warnings.EvaluationWarning("PlaySnd invalid frequency multiplier")
			}
		}
		// Literal 1.5 and 2 still play at the source pitch in this beta.
		request.Frequency = 1
		return context.PlaySound(request)
	}, true, nil
}
