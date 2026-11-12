package character

import "github.com/kazzmir/paintown/game/mugen/cns"

type appliedVelocity struct {
	local float64
	game  float32
	valid bool
}

// ApplyHitVelocity transfers selected axes directly from native game storage.
// Local physics keeps its existing representation; redirected reads retain
// the unrounded source while that axis is unchanged.
func (c *Character) ApplyHitVelocity(x, y bool) {
	for axis, selected := range [2]bool{x, y} {
		if !selected {
			continue
		}
		field := "xvel"
		if axis == 1 {
			field = "yvel"
		}
		value := c.GetHitVar(field)
		game, canonical := c.hitVelocityInUnits(field, c.hitVelocityGameWidth())
		if axis == 0 {
			value *= float64(c.Facing)
			game *= float32(c.Facing)
			c.VelX = value
		} else {
			c.VelY = value
		}
		c.appliedHitVelocity[axis] = appliedVelocity{local: value, game: game, valid: canonical}
	}
}

func (c *Character) hitVelocityGameWidth() int {
	if c.GameCoord.Width > 0 {
		return c.GameCoord.Width
	}
	return 320
}

// Native hit vectors live in game units. Convert authored units by division,
// then convert each observer directly from that storage. A receiver-local
// round trip changes float32 results for fractional coordinate profiles.
func (c *Character) captureHitVelocity(attacker *Character, hit cns.HitDef, lethal bool, facing int) {
	vector := hit.GroundVel
	switch {
	case c.GetHitGuarded && c.GetHitAir:
		vector = hit.AirGuardVel
	case c.GetHitGuarded:
		vector = []float64{hit.GuardVel, 0}
	case c.GetHitDown:
		vector = hit.DownVel
	case c.GetHitAir:
		vector = hit.AirVel
	}
	gameWidth := c.hitVelocityGameWidth()
	scale := float32(attacker.LocalCoordinateWidth()) / float32(gameWidth)
	v := []float64{0, 0}
	for axis := range v {
		if axis < len(vector) {
			v[axis] = float64(float32(vector[axis]) / scale)
		}
	}
	if lethal {
		// KO constants use the same canonical game-space vector. Down hits
		// keep their authored vector, as in the local HitDef metadata path.
		canonical := cns.HitDef{GroundVel: v, AirVel: v}
		c.koHitVelocity(&canonical, gameWidth)
		v = canonical.GroundVel
		if c.GetHitAir {
			v = canonical.AirVel
		}
	}
	c.getHitVelocity = [2]float32{float32(v[0]) * float32(-facing), float32(v[1])}
	c.hasHitVelocity = true
}

func (c *Character) hitVelocityInUnits(name string, width int) (float32, bool) {
	if !c.hasHitVelocity || name != "xvel" && name != "yvel" {
		return 0, false
	}
	axis := 0
	if name == "yvel" {
		axis = 1
	}
	return c.getHitVelocity[axis] * (float32(width) / float32(c.hitVelocityGameWidth())), true
}
