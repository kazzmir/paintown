package character

import (
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/combat"
)

func (c *Character) GetHitPowerRules() combat.PowerRules {
	if c.PowerRules != nil {
		return *c.PowerRules
	}
	return combat.DefaultPowerRules()
}

func (c *Character) creditHitPower(attacker *Character, hit cns.HitDef, guarded bool) {
	rules := attacker.GetHitPowerRules()
	damage := 0
	if len(hit.Damage) > 0 {
		damage = hit.Damage[0]
	}
	amount := func(values []int, factor float64) int {
		// Compiled HitDefs contain both components. The fallback also keeps
		// directly constructed simulation definitions consistent with them.
		value := int(float32(damage) * float32(factor))
		if len(values) > 0 {
			value = values[0]
		}
		if guarded {
			value /= 2
			if len(values) > 1 {
				value = values[1]
			}
		}
		return value
	}
	attacker.AddPower(amount(hit.GetPower, rules.Attack))
	c.AddPower(amount(hit.GivePower, rules.GetHit))
}
