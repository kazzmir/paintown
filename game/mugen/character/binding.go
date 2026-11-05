package character

import (
	"fmt"
	"math"

	"github.com/kazzmir/paintown/game/mugen/engine"
)

func (c *Character) executeSelfBind(params map[string]string, root bool) {
	version := ""
	if owner := c.currentStateOwner(); owner.StateFile != nil && owner.StateFile.States[c.StateNo] != nil {
		version = owner.StateFile.States[c.StateNo].LanguageVersion
	}
	if err := engine.ExecuteSelfBindParameters(params, c.stateEnvironment(), version, root); err != nil {
		c.RuntimeError = fmt.Errorf("self binding: %w", err)
	}
}

type actorBinding struct {
	anchor     *Character
	x, y       float64 // offset in the bound actor's storage units
	remaining  int
	generation uint64
	self       bool
	facing     int
}

// Losing an anchor through retirement or an incoming hit releases a valid
// bound victim into its own falling state. Explicit TargetDrop/TargetBind
// cancellation has a different lifecycle and does not use this transition.
func (c *Character) releaseHitBinding() {
	if c.binding == nil {
		return
	}
	valid := !c.binding.self && c.binding.generation == c.hitGeneration
	c.binding = nil
	if !c.destroyed && valid && c.MoveType == "H" {
		if !c.updateActive {
			c.bindingReported = false
		}
		c.stateOwner = nil
		c.changeStateWithEntry(5050, engine.EntryOverrides{}, true)
	}
}

// Self bindings share the exclusive constraint slot with TargetBind, but keep
// their own phase, velocity and GetHitVar policy.
func (c *Character) BindSelf(root bool, evaluate func() (engine.SelfBindParameters, error)) error {
	if c.Parent == nil || c.isProjectile || c.destroyed {
		return nil
	}
	anchor := c.Parent
	if root {
		anchor = c.rootPlayer()
	}
	if anchor == nil || anchor.destroyed {
		return nil
	}
	p, err := evaluate()
	if err != nil {
		return err
	}
	if p.Time < -1 {
		p.Time = 1
	}
	if p.Time == 0 {
		c.binding = nil
		c.boundThisTick = false
		return nil
	}
	width := p.CoordinateWidth
	if width <= 0 {
		width = c.LocalCoordinateWidth()
	}
	scale := float32(c.LocalCoordinateWidth()) / float32(width)
	c.binding = &actorBinding{anchor: anchor, x: float64(float32(p.X) * scale), y: float64(float32(p.Y) * scale), remaining: p.Time, generation: c.hitGeneration, self: true, facing: p.Facing}
	c.boundThisTick = true
	c.bindingReported = false
	return nil
}

func (c *Character) applySelfBinding() {
	b := c.binding
	if b == nil || !b.self || !c.boundThisTick {
		return
	}
	anchor := b.anchor
	if anchor == nil || anchor.destroyed {
		c.binding = nil
		return
	}
	// CNS observes the previous pose. Position writes within that pass are
	// replaced here, after forces, without replacing the actor's velocity.
	x, y := c.PositionOf(anchor)
	c.X, c.Y = x+b.x*float64(anchor.Facing), y+b.y
	if b.facing != 0 {
		face := anchor.Facing
		if b.facing < 0 {
			face = -face
		}
		if c.Facing != face {
			c.Turn()
		}
	}
}

func (c *Character) BindTargets(p engine.TargetBindParameters) error {
	if p.Time < -1 {
		p.Time = 1
	}
	for _, target := range c.acquiredTargets(p.ID, true) {
		scale := float32(target.LocalCoordinateWidth()) / float32(c.LocalCoordinateWidth())
		record := c.hitTargets[target]
		record.bindX, record.bindY = float64(float32(p.X)*scale), float64(float32(p.Y)*scale)
		c.hitTargets[target] = record
		if p.Time == 0 {
			target.binding = nil
			continue
		}
		target.binding = &actorBinding{anchor: c, x: record.bindX, y: record.bindY, remaining: p.Time, generation: target.hitGeneration}
	}
	return nil
}

// BindToTarget moves the caller now, then constrains the selected target to the
// caller. It is not a self constraint and does not suppress caller movement.
func (c *Character) BindCallerToTarget(id int, evaluate func() (engine.TargetSelfBindParameters, error)) error {
	var selected *Character
	for _, target := range c.acquiredTargets(-1, false) {
		if id == -1 || c.hitTargets[target].id == id {
			selected = target
			break
		}
	}
	if selected == nil {
		return nil
	}
	p, err := evaluate()
	if err != nil {
		return err
	}
	if p.Time < -1 {
		p.Time = 1
	}
	width := p.CoordinateWidth
	if width <= 0 {
		width = c.LocalCoordinateWidth()
	}
	gameWidth := c.GameCoord.Width
	if gameWidth <= 0 {
		gameWidth = 320
	}
	pointX, pointY := 0.0, 0.0
	if selected.StateFile != nil {
		var point []int
		if p.Point == 'h' {
			point = selected.StateFile.Size.HeadPos
		} else if p.Point == 'm' {
			point = selected.StateFile.Size.MidPos
		}
		if len(point) >= 2 {
			factor := float64(gameWidth) / float64(selected.LocalCoordinateWidth())
			pointX, pointY = math.Trunc(float64(point[0])*factor), math.Trunc(float64(point[1])*factor)
		}
	}
	x, y := c.PositionOf(selected)
	record := c.hitTargets[selected]
	toCaller := float64(c.LocalCoordinateWidth()) / float64(gameWidth)
	toTarget := float64(selected.LocalCoordinateWidth()) / float64(gameWidth)
	if p.HasX {
		offset := p.X*float64(gameWidth)/float64(width) + pointX
		c.X = x + offset*float64(selected.Facing)*toCaller
		record.bindX = math.Trunc(-offset*float64(selected.Facing*c.Facing)) * toTarget
	}
	if p.HasY {
		offset := p.Y*float64(gameWidth)/float64(width) + pointY
		c.Y = y + offset*toCaller
		record.bindY = math.Trunc(-offset) * toTarget
	}
	c.hitTargets[selected] = record
	if p.Time == 0 {
		selected.binding = nil
	} else {
		selected.binding = &actorBinding{anchor: c, x: record.bindX, y: record.bindY, remaining: p.Time, generation: selected.hitGeneration}
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
	c.bindingReported = !b.self && anchor.tickMoveType == "A"
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
		if target.destroyed || b == nil || b.self || b.anchor != c || b.generation != target.hitGeneration {
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
