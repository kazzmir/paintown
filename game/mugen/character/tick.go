package character

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

type contact struct {
	attacker, defender *Character
	hit                cns.HitDef
	guard              map[int]bool
}

// TickCharacters advances root players through shared state, contact and physics
// phases. Helper/projectile scheduling and the entity registry remain separate
// world work; their existing adapters are retained here.
func TickCharacters(players []*Character) error {
	defer func() {
		for _, c := range players {
			if c != nil {
				c.updateActive = false
			}
		}
	}()
	for _, c := range players {
		if c == nil {
			return fmt.Errorf("nil character in tick group")
		}
		if !c.beginTick() {
			return c.RuntimeError
		}
	}
	var contacts []contact
	projectileHits := map[*Character]bool{}
	for _, c := range players {
		attackers := append([]*Character{c}, c.Projectiles...)
		for _, attacker := range attackers {
			if attacker.HitGuard == nil {
				attacker.HitGuard = map[int]bool{}
			}
			for _, defender := range c.Target {
				if defender == attacker || defender.Life <= 0 || attacker.HitGuard[defender.PlayerID] {
					continue
				}
				if CheckCollision(attacker, defender) {
					contacts = append(contacts, contact{attacker, defender, attacker.CurrentHitDef, attacker.HitGuard})
					if attacker != c {
						projectileHits[attacker] = true
						break
					}
				}
			}
		}
	}
	// Snapshot all eligible contacts before any get-hit transition can disable
	// the other player's active attack. Equal-priority normal attacks can trade.
	for _, event := range contacts {
		event.guard[event.defender.PlayerID] = true
		c := event.attacker
		c.MoveHit, c.MoveContact, c.MoveGuarded = 1, 1, 0
		c.contactSerial++
		if len(event.hit.PauseTime) > 0 {
			c.ShakeTime = event.hit.PauseTime[0]
		}
	}
	type damageEvent struct {
		defender *Character
		damage   int
	}
	var damage []damageEvent
	var victims []*Character
	seen := map[*Character]bool{}
	for _, event := range contacts {
		amount := event.defender.prepareHit(event.attacker, event.hit)
		if event.defender.RuntimeError != nil {
			return event.defender.RuntimeError
		}
		damage = append(damage, damageEvent{event.defender, amount})
		if !seen[event.defender] {
			victims = append(victims, event.defender)
			seen[event.defender] = true
		}
	}
	// The first get-hit positive pass runs before contact damage, even for a
	// hit that starts hitpause. HitShakeOver still sees the actual pause value.
	for _, c := range victims {
		c.contactStatePass = true
		err := c.SM.RunPositivePass(c.StateFile.States, c)
		c.contactStatePass = false
		if err != nil {
			c.RuntimeError = err
			return err
		}
	}
	for _, event := range damage {
		event.defender.applyHitDamage(event.damage)
	}
	for _, c := range players {
		var active []*Character
		for _, p := range c.Projectiles {
			if !projectileHits[p] {
				active = append(active, p)
			}
		}
		c.Projectiles = active
		c.finishTick()
		if c.RuntimeError != nil {
			return c.RuntimeError
		}
	}
	return nil
}
