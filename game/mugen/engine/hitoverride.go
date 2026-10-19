package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/combat"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func compileHitOverride(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	// The supplied beta accepts omitted attr/stateno as an inactive override,
	// despite the manual labeling them required. Stock clearing uses this form.
	raw := ctrl.Params["attr"]
	attributes, err := combat.ParseAttributes(raw)
	if err != nil {
		return nil, true, err
	}
	return typedHandler(ctrl, []argumentSpec{{"stateno", true, false}, {"slot", true, false}, {"time", true, false}, {"forceair", true, false}}, func(env Environment, values map[string]evaluator.Value) error {
		override := combat.HitOverride{Attributes: attributes, StateNo: -1, Time: 1}
		if value, ok := values["stateno"]; ok {
			override.StateNo = int(value.Number())
		}
		if value, ok := values["slot"]; ok {
			override.Slot = int(value.Number())
		}
		if value, ok := values["time"]; ok {
			override.Time = int(value.Number())
		}
		if value, ok := values["forceair"]; ok {
			override.ForceAir = value.Bool()
		}
		if override.Slot < 0 || override.Slot > 7 || override.Time < -1 {
			return fmt.Errorf("invalid HitOverride slot/time")
		}
		context, ok := env.(interface {
			SetHitOverride(combat.HitOverride) error
		})
		if !ok {
			return fmt.Errorf("HitOverride context is unavailable")
		}
		return context.SetHitOverride(override)
	})
}
