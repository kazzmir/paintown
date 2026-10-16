package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/combat"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileHitFilter(ctrl *cns.StateController, exclude bool) (ControllerFunc, bool, error) {
	raw, first := ctrl.Params["value"]
	second, other := ctrl.Params["value2"]
	if !first && !other {
		return nil, true, fmt.Errorf("value or value2 is required")
	}
	slot := 0
	// The beta accepts both and uses value, contrary to the manual's XOR rule.
	if !first {
		raw, slot = second, 1
	}
	attributes, err := combat.ParseProtectionAttributes(raw)
	if err != nil {
		return nil, true, err
	}
	if exclude {
		attributes = attributes.Complement()
	}
	return typedHandler(ctrl, []argumentSpec{{"time", true, false}}, func(env Environment, values map[string]evaluator.Value) error {
		time := 1
		if value, ok := values["time"]; ok {
			time = int(value.Number())
		}
		context, ok := env.(interface {
			SetHitFilter(int, combat.HitFilter) error
		})
		if !ok {
			return fmt.Errorf("hit protection context is unavailable")
		}
		return context.SetHitFilter(slot, combat.HitFilter{Attributes: attributes, Time: time})
	})
}
