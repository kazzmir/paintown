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
	// The trigger publishes the result of the subject's most recent update,
	// rather than a freshly queued constraint on another actor's CNS pass.
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
	x, y := c.PositionOf(anchor)
	c.X, c.Y = x+b.x*float64(anchor.Facing), y+b.y
	scale := c.ScaleFrom(anchor)
	c.VelX = anchor.VelX * float64(anchor.Facing*c.Facing) * scale
	c.VelY = anchor.VelY * scale
	c.boundThisTick = true
	c.bindingReported = true
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
