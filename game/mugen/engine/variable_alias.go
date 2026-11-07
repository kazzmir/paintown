package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

func validateVariableLiterals(args []argument) error {
	for _, arg := range args {
		if arg.integer && integerFieldFloatLiteral(arg.expr) {
			return fmt.Errorf("%s: expected int value and read float", arg.name)
		}
	}
	return nil
}

// These older port aliases are explicit extensions, not additional beta
// controller families. Route them through the same compiled float schemas so
// they cannot silently truncate expressions or bypass validation.
func compileFloatVariableAlias(ctrl *cns.StateController, add bool) (ControllerFunc, bool, error) {
	normalized := *ctrl
	normalized.Params = make(map[string]string, len(ctrl.Params))
	for key, value := range ctrl.Params {
		if strings.HasPrefix(strings.ToLower(key), "var(") || strings.HasPrefix(strings.ToLower(key), "sysvar(") {
			return nil, true, fmt.Errorf("float variable alias requires a float variable index")
		}
		if strings.EqualFold(key, "v") {
			key = "fv"
			if _, present := ctrl.Params["fv"]; present {
				return nil, true, fmt.Errorf("variable index forms are mutually exclusive")
			}
		}
		normalized.Params[key] = value
	}
	return compileVariable(&normalized, add, false)
}

func compileFloatRangeAlias(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	normalized := *ctrl
	normalized.Params = make(map[string]string, len(ctrl.Params))
	for key, value := range ctrl.Params {
		if key != "range" && key != "value" {
			normalized.Params[key] = value
		}
	}
	if raw, present := ctrl.Params["value"]; present {
		if _, also := ctrl.Params["fvalue"]; also {
			return nil, true, fmt.Errorf("float variable range requires one value")
		}
		normalized.Params["fvalue"] = raw
	}
	if raw, present := ctrl.Params["range"]; present {
		parts := parsers.SplitList(raw)
		if len(parts) != 2 {
			return nil, true, fmt.Errorf("range requires first,last")
		}
		if _, first := ctrl.Params["first"]; first {
			return nil, true, fmt.Errorf("range and first are mutually exclusive")
		}
		if _, last := ctrl.Params["last"]; last {
			return nil, true, fmt.Errorf("range and last are mutually exclusive")
		}
		normalized.Params["first"], normalized.Params["last"] = parts[0], parts[1]
	}
	return compileVariableRange(&normalized)
}
