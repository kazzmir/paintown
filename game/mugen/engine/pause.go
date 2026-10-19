package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compilePause(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	return typedHandler(ctrl, []argumentSpec{{"time", true, false}, {"movetime", true, false}, {"endcmdbuftime", true, false}, {"pausebg", true, false}}, func(env Environment, values map[string]evaluator.Value) error {
		context, ok := env.(interface{ PauseGame(PauseParameters) error })
		if !ok {
			return fmt.Errorf("Pause context is unavailable")
		}
		parameters := PauseParameters{Time: int(values["time"].Number()), PauseBackground: true}
		parameters.MoveTime = int(values["movetime"].Number())
		parameters.EndCommandBufferTime = int(values["endcmdbuftime"].Number())
		if value, present := values["pausebg"]; present {
			parameters.PauseBackground = value.Bool()
		}
		return context.PauseGame(parameters)
	})
}
