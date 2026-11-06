package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type ExplodParameters struct {
	Action, ID, RemoveTime, Facing, VFacing, SpritePriority    int
	PauseMoveTime, SuperMoveTime, BindID, BindTime             int
	X, Y, VelX, VelY, AccelX, AccelY, ScaleX, ScaleY           float64
	Common, ScreenSpace, IgnoreHitPause, RemoveOnGetHit, OnTop bool
	SpaceSpecified, Shadow                                     bool
	OwnPalette                                                 bool
	RemapPalette                                               [2]int
	PosType                                                    string
}

type ExplodBindTimeParameters struct {
	ID, Time int
}

func compileExplodBindTime(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	key := "time"
	if _, present := ctrl.Params[key]; !present {
		key = "value"
	}
	// The beta compiles only the selected alias, in ID/time order.
	args, err := compileArguments(ctrl, []argumentSpec{{"id", true, false}, {key, true, false}})
	if err != nil {
		return nil, true, err
	}
	for _, arg := range args {
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
		if integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		v, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		p := ExplodBindTimeParameters{ID: -1, Time: 1}
		if value, present := v["id"]; present {
			p.ID = int(value.Number())
		}
		if value, present := v[key]; present {
			p.Time = int(value.Number())
		}
		context, ok := env.(interface {
			SetExplodBindTime(ExplodBindTimeParameters) error
		})
		if !ok {
			return fmt.Errorf("ExplodBindTime context is unavailable")
		}
		return context.SetExplodBindTime(p)
	}, true, nil
}

// ExecuteExplodParameters preserves the legacy Environment entry point while
// executable state controllers use the cached compiler below.
func ExecuteExplodParameters(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "Explod", Params: params, LanguageVersion: version}
	fn, _, err := compileExplod(ctrl)
	if err != nil {
		return err
	}
	return fn(ctrl, env)
}

func compileExplod(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	raw, present := ctrl.Params["anim"]
	if !present {
		// Beta accepts omission, warns and falls back to action zero.
		raw = "-1"
	}
	raw = strings.TrimSpace(raw)
	common := len(raw) > 0 && (raw[0] == 'F' || raw[0] == 'f')
	if common {
		raw = raw[1:]
	}
	expr, err := evaluator.ParseReferenceForVersion(raw, ctrl.LanguageVersion)
	if err != nil {
		return nil, true, fmt.Errorf("anim: %w", err)
	}
	args := []argument{{argumentSpec{"anim", true, true}, expr}}
	paletteArgs, err := compileArguments(ctrl, []argumentSpec{{"ownpal", true, false}})
	if err != nil {
		return nil, true, err
	}
	args = append(args, paletteArgs...)
	if raw, present := ctrl.Params["remappal"]; present {
		tuple, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
		if err != nil {
			if single, singleErr := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion); singleErr == nil {
				tuple, err = append(single, &evaluator.Number{Value: 0}), nil
			}
		}
		if err != nil {
			return nil, true, fmt.Errorf("remappal: %w", err)
		}
		for index, expr := range tuple {
			args = append(args, argument{argumentSpec{fmt.Sprintf("remappal%d", index), true, true}, expr})
		}
	}
	// The beta evaluates palette fields before checking animation availability.
	// ID, facing, position and the later fields remain behind that gate.
	paletteEnd := len(args)
	scalars, err := compileArguments(ctrl, []argumentSpec{{"id", true, false}, {"facing", true, false}, {"vfacing", true, false}})
	if err != nil {
		return nil, true, err
	}
	args = append(args, scalars...)
	for _, key := range []string{"pos", "vel", "accel", "scale"} {
		if raw, present := ctrl.Params[key]; present {
			tuple, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
			if err != nil {
				return nil, true, fmt.Errorf("%s: %w", key, err)
			}
			for index, expr := range tuple {
				args = append(args, argument{argumentSpec{fmt.Sprintf("%s%d", key, index), false, true}, expr})
			}
		}
	}
	scalars, err = compileArguments(ctrl, []argumentSpec{{"bindtime", true, false}, {"bindid", true, false}, {"ontop", true, false}, {"removetime", true, false}, {"supermovetime", true, false}, {"pausemovetime", true, false}, {"sprpriority", true, false}, {"shadow", true, false}, {"removeongethit", true, false}})
	if err != nil {
		return nil, true, err
	}
	args = append(args, scalars...)
	for _, arg := range args {
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
		if arg.integer && integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
	}
	postype := "none"
	if ctrl.LanguageVersion != "" && ctrl.LanguageVersion != "1.1" {
		postype = "p1"
	}
	if raw, present := ctrl.Params["postype"]; present {
		postype = strings.ToLower(strings.Trim(strings.TrimSpace(raw), "\""))
	}
	switch postype {
	case "none", "p1", "p2", "front", "back", "left", "right":
	default:
		return nil, true, fmt.Errorf("invalid postype %q", postype)
	}
	screen := false
	_, explicitSpace := ctrl.Params["space"]
	if raw, present := ctrl.Params["space"]; present {
		switch strings.ToLower(strings.Trim(strings.TrimSpace(raw), "\"")) {
		case "stage":
		case "screen":
			screen = true
		default:
			return nil, true, fmt.Errorf("invalid space %q", raw)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		early, err := evaluateArgumentsWithBottom(args[:paletteEnd], env, true)
		if err != nil {
			return err
		}
		action := max(0, int(early["anim"].Number()))
		context, ok := env.(interface {
			CreateExplodEvaluated(int, bool, func() (ExplodParameters, error)) error
		})
		if !ok {
			return fmt.Errorf("Explod context is unavailable")
		}
		return context.CreateExplodEvaluated(action, common, func() (ExplodParameters, error) {
			v, err := evaluateArgumentsWithBottom(args[paletteEnd:], env, true)
			if err != nil {
				return ExplodParameters{}, err
			}
			integer := func(key string, fallback int) int {
				if value, present := v[key]; present {
					return int(value.Number())
				}
				return fallback
			}
			p := ExplodParameters{Action: action, ID: integer("id", 0), RemoveTime: integer("removetime", -2), Facing: integer("facing", 1), VFacing: integer("vfacing", 1), SpritePriority: integer("sprpriority", 0), PauseMoveTime: integer("pausemovetime", 0), SuperMoveTime: integer("supermovetime", 0), BindID: integer("bindid", -2), BindTime: integer("bindtime", 1), X: v["pos0"].Number(), Y: v["pos1"].Number(), VelX: v["vel0"].Number(), VelY: v["vel1"].Number(), AccelX: v["accel0"].Number(), AccelY: v["accel1"].Number(), ScaleX: 1, ScaleY: 1, Common: common, PosType: postype, ScreenSpace: screen, IgnoreHitPause: true, RemoveOnGetHit: integer("removeongethit", 0) != 0, OnTop: integer("ontop", 0) != 0, Shadow: integer("shadow", 0) != 0}
			if ctrl.IgnoreHitPauseSource != "" {
				p.IgnoreHitPause = ctrl.IgnoreHitPause != 0
			}
			p.SpaceSpecified = explicitSpace
			p.OwnPalette = common
			if value, present := early["ownpal"]; present {
				p.OwnPalette = value.Number() != 0
			}
			p.RemapPalette = [2]int{-1, 0}
			if value, present := early["remappal0"]; present {
				p.RemapPalette = [2]int{int(value.Number()), int(early["remappal1"].Number())}
			}
			if value, present := v["scale0"]; present {
				p.ScaleX, p.ScaleY = value.Number(), v["scale1"].Number()
			}
			return p, nil
		})
	}, true, nil
}
