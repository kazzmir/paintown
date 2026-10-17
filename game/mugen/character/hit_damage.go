package character

import (
	"math"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func (c *Character) contactDamage(attacker *Character, hit cns.HitDef, guarded bool) (int, bool) {
	damage := 0
	index, kill := 0, hit.Kill
	if guarded {
		index, kill = 1, hit.GuardKill
	}
	if len(hit.Damage) > index {
		damage = hit.Damage[index]
	}
	// Idle defence adjustments expire at a fresh get-hit sequence; H-to-H
	// contacts retain the adjustment established while being hit.
	if c.MoveType != "H" {
		c.DefenceMul = 1
	}
	superDefence := c.superDefenceMul
	if superDefence <= 0 {
		superDefence = 1
	}
	damage = int(float64(damage)*attacker.AttackMul*c.DefenceMul/superDefence + .5)
	life := max(0, min(c.MaxLife, c.Life-c.pendingHitDamage))
	minimum := 0
	if kill != nil && !*kill {
		minimum = 1
	}
	damage = min(damage, max(0, life-minimum))
	return damage, life > 0 && damage >= life
}

// ApplyHitFallDamage follows beta1's controller, which consumes the stored
// amount even outside a falling H state and does not enforce fall.kill.
func (c *Character) ApplyHitFallDamage() {
	damage := c.GetHit.FallDamage
	c.GetHit.FallDamage = 0
	c.AddLife(-damage)
}

// koHitVelocity changes the selected get-hit vector, before CNS/HitVelSet.
// All constants are 240p quantities; the native arithmetic rounds to float32.
func (c *Character) koHitVelocity(hit *cns.HitDef) {
	// Native down-hit contacts retain the supplied down vector on KO.
	if c.GetHitDown {
		return
	}
	velocity := &hit.GroundVel
	if c.GetHitAir {
		velocity = &hit.AirVel
	}
	x, y := float32(0), float32(0)
	if len(*velocity) > 0 {
		x = float32((*velocity)[0])
	}
	if len(*velocity) > 1 {
		y = float32((*velocity)[1])
	}
	scale := float32(c.LocalCoordinateWidth()) / 320
	if !c.GetHitAir && y == 0 {
		x *= .66
	}
	if x != 0 {
		x += float32(math.Copysign(float64(float32(2.5)*scale), float64(x)))
	}
	if y <= 0 {
		ceiling := float32(-6) * scale
		if c.GetHitAir {
			ceiling = -3 * scale
		}
		y = min(y-2*scale, ceiling)
	}
	*velocity = []float64{float64(x), float64(y)}
}
