package evaluator

import (
	"fmt"
	"math"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

// EvaluatorEnvironment provides the runtime data needed to evaluate MUGEN expressions.
type EvaluatorEnvironment interface {
	GetTime() int
	GetAnimTime() int
	GetPos() (float64, float64)
	GetVel() (float64, float64)
	GetVar(idx int) int
	GetFVar(idx int) float64
	GetSysVar(idx int) int
	GetSysFVar(idx int) float64
	GetStateNo() int
	GetCtrl() bool
	GetStateType() string
	GetMoveType() string
	GetPhysics() string
	GetAnim() int
	GetAnimElem() int
	GetFacing() int
	Command(name string) bool
	GetCNSVelocity() *cns.Velocity
	GetCNSMovement() *cns.Movement
}

// Environment is an alias to EvaluatorEnvironment for easier access
type Environment = EvaluatorEnvironment

// CharacterEnvironment extends EvaluatorEnvironment with game-specific
// identifiers that require access to combat state and opponent position.
type CharacterEnvironment interface {
	EvaluatorEnvironment
	GetTargetPos() (float64, float64) // opponent position (world space)
	GetMoveHit() int
	GetMoveContact() int
	GetMoveGuarded() int
	GetPower() int
	GetLife() int
	GetHitVar(name string) float64
	GetHitShakeOver() bool
	GetHitOver() bool
	GetHitFall() bool
	HasAnim(animNo int) bool
	GetInGuardDist() bool
	GetMatchOver() bool
	GetRoundNo() int
	GetRoundState() int
	GetPrevStateNo() int
	GetRoundsExisted() int
}

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
		name := normalizeIdentifier(e.Name)
		res := evaluateIdentifier(name, env)
		if res == math.MaxFloat64 && customEval != nil {
			res = customEval(name, env)
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
					matched := env.Command(str.Value)
					if env.GetStateNo() == 40 && str.Value == "holdfwd" {
						fmt.Printf("[DEBUG] State 40: eval command = holdfwd -> %v\n", matched)
					}
					if matched {
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

		// Interval evaluation
		if iv, ok := e.Right.(*Interval); ok {
			min := Evaluate(iv.Left, env)
			max := Evaluate(iv.Right, env)

			inRange := true
			if iv.LeftInc {
				inRange = inRange && left >= min
			} else {
				inRange = inRange && left > min
			}
			if iv.RightInc {
				inRange = inRange && left <= max
			} else {
				inRange = inRange && left < max
			}

			if e.Operator == "=" || e.Operator == "==" {
				if inRange {
					return 1
				}
				return 0
			} else if e.Operator == "!=" {
				if !inRange {
					return 1
				}
				return 0
			}
			return 0
		}

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

func normalizeIdentifier(name string) string {
	// Collapse all internal whitespace into a single space and lowercase
	return strings.Join(strings.Fields(strings.ToLower(name)), " ")
}

func evaluateIdentifier(name string, env EvaluatorEnvironment) float64 {
	// name is already normalized here
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
	case "pos x":
		x, _ := env.GetPos()
		return x
	case "pos y":
		_, y := env.GetPos()
		return y
	case "vel x":
		// vel x is in character-local space (positive = forward toward opponent)
		x, _ := env.GetVel()
		return x
	case "vel y":
		_, y := env.GetVel()
		return y
	case "facing":
		return float64(env.GetFacing())
	case "p2bodydist x", "p2dist x":
		// Distance from this character's axis to P2's axis, in character-local space.
		// Negative means P2 is behind (crossed over).
		if charEnv, ok := env.(CharacterEnvironment); ok {
			tx, _ := charEnv.GetTargetPos()
			cx, _ := env.GetPos()
			return (tx - cx) * float64(env.GetFacing())
		}
		return 0
	case "p2bodydist y", "p2dist y":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			_, ty := charEnv.GetTargetPos()
			_, cy := env.GetPos()
			return ty - cy
		}
		return 0
	case "movehit":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			return float64(charEnv.GetMoveHit())
		}
		return 0
	case "movecontact":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			return float64(charEnv.GetMoveContact())
		}
		return 0
	case "moveguarded":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			return float64(charEnv.GetMoveGuarded())
		}
		return 0
	case "power":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			return float64(charEnv.GetPower())
		}
		return 0
	case "hitshakeover":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			if charEnv.GetHitShakeOver() {
				return 1
			}
		}
		return 0
	case "hitover":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			if charEnv.GetHitOver() {
				return 1
			}
		}
		return 0
	case "hitfall":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			if charEnv.GetHitFall() {
				return 1
			}
		}
		return 0
	case "alive":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			if charEnv.GetLife() > 0 {
				return 1
			}
		}
		return 0
	case "canrecover":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			if charEnv.GetHitVar("fall.recover") != 0 {
				return 1
			}
		}
		return 0
	case "roundsexisted":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			return float64(charEnv.GetRoundsExisted())
		}
		return 0
	case "life":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			return float64(charEnv.GetLife())
		}
		return 1000
	case "inguarddist":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			if charEnv.GetInGuardDist() {
				return 1
			}
		}
		return 0
	case "matchover":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			if charEnv.GetMatchOver() {
				return 1
			}
		}
		return 0
	case "roundno":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			return float64(charEnv.GetRoundNo())
		}
		return 1
	case "roundstate":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			return float64(charEnv.GetRoundState())
		}
		return 2 // Fighting
	case "prevstateno":
		if charEnv, ok := env.(CharacterEnvironment); ok {
			return float64(charEnv.GetPrevStateNo())
		}
		return 0
	case "vel z":
		return 0 // Z-axis not currently supported
	}
	return math.MaxFloat64 // Mark as unknown
}

func evaluateCall(call *Call, env EvaluatorEnvironment) float64 {
	switch strings.ToLower(call.Name) {
	case "animexist", "selfanimexist":
		if len(call.Arguments) > 0 {
			animNo := int(Evaluate(call.Arguments[0], env))
			if charEnv, ok := env.(CharacterEnvironment); ok {
				if charEnv.HasAnim(animNo) {
					return 1
				}
			}
			return 0
		}
	case "gethitvar":
		if len(call.Arguments) > 0 {
			if id, ok := call.Arguments[0].(*Identifier); ok {
				name := strings.ToLower(id.Name)
				if charEnv, ok := env.(CharacterEnvironment); ok {
					return charEnv.GetHitVar(name)
				}
			}
		}
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
	case "sysvar":
		if len(call.Arguments) > 0 {
			idx := int(Evaluate(call.Arguments[0], env))
			return float64(env.GetSysVar(idx))
		}
	case "sysfvar":
		if len(call.Arguments) > 0 {
			idx := int(Evaluate(call.Arguments[0], env))
			return env.GetSysFVar(idx)
		}
	case "abs":
		if len(call.Arguments) > 0 {
			return math.Abs(Evaluate(call.Arguments[0], env))
		}
	case "ifelse":
		// ifelse(condition, trueValue, falseValue)
		if len(call.Arguments) == 3 {
			cond := Evaluate(call.Arguments[0], env)
			if cond != 0 {
				return Evaluate(call.Arguments[1], env)
			}
			return Evaluate(call.Arguments[2], env)
		}
	case "floor":
		if len(call.Arguments) > 0 {
			return math.Floor(Evaluate(call.Arguments[0], env))
		}
	case "ceil":
		if len(call.Arguments) > 0 {
			return math.Ceil(Evaluate(call.Arguments[0], env))
		}
	case "const":
		if len(call.Arguments) > 0 {
			// const(velocity.walk.fwd)
			if id, ok := call.Arguments[0].(*Identifier); ok {
				name := strings.ToLower(id.Name)
				vel := env.GetCNSVelocity()
				if vel != nil {
					switch name {
					case "velocity.walk.fwd", "velocity.walk.fwd.x":
						return vel.WalkFwd
					case "velocity.walk.back", "velocity.walk.back.x":
						return vel.WalkBack
					case "velocity.run.fwd.x":
						if len(vel.RunFwd) >= 1 {
							return vel.RunFwd[0]
						}
					case "velocity.run.fwd.y":
						if len(vel.RunFwd) >= 2 {
							return vel.RunFwd[1]
						}
					case "velocity.run.back.x":
						if len(vel.RunBack) >= 1 {
							return vel.RunBack[0]
						}
					case "velocity.run.back.y":
						if len(vel.RunBack) >= 2 {
							return vel.RunBack[1]
						}
					case "velocity.jump.y", "velocity.jump.neu.y":
						if len(vel.JumpNeu) >= 2 {
							return vel.JumpNeu[1]
						}
					case "velocity.jump.x", "velocity.jump.neu.x":
						if len(vel.JumpNeu) >= 1 {
							return vel.JumpNeu[0]
						}
					case "velocity.jump.fwd", "velocity.jump.fwd.x":
						return vel.JumpFwd
					case "velocity.jump.back", "velocity.jump.back.x":
						return vel.JumpBack
					case "movement.yaccel":
						m := env.GetCNSMovement()
						if m != nil {
							return m.YAccel
						}
					case "movement.stand.friction":
						m := env.GetCNSMovement()
						if m != nil {
							return m.StandFriction
						}
					case "movement.crouch.friction":
						m := env.GetCNSMovement()
						if m != nil {
							return m.CrouchFriction
						}
					}
				}
			}
		}
	}
	return 0
}
