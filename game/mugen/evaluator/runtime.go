package evaluator

import (
	"fmt"
	"math"
	"strings"
)

// These optional interfaces bind extensions to one evaluation environment.
// There are no process-global lookup or random-number hooks.
type IdentifierEnvironment interface{ LookupIdentifier(string) (Value, bool) }
type FunctionEnvironment interface {
	LookupFunction(string, []Value) (Value, bool)
}
type RedirectEnvironment interface {
	Redirect(string, Value, bool) EvaluatorEnvironment
}
type LegacyEnvironment interface {
	LookupLegacy(string, string, []Value, string) (Value, bool)
}
type WarningEnvironment interface{ EvaluationWarning(string) }
type VariableEnvironment interface {
	SetVar(int, int)
	SetFVar(int, float64)
}
type AnimationEnvironment interface {
	GetAnimElemTime(int) (int, bool)
	GetAnimElemNo(int) (int, bool)
}

type ConstantEnvironment interface {
	LookupConstant(string) (Value, bool)
}

type CoordinateEnvironment interface {
	LocalCoordinateWidth() int
}

func failure(env EvaluatorEnvironment, format string, args ...any) Value {
	if sink, ok := env.(WarningEnvironment); ok {
		sink.EvaluationWarning(fmt.Sprintf(format, args...))
	}
	return Value{}
}

func malformed(env EvaluatorEnvironment, message string) Value {
	err := fmt.Errorf("malformed expression: %s", message)
	if sink, ok := env.(interface{ EvaluationFailure(error) }); ok {
		sink.EvaluationFailure(err)
	}
	return Value{runtimeError: err.Error()}
}

func EvaluateValue(expr Expression, env EvaluatorEnvironment) Value {
	switch e := expr.(type) {
	case *Number:
		if e.IsFloat {
			return Float(float32(e.Value))
		}
		return Int(int32(e.Value))
	case *StringExpr:
		return Text(e.Value)
	case *Group:
		return EvaluateValue(e.Expression, env)
	case *Identifier:
		return identifier(e.Name, env)
	case *Unary:
		v := EvaluateValue(e.Expression, env)
		if v.RuntimeError() != nil {
			return v
		}
		if e.Operator == "~" && v.IsBottom() {
			return malformed(env, "bitwise not of bottom")
		}
		if v.IsBottom() {
			v = Int(0)
		}
		if !v.IsNumber() {
			return failure(env, "invalid operand for %s", e.Operator)
		}
		switch e.Operator {
		case "+":
			return v
		case "-":
			if v.kind == IntKind {
				return Int(-v.integer)
			}
			return Float(-v.real)
		case "!":
			return Bool(!v.Bool())
		case "~":
			if v.kind == IntKind {
				return Int(^v.integer)
			}
		}
		return failure(env, "invalid %s operand type", e.Operator)
	case *Binary:
		return binary(e, env)
	case *Call:
		return call(e, env)
	case *Redirect:
		resolver, ok := env.(RedirectEnvironment)
		if !ok {
			return failure(env, "redirection is unavailable: %s", e.Selector.String())
		}
		var name string
		var id Value
		hasID := false
		switch s := e.Selector.(type) {
		case *Identifier:
			name = strings.ToLower(s.Name)
		case *Call:
			name = strings.ToLower(s.Name)
			if len(s.Arguments) != 1 {
				return failure(env, "%s redirection expects one ID", name)
			}
			hasID = true
			id = EvaluateValue(s.Arguments[0], env)
			n, err := CoerceInt(id, env, name+" redirection ID")
			if err != nil {
				return failure(env, "%v", err)
			}
			id = Int(n)
		}
		dest := resolver.Redirect(name, id, hasID)
		if dest == nil {
			return failure(env, "redirection destination not found: %s", e.Selector.String())
		}
		return EvaluateValue(e.Trigger, dest)
	case *Legacy:
		return legacy(e, env)
	}
	return failure(env, "invalid expression")
}

func identifier(name string, env EvaluatorEnvironment) Value {
	name = normalizeIdentifier(name)
	switch name {
	case "pi":
		return Float(float32(math.Pi))
	case "e":
		return Float(float32(math.E))
	case "s", "c", "a", "l", "i", "h", "n":
		return Int(int32(enumValue(name)))
	}
	if context, ok := env.(IdentifierEnvironment); ok {
		if v, found := context.LookupIdentifier(name); found {
			return v
		}
	}
	if env == nil {
		return failure(env, "unknown identifier %s", name)
	}
	n := evaluateIdentifier(name, env)
	if n == math.MaxFloat64 {
		return failure(env, "unknown identifier %s", name)
	}
	switch name {
	case "pos x", "pos y", "vel x", "vel y", "vel z", "p2dist x", "p2dist y", "p2bodydist x", "p2bodydist y":
		return Float(float32(n))
	default:
		return Int(int32(n))
	}
}

func binary(e *Binary, env EvaluatorEnvironment) Value {
	if e.Operator == ":=" {
		return assignment(e, env)
	}
	if id, ok := e.Left.(*Identifier); ok && strings.EqualFold(id.Name, "command") {
		if text, ok := e.Right.(*StringExpr); ok && env != nil {
			matched := env.Command(text.Value)
			if e.Operator == "!=" {
				matched = !matched
			}
			return Bool(matched)
		}
	}
	left := EvaluateValue(e.Left, env)
	if left.RuntimeError() != nil {
		return left
	}
	if left.IsBottom() {
		left = Int(0)
	}
	if interval, ok := e.Right.(*Interval); ok {
		low := EvaluateValue(interval.Left, env)
		high := EvaluateValue(interval.Right, env)
		if low.RuntimeError() != nil {
			return low
		}
		if high.RuntimeError() != nil {
			return high
		}
		if low.IsBottom() {
			low = Int(0)
		}
		if high.IsBottom() {
			high = Int(0)
		}
		if !left.IsNumber() || !low.IsNumber() || !high.IsNumber() {
			return failure(env, "invalid interval operand")
		}
		if left.kind == FloatKind || low.kind == FloatKind || high.kind == FloatKind {
			left = Float(float32(left.Number()))
			low = Float(float32(low.Number()))
			high = Float(float32(high.Number()))
		}
		a, b, c := left.Number(), low.Number(), high.Number()
		in := (a > b || interval.LeftInc && a == b) && (a < c || interval.RightInc && a == c)
		if e.Operator == "!=" {
			in = !in
		}
		return Bool(in)
	}
	// Ordinary operators evaluate both operands; only Cond suppresses unused work.
	right := EvaluateValue(e.Right, env)
	if right.RuntimeError() != nil {
		return right
	}
	if right.IsBottom() {
		right = Int(0)
	}
	if left.kind == TextKind || right.kind == TextKind {
		if left.kind != TextKind || right.kind != TextKind {
			return failure(env, "string/numeric operand mismatch")
		}
		if e.Operator == "=" || e.Operator == "==" {
			return Bool(left.text == right.text)
		}
		if e.Operator == "!=" {
			return Bool(left.text != right.text)
		}
		return failure(env, "invalid string operator %s", e.Operator)
	}
	switch e.Operator {
	case "&&":
		return Bool(left.Bool() && right.Bool())
	case "||":
		return Bool(left.Bool() || right.Bool())
	case "^^":
		return Bool(left.Bool() != right.Bool())
	case "&", "|", "^", "%":
		if left.kind != IntKind || right.kind != IntKind {
			return failure(env, "%s requires integer operands", e.Operator)
		}
		switch e.Operator {
		case "&":
			return Int(left.integer & right.integer)
		case "|":
			return Int(left.integer | right.integer)
		case "^":
			return Int(left.integer ^ right.integer)
		case "%":
			if right.integer == 0 {
				return failure(env, "modulo by zero")
			}
			return Int(left.integer % right.integer)
		}
	}
	if left.kind == FloatKind || right.kind == FloatKind {
		a, b := float32(left.Number()), float32(right.Number())
		switch e.Operator {
		case "+":
			return floatingResult(env, float64(a+b))
		case "-":
			return floatingResult(env, float64(a-b))
		case "*":
			return floatingResult(env, float64(a*b))
		case "/":
			if b == 0 {
				return failure(env, "division by zero")
			}
			return floatingResult(env, float64(a/b))
		case "**":
			return floatingResult(env, math.Pow(float64(a), float64(b)))
		default:
			return comparison(e.Operator, float64(a), float64(b), env)
		}
	}
	a, b := left.integer, right.integer
	switch e.Operator {
	case "+":
		return Int(a + b)
	case "-":
		return Int(a - b)
	case "*":
		return Int(a * b)
	case "/":
		if b == 0 {
			return failure(env, "division by zero")
		}
		return Int(a / b)
	case "**":
		result := math.Pow(float64(a), float64(b))
		if a >= 0 && b >= 0 {
			if result > math.MaxInt32 {
				failure(env, "integer exponentiation overflow")
				return Int(math.MaxInt32)
			}
			return Int(int32(result))
		}
		return floatingResult(env, result)
	default:
		return comparison(e.Operator, float64(a), float64(b), env)
	}
}

func comparison(op string, a, b float64, env EvaluatorEnvironment) Value {
	switch op {
	case "=", "==":
		return Bool(a == b)
	case "!=":
		return Bool(a != b)
	case "<":
		return Bool(a < b)
	case ">":
		return Bool(a > b)
	case "<=":
		return Bool(a <= b)
	case ">=":
		return Bool(a >= b)
	}
	return failure(env, "unknown operator %s", op)
}

func assignment(e *Binary, env EvaluatorEnvironment) Value {
	dest, ok := env.(VariableEnvironment)
	if !ok {
		return failure(env, "variables are not writable")
	}
	variable, ok := e.Left.(*Call)
	if !ok || len(variable.Arguments) != 1 {
		return failure(env, "invalid assignment destination")
	}
	index := EvaluateValue(variable.Arguments[0], env)
	value := EvaluateValue(e.Right, env)
	if index.RuntimeError() != nil {
		return index
	}
	if value.RuntimeError() != nil {
		return value
	}
	indexNumber, indexErr := CoerceInt(index, env, "assignment index")
	if indexErr != nil || !value.IsNumber() {
		return failure(env, "invalid assignment operand")
	}
	idx := int(indexNumber)
	if strings.EqualFold(variable.Name, "var") && idx >= 0 && idx < 60 {
		n, ok := value.truncate()
		if !ok {
			return failure(env, "integer assignment out of range")
		}
		dest.SetVar(idx, int(n))
		return Int(n)
	}
	if strings.EqualFold(variable.Name, "fvar") && idx >= 0 && idx < 40 {
		v := Float(float32(value.Number()))
		dest.SetFVar(idx, v.Number())
		return v
	}
	return failure(env, "invalid assignment variable %s(%d)", variable.Name, idx)
}

func call(e *Call, env EvaluatorEnvironment) Value {
	name := normalizeIdentifier(e.Name)
	if name == "ifelse" || name == "cond" {
		if len(e.Arguments) != 3 {
			return failure(env, "%s expects three arguments", name)
		}
		condition := EvaluateValue(e.Arguments[0], env)
		if condition.RuntimeError() != nil {
			return condition
		}
		if name == "cond" {
			if !condition.IsNumber() {
				return Value{}
			}
			if condition.Bool() {
				return EvaluateValue(e.Arguments[1], env)
			}
			return EvaluateValue(e.Arguments[2], env)
		}
		yes := EvaluateValue(e.Arguments[1], env)
		if yes.RuntimeError() != nil {
			return yes
		}
		no := EvaluateValue(e.Arguments[2], env)
		if no.RuntimeError() != nil {
			return no
		}
		if !condition.IsNumber() && !condition.IsBottom() {
			return Value{}
		}
		if condition.Bool() {
			return yes
		}
		return no
	}
	args := make([]Value, len(e.Arguments))
	symbolic := name == "const" || name == "gethitvar" || name == "stagevar"
	for i, expr := range e.Arguments {
		if symbolic && i == 0 {
			if id, ok := expr.(*Identifier); ok {
				args[i] = Text(normalizeIdentifier(id.Name))
				continue
			}
		}
		args[i] = EvaluateValue(expr, env)
		if args[i].RuntimeError() != nil {
			return args[i]
		}
	}
	if context, ok := env.(FunctionEnvironment); ok {
		if value, found := context.LookupFunction(name, args); found {
			return value
		}
	}
	if name == "const" {
		return constant(e, args, env)
	}
	if name == "const240p" || name == "const480p" || name == "const720p" {
		if len(args) != 1 || !args[0].IsNumber() {
			return failure(env, "%s requires one numeric argument", name)
		}
		context, ok := env.(CoordinateEnvironment)
		if !ok || context.LocalCoordinateWidth() <= 0 {
			return failure(env, "%s requires a player coordinate space", name)
		}
		base := float32(320)
		if name == "const480p" {
			base = 640
		} else if name == "const720p" {
			base = 1280
		}
		return Float(float32(args[0].Number()) * float32(context.LocalCoordinateWidth()) / base)
	}
	if name == "gethitvar" {
		if len(args) != 1 || args[0].kind != TextKind {
			return failure(env, "GetHitVar expects a field")
		}
		who, ok := env.(CharacterEnvironment)
		if !ok {
			return failure(env, "GetHitVar is unavailable")
		}
		field := args[0].text
		switch field {
		case "xvel", "yvel", "yaccel", "fall.xvel", "fall.yvel":
			return Float(float32(who.GetHitVar(field)))
		case "animtype", "air.animtype", "groundtype", "fall", "fall.damage", "fall.recover", "hittime", "slidetime", "ctrltime", "damage", "guarded", "hitshaketime":
			return Int(int32(who.GetHitVar(field)))
		default:
			return failure(env, "unimplemented GetHitVar field %s", field)
		}
	}
	if len(args) == 1 && env != nil && integerArgumentTrigger(name) {
		index, err := CoerceInt(args[0], env, name+" argument")
		if err != nil {
			return failure(env, "%v", err)
		}
		{
			idx := int(index)
			switch name {
			case "var":
				if idx >= 0 && idx < 60 {
					return Int(int32(env.GetVar(idx)))
				}
			case "fvar":
				if idx >= 0 && idx < 40 {
					return Float(float32(env.GetFVar(idx)))
				}
			case "sysvar":
				if idx >= 0 && idx < 5 {
					return Int(int32(env.GetSysVar(idx)))
				}
			case "sysfvar":
				if idx >= 0 && idx < 5 {
					return Float(float32(env.GetSysFVar(idx)))
				}
			case "animexist", "selfanimexist":
				if who, ok := env.(CharacterEnvironment); ok {
					return Bool(who.HasAnim(idx))
				}
			case "animelemtime":
				if who, ok := env.(AnimationEnvironment); ok {
					n, valid := who.GetAnimElemTime(idx)
					if valid {
						return Int(int32(n))
					}
				}
			case "animelemno":
				if who, ok := env.(AnimationEnvironment); ok {
					n, valid := who.GetAnimElemNo(idx)
					if valid {
						return Int(int32(n))
					}
				}
			}
		}
	}
	for _, arg := range args {
		if !arg.IsNumber() {
			return failure(env, "invalid %s argument", name)
		}
	}
	if name == "log" {
		if len(args) != 2 {
			return failure(env, "Log expects base and argument")
		}
		base, n := float64(float32(args[0].Number())), float64(float32(args[1].Number()))
		if base <= 0 || base == 1 || n <= 0 {
			return failure(env, "Log domain error")
		}
		return floatingResult(env, math.Log(n)/math.Log(base))
	}
	if len(args) == 1 {
		v := args[0]
		n := v.Number()
		switch name {
		case "abs":
			if v.kind == IntKind {
				if v.integer < 0 {
					return Int(-v.integer)
				}
				return v
			}
			return Float(float32(math.Abs(n)))
		case "floor", "ceil":
			if name == "floor" {
				n = math.Floor(n)
			} else {
				n = math.Ceil(n)
			}
			if n < math.MinInt32 || n > math.MaxInt32 {
				return failure(env, "%s result outside int32 range", name)
			}
			return Int(int32(n))
		case "sin":
			return floatingResult(env, math.Sin(float64(float32(n))))
		case "cos":
			return floatingResult(env, math.Cos(float64(float32(n))))
		case "tan":
			return floatingResult(env, math.Tan(float64(float32(n))))
		case "asin":
			return floatingResult(env, math.Asin(float64(float32(n))))
		case "acos":
			return floatingResult(env, math.Acos(float64(float32(n))))
		case "atan":
			return floatingResult(env, math.Atan(float64(float32(n))))
		case "exp":
			return floatingResult(env, math.Exp(float64(float32(n))))
		case "ln":
			if n <= 0 {
				return failure(env, "Ln domain error")
			}
			return floatingResult(env, math.Log(float64(float32(n))))
		}
	}
	return failure(env, "unknown trigger or invalid arguments: %s", e.String())
}

func integerArgumentTrigger(name string) bool {
	switch name {
	case "var", "fvar", "sysvar", "sysfvar", "animexist", "selfanimexist", "animelemtime", "animelemno":
		return true
	}
	return false
}

// Typed hosts preserve each constant's declared numeric type. Older environment
// adapters retain their limited float-valued compatibility path.
func constant(e *Call, args []Value, env EvaluatorEnvironment) Value {
	if env == nil || len(args) != 1 || args[0].kind != TextKind {
		return failure(env, "Const expects an available constant")
	}
	if constants, ok := env.(ConstantEnvironment); ok {
		if value, found := constants.LookupConstant(args[0].text); found {
			return value
		}
		return failure(env, "unknown or unavailable constant %s", args[0].text)
	}
	if n, found := legacyConstant(args[0].text, env); found {
		return Float(float32(n))
	}
	return failure(env, "unknown constant %s", args[0].text)
}

func legacy(e *Legacy, env EvaluatorEnvironment) Value {
	args := make([]Value, len(e.Arguments))
	for i, arg := range e.Arguments {
		args[i] = EvaluateValue(arg, env)
	}
	if context, ok := env.(LegacyEnvironment); ok {
		if v, found := context.LookupLegacy(strings.ToLower(e.Name), e.Operator, args, e.ExtraOperator); found {
			return v
		}
	}
	switch strings.ToLower(e.Name) {
	case "timemod":
		if env == nil || len(args) != 2 || args[0].kind != IntKind || args[0].integer == 0 {
			return failure(env, "invalid TimeMod divisor")
		}
		return comparison(e.Operator, float64(int32(env.GetTime())%args[0].integer), args[1].Number(), env)
	case "animelem":
		animation, ok := env.(AnimationEnvironment)
		if !ok || len(args) == 0 || args[0].kind != IntKind {
			return failure(env, "AnimElem is unavailable")
		}
		elapsed, valid := animation.GetAnimElemTime(int(args[0].integer))
		if !valid {
			return failure(env, "invalid AnimElem index")
		}
		value := Int(0)
		op := "="
		if len(args) == 2 {
			value = args[1]
			op = e.ExtraOperator
		}
		result := comparison(op, float64(elapsed), value.Number(), env)
		if e.Operator == "!=" && !result.IsBottom() {
			return Bool(!result.Bool())
		}
		return result
	}
	return failure(env, "legacy trigger is unavailable: %s", e.Name)
}

func floatingResult(env EvaluatorEnvironment, n float64) Value {
	v := Float(float32(n))
	if v.IsBottom() {
		return failure(env, "NaN arithmetic result")
	}
	return v
}
