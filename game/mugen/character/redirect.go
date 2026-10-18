package character

import (
	"strings"

	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// A redirected geometric trigger returns the origin player's units. Scalar
// variables, IDs, counters and dimensionless values keep their original value.
type redirectEnvironment struct {
	*Character
	width int
}

func (c *Character) redirectEnvironment(target *Character) evaluator.EvaluatorEnvironment {
	return &redirectEnvironment{Character: target, width: c.LocalCoordinateWidth()}
}

func (r *redirectEnvironment) LocalCoordinateWidth() int { return r.width }
func (r *redirectEnvironment) RedirectArgumentContext() evaluator.EvaluatorEnvironment {
	return r.Character
}
func (r *redirectEnvironment) scale() float32 {
	return float32(r.width) / float32(r.Character.LocalCoordinateWidth())
}
func (r *redirectEnvironment) vector(x, y float64) (float64, float64) {
	return float64(float32(x) * r.scale()), float64(float32(y) * r.scale())
}
func (r *redirectEnvironment) value(v evaluator.Value) evaluator.Value {
	if !v.IsNumber() {
		return v
	}
	return evaluator.Float(float32(v.Number()) * r.scale())
}
func (r *redirectEnvironment) GetPos() (float64, float64) { return r.vector(r.Character.GetPos()) }
func (r *redirectEnvironment) GetVel() (float64, float64) {
	x, y := r.Character.GetVel()
	x, y = r.vector(x, y)
	// HitVelSet transfers the canonical hit vector without an intermediate
	// local round trip. Keep that precision until physics/CNS changes an axis.
	values := [2]float64{x, y}
	local := [2]float64{r.Character.VelX, r.Character.VelY}
	for axis, applied := range r.Character.appliedHitVelocity {
		if applied.valid && applied.local == local[axis] {
			values[axis] = float64(applied.game * (float32(r.width) / float32(r.Character.hitVelocityGameWidth())))
		}
	}
	return values[0], values[1]
}
func (r *redirectEnvironment) GetTargetPos() (float64, float64) {
	return r.vector(r.Character.GetTargetPos())
}
func (r *redirectEnvironment) GetOpponentBodyDistance() (float64, float64) {
	return r.vector(r.Character.GetOpponentBodyDistance())
}
func (r *redirectEnvironment) GetHitVar(name string) float64 {
	if value, ok := r.Character.hitVelocityInUnits(name, r.width); ok {
		return float64(value)
	}
	value := r.Character.GetHitVar(name)
	switch name {
	case "xvel", "yvel", "yoff", "yaccel", "fall.xvel", "fall.yvel":
		return float64(float32(value) * r.scale())
	}
	return value
}
func (r *redirectEnvironment) LookupIdentifier(name string) (evaluator.Value, bool) {
	value, known := r.Character.LookupIdentifier(name)
	if !known {
		return value, false
	}
	switch name {
	case "gamewidth", "gameheight", "screenwidth", "screenheight", "screenpos x", "screenpos y", "camerapos x", "camerapos y", "parentdist x", "parentdist y", "rootdist x", "rootdist y":
		value = r.value(value)
	case "leftedge", "rightedge", "frontedge", "backedge", "frontedgedist", "backedgedist", "frontedgebodydist", "backedgebodydist":
		value = r.value(value)
	}
	return value, true
}
func (r *redirectEnvironment) LookupConstant(name string) (evaluator.Value, bool) {
	value, known := r.Character.LookupConstant(name)
	if !known {
		return value, false
	}
	coordinate := strings.HasPrefix(name, "velocity.")
	if strings.HasPrefix(name, "size.") {
		coordinate = name != "size.xscale" && name != "size.yscale" && name != "size.proj.doscale"
	}
	if strings.HasPrefix(name, "movement.") {
		coordinate = name != "movement.airjump.num" && !strings.HasSuffix(name, ".friction")
	}
	if coordinate {
		scaled := float32(value.Number()) * r.scale()
		if value.Kind() == evaluator.IntKind {
			value = evaluator.Int(int32(scaled))
		} else {
			value = evaluator.Float(scaled)
		}
	}
	return value, true
}
