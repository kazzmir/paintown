package character

import (
	"fmt"
	"github.com/kazzmir/paintown/game/mugen/combat"
)

func (c *Character) SetHitOverride(override combat.HitOverride) error {
	if override.Slot < 0 || override.Slot >= len(c.HitOverrides) || override.Time < -1 {
		return fmt.Errorf("invalid HitOverride slot/time")
	}
	if override.Time == 0 || override.StateNo < 0 || override.Attributes.Empty() {
		c.HitOverrides[override.Slot] = combat.HitOverride{}
	} else {
		c.HitOverrides[override.Slot] = override
	}
	return nil
}

func (c *Character) matchingOverride(attributes string) (combat.HitOverride, bool) {
	hit, err := combat.ParseAttributes(attributes)
	if err != nil {
		c.EvaluationWarning("HitDef attr: " + err.Error())
		return combat.HitOverride{}, false
	}
	for _, override := range c.HitOverrides {
		if override.Time != 0 && override.Attributes.Matches(hit) {
			return override, true
		}
	}
	return combat.HitOverride{}, false
}

func (c *Character) tickHitOverrides() {
	for slot := range c.HitOverrides {
		if c.HitOverrides[slot].Time > 0 {
			c.HitOverrides[slot].Time--
		}
	}
}
