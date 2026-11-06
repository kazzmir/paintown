package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func compileAssertSpecial(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	flags := make([]string, 0, 3)
	for _, key := range []string{"flag", "flag2", "flag3"} {
		raw, present := ctrl.Params[key]
		if !present {
			if key == "flag" {
				return nil, true, fmt.Errorf("missing required parameter flag")
			}
			continue
		}
		flag := strings.ToLower(strings.TrimSpace(raw))
		switch flag {
		case "intro", "invisible", "roundnotover", "nobardisplay", "nobg", "nofg",
			"nostandguard", "nocrouchguard", "noairguard", "noautoturn", "nojugglecheck",
			"nokosnd", "nokoslow", "noshadow", "globalnoshadow", "nomusic", "nowalk",
			"timerfreeze", "unguardable", "noko":
			flags = append(flags, flag)
		default:
			return nil, true, fmt.Errorf("%s = %s: invalid AssertSpecial flag", key, raw)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		for _, flag := range flags {
			env.AssertSpecial(flag)
		}
		return nil
	}, true, nil
}
