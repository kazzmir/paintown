package character

import (
	"math"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

// Bounds and the anchor axis are captured at contact. A zero-duration snap
// queues their displacement, which survives entry CNS writes and is consumed
// before movement. A timed snap instead uses the ordinary binding lifecycle.
func (c *Character) queueHitConstraints(attacker *Character, hit cns.HitDef, facing int) *actorBinding {
	if len(hit.MinDist) == 0 && len(hit.MaxDist) == 0 {
		return nil
	}
	ax, ay := c.PositionOf(attacker)
	x, y := float32(c.X)-float32(ax), float32(c.Y)-float32(ay)
	x *= float32(facing)
	clamp := func(axis int, value float32) float32 {
		low, high := float32(math.Inf(-1)), float32(math.Inf(1))
		if axis < len(hit.MinDist) {
			low = float32(hit.MinDist[axis])
		}
		if axis < len(hit.MaxDist) {
			high = float32(hit.MaxDist[axis])
		}
		if low > high {
			return value
		}
		return max(low, min(high, value))
	}
	gx, gy := clamp(0, x), clamp(1, y)
	if hit.SnapTime != 0 {
		return &actorBinding{anchor: attacker, x: float64(gx) * float64(facing*attacker.Facing), y: float64(gy), remaining: hit.SnapTime}
	}
	c.pendingHitXOffset += float64((gx - x) * float32(facing))
	c.pendingHitYOffset += float64(gy - y)
	return nil
}
