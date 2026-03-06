package evaluator

import (
	"math"
	"strings"
)

// EvaluatorEnvironment provides the runtime data needed to evaluate MUGEN expressions.
type EvaluatorEnvironment interface {
	GetTime() int
	GetAnimTime() int
	GetPos() (float64, float64)
	GetVel() (float64, float64)
	GetVar(idx int) int
	GetFVar(idx int) float64
	GetStateNo() int
	GetCtrl() bool
	GetStateType() string
	GetMoveType() string
	GetPhysics() string
	GetAnim() int
	GetAnimElem() int
	Command(name string) bool
}

// Environment is an alias to EvaluatorEnvironment for easier access
type Environment = EvaluatorEnvironment

var customEval func(string, EvaluatorEnvironment) float64

// SetCustomEvaluation sets a global hook for extended identifier evaluation.
func SetCustomEvaluation(f func(string, EvaluatorEnvironment) float64) {
	customEval = f
}

// Evaluate recursively computes the value of an expression.
// In MUGEN, most expressions return a float64 (or bool represented as 0/1).
func Evaluate(expr Expression, env EvaluatorEnvironment) float64 {
	switch e := expr.(type) {
	case *Number:
		return e.Value
	case *Identifier:
		res := evaluateIdentifier(strings.ToLower(e.Name), env)
		if res == math.MaxFloat64 && customEval != nil {
			res = customEval(e.Name, env)
		}
		return res
	case *Unary:
		val := Evaluate(e.Expression, env)
		switch e.Operator {
		case "!":
			if val == 0 {
				return 1
			}
			return 0
		case "-":
			return -val
		case "~":
			return float64(^int64(val))
		}
	case *Binary:
		// Special case for command = "string"
		if id, ok := e.Left.(*Identifier); ok && strings.ToLower(id.Name) == "command" {
			if str, ok := e.Right.(*StringExpr); ok {
				if e.Operator == "=" || e.Operator == "==" {
					if env.Command(str.Value) {
						return 1
					}
					return 0
				}
				if e.Operator == "!=" {
					if !env.Command(str.Value) {
						return 1
					}
					return 0
				}
			}
		}

		left := Evaluate(e.Left, env)
		right := Evaluate(e.Right, env)

		// Hard fix for unknown identifiers defaulting to 0
		if left == math.MaxFloat64 || right == math.MaxFloat64 {
			return 0
		}
		switch e.Operator {
		case "=":
			if left == right {
				return 1
			}
			return 0
		case "!=":
			if left != right {
				return 1
			}
			return 0
		case ">":
			if left > right {
				return 1
			}
			return 0
		case ">=":
			if left >= right {
				return 1
			}
			return 0
		case "<":
			if left < right {
				return 1
			}
			return 0
		case "<=":
			if left <= right {
				return 1
			}
			return 0
		case "+":
			return left + right
		case "-":
			return left - right
		case "*":
			return left * right
		case "/":
			if right == 0 {
				return 0
			}
			return left / right
		case "%":
			return float64(int64(left) % int64(right))
		case "&&":
			if left != 0 && right != 0 {
				return 1
			}
			return 0
		case "||":
			if left != 0 || right != 0 {
				return 1
			}
			return 0
		case "**":
			return math.Pow(left, right)
		}
	case *Call:
		return evaluateCall(e, env)
	}
	return 0
}

func evaluateIdentifier(name string, env EvaluatorEnvironment) float64 {
	switch name {
	case "time":
		return float64(env.GetTime())
	case "animtime":
		return float64(env.GetAnimTime())
	case "animelem":
		return float64(env.GetAnimElem())
	case "anim":
		return float64(env.GetAnim())
	case "stateno":
		return float64(env.GetStateNo())
	case "ctrl":
		if env.GetCtrl() {
			return 1
		}
		return 0
	case "statetype":
		switch strings.ToUpper(env.GetStateType()) {
		case "S":
			return 1
		case "C":
			return 2
		case "A":
			return 3
		case "L":
			return 4
		}
		return 0
	case "movetype":
		switch strings.ToUpper(env.GetMoveType()) {
		case "I":
			return 1
		case "A":
			return 2
		case "H":
			return 3
		}
		return 0
	case "physics":
		switch strings.ToUpper(env.GetPhysics()) {
		case "S":
			return 1
		case "C":
			return 2
		case "A":
			return 3
		case "N":
			return 4
		}
		return 0
	case "pos x":
		x, _ := env.GetPos()
		return x
	case "pos y":
		_, y := env.GetPos()
		return y
	case "vel x":
		x, _ := env.GetVel()
		return x
	case "vel y":
		_, y := env.GetVel()
		return y
	case "movehit":
		return 0 // TODO
	case "movecontact":
		return 0 // TODO
	case "power":
		return 1000 // Mock 1 bar
	}
	return math.MaxFloat64 // Mark as unknown
}

func evaluateCall(call *Call, env EvaluatorEnvironment) float64 {
	switch call.Name {
	case "var":
		if len(call.Arguments) > 0 {
			idx := int(Evaluate(call.Arguments[0], env))
			return float64(env.GetVar(idx))
		}
	case "fvar":
		if len(call.Arguments) > 0 {
			idx := int(Evaluate(call.Arguments[0], env))
			return env.GetFVar(idx)
		}
	case "abs":
		if len(call.Arguments) > 0 {
			return math.Abs(Evaluate(call.Arguments[0], env))
		}
	case "floor":
		if len(call.Arguments) > 0 {
			return math.Floor(Evaluate(call.Arguments[0], env))
		}
	case "ceil":
		if len(call.Arguments) > 0 {
			return math.Ceil(Evaluate(call.Arguments[0], env))
		}
	}
	return 0
}
