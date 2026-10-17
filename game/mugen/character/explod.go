package character

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

func (c *Character) CreateExplod(p engine.ExplodParameters) error {
	p.ID = max(0, p.ID)
	width := c.GameCoord.Width
	if width <= 0 {
		width = 320
	}
	scale := float64(width) / float64(c.LocalCoordinateWidth())
	source := c.CommonEffects
	if !p.Common {
		source = effect.Source{Animations: c.AirFile, Palette: c.Palettes, Scale: scale}
		if c.Palettes != nil {
			source.Sprites = c.Palettes.Archive
		}
	}
	if source.Animations == nil || source.Animations.Actions[p.Action] == nil {
		c.EvaluationWarning(fmt.Sprintf("Explod action %d is unavailable", p.Action))
		return nil
	}
	clock := air.NewPlayer(source.Animations.Actions[p.Action])
	if clock.Error != nil {
		return clock.Error
	}
	if p.RemoveTime < -2 {
		p.RemoveTime = -2
	}
	face := 1
	if p.Facing < 0 {
		face = -1
	}
	x, y := p.X, p.Y
	e := &ExplodData{ID: p.ID, Anim: p.Action, X: x, Y: y, Facing: face, VelX: p.VelX, VelY: p.VelY, AccelX: p.AccelX, AccelY: p.AccelY, Time: p.RemoveTime, Fresh: true, IgnoreHitPause: p.IgnoreHitPause, RemoveOnGetHit: p.RemoveOnGetHit, ScaleToGame: scale, OffsetX: x, OffsetY: y}
	screen := p.ScreenSpace
	switch p.PosType {
	case "p1", "p2":
		e.Anchor = c
		if p.PosType == "p2" {
			if len(c.Opponents) == 0 {
				return nil
			}
			e.Anchor = c.Opponents[0]
		}
		e.BindingTime = p.BindTime
		face *= e.Anchor.Facing
		x, y = c.PositionOf(e.Anchor)
		x, y = x+p.X*float64(e.Anchor.Facing), y+p.Y
	case "front", "back", "left", "right":
		screen = !p.SpaceSpecified || p.ScreenSpace
		observed, _ := c.lookupViewport("screenwidth")
		viewWidth := observed.Number()
		switch p.PosType {
		case "right":
			x += viewWidth
		case "front":
			if c.Facing > 0 {
				x += viewWidth
			}
		case "back":
			x *= float64(c.Facing)
			face *= c.Facing
			if c.Facing < 0 {
				x += viewWidth
			}
		}
		if !screen {
			x += c.cameraPositionX() - viewWidth/2
		}
	}
	if p.BindID != -2 {
		return fmt.Errorf("Explod bindID requires the pending entity-binding implementation")
	}
	e.X, e.Y, e.Facing = x, y, face
	request := effect.Request{Source: source, Action: p.Action, Facing: face, X: x * scale, Y: y * scale, RemoveTime: &e.Time, SpritePriority: p.SpritePriority, VFacing: p.VFacing, ScreenSpace: screen, OnTop: p.OnTop, PauseMoveTime: p.PauseMoveTime, SuperMoveTime: p.SuperMoveTime, ScaleX: p.ScaleX, ScaleY: p.ScaleY, HasScale: true}
	e.Visual = &effect.Instance{Request: request, Clock: clock, Active: true, OwnerDriven: true}
	c.Explods = append(c.Explods, e)
	// State-only hosts can model lifetime/counts without a graphics archive.
	// Production players bind their own/common banks before running controllers.
	if source.Sprites != nil {
		if c.EffectQueue == nil {
			c.BindEffects(nil, c.CommonEffects)
		}
		return c.EffectQueue.PushOwned(e.Visual)
	}
	return nil
}

func (c *Character) advanceExplods(paused, superPaused bool) error {
	keep := c.Explods[:0]
	for _, e := range c.Explods {
		if e.Visual == nil || !e.Visual.Active {
			continue
		}
		if e.Anchor != nil && e.BindingTime != 0 && !e.Anchor.destroyed {
			x, y := c.PositionOf(e.Anchor)
			e.X, e.Y = x+e.OffsetX*float64(e.Anchor.Facing), y+e.OffsetY
			if e.BindingTime > 0 {
				e.BindingTime--
			}
		}
		if e.Fresh {
			e.Fresh = false
			if e.Time == 0 {
				e.Visual.Active = false
			}
		} else if e.IgnoreHitPause || !c.IsPaused() {
			previous := e.Visual.Age
			if err := e.Visual.Advance(paused, superPaused); err != nil {
				return err
			}
			if e.Visual.Age != previous {
				e.X += e.VelX * float64(e.Facing)
				e.Y += e.VelY
				e.VelX, e.VelY = e.VelX+e.AccelX, e.VelY+e.AccelY
			}
		}
		e.Visual.X, e.Visual.Y = e.X*e.ScaleToGame, e.Y*e.ScaleToGame
		e.CurrentTime = e.Visual.Age
		if e.Visual.Active {
			keep = append(keep, e)
		}
	}
	clear(c.Explods[len(keep):])
	c.Explods = keep
	return nil
}

func (c *Character) numExplods(id int, filtered bool) int {
	count := 0
	for _, e := range c.Explods {
		if e.Visual != nil && e.Visual.Active && (!filtered || id < 0 || e.ID == id) {
			count++
		}
	}
	return count
}
