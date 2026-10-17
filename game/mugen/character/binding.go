package character

import "github.com/kazzmir/paintown/game/mugen/engine"

type actorBinding struct {
	anchor     *Character
	x, y       float64 // offset in the bound actor's storage units
	remaining  int
	generation uint64
}

func (c *Character) BindTargets(p engine.TargetBindParameters) error {
	if p.Time < -1 {
		p.Time = 1
	}
	for _, target := range c.acquiredTargets(p.ID, true) {
		if p.Time == 0 {
			target.binding = nil
			continue
		}
		scale := float32(target.LocalCoordinateWidth()) / float32(c.LocalCoordinateWidth())
		target.binding = &actorBinding{anchor: c, x: float64(float32(p.X) * scale), y: float64(float32(p.Y) * scale), remaining: p.Time, generation: target.hitGeneration}
	}
	return nil
}

func (c *Character) prepareBinding() {
	// Own CNS sees whether this pass applied the binding. Other entities see
	// completed requests/passes, published by publishBindings.
	c.bindingReported = false
	b := c.binding
	if b == nil {
		return
	}
	anchor := b.anchor
	if anchor == nil || anchor.destroyed || anchor.World != c.World || b.generation != c.hitGeneration {
		c.binding = nil
		return
	}
	if c.World != nil {
		registered, live := c.World.Registry.Lookup(anchor.EntityID)
		if !live || registered != anchor {
			c.binding = nil
			return
		}
	}
	c.boundThisTick = true
	c.bindingReported = anchor.tickMoveType == "A"
}

func (c *Character) publishBindings() {
	if c.World == nil || c.destroyed || c.worldFrozen {
		return
	}
	// Constraints publish at the anchor's completed movement, before another
	// actor runs. An idle anchor still moves its targets, whose own CNS flag is
	// false; their bound physics remains suppressed while the constraint lives.
	apply := func(target *Character) {
		b := target.binding
		if target.destroyed || b == nil || b.anchor != c || b.generation != target.hitGeneration {
			return
		}
		x, y := target.PositionOf(c)
		target.X, target.Y = x+b.x*float64(c.Facing), y+b.y
		scale := target.ScaleFrom(c)
		target.VelX = c.VelX * float64(c.Facing*target.Facing) * scale
		target.VelY = c.VelY * scale
		target.bindingReported = true
	}
	for _, target := range c.World.Roots {
		apply(target)
	}
	for _, target := range c.World.Helpers {
		apply(target)
	}
}

func (c *Character) ageBinding() {
	if !c.boundThisTick || c.binding == nil || c.binding.remaining < 0 {
		return
	}
	c.binding.remaining--
	if c.binding.remaining == 0 {
		c.binding = nil
	}
}
