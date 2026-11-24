package match

import "github.com/kazzmir/paintown/game/mugen/character"

// recovery requests a physical two-button chord; common CNS remains responsible
// for CanRecover, hit time, state transitions and recovery velocity. Native gives
// higher chances above the floor and around the apex of a falling hit reaction.
func (b *aiBrain) recovery(c *character.Character, gameWidth int) {
	if c.MoveType != "H" || !c.GetHitFall() {
		return
	}
	if gameWidth <= 0 {
		gameWidth = 320
	}
	scale := float64(gameWidth) / float64(c.LocalCoordinateWidth())
	chance := 2
	if c.Y*scale <= -20 {
		chance = 20
	}
	if velocity := c.VelY * scale; velocity >= -1 && velocity < 3 {
		chance += 8
	}
	if int(b.random.Next()%100) < chance {
		if b.random.Next()%100 < 50 {
			b.keys[4], b.keys[5] = true, true
		} else {
			b.keys[7], b.keys[8] = true, true
		}
	}
}
