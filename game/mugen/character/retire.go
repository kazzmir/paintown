package character

import "github.com/kazzmir/paintown/game/mugen/engine"

func (c *Character) DestroyHelper(p engine.DestroySelfParameters) error {
	if c.Parent == nil || c.isProjectile || c.destroyed {
		return nil
	}
	retired := map[*Character]bool{c: true}
	ordered := []*Character{c}
	if c.World != nil && p.Recursive {
		for _, helper := range c.World.Helpers {
			for parent := helper.Parent; parent != nil; parent = parent.Parent {
				if parent == c {
					retired[helper] = true
					ordered = append(ordered, helper)
					break
				}
			}
		}
	}
	for _, helper := range ordered {
		helper.destroyed = true
		// Beta sound channels outlive DestroySelf, including recursive removal.
		// Keep the independent audio owner; helperaudio-* pins this behavior.
		if c.World != nil {
			c.World.Registry.Remove(helper.EntityID)
		}
		if p.RemoveExplods {
			for _, e := range helper.Explods {
				if e.Visual != nil {
					e.Visual.Active = false
				}
			}
			helper.Explods = nil
		} else if len(helper.Explods) > 0 && c.World != nil {
			c.World.orphanEffectOwners = append(c.World.orphanEffectOwners, helper)
		}
	}
	if c.World == nil {
		return nil
	}
	actors := append(append([]*Character(nil), c.World.Roots...), c.World.Helpers...)
	actors = append(actors, c.World.orphanEffectOwners...)
	for _, actor := range actors {
		if actor.binding != nil && retired[actor.binding.anchor] {
			actor.binding = nil
		}
		for _, e := range actor.Explods {
			if retired[e.Anchor] {
				e.Anchor, e.BindingTime = nil, 0
			}
		}
	}
	active := c.World.Helpers[:0]
	for _, helper := range c.World.Helpers {
		if !helper.destroyed {
			active = append(active, helper)
		}
	}
	clear(c.World.Helpers[len(active):])
	c.World.Helpers = active
	return nil
}

// Orphaned Explods retain their resources/units, but no longer depend on an
// active helper CNS pass or its hitpause clock. The world advances them once.
func (w *World) advanceOrphanExplods(roots map[*Character]bool, paused, superPaused bool) error {
	keep := w.orphanEffectOwners[:0]
	for _, owner := range w.orphanEffectOwners {
		if roots[owner.rootPlayer()] {
			if err := owner.advanceExplods(paused, superPaused); err != nil {
				return err
			}
		}
		if len(owner.Explods) > 0 {
			keep = append(keep, owner)
		}
	}
	clear(w.orphanEffectOwners[len(keep):])
	w.orphanEffectOwners = keep
	return nil
}
