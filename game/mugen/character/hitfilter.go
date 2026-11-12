package character

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/combat"
)

// The beta uses the attacker's current posture for protection matching even
// when the authored HitDef attr names a different posture.
func (c *Character) attributesForHit(hit cns.HitDef) string {
	_, classes, ok := strings.Cut(hit.Attr, ",")
	if !ok {
		return hit.Attr
	}
	return c.StateType + "," + classes
}

func (c *Character) SetHitFilter(slot int, filter combat.HitFilter) error {
	if slot < 0 || slot >= len(c.HitFilters) {
		return fmt.Errorf("invalid hit protection slot %d", slot)
	}
	c.HitFilters[slot] = filter
	return nil
}

func (c *Character) allowsHit(attributes string) bool {
	for _, filter := range c.HitFilters {
		if filter.Time == 0 {
			continue
		}
		hit, err := combat.ParseAttributes(attributes)
		if err != nil || !filter.Attributes.Matches(hit) {
			return false
		}
	}
	return true
}
