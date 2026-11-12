package character

import (
	"fmt"
	"math"

	"github.com/kazzmir/paintown/game/mugen/effect"
)

func (c *Character) DustClock() (int32, error) {
	if c.Timeline == nil {
		return 0, fmt.Errorf("MakeDust round timeline is unavailable")
	}
	return c.Timeline.RoundEffectTime(), nil
}

func (c *Character) DustPositionAxis(axis int, offset float64) (float64, error) {
	if err := c.ensureWorld(); err != nil {
		return 0, err
	}
	width := c.GameCoord.Width
	if width <= 0 {
		width = 320
	}
	ownScale := float64(width) / float64(c.LocalCoordinateWidth())
	stateScale := float64(width) / float64(c.currentStateOwner().LocalCoordinateWidth())
	position := c.Y * ownScale
	if axis == 0 {
		position = c.X * ownScale
		offset *= float64(c.Facing)
	}
	// Dust adds common-unit jitter to the actual double position before
	// integer conversion. Jitter itself does not follow facing.
	return math.Trunc(position + offset*stateScale + float64(c.World.random.Range(-1, 1))), nil
}

func (c *Character) CreateDust(position [2]float64) error {
	source := c.CommonEffects
	if source.Sprites == nil || source.Animations == nil || source.Animations.Actions[120] == nil {
		c.EvaluationWarning("MakeDust common action 120 is unavailable")
		return nil
	}
	if c.EffectQueue == nil {
		c.BindEffects(nil, c.CommonEffects)
	}
	// Ordinary scene cue: shared admission, no NumExplod owner, no sprite
	// flip, ordinary pause clocks and one-pass animation lifetime.
	return c.EffectQueue.Push(effect.Request{Source: source, Action: 120,
		Facing: 1, X: position[0], Y: position[1]})
}
