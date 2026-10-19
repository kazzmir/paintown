package character

import (
	"math"

	"github.com/kazzmir/paintown/game/mugen/engine"
)

func (c *Character) interruptTargets() error {
	for _, target := range c.targetOrder {
		if target.binding != nil && target.binding.anchor == c {
			target.releaseHitBinding()
			if target.RuntimeError != nil {
				return target.RuntimeError
			}
		}
	}
	c.hitTargets, c.targetOrder = nil, nil
	return nil
}

func (c *Character) DropTargets(p engine.TargetDropParameters) error {
	previous := append([]*Character(nil), c.targetOrder...)
	keep := c.acquiredTargets(p.ExcludeID, true)
	if p.ExcludeID < 0 {
		keep = nil
	}
	if p.KeepOne && len(keep) > 1 {
		if err := c.ensureWorld(); err != nil {
			return err
		}
		keep = []*Character{keep[c.World.random.Next()%int64(len(keep))]}
	}
	kept := make(map[*Character]hitTarget, len(keep))
	for _, target := range keep {
		kept[target] = c.hitTargets[target]
	}
	for _, target := range previous {
		if _, retained := kept[target]; !retained && target.binding != nil && target.binding.anchor == c {
			target.binding = nil
		}
	}
	c.hitTargets, c.targetOrder = kept, keep
	return nil
}

func (c *Character) AddTargetLife(p engine.TargetLifeParameters) error {
	for _, target := range c.acquiredTargets(p.ID, true) {
		amount := p.Value
		if !p.Absolute && amount < 0 {
			factor := target.DefenceMul
			if target.superDefenceMul > 0 {
				factor /= target.superDefenceMul
			}
			amount = int(math.Round(float64(p.Value) * factor))
		}
		life := target.Life + amount
		if !p.Kill {
			life = max(1, life)
		}
		target.SetLife(life)
	}
	return nil
}

func (c *Character) SetTargetVelocity(p engine.TargetVelocityParameters) error {
	for _, target := range c.acquiredTargets(p.ID, true) {
		scale := float32(target.LocalCoordinateWidth()) / float32(c.LocalCoordinateWidth())
		if p.HasX {
			x := float32(p.X) * scale
			if p.Add {
				x += float32(target.VelX)
			}
			target.VelX = float64(x)
		}
		if p.HasY {
			y := float32(p.Y) * scale
			if p.Add {
				y += float32(target.VelY)
			}
			target.VelY = float64(y)
		}
	}
	return nil
}
