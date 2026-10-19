package character

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/effect"
)

func (c *Character) BindEffects(queue *effect.Queue, common effect.Source) {
	if queue == nil {
		queue = &effect.Queue{}
	}
	c.EffectQueue, c.CommonEffects = queue, common
}

func (c *Character) contactEffect(attacker *Character, hit cns.HitDef, guarded bool) error {
	return c.contactEffectFrom(attacker, attacker, hit, guarded)
}

// Reversals use the incoming attack's geometry with the reverser's resources
// and spark offset units. Ordinary contacts have the same owner for both.
func (c *Character) contactEffectFrom(attacker, sourceOwner *Character, hit cns.HitDef, guarded bool) error {
	action, own := hit.SparkNo, hit.SparkOwn
	if guarded {
		action, own = hit.GuardSparkNo, hit.GuardSparkOwn
	}
	if action < 0 {
		return nil
	}
	gameWidth := attacker.GameCoord.Width
	if gameWidth <= 0 {
		gameWidth = 320
	}
	attackerScale := float64(gameWidth) / float64(attacker.LocalCoordinateWidth())
	sourceScale := float64(gameWidth) / float64(sourceOwner.LocalCoordinateWidth())
	defenderScale := float64(gameWidth) / float64(c.LocalCoordinateWidth())
	source := sourceOwner.CommonEffects
	if own {
		source = effect.Source{Animations: sourceOwner.AirFile, Palette: sourceOwner.Palettes, Scale: sourceScale}
		if sourceOwner.Palettes != nil {
			source.Sprites = sourceOwner.Palettes.Archive
		}
	}
	if source.Sprites == nil || source.Animations == nil || source.Animations.Actions[action] == nil {
		attacker.EvaluationWarning(fmt.Sprintf("spark action %d is unavailable", action))
		return nil
	}
	direction := -attacker.Facing
	attackerX, _ := c.PositionOf(attacker)
	if attackerX > c.X {
		direction = 1
	} else if attackerX < c.X {
		direction = -1
	}
	front, back := c.bodyWidths()
	width := front
	if c.Facing != direction {
		width = back
	}
	var offsetX, offsetY float64
	if len(hit.SparkXY) > 0 {
		offsetX = float64(hit.SparkXY[0])
	}
	if len(hit.SparkXY) > 1 {
		offsetY = float64(hit.SparkXY[1])
	}
	x := (c.X+float64(direction)*width)*defenderScale + float64(direction)*offsetX*sourceScale
	y := (attacker.Y + offsetY) * attackerScale
	if sourceOwner != attacker {
		y = attacker.Y*attackerScale + offsetY*sourceScale
	}
	if attacker.EffectQueue == nil {
		attacker.BindEffects(nil, attacker.CommonEffects)
	}
	return attacker.EffectQueue.Push(effect.Request{Source: source, Action: action, Facing: attacker.Facing, X: x, Y: y})
}
