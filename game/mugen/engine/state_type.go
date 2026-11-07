package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func compileStateTypeSet(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	values := map[string]string{}
	for _, field := range []struct{ name, allowed string }{{"statetype", "SCALU"}, {"movetype", "IAHU"}, {"physics", "SCANU"}} {
		if raw, present := ctrl.Params[field.name]; present {
			value := strings.ToUpper(strings.TrimSpace(raw))
			if len(value) != 1 || !strings.Contains(field.allowed, value) {
				return nil, true, fmt.Errorf("%s = %s: invalid literal state attribute", field.name, raw)
			}
			values[field.name] = value
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		if value := values["statetype"]; value != "" && value != "U" {
			env.SetStateType(value)
		}
		if value := values["movetype"]; value != "" && value != "U" {
			env.SetMoveType(value)
		}
		if value := values["physics"]; value != "" && value != "U" {
			env.SetPhysics(value)
		}
		return nil
	}, true, nil
}
