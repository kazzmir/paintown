package character

import (
	"fmt"
	"github.com/kazzmir/paintown/game/mugen/air"
)

func (c *Character) animationClock() *air.Player {
	frames := c.AnimationFile()
	if frames == nil {
		return nil
	}
	action := frames.Actions[c.Anim]
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
	return c.changeAnimationFrom(c.AirFile, number, element)
}

func (c *Character) ChangeAnimation2(number, element int) error {
	return c.changeAnimationFrom(c.currentStateOwner().AirFile, number, element)
}

// AnimationFile follows the current action's owner independently of state
// ownership. Borrowed AIR actions still address this player's sprites.
func (c *Character) AnimationFile() *air.Data {
	if c.animationFile != nil {
		return c.animationFile
	}
	return c.AirFile
}

func (c *Character) changeAnimationFrom(frames *air.Data, number, element int) error {
	if frames == nil {
		c.Anim, c.AnimTime, c.AnimElem, c.AnimElemTime = number, 0, 0, 0
		c.animation = nil
		return nil
	}
	action := frames.Actions[number]
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
	c.Anim, c.animation, c.animationFile = number, player, frames
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
