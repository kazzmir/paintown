package engine

import (
	"fmt"
	"math"
	"sort"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type argumentSpec struct {
	name              string
	integer, required bool
}
type argument struct {
	argumentSpec
	expr evaluator.Expression
}
type transitionEnvironment interface {
	Transition(int, EntryOverrides, bool) error
}
type parentEnvironment interface{ ParentEnvironment() Environment }
type defenceEnvironment interface{ GetDefenceMultiplier() float64 }

func compileArguments(ctrl *cns.StateController, specs []argumentSpec) ([]argument, error) {
	var args []argument
	for _, spec := range specs {
		raw, present := ctrl.Params[spec.name]
		if !present {
			if spec.required {
				return nil, fmt.Errorf("missing required parameter %s", spec.name)
			}
			continue
		}
		expr, err := evaluator.ParseReferenceForVersion(raw, ctrl.LanguageVersion)
		if err != nil {
			return nil, fmt.Errorf("%s = %s: %w", spec.name, raw, err)
		}
		args = append(args, argument{argumentSpec: spec, expr: expr})
	}
	return args, nil
}

func evaluateArguments(args []argument, env Environment) (map[string]evaluator.Value, error) {
	values := make(map[string]evaluator.Value, len(args))
	for _, arg := range args {
		value := evaluator.EvaluateValue(arg.expr, env)
		if !value.IsNumber() {
			return nil, fmt.Errorf("%s = %s evaluated to %v", arg.name, arg.expr.String(), value)
		}
		if arg.integer {
			n, err := evaluator.CoerceInt(value, env, arg.name)
			if err != nil {
				return nil, err
			}
			value = evaluator.Int(n)
		} else {
			value = evaluator.Float(float32(value.Number()))
		}
		values[arg.name] = value
	}
	return values, nil
}

func typedHandler(ctrl *cns.StateController, specs []argumentSpec, apply func(Environment, map[string]evaluator.Value) error) (ControllerFunc, bool, error) {
	args, err := compileArguments(ctrl, specs)
	if err != nil {
		return nil, true, err
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArguments(args, env)
		if err != nil {
			return err
		}
		return apply(env, values)
	}, true, nil
}

func compileCoreController(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	name := strings.ToLower(ctrl.Type)
	switch name {
	case "hitdef":
		return compileHitDef(ctrl)
	case "playsnd", "sndpan", "stopsnd":
		return compileSound(ctrl, name)
	case "playerpush":
		return typedHandler(ctrl, []argumentSpec{{"value", true, true}}, func(env Environment, values map[string]evaluator.Value) error {
			context, ok := env.(interface{ SetPlayerPush(bool) })
			if !ok {
				return fmt.Errorf("PlayerPush context is unavailable")
			}
			context.SetPlayerPush(values["value"].Bool())
			return nil
		})
	case "attackdist":
		return typedHandler(ctrl, []argumentSpec{{"value", true, true}}, func(env Environment, values map[string]evaluator.Value) error {
			context, ok := env.(interface{ SetAttackDistance(int) })
			if !ok {
				return fmt.Errorf("AttackDist context is unavailable")
			}
			context.SetAttackDistance(int(values["value"].Number()))
			return nil
		})
	case "hitby", "nothitby":
		return compileHitFilter(ctrl, name == "nothitby")
	case "hitoverride":
		return compileHitOverride(ctrl)
	case "remappal":
		return compileRemapPal(ctrl)
	case "varset", "varadd":
		return compileVariable(ctrl, name == "varadd", false)
	case "parentvarset", "parentvaradd":
		return compileVariable(ctrl, name == "parentvaradd", true)
	case "varrangeset":
		return compileVariableRange(ctrl)
	case "changeanim":
		return typedHandler(ctrl, []argumentSpec{{"value", true, true}, {"elem", true, false}}, func(env Environment, v map[string]evaluator.Value) error {
			element := 1
			if value, present := v["elem"]; present {
				element = int(value.Number())
			}
			if animation, ok := env.(interface{ ChangeAnimation(int, int) error }); ok {
				return animation.ChangeAnimation(int(v["value"].Number()), element)
			}
			if element != 1 {
				return fmt.Errorf("ChangeAnim element selection is unavailable")
			}
			env.ChangeAnim(int(v["value"].Number()))
			return nil
		})
	case "changestate", "selfstate":
		return typedHandler(ctrl, []argumentSpec{{"value", true, true}, {"ctrl", true, false}, {"anim", true, false}}, func(env Environment, v map[string]evaluator.Value) error {
			ctrlValue, hasCtrl := v["ctrl"]
			animValue, hasAnim := v["anim"]
			overrides := EntryOverrides{HasCtrl: hasCtrl, Ctrl: ctrlValue.Bool(), HasAnim: hasAnim, Anim: int(animValue.Number())}
			if transition, ok := env.(transitionEnvironment); ok {
				return transition.Transition(int(v["value"].Number()), overrides, name == "selfstate")
			}
			control, anim := -1, -1
			if hasCtrl {
				control = 0
				if ctrlValue.Bool() {
					control = 1
				}
			}
			if hasAnim {
				anim = overrides.Anim
			}
			if name == "selfstate" {
				env.SelfState(int(v["value"].Number()), control, anim)
			} else {
				env.ChangeState(int(v["value"].Number()), control, anim)
			}
			return nil
		})
	case "velset", "veladd", "velmul", "posset", "posadd":
		return typedHandler(ctrl, []argumentSpec{{"x", false, false}, {"y", false, false}}, func(env Environment, v map[string]evaluator.Value) error {
			x, y := 0.0, 0.0
			if name == "velset" {
				x, y = env.GetVel()
			}
			if name == "posset" {
				x, y = env.GetPos()
			}
			if name == "velmul" {
				x, y = 1, 1
			}
			if value, ok := v["x"]; ok {
				x = value.Number()
			}
			if value, ok := v["y"]; ok {
				y = value.Number()
			}
			switch name {
			case "velset":
				env.SetVelocity(x, y)
			case "veladd":
				env.AddVelocity(x, y)
			case "velmul":
				env.VelMul(x, y)
			case "posset":
				env.SetPosition(x, y)
			case "posadd":
				env.AddPosition(x*float64(env.GetFacing()), y)
			}
			return nil
		})
	case "lifeadd":
		return typedHandler(ctrl, []argumentSpec{{"value", true, true}, {"kill", true, false}, {"absolute", true, false}}, func(env Environment, v map[string]evaluator.Value) error {
			amount := int(v["value"].Number())
			absolute, hasAbsolute := v["absolute"]
			if !hasAbsolute || !absolute.Bool() {
				if defence, ok := env.(defenceEnvironment); ok {
					mul := defence.GetDefenceMultiplier()
					if mul <= 0 || math.IsNaN(mul) || math.IsInf(mul, 0) {
						return fmt.Errorf("invalid defence multiplier %g", mul)
					}
					amount = int(float64(amount) / mul)
				}
			}
			if kill, present := v["kill"]; present && !kill.Bool() && env.GetLife()+amount < 1 {
				amount = 1 - env.GetLife()
			}
			env.AddLife(amount)
			return nil
		})
	case "ctrlset", "lifeset", "powerset", "poweradd", "sprpriority", "hitadd":
		return typedHandler(ctrl, []argumentSpec{{"value", true, true}}, func(env Environment, v map[string]evaluator.Value) error {
			n := int(v["value"].Number())
			switch name {
			case "ctrlset":
				env.SetCtrl(n != 0)
			case "lifeset":
				env.SetLife(n)
			case "powerset":
				env.SetPower(n)
			case "poweradd":
				env.AddPower(n)
			case "sprpriority":
				env.SetSprPriority(n)
			case "hitadd":
				env.HitAdd(n)
			}
			return nil
		})
	case "victoryquote":
		return typedHandler(ctrl, []argumentSpec{{"value", true, false}}, func(env Environment, v map[string]evaluator.Value) error {
			quote := -1
			if value, present := v["value"]; present {
				quote = int(value.Number())
			}
			env.VictoryQuote(quote)
			return nil
		})
	case "hitvelset":
		return typedHandler(ctrl, []argumentSpec{{"x", true, false}, {"y", true, false}}, func(env Environment, v map[string]evaluator.Value) error {
			hit, ok := env.(interface{ GetHitVar(string) float64 })
			if !ok {
				return fmt.Errorf("HitVelSet requires hit data")
			}
			x, y := env.GetVel()
			if v["x"].Bool() {
				x = hit.GetHitVar("xvel")
			}
			if v["y"].Bool() {
				y = hit.GetHitVar("yvel")
			}
			env.SetVelocity(x, y)
			return nil
		})
	case "attackmulset", "defencemulset", "angleadd", "angleset", "anglemul":
		return typedHandler(ctrl, []argumentSpec{{"value", false, true}}, func(env Environment, v map[string]evaluator.Value) error {
			n := v["value"].Number()
			switch name {
			case "attackmulset":
				env.SetAttackMul(n)
			case "defencemulset":
				env.SetDefenceMul(n)
			case "angleadd":
				env.AngleAdd(n)
			case "angleset":
				env.AngleSet(n)
			case "anglemul":
				env.AngleMul(n)
			}
			return nil
		})
	case "posfreeze":
		return typedHandler(ctrl, []argumentSpec{{"value", true, false}}, func(env Environment, v map[string]evaluator.Value) error {
			value, present := v["value"]
			env.PosFreeze(!present || value.Bool())
			return nil
		})
	case "removeexplod":
		return typedHandler(ctrl, []argumentSpec{{"id", true, false}}, func(env Environment, v map[string]evaluator.Value) error {
			id := -1
			if value, present := v["id"]; present {
				id = int(value.Number())
			}
			env.RemoveExplod(id)
			return nil
		})
	}
	return nil, false, nil
}

func compileVariable(ctrl *cns.StateController, add, parent bool) (ControllerFunc, bool, error) {
	kind, indexText, valueText := "", "", ""
	for _, spec := range []struct{ key, kind string }{{"v", "var"}, {"fv", "fvar"}} {
		if raw, present := ctrl.Params[spec.key]; present {
			if kind != "" {
				return nil, true, fmt.Errorf("variable index forms are mutually exclusive")
			}
			kind = spec.kind
			indexText = raw
			valueText = ctrl.Params["value"]
		}
	}
	var keys []string
	for key := range ctrl.Params {
		keys = append(keys, key)
	}
	sort.Strings(keys)
	for _, key := range keys {
		lower := strings.ToLower(key)
		for _, prefix := range []string{"var(", "fvar(", "sysvar(", "sysfvar("} {
			if strings.HasPrefix(lower, prefix) && strings.HasSuffix(lower, ")") {
				if kind != "" {
					return nil, true, fmt.Errorf("variable index forms are mutually exclusive")
				}
				kind = strings.TrimSuffix(prefix, "(")
				indexText = key[len(prefix) : len(key)-1]
				valueText = ctrl.Params[key]
			}
		}
	}
	if kind == "" || valueText == "" {
		return nil, true, fmt.Errorf("variable controller requires an index and value")
	}
	if parent && strings.HasPrefix(kind, "sys") {
		return nil, true, fmt.Errorf("parent variable controllers cannot set system variables")
	}
	index, err := evaluator.ParseReferenceForVersion(indexText, ctrl.LanguageVersion)
	if err != nil {
		return nil, true, fmt.Errorf("variable index %s: %w", indexText, err)
	}
	value, err := evaluator.ParseReferenceForVersion(valueText, ctrl.LanguageVersion)
	if err != nil {
		return nil, true, fmt.Errorf("variable value %s: %w", valueText, err)
	}
	args := []argument{{argumentSpec{"index", true, true}, index}, {argumentSpec{"value", !strings.Contains(kind, "fvar"), true}, value}}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArguments(args, env)
		if err != nil {
			return err
		}
		idx := int(values["index"].Number())
		limit := 60
		if kind == "fvar" {
			limit = 40
		}
		if strings.HasPrefix(kind, "sys") {
			limit = 5
		}
		if idx < 0 || idx >= limit {
			return fmt.Errorf("%s index %d outside 0..%d", kind, idx, limit-1)
		}
		dest := env
		if parent {
			owner, ok := env.(parentEnvironment)
			if !ok || owner.ParentEnvironment() == nil {
				return nil
			}
			dest = owner.ParentEnvironment()
		}
		v := values["value"]
		switch kind {
		case "var":
			n := int32(v.Number())
			if add {
				n += int32(dest.GetVar(idx))
			}
			dest.SetVar(idx, int(n))
		case "sysvar":
			n := int32(v.Number())
			if add {
				n += int32(dest.GetSysVar(idx))
			}
			dest.SetSysVar(idx, int(n))
		case "fvar":
			n := float32(v.Number())
			if add {
				n += float32(dest.GetFVar(idx))
			}
			dest.SetFVar(idx, float64(n))
		case "sysfvar":
			n := float32(v.Number())
			if add {
				n += float32(dest.GetSysFVar(idx))
			}
			dest.SetSysFVar(idx, float64(n))
		}
		return nil
	}, true, nil
}

func compileVariableRange(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	_, ints := ctrl.Params["value"]
	_, floats := ctrl.Params["fvalue"]
	if ints == floats {
		return nil, true, fmt.Errorf("VarRangeSet requires exactly one of value or fvalue")
	}
	key := "value"
	last := 59
	if floats {
		key = "fvalue"
		last = 39
	}
	return typedHandler(ctrl, []argumentSpec{{"first", true, false}, {"last", true, false}, {key, !floats, true}}, func(env Environment, v map[string]evaluator.Value) error {
		first, end := 0, last
		if value, ok := v["first"]; ok {
			first = int(value.Number())
		}
		if value, ok := v["last"]; ok {
			end = int(value.Number())
		}
		if first < 0 || end > last {
			return fmt.Errorf("variable range %d..%d outside 0..%d", first, end, last)
		}
		value := v[key]
		for i := first; i <= end; i++ {
			if floats {
				env.SetFVar(i, value.Number())
			} else {
				env.SetVar(i, int(value.Number()))
			}
		}
		return nil
	})
}
