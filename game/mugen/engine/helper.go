package engine

import (
	"fmt"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type HelperParameters struct {
	ID, StateNo, Facing, PauseMoveTime, SuperMoveTime int
	X, Y                                              float64
	KeyControl, OwnPalette, Named                     bool
	Type, PosType, Name                               string
	RemapPalette                                      [2]int
	Size                                              map[string][]float64
}

var helperSizeFields = []struct {
	name    string
	count   int
	integer bool
}{
	{"size.xscale", 1, false}, {"size.yscale", 1, false},
	{"size.ground.back", 1, true}, {"size.ground.front", 1, true},
	{"size.air.back", 1, true}, {"size.air.front", 1, true},
	{"size.height", 1, true}, {"size.proj.doscale", 1, true},
	{"size.head.pos", 2, true}, {"size.mid.pos", 2, true}, {"size.shadowoffset", 1, true},
}

func ExecuteHelperParameters(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "Helper", Params: params, LanguageVersion: version}
	fn, _, err := compileHelper(ctrl)
	if err != nil {
		return err
	}
	return fn(ctrl, env)
}

func compileHelper(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	args, err := compileArguments(ctrl, []argumentSpec{{"id", true, false}, {"stateno", true, false}, {"facing", true, false}, {"keyctrl", true, false}, {"ownpal", true, false}, {"pausemovetime", true, false}, {"supermovetime", true, false}})
	if err != nil {
		return nil, true, err
	}
	tuple := func(name string, count int, integer bool) error {
		raw, present := ctrl.Params[name]
		if !present {
			return nil
		}
		values, err := evaluator.ParseReferenceTupleForVersion(raw, count, ctrl.LanguageVersion)
		if err != nil && count == 2 {
			if single, e := evaluator.ParseReferenceTupleForVersion(strings.TrimSpace(strings.TrimSuffix(strings.TrimSpace(raw), ",")), 1, ctrl.LanguageVersion); e == nil {
				values, err = append(single, &evaluator.Number{Value: 0}), nil
			}
		}
		if err != nil {
			return fmt.Errorf("%s: %w", name, err)
		}
		for i, value := range values {
			args = append(args, argument{argumentSpec{fmt.Sprintf("%s%d", name, i), integer, false}, value})
		}
		return nil
	}
	if err := tuple("pos", 2, false); err != nil {
		return nil, true, err
	}
	if err := tuple("remappal", 2, true); err != nil {
		return nil, true, err
	}
	for _, field := range helperSizeFields {
		if err := tuple(field.name, field.count, field.integer); err != nil {
			return nil, true, err
		}
	}
	for _, arg := range args {
		if arg.integer && integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
	}
	enum := func(name, fallback string) string {
		if raw, present := ctrl.Params[name]; present {
			return strings.ToLower(strings.TrimSpace(raw))
		}
		return fallback
	}
	helperType, posType := enum("helpertype", "normal"), enum("postype", "p1")
	if helperType != "normal" && helperType != "player" {
		return nil, true, fmt.Errorf("illegal helper type %q", helperType)
	}
	switch {
	case strings.HasPrefix(posType, "p2"):
		posType = "p2"
	case strings.HasPrefix(posType, "p"):
		posType = "p1"
	case strings.HasPrefix(posType, "f"):
		posType = "front"
	case strings.HasPrefix(posType, "b"):
		posType = "back"
	case strings.HasPrefix(posType, "l"):
		posType = "left"
	case strings.HasPrefix(posType, "r"):
		posType = "right"
	default:
		return nil, true, fmt.Errorf("invalid Helper postype %q", posType)
	}
	name, named := ctrl.Params["name"]
	if named {
		name = strings.TrimSpace(name)
		if strings.HasPrefix(name, "\"") {
			name, err = strconv.Unquote(name)
			if err != nil {
				return nil, true, fmt.Errorf("name: %w", err)
			}
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		integer := func(key string, fallback int) int {
			if value, present := values[key]; present {
				return int(value.Number())
			}
			return fallback
		}
		p := HelperParameters{ID: integer("id", 0), StateNo: integer("stateno", 0), Facing: integer("facing", 1), KeyControl: integer("keyctrl", 0) != 0, OwnPalette: integer("ownpal", 0) != 0, PauseMoveTime: integer("pausemovetime", 0), SuperMoveTime: integer("supermovetime", 0), X: values["pos0"].Number(), Y: values["pos1"].Number(), Type: helperType, PosType: posType, Name: name, Named: named, RemapPalette: [2]int{integer("remappal0", -1), integer("remappal1", 0)}, Size: map[string][]float64{}}
		for _, field := range helperSizeFields {
			if _, present := ctrl.Params[field.name]; !present {
				continue
			}
			for i := 0; i < field.count; i++ {
				p.Size[field.name] = append(p.Size[field.name], values[fmt.Sprintf("%s%d", field.name, i)].Number())
			}
		}
		context, ok := env.(interface{ CreateHelper(HelperParameters) error })
		if !ok {
			return fmt.Errorf("Helper context is unavailable")
		}
		return context.CreateHelper(p)
	}, true, nil
}
