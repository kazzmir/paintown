package character

import (
	"fmt"
	"github.com/kazzmir/paintown/game/mugen/air"
)

func (c *Character) animationClock() *air.Player {
	if c.AirFile == nil {
		return nil
	}
	action := c.AirFile.Actions[c.Anim]
	if action == nil {
		return nil
	}
	if c.animation == nil || c.animation.Action != action {
		c.animation = air.NewPlayer(action)
		c.syncAnimation()
		if c.animation.Error != nil {
			c.RuntimeError = c.animation.Error
		}
	}
	return c.animation
}

func (c *Character) syncAnimation() {
	if c.animation == nil {
		return
	}
	c.AnimTime = c.animation.Elapsed
	c.AnimElem = c.animation.CurrentIdx
	c.AnimElemTime = c.animation.TickCounter
}

func (c *Character) ChangeAnimation(number, element int) error {
	if c.AirFile == nil {
		c.Anim, c.AnimTime, c.AnimElem, c.AnimElemTime = number, 0, 0, 0
		c.animation = nil
		return nil
	}
	action := c.AirFile.Actions[number]
	if action == nil {
		c.EvaluationWarning(fmt.Sprintf("ChangeAnim: action %d is missing", number))
		return nil
	}
	player := air.NewPlayer(action)
	if element != 1 {
		player.Reset(element)
	}
	if player.Error != nil {
		return player.Error
	}
	c.Anim, c.animation = number, player
	c.syncAnimation()
	return nil
}

func (c *Character) GetAnimElemTime(element int) (int, bool) {
	if p := c.animationClock(); p != nil {
		return p.ElemTime(element)
	}
	return 0, false
}

func (c *Character) GetAnimElemNo(offset int) (int, bool) {
	if p := c.animationClock(); p != nil {
		return p.ElemNo(offset)
	}
	return 0, false
}
