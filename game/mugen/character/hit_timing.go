package character

import "github.com/kazzmir/paintown/game/mugen/cns"

// A vertical launch uses the air timer even when the contact began grounded
// or lying down. GetHitAir still records the incoming posture for vector and
// guard selection; changing it here would select the wrong hit velocity.
func (c *Character) hitUsesAirTime(hit cns.HitDef) bool {
	if c.GetHitAir {
		return true
	}
	velocity := hit.GroundVel
	if c.GetHitDown {
		velocity = hit.DownVel
	}
	return len(velocity) > 1 && velocity[1] != 0
}

func (c *Character) setHitTime(hit cns.HitDef, guarded bool) {
	switch {
	case guarded && c.GetHitAir:
		c.HitTime = -1
	case guarded:
		c.HitTime = hit.GuardHitTime
	case c.hitUsesAirTime(hit):
		c.HitTime = hit.AirHitTime
	case c.GetHitDown:
		c.HitTime = hit.DownHitTime
	default:
		c.HitTime = hit.GroundHitTime
	}
}
