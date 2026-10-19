package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// ProjectileParameters keeps projectile state separate from its HitDef payload.
// Afterimage rendering and the shared stage-shadow renderer remain separate gates.
type ProjectileParameters struct {
	CoordinateWidth                                                    int // Active CNS owner's units, including foreign states.
	ID, Action, HitAnimation, RemoveAnimation, CancelAnimation         int
	RemoveTime, Hits, MissTime, Priority, SpritePriority               int
	EdgeBound, StageBound                                              int
	HeightBound                                                        [2]float64
	Offset, Velocity, RemoveVelocity, Accel, VelocityMultiplier, Scale [2]float64
	PauseMoveTime, SuperMoveTime                                       int
	Remove, Shadow, OwnPalette                                         bool
	RemapPalette                                                       [2]int
	PosType                                                            string
}

type projectileField struct {
	name     string
	count    int
	integer  bool
	defaults []float64
}

// Defaults and field shapes are separate from the measured execution order.
var projectileFields = []projectileField{
	{"projid", 1, true, []float64{0}}, {"projanim", 1, true, []float64{0}},
	{"projhitanim", 1, true, []float64{-1}}, {"projremanim", 1, true, nil}, {"projcancelanim", 1, true, nil},
	{"projscale", 2, false, []float64{1, 1}}, {"projremove", 1, true, []float64{1}}, {"projremovetime", 1, true, []float64{-1}},
	{"velocity", 2, false, []float64{0, 0}}, {"remvelocity", 2, false, []float64{0, 0}},
	{"accel", 2, false, []float64{0, 0}}, {"velmul", 2, false, []float64{1, 1}},
	{"projhits", 1, true, []float64{1}}, {"projmisstime", 1, true, []float64{0}},
	{"projpriority", 1, true, []float64{1}}, {"projsprpriority", 1, true, []float64{3}},
	{"projedgebound", 1, true, nil}, {"projstagebound", 1, true, nil}, {"projheightbound", 2, true, nil},
	{"offset", 2, true, []float64{0, 0}}, {"projshadow", 1, true, []float64{0}},
	{"supermovetime", 1, true, []float64{0}}, {"pausemovetime", 1, true, []float64{0}},
	{"ownpal", 1, true, []float64{0}}, {"remappal", 2, true, []float64{-1, 0}},
}

func ExecuteProjectileParameters(params map[string]string, env Environment, version string) error {
	ctrl := &cns.StateController{Type: "Projectile", Params: params, LanguageVersion: version}
	fn, _, err := compileProjectile(ctrl)
	if err != nil {
		return err
	}
	return fn(ctrl, env)
}

func compileProjectileArguments(ctrl *cns.StateController) ([]argument, string, error) {
	var args []argument
	// Source defaults are evaluated again at their own position in the beta's
	// order, rather than copying the result of the earlier expression.
	params := make(map[string]string, len(ctrl.Params))
	for key, value := range ctrl.Params {
		params[key] = value
	}
	for _, pair := range [][2]string{{"projremanim", "projhitanim"}, {"projcancelanim", "projremanim"}} {
		if _, present := params[pair[0]]; !present {
			if raw, ok := params[pair[1]]; ok {
				params[pair[0]] = raw
			}
		}
	}
	for _, field := range projectileFields {
		raw, present := params[field.name]
		if !present {
			continue
		}
		exprs, err := evaluator.ParseReferenceTupleForVersion(raw, field.count, ctrl.LanguageVersion)
		if err != nil && field.count == 2 {
			if one, singleErr := evaluator.ParseReferenceTupleForVersion(raw, 1, ctrl.LanguageVersion); singleErr == nil {
				if field.name == "projheightbound" {
					// An omitted high bound retains its profile-scaled default.
					exprs, err = one, nil
				} else {
					fallback := 0.0
					if field.name == "velmul" || field.name == "projscale" {
						fallback = 1
					}
					exprs, err = append(one, &evaluator.Number{Value: fallback}), nil
				}
			}
		}
		if err != nil {
			return nil, "", fmt.Errorf("%s: %w", field.name, err)
		}
		for i, expr := range exprs {
			if field.integer && integerFieldFloatLiteral(expr) {
				return nil, "", fmt.Errorf("%s: expected int value and read float", field.name)
			}
			if err := evaluator.ValidateKnownTriggers(expr); err != nil {
				return nil, "", fmt.Errorf("%s: %w", field.name, err)
			}
			args = append(args, argument{argumentSpec{fmt.Sprintf("%s%d", field.name, i), field.integer, false}, expr})
		}
	}
	postype := "p1"
	if raw, present := ctrl.Params["postype"]; present {
		raw = strings.ToLower(strings.TrimSpace(raw))
		switch {
		case strings.HasPrefix(raw, "p2"):
			postype = "p2"
		case strings.HasPrefix(raw, "p"):
			postype = "p1"
		case strings.HasPrefix(raw, "f"):
			postype = "front"
		case strings.HasPrefix(raw, "b"):
			postype = "back"
		case strings.HasPrefix(raw, "l"):
			postype = "left"
		case strings.HasPrefix(raw, "r"):
			postype = "right"
		default:
			return nil, "", fmt.Errorf("invalid Projectile postype %q", raw)
		}
	}
	return args, postype, nil
}

func projectileParameters(values map[string]evaluator.Value, postype string, width int) ProjectileParameters {
	fallback := make(map[string][]float64, len(projectileFields))
	for _, field := range projectileFields {
		fallback[field.name] = field.defaults
	}
	fallback["projedgebound"] = []float64{40 * float64(width) / 320}
	fallback["projstagebound"] = fallback["projedgebound"]
	fallback["projheightbound"] = []float64{-240 * float64(width) / 320, float64(width) / 320}
	scalar := func(key string, index int) float64 {
		if v, ok := values[fmt.Sprintf("%s%d", key, index)]; ok {
			return v.Number()
		}
		return fallback[key][index]
	}
	fallback["projremanim"] = []float64{scalar("projhitanim", 0)}
	fallback["projcancelanim"] = []float64{scalar("projremanim", 0)}
	n := func(key string) int { return int(scalar(key, 0)) }
	pair := func(key string) [2]float64 { return [2]float64{scalar(key, 0), scalar(key, 1)} }
	return ProjectileParameters{ID: max(0, n("projid")), Action: n("projanim"), HitAnimation: n("projhitanim"), RemoveAnimation: n("projremanim"), CancelAnimation: n("projcancelanim"), RemoveTime: n("projremovetime"), Hits: n("projhits"), MissTime: n("projmisstime"), Priority: n("projpriority"), SpritePriority: n("projsprpriority"), EdgeBound: n("projedgebound"), StageBound: n("projstagebound"), HeightBound: pair("projheightbound"), Offset: pair("offset"), Velocity: pair("velocity"), RemoveVelocity: pair("remvelocity"), Accel: pair("accel"), VelocityMultiplier: pair("velmul"), Scale: pair("projscale"), PauseMoveTime: n("pausemovetime"), SuperMoveTime: n("supermovetime"), Remove: n("projremove") != 0, Shadow: n("projshadow") != 0, OwnPalette: n("ownpal") != 0, RemapPalette: [2]int{int(scalar("remappal", 0)), int(scalar("remappal", 1))}, PosType: postype}
}

func compileProjectile(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	args, postype, err := compileProjectileArguments(ctrl)
	if err != nil {
		return nil, true, err
	}
	hit, _, err := compileHitDefValues(ctrl)
	if err != nil {
		return nil, true, err
	}
	// Tagged beta output places its HitDef evaluation between scale and offset.
	order := []string{"projid", "projremove", "projremovetime", "projshadow", "projmisstime", "projhits", "projpriority", "projhitanim", "projremanim", "projcancelanim", "velocity", "velmul", "remvelocity", "accel", "projscale", "offset", "projsprpriority", "projstagebound", "projedgebound", "projheightbound", "projanim", "pausemovetime", "supermovetime", "ownpal", "remappal"}
	byName := map[string][]argument{}
	for _, arg := range args {
		name := arg.name[:len(arg.name)-1]
		byName[name] = append(byName[name], arg)
	}
	var before, after []argument
	for i, name := range order {
		if i < 15 {
			before = append(before, byName[name]...)
		} else {
			after = append(after, byName[name]...)
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArgumentsWithBottom(before, env, true)
		if err != nil {
			return err
		}
		hitValues, err := hit(env)
		if err != nil {
			return err
		}
		post, err := evaluateArgumentsWithBottom(after, env, true)
		if err != nil {
			return err
		}
		for key, value := range post {
			values[key] = value
		}
		width := 320
		if context, ok := env.(interface{ LocalCoordinateWidth() int }); ok && context.LocalCoordinateWidth() > 0 {
			width = context.LocalCoordinateWidth()
		}
		context, ok := env.(interface {
			CreateProjectile(ProjectileParameters, map[string]string) error
		})
		if !ok {
			return fmt.Errorf("Projectile context is unavailable")
		}
		parameters := projectileParameters(values, postype, width)
		parameters.CoordinateWidth = width
		return context.CreateProjectile(parameters, hitValues)
	}, true, nil
}
