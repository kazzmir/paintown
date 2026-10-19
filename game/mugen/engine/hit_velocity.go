package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func compileHitVelSet(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	args, err := compileArguments(ctrl, []argumentSpec{{"x", true, false}, {"y", true, false}})
	if err != nil {
		return nil, true, err
	}
	for _, arg := range args {
		if integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		if actor, ok := env.(interface{ ApplyHitVelocity(bool, bool) }); ok {
			actor.ApplyHitVelocity(values["x"].Bool(), values["y"].Bool())
			return nil
		}
		character, ok := env.(interface{ GetHitVar(string) float64 })
		if !ok {
			return fmt.Errorf("HitVelSet requires hit data")
		}
		x, y := env.GetVel()
		if flag, present := values["x"]; present && flag.Bool() {
			// The trigger exposes world-oriented X; Vel X and the physics
			// setter use the receiver's facing-relative coordinate system.
			x = character.GetHitVar("xvel") * float64(env.GetFacing())
		}
		if flag, present := values["y"]; present && flag.Bool() {
			y = character.GetHitVar("yvel")
		}
		env.SetVelocity(x, y)
		return nil
	}, true, nil
}
