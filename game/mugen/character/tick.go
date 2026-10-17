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
// phases. The World registry owns identities; complete helper/projectile phase
// scheduling remains separate work and their existing child adapters remain.
func TickCharacters(players []*Character) error {
	if len(players) == 0 {
		return nil
	}
	if players[0] == nil {
		return fmt.Errorf("nil character in tick group")
	}
	if err := players[0].ensureWorld(); err != nil {
		return err
	}
	world := players[0].World
	for _, c := range players {
		if c != nil && c.Parent == nil {
			if err := world.AddRoots(c); err != nil {
				return err
			}
		}
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
	pausedEffects, superPausedEffects := timeline.Paused(), timeline.SuperPaused()
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
			if attacker.HitGuard == nil {
				attacker.HitGuard = map[int]bool{}
			}
			for _, defender := range c.Opponents {
				if timeline.Frozen(attacker.pauseActor) && timeline.Frozen(defender.pauseActor) {
					continue
				}
				if defender == attacker || defender.Life <= 0 || attacker.HitGuard[int(defender.EntityID)] {
					continue
				}
				if tickCollision(attacker, defender) {
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
		event.guard[int(event.defender.EntityID)] = true
		c := event.attacker
		c.MoveHit, c.MoveContact, c.MoveGuarded = 1, 1, 0
		if event.guarded {
			c.MoveHit, c.MoveGuarded = 0, 1
		}
		pause := event.hit.PauseTime
		if event.guarded {
			pause = event.hit.GuardPauseTime
		}
		if len(pause) > 0 {
			c.HitPauseTime = pause[0]
		}
	}
	for _, event := range contacts {
		amount := event.defender.prepareContact(event.attacker, event.hit, event.guarded)
		if event.defender.RuntimeError != nil {
			return event.defender.RuntimeError
		}
		event.defender.pendingHitDamage += amount
		event.defender.hasPendingHitDamage = true
	}
	// Previous-tick definitions collide before this tick's state passes.
	// Timed defence remains active for contact, then can be renewed by CNS.
	// Attackers execute first; root order breaks ties. Changes during a pass
	// affect the following tick's order, not the already selected sequence.
	// Each root completes CNS, pending damage, physics and clocks before the
	// next root's CNS. Native trade snapshots can see the first root's reduced
	// life, just as redirected Pos/Time see its completed movement and clock.
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
		if !c.worldFrozen && c.hasPendingHitDamage {
			c.applyHitDamage(c.pendingHitDamage)
			c.pendingHitDamage, c.hasPendingHitDamage = 0, false
		}
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
		if c.Parent == nil && world.AfterRootUpdate != nil {
			world.AfterRootUpdate()
		}
	}
	// Body push waits until every root has run CNS/movement. A later root can
	// observe authored PosSet and completed physics before this body correction.
	for _, c := range players {
		if !c.worldFrozen && !c.pausedThisTick && !c.PosFrozen {
			c.ResolvePushCollision()
		}
	}
	for _, c := range players {
		if err := c.advanceExplods(pausedEffects, superPausedEffects); err != nil {
			return err
		}
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
			c.RuntimeError = fmt.Errorf("player %d KO sound: %w", c.InputSlot, err)
			return c.RuntimeError
		}
	}
	return nil
}
