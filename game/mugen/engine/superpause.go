package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type SuperPauseParameters struct {
	PauseParameters
	Animation, SoundGroup, SoundItem int
	OwnAnimation, OwnSound           bool
	Position                         [2]float64
	Darken, Unhittable               bool
	PowerAdd                         int
	TargetDefenceMul                 float32
}

func compileSuperPause(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	args, err := compileArguments(ctrl, []argumentSpec{{"time", true, false}, {"movetime", true, false}, {"endcmdbuftime", true, false}, {"pausebg", true, false},
		{"darken", true, false}, {"unhittable", true, false}, {"poweradd", true, false}, {"p2defmul", false, false}})
	if err != nil {
		return nil, true, err
	}
	ownAnimation, ownSound := false, false
	for _, field := range []struct {
		key   string
		count int
	}{{"anim", 1}, {"sound", 2}, {"pos", 2}} {
		raw, present := ctrl.Params[field.key]
		if !present {
			continue
		}
		raw = strings.TrimSpace(raw)
		if field.key != "pos" && len(raw) > 0 {
			switch raw[0] {
			case 'S', 's':
				if field.key == "anim" {
					ownAnimation = true
				} else {
					ownSound = true
				}
				raw = raw[1:]
			case 'F', 'f':
				raw = raw[1:]
			}
		}
		expressions, parseErr := evaluator.ParseReferenceTupleForVersion(raw, field.count, ctrl.LanguageVersion)
		if parseErr != nil && field.key == "sound" {
			if single, singleErr := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion); singleErr == nil {
				expressions, parseErr = append(single, &evaluator.Number{Value: 0}), nil
			}
		}
		if parseErr != nil {
			return nil, true, fmt.Errorf("%s: %w", field.key, parseErr)
		}
		for index, expression := range expressions {
			if err := evaluator.ValidateKnownTriggers(expression); err != nil {
				return nil, true, fmt.Errorf("%s: %w", field.key, err)
			}
			args = append(args, argument{argumentSpec{fmt.Sprintf("%s%d", field.key, index), field.key != "pos", true}, expression})
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArguments(args, env)
		if err != nil {
			return err
		}
		context, ok := env.(interface {
			SuperPauseGame(SuperPauseParameters) error
		})
		if !ok {
			return fmt.Errorf("SuperPause context is unavailable")
		}
		parameters := SuperPauseParameters{PauseParameters: PauseParameters{Time: 30, PauseBackground: true}, Animation: 30, SoundGroup: -1,
			OwnAnimation: ownAnimation, OwnSound: ownSound, Darken: true, Unhittable: true}
		for _, field := range []struct {
			key    string
			target *int
		}{{"time", &parameters.Time}, {"movetime", &parameters.MoveTime}, {"endcmdbuftime", &parameters.EndCommandBufferTime}, {"poweradd", &parameters.PowerAdd},
			{"anim0", &parameters.Animation}, {"sound0", &parameters.SoundGroup}, {"sound1", &parameters.SoundItem}} {
			if value, present := values[field.key]; present {
				*field.target = int(value.Number())
			}
		}
		for _, field := range []struct {
			key    string
			target *bool
		}{{"pausebg", &parameters.PauseBackground}, {"darken", &parameters.Darken}, {"unhittable", &parameters.Unhittable}} {
			if value, present := values[field.key]; present {
				*field.target = value.Bool()
			}
		}
		parameters.TargetDefenceMul = float32(values["p2defmul"].Number())
		parameters.Position = [2]float64{values["pos0"].Number(), values["pos1"].Number()}
		return context.SuperPauseGame(parameters)
	}, true, nil
}
