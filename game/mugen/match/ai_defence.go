package match

import (
	"strings"

	"github.com/kazzmir/paintown/game/mugen/character"
)

// defence is a physical input decision, including while getting hit. CNS still
// decides whether the current state can guard. A successful reaction owns this
// pass; retaining held keys avoids repeatedly releasing an established guard.
func (b *aiBrain) defence(c, enemy *character.Character, distance, level int) bool {
	threat := c.GuardThreat()
	if !(threat != nil && c.MoveType == "I" && c.Ctrl) && c.MoveType != "H" {
		return false
	}
	base := 10
	if c.Time < 2 {
		base = 26 + level/2
	}
	if c.Time < 5 {
		base += 5
	}
	if c.Time > 10 {
		base -= 5
	}
	if distance < 30 {
		base += level/2 + 6
		if c.Time < 2 {
			base += 10
		}
	}
	if distance > 160 {
		base /= 2
	}
	if threat == nil {
		base /= 5
	}
	if int(b.random.Next()%100) >= (level+6)*base/14 {
		return false
	}
	guarding := c.StateNo >= 120 && c.StateNo < 160
	if !guarding {
		b.keys[0], b.keys[1], b.keys[3] = false, false, true
		crouch := 25
		if enemy.StateType == "C" {
			crouch = 75
		}
		b.keys[2] = int(b.random.Next()%100) < crouch
	} else if b.random.Next()%100 < 30 {
		flags := enemy.CurrentHitDef.GuardFlag
		if threat != nil {
			flags = threat.CurrentHitDef.GuardFlag
		}
		flags = strings.ToUpper(flags)
		low, high := strings.Contains(flags, "L"), strings.Contains(flags, "H")
		if low != high {
			b.keys[2] = low
		}
	}
	// A guard reaction interrupts an unfinished physical motion; resuming its
	// tail after a guard would invent a charge/strict-sequence history.
	b.queue, b.held, b.remaining = nil, nil, 0
	return true
}
