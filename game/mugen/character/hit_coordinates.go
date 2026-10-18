package character

import "github.com/kazzmir/paintown/game/mugen/cns"

// hitInLocalSpace converts stored hit vectors without mutating the attacker's
// slices. Timers, damage and resource indices are not coordinate quantities.
func (c *Character) hitInLocalSpace(attacker *Character, hit cns.HitDef) cns.HitDef {
	scale := float32(c.ScaleFrom(attacker))
	vector := func(source []float64) []float64 {
		if source == nil {
			return nil
		}
		result := make([]float64, len(source))
		for index, value := range source {
			result[index] = float64(float32(value) * scale)
		}
		return result
	}
	hit.GroundVel = vector(hit.GroundVel)
	hit.AirVel = vector(hit.AirVel)
	hit.AirGuardVel = vector(hit.AirGuardVel)
	hit.DownVel = vector(hit.DownVel)
	hit.FallVel = vector(hit.FallVel)
	hit.MinDist = vector(hit.MinDist)
	hit.MaxDist = vector(hit.MaxDist)
	hit.YAccel = float64(float32(hit.YAccel) * scale)
	hit.GuardVel = float64(float32(hit.GuardVel) * scale)
	hit.GuardDist = float64(float32(hit.GuardDist) * scale)
	hit.GroundCornerPush = float64(float32(hit.GroundCornerPush) * scale)
	hit.AirCornerPush = float64(float32(hit.AirCornerPush) * scale)
	hit.DownCornerPush = float64(float32(hit.DownCornerPush) * scale)
	hit.GuardCornerPush = float64(float32(hit.GuardCornerPush) * scale)
	hit.AirGuardCornerPush = float64(float32(hit.AirGuardCornerPush) * scale)
	return hit
}
