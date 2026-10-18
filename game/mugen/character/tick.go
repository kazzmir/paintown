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
	projectile         bool
}

// TickCharacters advances roots and helpers through shared state and physics
// phases. New helpers join the end of the current pass. Complete multi-entity
// contacts and the legacy projectile adapters still need their own integration.
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
	rootGroup := map[*Character]bool{}
	for _, c := range players {
		if c != nil && c.Parent == nil {
			rootGroup[c] = true
			if err := world.AddRoots(c); err != nil {
				return err
			}
		}
	}
	queued := map[*Character]bool{}
	for _, c := range players {
		queued[c] = true
	}
	newHelpers := func() []*Character {
		var added []*Character
		for _, helper := range world.Helpers {
			if !helper.destroyed && rootGroup[helper.rootPlayer()] && !queued[helper] {
				queued[helper] = true
				players = append(players, helper)
				added = append(added, helper)
			}
		}
		return added
	}
	newHelpers()
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
	var rawContacts []contact
	projectileHits := map[*Character]bool{}
	for _, c := range players {
		attackers := append([]*Character{c}, c.Projectiles...)
		for _, attacker := range attackers {
			if attacker.HitGuard == nil {
				attacker.HitGuard = map[int]bool{}
			}
			if attacker.CurrentHitDef.HitOnce && len(attacker.HitGuard) > 0 {
				continue
			}
			defenders := c.Opponents
			if !c.isProjectile {
				defenders = world.contactCandidates(c, attacker.CurrentHitDef.AffectTeam)
			}
			for _, defender := range defenders {
				if attacker.worldFrozen && defender.worldFrozen {
					continue
				}
				if defender == attacker || defender.Life <= 0 || attacker.HitGuard[int(defender.EntityID)] {
					continue
				}
				if tickCollision(attacker, defender) {
					hit := attacker.CurrentHitDef
					hit.Attr = attacker.attributesForHit(hit)
					rawContacts = append(rawContacts, contact{attacker, defender, hit, attacker.HitGuard, defender.guardsHit(hit), attacker != c})
					if attacker != c {
						break
					}
				}
			}
		}
	}
	contacts := resolveClashes(rawContacts)
	// Root receivers take contact before helper receivers, independent of
	// which root owns the helper. Reciprocal helper trades therefore leave
	// the root's newly acquired helper target after its own interruption.
	sort.SliceStable(contacts, func(i, j int) bool {
		return contacts[i].defender.Parent == nil && contacts[j].defender.Parent != nil
	})
	// Snapshot all eligible contacts before any get-hit transition can disable
	// the other player's active attack. Equal-priority normal attacks can trade.
	counted := map[*Character]bool{}
	for _, event := range contacts {
		if event.defender.metadataOnlyHit(event.hit, event.guarded) {
			event.guard[int(event.defender.EntityID)] = true
			continue
		}
		if event.projectile {
			projectileHits[event.attacker] = true
		}
		event.guard[int(event.defender.EntityID)] = true
		c := event.attacker
		c.MoveHit, c.MoveContact, c.MoveGuarded = 1, 1, 0
		if event.guarded {
			c.MoveHit, c.MoveGuarded = 0, 1
		} else {
			number := 1
			if event.hit.NumHits != nil {
				number = *event.hit.NumHits
			}
			if !counted[c] {
				c.HitCount += number
				counted[c] = true
			}
			c.UniqHitCount += number
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
		metadataOnly := event.defender.metadataOnlyHit(event.hit, event.guarded)
		amount := event.defender.prepareContact(event.attacker, event.hit, event.guarded)
		if event.defender.RuntimeError != nil {
			return event.defender.RuntimeError
		}
		if metadataOnly {
			continue
		}
		if event.guarded && !event.defender.GetHitGuarded {
			c := event.attacker
			c.MoveHit, c.MoveGuarded = 1, 0
			number := 1
			if event.hit.NumHits != nil {
				number = *event.hit.NumHits
			}
			if !counted[c] {
				c.HitCount += number
				counted[c] = true
			}
			c.UniqHitCount += number
			c.HitPauseTime = 0
			if len(event.hit.PauseTime) > 0 {
				c.HitPauseTime = event.hit.PauseTime[0]
			}
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
	for _, c := range ordered {
		c.tickMoveType = c.MoveType
	}
	sort.SliceStable(ordered, func(i, j int) bool {
		return ordered[i].MoveType == "A" && ordered[j].MoveType != "A"
	})
	for index := 0; index < len(ordered); index++ {
		c := ordered[index]
		if c.destroyed {
			continue
		}
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
		if !c.isProjectile && world.AfterEntityUpdate != nil {
			world.AfterEntityUpdate()
		}
		for _, helper := range newHelpers() {
			helper.BindTimeline(timeline)
			if !helper.startTick() {
				return helper.RuntimeError
			}
			ordered = append(ordered, helper)
		}
	}
	// Body push waits until every root has run CNS/movement. A later root can
	// observe authored PosSet and completed physics before this body correction.
	for _, c := range players {
		if !c.destroyed && !c.worldFrozen && !c.pausedThisTick && !c.PosFrozen {
			c.ResolvePushCollision()
		}
		var children []*Character
		for _, child := range c.Helpers {
			if !child.destroyed {
				children = append(children, child)
			}
		}
		c.Helpers = children
	}
	for _, c := range players {
		if c.destroyed {
			continue
		}
		if err := c.advanceExplods(pausedEffects, superPausedEffects); err != nil {
			return err
		}
	}
	if err := world.advanceOrphanExplods(rootGroup, pausedEffects, superPausedEffects); err != nil {
		return err
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
