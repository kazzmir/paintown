package character

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

func (c *Character) CreateExplodEvaluated(action int, common bool, evaluate func() (engine.ExplodParameters, error)) error {
	actions := c.AirFile
	if common {
		actions = c.CommonEffects.Animations
	}
	if actions == nil || actions.Actions[action] == nil {
		c.EvaluationWarning(fmt.Sprintf("Explod action %d is unavailable", action))
		return nil
	}
	p, err := evaluate()
	if err != nil {
		return err
	}
	p.Action, p.Common = action, common
	return c.CreateExplod(p)
}

func (c *Character) CreateExplod(p engine.ExplodParameters) error {
	p.Action = max(0, p.Action)
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
	if p.Facing == -1 {
		face = -1
	}
	if p.VFacing != -1 && p.VFacing != 1 {
		p.VFacing = 1
	}
	x, y := p.X, p.Y
	e := &ExplodData{ID: p.ID, Anim: p.Action, X: x, Y: y, Facing: face, MotionFacing: 1, VelX: p.VelX, VelY: p.VelY, AccelX: p.AccelX, AccelY: p.AccelY, Time: p.RemoveTime, Fresh: true, IgnoreHitPause: p.IgnoreHitPause, RemoveOnGetHit: p.RemoveOnGetHit, ScaleToGame: scale, OffsetX: x, OffsetY: y, BindingTime: p.BindTime}
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
		e.OffsetX *= float64(e.Anchor.Facing)
		e.MotionFacing = e.Anchor.Facing
		face *= e.Anchor.Facing
		x, y = c.PositionOf(e.Anchor)
		x, y = x+p.X*float64(e.Anchor.Facing), y+p.Y
		if screen {
			// Legacy postype in screen space puts the actor's screen pose in
			// the offset, while retaining the old actor anchor. Native later
			// adds its stage pose to that offset, even after rejecting bindID.
			ax, _ := e.Anchor.lookupViewport("screenpos x")
			ay, _ := e.Anchor.lookupViewport("screenpos y")
			factor := c.ScaleFrom(e.Anchor)
			e.OffsetX += ax.Number() * factor
			e.OffsetY += ay.Number() * factor
			x, y = e.OffsetX, e.OffsetY
		}
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
	if e.BindingTime < -1 {
		e.BindingTime = -1
	}
	c.retargetExplod(e, p.BindID, screen)
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

func (c *Character) retargetExplod(e *ExplodData, id int, screen bool) {
	if id < -1 {
		return
	}
	if screen {
		c.EvaluationWarning("screen-space Explod cannot bind to a player")
		return
	}
	if id == -1 {
		e.Anchor = c
		return
	}
	if c.World != nil {
		if actor, found := c.World.Registry.Lookup(int32(id)); found && !actor.destroyed {
			e.Anchor = actor
			return
		}
	}
	// A failed retarget retains the old legacy/mutation binding.
	c.EvaluationWarning(fmt.Sprintf("Explod bindID %d is unavailable", id))
}

func (c *Character) SetExplodBindTime(p engine.ExplodBindTimeParameters) error {
	for _, e := range c.Explods {
		if e.Visual != nil && e.Visual.Active && (p.ID < 0 || e.ID == p.ID) {
			e.BindingTime = p.Time
		}
	}
	return nil
}

func (c *Character) ModifyExplods(id int, evaluate func(bool) (engine.ExplodModification, error)) error {
	for _, e := range c.Explods {
		if e.Visual == nil || !e.Visual.Active || id != -1 && e.ID != id {
			continue
		}
		v := e.Visual
		p, err := evaluate(v.OnTop)
		if err != nil {
			return err
		}
		if p.ScaleX != nil {
			v.ScaleX, v.HasScale = *p.ScaleX, true
		}
		if p.ScaleY != nil {
			v.ScaleY, v.HasScale = *p.ScaleY, true
		}
		if p.Angle != nil {
			v.Angle = *p.Angle
		}
		if p.XAngle != nil {
			v.XAngle = *p.XAngle
		}
		if p.YAngle != nil {
			v.YAngle = *p.YAngle
		}
		if p.BindTime != nil {
			e.BindingTime = *p.BindTime
		}
		if p.BindID != nil && *p.BindID != -2 {
			c.retargetExplod(e, *p.BindID, v.ScreenSpace)
		}
		if p.OnTop != nil {
			v.OnTop = *p.OnTop
		}
		if p.RemoveTime != nil {
			e.Time = *p.RemoveTime
			// Creation's -2 animation sentinel is not reused by modification.
			if e.Time < -1 {
				e.Time = 0
			}
		}
		if p.SuperMoveTime != nil {
			v.SuperMoveTime, v.IgnoreSuperPause = *p.SuperMoveTime, false
		}
		if p.SuperMove != nil {
			v.IgnoreSuperPause = *p.SuperMove
		}
		if p.PauseMoveTime != nil {
			v.PauseMoveTime = *p.PauseMoveTime
		}
		if p.SpritePriority != nil {
			v.SpritePriority = *p.SpritePriority
		}
		if p.Shadow != nil {
			v.Shadow = *p.Shadow != 0
		}
		if p.RemoveOnGetHit != nil {
			e.RemoveOnGetHit = *p.RemoveOnGetHit
		}
		if p.IgnoreHitPause != nil {
			e.IgnoreHitPause = *p.IgnoreHitPause
		}
		if p.HasBlend {
			v.HasBlend, v.Blend, v.AlphaSrc, v.AlphaDst = true, p.Blend, p.AlphaSrc, p.AlphaDst
		}
	}
	return nil
}

func (c *Character) advanceExplods(paused, superPaused bool) error {
	keep := c.Explods[:0]
	for _, e := range c.Explods {
		if e.Visual == nil || !e.Visual.Active {
			continue
		}
		fresh := e.Fresh
		previous := e.Visual.Age
		if e.Fresh {
			e.Fresh = false
			if e.Time == 0 {
				e.Visual.Active = false
			}
		} else if c.destroyed || e.IgnoreHitPause || !c.IsPaused() {
			if err := e.Visual.Advance(paused, superPaused); err != nil {
				return err
			}
		}
		if fresh || e.Visual.Age != previous {
			bound := e.Anchor != nil && e.BindingTime != 0 && !e.Anchor.destroyed
			if bound {
				x, y := c.PositionOf(e.Anchor)
				e.X, e.Y = x+e.OffsetX, y+e.OffsetY
				if e.BindingTime > 0 {
					e.BindingTime--
				}
			}
			if !fresh {
				if !bound {
					e.X += e.VelX * float64(e.MotionFacing)
					e.Y += e.VelY
				}
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
