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
	SpaceSpecified                                             bool
	PosType                                                    string
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
		// Despite the manual's required label, the beta accepts omission and
		// creates no object. Still compile/evaluate the other parameters.
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
	scalars, err := compileArguments(ctrl, []argumentSpec{{"id", true, false}, {"removetime", true, false}, {"facing", true, false}, {"vfacing", true, false}, {"sprpriority", true, false}, {"pausemovetime", true, false}, {"supermovetime", true, false}, {"bindid", true, false}, {"bindtime", true, false}, {"ignorehitpause", true, false}, {"removeongethit", true, false}, {"ontop", true, false}})
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
		v, err := evaluateArgumentsWithBottom(args, env, true)
		if err != nil {
			return err
		}
		integer := func(key string, fallback int) int {
			if value, present := v[key]; present {
				return int(value.Number())
			}
			return fallback
		}
		p := ExplodParameters{Action: integer("anim", 0), ID: integer("id", 0), RemoveTime: integer("removetime", -2), Facing: integer("facing", 1), VFacing: integer("vfacing", 1), SpritePriority: integer("sprpriority", 0), PauseMoveTime: integer("pausemovetime", 0), SuperMoveTime: integer("supermovetime", 0), BindID: integer("bindid", -2), BindTime: integer("bindtime", 1), X: v["pos0"].Number(), Y: v["pos1"].Number(), VelX: v["vel0"].Number(), VelY: v["vel1"].Number(), AccelX: v["accel0"].Number(), AccelY: v["accel1"].Number(), ScaleX: 1, ScaleY: 1, Common: common, PosType: postype, ScreenSpace: screen, IgnoreHitPause: integer("ignorehitpause", 1) != 0, RemoveOnGetHit: integer("removeongethit", 0) != 0, OnTop: integer("ontop", 0) != 0}
		p.SpaceSpecified = explicitSpace
		if value, present := v["scale0"]; present {
			p.ScaleX, p.ScaleY = value.Number(), v["scale1"].Number()
		}
		context, ok := env.(interface{ CreateExplod(ExplodParameters) error })
		if !ok {
			return fmt.Errorf("Explod context is unavailable")
		}
		return context.CreateExplod(p)
	}, true, nil
}
