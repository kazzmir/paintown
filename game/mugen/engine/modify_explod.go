package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// Optional fields retain existing values when absent. Parsed but unevaluated
// beta fields (anim, facing, motion, ownpal, remappal, random, postype) are
// deliberately excluded from the mutation packet.
type ExplodModification struct {
	ScaleX, ScaleY, Angle, XAngle, YAngle                      *float64
	BindTime, BindID, RemoveTime, SuperMoveTime, PauseMoveTime *int
	SpritePriority, Shadow                                     *int
	OnTop, RemoveOnGetHit, IgnoreHitPause, SuperMove           *bool
	HasBlend                                                   bool
	Blend                                                      string
	AlphaSrc, AlphaDst                                         int
}

func compileModifyExplod(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	var args []argument
	for _, key := range []string{"id", "anim", "bindtime", "bindid", "removetime", "facing", "vfacing", "sprpriority", "pausemovetime", "supermovetime", "removeongethit", "ontop", "ownpal", "shadow", "supermove", "angle", "xangle", "yangle"} {
		raw, present := ctrl.Params[key]
		if !present {
			continue
		}
		if key == "anim" {
			raw = strings.TrimSpace(raw)
			if len(raw) > 0 && (raw[0] == 'F' || raw[0] == 'f') {
				raw = raw[1:]
			}
		}
		expr, err := evaluator.ParseReferenceForVersion(raw, ctrl.LanguageVersion)
		if err != nil {
			return nil, true, fmt.Errorf("%s: %w", key, err)
		}
		args = append(args, argument{argumentSpec{key, key != "angle" && key != "xangle" && key != "yangle", false}, expr})
	}
	trans := strings.ToLower(strings.Trim(strings.TrimSpace(ctrl.Params["trans"]), "\""))
	additive := trans == "add" || trans == "add1" || trans == "addalpha"
	readsAlpha := additive || trans == "none" || trans == "sub" || trans == "default"
	for _, key := range []string{"pos", "vel", "accel", "scale", "random", "remappal", "alpha"} {
		raw, present := ctrl.Params[key]
		if !present || key == "alpha" && !readsAlpha {
			continue
		}
		tuple, err := evaluator.ParseReferenceTupleForVersion(raw, 2, ctrl.LanguageVersion)
		if err != nil {
			tuple, err = evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion)
		}
		if err != nil {
			return nil, true, fmt.Errorf("%s: %w", key, err)
		}
		for i, expr := range tuple {
			args = append(args, argument{argumentSpec{fmt.Sprintf("%s%d", key, i), key == "random" || key == "remappal" || key == "alpha", false}, expr})
		}
	}
	if raw, present := ctrl.Params["postype"]; present {
		switch strings.ToLower(strings.Trim(strings.TrimSpace(raw), "\"")) {
		case "p1", "p2", "front", "back", "left", "right", "none":
		default:
			return nil, true, fmt.Errorf("invalid postype %q", raw)
		}
	}
	// Space is not read by the beta's ModifyExplod parser. Other ignored
	// fields still undergo expression/type validation at load time.
	for _, arg := range args {
		if err := evaluator.ValidateKnownTriggers(arg.expr); err != nil {
			return nil, true, fmt.Errorf("%s: %w", arg.name, err)
		}
		if arg.integer && integerFieldFloatLiteral(arg.expr) {
			return nil, true, fmt.Errorf("%s: expected int value and read float", arg.name)
		}
	}
	byName := make(map[string]argument, len(args))
	for _, arg := range args {
		byName[arg.name] = arg
	}
	return func(_ *cns.StateController, env Environment) error {
		read := func(key string) (evaluator.Value, bool, error) {
			arg, present := byName[key]
			if !present {
				return evaluator.Int(0), false, nil
			}
			v, err := evaluateArgumentsWithBottom([]argument{arg}, env, true)
			return v[key], true, err
		}
		id := -1
		if value, present, err := read("id"); err != nil {
			return err
		} else if present {
			id = int(value.Number())
		}
		context, ok := env.(interface {
			ModifyExplods(int, func(bool) (ExplodModification, error)) error
		})
		if !ok {
			return fmt.Errorf("ModifyExplod context is unavailable")
		}
		return context.ModifyExplods(id, func(onTop bool) (ExplodModification, error) {
			p := ExplodModification{}
			// Native evaluates this sequence separately for each matching object.
			for _, key := range []string{"scale0", "scale1", "angle", "yangle", "xangle", "bindtime", "bindid", "ontop", "removetime", "supermovetime", "supermove", "pausemovetime", "sprpriority", "shadow", "removeongethit", "alpha0", "alpha1"} {
				if key == "supermove" {
					if _, present := byName["supermovetime"]; present {
						continue
					}
				}
				if key == "sprpriority" && onTop {
					continue
				}
				v, present, err := read(key)
				if err != nil {
					return p, err
				}
				if !present {
					continue
				}
				f, n, b := v.Number(), int(v.Number()), v.Bool()
				switch key {
				case "scale0":
					p.ScaleX = &f
				case "scale1":
					p.ScaleY = &f
				case "angle":
					p.Angle = &f
				case "yangle":
					p.YAngle = &f
				case "xangle":
					p.XAngle = &f
				case "bindtime":
					p.BindTime = &n
				case "bindid":
					p.BindID = &n
				case "ontop":
					p.OnTop, onTop = &b, b
				case "removetime":
					p.RemoveTime = &n
				case "supermovetime":
					p.SuperMoveTime = &n
				case "supermove":
					p.SuperMove = &b
				case "pausemovetime":
					p.PauseMoveTime = &n
				case "sprpriority":
					p.SpritePriority = &n
				case "shadow":
					p.Shadow = &n
				case "removeongethit":
					p.RemoveOnGetHit = &b
				case "alpha0":
					p.AlphaSrc = max(0, min(256, n))
				case "alpha1":
					p.AlphaDst = max(0, min(256, n))
				}
			}
			if ctrl.IgnoreHitPauseSource != "" {
				b := ctrl.IgnoreHitPause != 0
				p.IgnoreHitPause = &b
			}
			if additive {
				p.HasBlend, p.Blend = true, "alpha"
				if _, present := byName["alpha0"]; !present {
					p.AlphaSrc = 256
				}
				if _, present := byName["alpha1"]; !present {
					switch trans {
					case "add":
						p.AlphaDst = 256
					case "add1":
						p.AlphaDst = 128
					}
				}
			} else if trans == "sub" || trans == "none" {
				p.HasBlend, p.Blend = true, trans
				p.AlphaSrc, p.AlphaDst = 256, 256
			}
			return p, nil
		})
	}, true, nil
}
