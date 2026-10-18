package character

import "github.com/kazzmir/paintown/game/mugen/cns"

func (c *Character) contactOwner() *Character {
	if c.isProjectile && c.Parent != nil {
		return c.Parent
	}
	return c
}

func (c *Character) allowsHitChain(attacker *Character, hit cns.HitDef) bool {
	same := c.MoveType == "H" && c.lastHitOwner == attacker.contactOwner()
	if hit.ChainID != nil && *hit.ChainID >= 0 {
		return same && c.lastHitID == *hit.ChainID
	}
	if same && len(hit.NoChainID) > 0 && hit.NoChainID[0] >= 0 {
		for _, id := range hit.NoChainID {
			if id >= 0 && c.lastHitID == id {
				return false
			}
		}
	}
	return true
}

func (c *Character) juggleCost(attacker *Character, hit cns.HitDef) int {
	if attacker.isProjectile {
		return hit.AirJuggle
	}
	return attacker.JuggleCost
}

func (c *Character) juggleBudget() int {
	if c.juggleInitialized {
		return c.juggleRemaining
	}
	if c.StateFile != nil {
		return c.StateFile.Data.AirJuggle
	}
	return 15
}

func (c *Character) juggleAlreadyPaid(attacker *Character) bool {
	generation, paid := c.juggleMoves[attacker]
	return paid && generation == attacker.juggleGeneration
}

func (c *Character) allowsJuggle(attacker *Character, hit cns.HitDef) bool {
	if !c.juggleRelevant(hit) || c.guardsHit(hit) || attacker.Specials["nojugglecheck"] || c.juggleAlreadyPaid(attacker) {
		return true
	}
	return c.StateType != "A" && c.StateType != "L" || c.juggleBudget() >= c.juggleCost(attacker, hit)
}

func (c *Character) consumeJuggle(attacker *Character, hit cns.HitDef) {
	if !c.juggleRelevant(hit) || c.juggleAlreadyPaid(attacker) {
		return
	}
	if !c.juggleInitialized {
		c.juggleRemaining = c.juggleBudget()
		c.juggleInitialized = true
	}
	if c.juggleMoves == nil {
		c.juggleMoves = map[*Character]uint64{}
	}
	c.juggleRemaining -= c.juggleCost(attacker, hit)
	c.juggleMoves[attacker] = attacker.juggleGeneration
}

func (c *Character) juggleRelevant(hit cns.HitDef) bool {
	return c.GetHitFall() || c.StateType == "L" || hit.Fall != 0 || (c.StateType == "A" && hit.AirFall != 0)
}

// Projectiles advance without executing the owner's CNS or starting another
// world frame. This adapter covers ordinary single-contact projectiles;
// cancellation, removal animations and projectile query/lifecycle rules remain.
func (c *Character) advanceProjectiles() {
	active := c.Projectiles[:0]
	for _, p := range c.Projectiles {
		if p.projectileLifetime == 0 {
			continue
		}
		if !c.worldFrozen {
			p.X += p.VelX * float64(p.Facing)
			p.Y += p.VelY
			p.Time++
			if p.projectileLifetime > 0 {
				p.projectileLifetime--
			}
		}
		active = append(active, p)
	}
	c.Projectiles = active
}
