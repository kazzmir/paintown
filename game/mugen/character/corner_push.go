package character

import (
	"math"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func (c *Character) cornerScale() float64 {
	width := c.GameCoord.Width
	if width <= 0 {
		width = 320
	}
	return float64(width) / float64(c.LocalCoordinateWidth())
}

func (c *Character) recordCornerPush(attacker *Character, hit cns.HitDef) {
	// The legacy projectile adapter still needs owner-directed corner push.
	if attacker.isProjectile || c.screenBoundsDisabled {
		return
	}
	left, right := c.LeftBound, c.RightBound
	if c.ScreenWidth > 0 {
		nearLeft, nearRight := c.screenPositionBounds()
		left, right = math.Max(left, nearLeft), math.Min(right, nearRight)
	}
	// Native tests proximity in common engine pixels, independent of the
	// receiver's heading and the direction/speed of the incoming knockback.
	scale := c.cornerScale()
	if math.Abs((c.X-left)*scale) >= 5 && math.Abs((c.X-right)*scale) >= 5 {
		return
	}
	velocity := hit.GroundCornerPush
	if c.GetHitGuarded {
		velocity = hit.GuardCornerPush
		if c.GetHitAir {
			velocity = hit.AirGuardCornerPush
		}
	} else if c.GetHitDown {
		velocity = hit.DownCornerPush
	} else if c.GetHitAir {
		velocity = hit.AirCornerPush
	}
	// The selected hit field is float32 in common space; the separate world
	// impulse then accumulates/decays in double precision and survives turns.
	attacker.cornerPushVelocity = float64(float32(float64(float32(velocity))*attacker.cornerScale())) * float64(attacker.Facing)
}

func (c *Character) advanceCornerPush() {
	c.X += c.cornerPushVelocity / c.cornerScale()
	c.cornerPushVelocity *= float64(float32(.7))
	if math.Abs(c.cornerPushVelocity) < 1 {
		c.cornerPushVelocity = 0
	}
}
