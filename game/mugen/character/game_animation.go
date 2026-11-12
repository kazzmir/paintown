package character

import (
	"fmt"
	"math"

	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

func (c *Character) GameAnimationDisplacement(amount int) ([2]int32, error) {
	var offset [2]int32
	if amount <= 0 {
		return offset, nil
	}
	if err := c.ensureWorld(); err != nil {
		return offset, err
	}
	for axis := range offset {
		// Amount one still consumes a draw. Unlike Explod random, these offsets
		// stay in common game units rather than the caller's local profile.
		offset[axis] = c.World.random.Range(0, int32(amount-1)) - int32(amount/2)
	}
	return offset, nil
}

func (c *Character) CreateGameAnimation(p engine.GameAnimationParameters) error {
	if p.Action < 0 {
		c.EvaluationWarning("GameMakeAnim negative action uses common action 0")
		p.Action = 0
	}
	source := c.CommonEffects
	if source.Sprites == nil || source.Animations == nil || source.Animations.Actions[p.Action] == nil {
		c.EvaluationWarning(fmt.Sprintf("GameMakeAnim common action %d is unavailable", p.Action))
		return nil
	}
	width := c.GameCoord.Width
	if width <= 0 {
		width = 320
	}
	ownScale := float64(width) / float64(c.LocalCoordinateWidth())
	stateScale := float64(width) / float64(c.currentStateOwner().LocalCoordinateWidth())
	// Native integerizes with absolute game/stage origins before adding random
	// displacement. Camera subtraction happens later when the cue is drawn.
	// Camera-shifted controls distinguish this from rounding viewport positions
	// or bare world coordinates, including either side of the converted zero.
	originX, originY := float64(width)/2, 0.0
	if c.Viewport != nil {
		originY = c.Viewport.GroundY
	}
	x := math.Trunc(c.X*ownScale+float64(c.Facing)*p.Position[0]*stateScale+originX) - originX
	y := math.Trunc(c.Y*ownScale+p.Position[1]*stateScale+originY) - originY
	x += float64(c.Facing) * float64(p.Displacement[0])
	y += float64(p.Displacement[1])
	if c.EffectQueue == nil {
		c.BindEffects(nil, c.CommonEffects)
	}
	return c.EffectQueue.Push(effect.Request{Source: source, Action: p.Action,
		Facing: c.Facing, X: x, Y: y, Under: p.Under, GameAnimation: true, IgnorePause: true, IgnoreSuperPause: true})
}
