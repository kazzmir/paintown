package evaluator

import (
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
	GetTargetPos() (float64, float64) // opponent position in caller's local units
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

// Evaluate is the numeric compatibility facade. Bottom is false/zero at a
// controller boundary; recursive evaluation uses EvaluateValue so bottom is
// never accidentally converted to an ordinary zero inside an expression.
func Evaluate(expr Expression, env EvaluatorEnvironment) float64 {
	return EvaluateValue(expr, env).Number()
}

func normalizeIdentifier(name string) string {
	// Collapse all internal whitespace into a single space and lowercase
	return strings.Join(strings.Fields(strings.ToLower(name)), " ")
}

func evaluateIdentifier(name string, env EvaluatorEnvironment) float64 {
	// name is already normalized here
	switch name {
	case "statetype":
		return enumValue(env.GetStateType())
	case "movetype":
		return enumValue(env.GetMoveType())
	case "physics":
		return enumValue(env.GetPhysics())
	case "s", "c", "a", "l", "i", "h", "n":
		return enumValue(name)
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
	case "p2bodydist x", "p2bodydist y":
		if context, ok := env.(interface{ GetOpponentBodyDistance() (float64, float64) }); ok {
			x, y := context.GetOpponentBodyDistance()
			if name == "p2bodydist x" {
				return x
			}
			return y
		}
		return math.MaxFloat64
	case "p2dist x":
		// Distance from this character's axis to P2's axis, in character-local space.
		// Negative means P2 is behind (crossed over).
		if charEnv, ok := env.(CharacterEnvironment); ok {
			tx, _ := charEnv.GetTargetPos()
			cx, _ := env.GetPos()
			return (tx - cx) * float64(env.GetFacing())
		}
		return 0
	case "p2dist y":
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

// legacyConstant adapts the existing Character constant getters. New world
// environments provide their complete typed constants through LookupFunction.
func legacyConstant(name string, env EvaluatorEnvironment) (float64, bool) {
	if strings.HasPrefix(name, "movement.") {
		m := env.GetCNSMovement()
		if m == nil {
			return 0, false
		}
		switch name {
		case "movement.yaccel":
			return m.YAccel, true
		case "movement.stand.friction":
			return m.StandFriction, true
		case "movement.crouch.friction":
			return m.CrouchFriction, true
		}
		return 0, false
	}
	v := env.GetCNSVelocity()
	if v == nil {
		return 0, false
	}
	component := func(values []float64, index int) (float64, bool) {
		if index < len(values) {
			return values[index], true
		}
		return 0, false
	}
	switch name {
	case "velocity.walk.fwd", "velocity.walk.fwd.x":
		return v.WalkFwd, true
	case "velocity.walk.back", "velocity.walk.back.x":
		return v.WalkBack, true
	case "velocity.run.fwd.x":
		return component(v.RunFwd, 0)
	case "velocity.run.fwd.y":
		return component(v.RunFwd, 1)
	case "velocity.run.back.x":
		return component(v.RunBack, 0)
	case "velocity.run.back.y":
		return component(v.RunBack, 1)
	case "velocity.jump.x", "velocity.jump.neu.x":
		return component(v.JumpNeu, 0)
	case "velocity.jump.y", "velocity.jump.neu.y":
		return component(v.JumpNeu, 1)
	case "velocity.jump.fwd", "velocity.jump.fwd.x":
		return v.JumpFwd, true
	case "velocity.jump.back", "velocity.jump.back.x":
		return v.JumpBack, true
	}
	return 0, false
}

func enumValue(name string) float64 {
	switch strings.ToLower(name) {
	case "s":
		return 2
	case "c":
		return 3
	case "a":
		return 1
	case "l":
		return 4
	case "i", "n":
		return 0
	case "h":
		return 5
	}
	return math.MaxFloat64
}
