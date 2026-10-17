package character

import (
	"fmt"
	"sort"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

type contact struct {
	attacker, defender *Character
	hit                cns.HitDef
	guard              map[int]bool
	guarded            bool
}

// TickCharacters advances root players through shared state, contact and physics
// phases. Helper/projectile scheduling and the entity registry remain separate
// world work; their existing adapters are retained here.
func TickCharacters(players []*Character) error {
	if len(players) == 0 {
		return nil
	}
	if players[0] == nil {
		return fmt.Errorf("nil character in tick group")
	}
	timeline := players[0].Timeline
	if timeline == nil {
		timeline = &engine.Timeline{}
	}
	for _, c := range players {
		if c == nil {
			return fmt.Errorf("nil character in tick group")
		}
		c.BindTimeline(timeline)
	}
	timeline.BeginFrame()
	defer timeline.EndFrame()
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
		if !c.startTick() {
			return c.RuntimeError
		}
	}
	var contacts []contact
	projectileHits := map[*Character]bool{}
	for _, c := range players {
		attackers := append([]*Character{c}, c.Projectiles...)
		for _, attacker := range attackers {
			if timeline.Frozen(attacker.pauseActor) {
				continue
			}
			if attacker.HitGuard == nil {
				attacker.HitGuard = map[int]bool{}
			}
			for _, defender := range c.Target {
				if defender == attacker || defender.Life <= 0 || attacker.HitGuard[defender.PlayerID] {
					continue
				}
				if CheckCollision(attacker, defender) {
					hit := attacker.CurrentHitDef
					hit.Attr = attacker.attributesForHit(hit)
					contacts = append(contacts, contact{attacker, defender, hit, attacker.HitGuard, defender.guardsHit(hit)})
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
		if event.guarded {
			c.MoveHit, c.MoveGuarded = 0, 1
		}
		c.contactSerial++
		pause := event.hit.PauseTime
		if event.guarded {
			pause = event.hit.GuardPauseTime
		}
		if len(pause) > 0 {
			c.HitPauseTime = pause[0]
		}
	}
	type damageEvent struct {
		defender *Character
		damage   int
	}
	var damage []damageEvent
	for _, event := range contacts {
		amount := event.defender.prepareContact(event.attacker, event.hit, event.guarded)
		if event.defender.RuntimeError != nil {
			return event.defender.RuntimeError
		}
		damage = append(damage, damageEvent{event.defender, amount})
	}
	// Previous-tick definitions collide before this tick's state passes.
	// Timed defence remains active for contact, then can be renewed by CNS.
	// Attackers execute first; root order breaks ties. Changes during a pass
	// affect the following tick's order, not the already selected sequence.
	ordered := append([]*Character(nil), players...)
	sort.SliceStable(ordered, func(i, j int) bool {
		return ordered[i].MoveType == "A" && ordered[j].MoveType != "A"
	})
	for _, c := range ordered {
		if !c.worldFrozen {
			c.ageDefenses()
		}
		if !c.runStateTick() {
			return c.RuntimeError
		}
	}

	for _, event := range damage {
		event.defender.applyHitDamage(event.damage)
	}
	suppressKO := false
	for _, c := range players {
		suppressKO = suppressKO || c.Specials["nokosnd"]
	}
	for _, c := range players {
		if c.worldFrozen {
			continue
		}
		if err := c.updateKOSound(suppressKO); err != nil {
			c.RuntimeError = fmt.Errorf("player %d KO sound: %w", c.PlayerID, err)
			return c.RuntimeError
		}
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
