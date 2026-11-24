package match

import (
	"strings"

	"github.com/kazzmir/paintown/game/mugen/character"
)

// attackWeight is a per-pass weight out of 1000, not a cooldown. Close range and
// a confirmed hit increase attempts; getting hit greatly reduces them. Values
// above 1000 deliberately mean certain admission on the enclosing decision pass.
func attackWeight(c *character.Character, distance, level int) int {
	base := 100
	if c.MoveType == "A" {
		base = 150
		if c.MoveHit > 0 {
			base += (level + 12) * 350 / 20
		}
	}
	if distance < 150 {
		base += 50
	}
	if distance < 100 {
		base += 75
	}
	if distance < 60 {
		base += level*5 + 105
	}
	if distance < 45 {
		base += level*3 + 120
	}
	if distance < 30 {
		base += level*3 + 120
	}
	weight := (level + 6) * base / 14
	if c.MoveType == "H" {
		weight /= 10
	}
	return weight
}

func (b *aiBrain) toggle(key int) {
	if key >= 0 {
		b.keys[key] = !b.keys[key]
	}
}

func (b *aiBrain) attack(c *character.Character, distance, level, first, second int, cheat bool) {
	if int(b.random.Next()%1000) >= attackWeight(c, distance, level) {
		return
	}
	physical := 750
	if c.MoveHit > 0 {
		physical = 500
		// Direct attacks/throws favour command attempts more than projectiles.
		parts := strings.Split(strings.ToUpper(c.CurrentHitDef.Attr), ",")
		for _, part := range parts[1:] {
			switch strings.TrimSpace(part) {
			case "NA", "NT", "SA", "ST", "HA", "HT":
				physical = 250
			}
		}
	}
	if int(b.random.Next()%1000) < physical {
		b.toggle(4 + int(b.random.Next()%6))
		return
	}
	b.releaseDirections()
	if int(b.random.Next()%1000) < level*30+550 && cheat && len(b.commandNames) > 0 {
		// Admission carries a label, never a forged physical charge or an
		// unconditional state change. The authored -1 state decides its use.
		index := int(b.random.Next() % int64(len(b.commandNames)))
		b.admitted = b.commandNames[index : index+1]
	} else if !cheat && len(b.commands) > 0 && (c.Ctrl || c.MoveType == "A" && c.MoveContact > 0) {
		// Port policy: retain the existing bounded timed-plan consumer when
		// cheating is disabled, including remaps, releases and long charges.
		b.queue = b.commands[int(b.random.Next()%int64(len(b.commands)))]
		b.planActive = true
		b.nextInput()
		return
	}
	b.toggle(first)
	b.toggle(second)
}
