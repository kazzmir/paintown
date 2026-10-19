package character

import (
	"strings"

	"github.com/kazzmir/paintown/game/mugen/combat"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func (c *Character) LookupLegacy(name, operator string, args []evaluator.Value, extra string) (evaluator.Value, bool) {
	if name != "hitdefattr" {
		return evaluator.Value{}, false
	}
	parts := make([]string, len(args))
	for i, arg := range args {
		value, ok := arg.Text()
		if !ok {
			return evaluator.Value{}, true
		}
		parts[i] = value
	}
	mask, err := combat.ParseAttributes(strings.Join(parts, ","))
	if err != nil {
		c.EvaluationWarning("HitDefAttr: " + err.Error())
		return evaluator.Value{}, true
	}
	hit, _ := combat.ParseAttributes(c.attributesForHit(c.CurrentHitDef))
	matched := !hit.Empty() && mask.Matches(hit)
	if operator == "!=" {
		matched = !matched
	}
	return evaluator.Bool(matched), true
}

// Script X positions use the current screen center; Character.X and all
// collision/binding geometry stay in absolute stage coordinates.
func (c *Character) cameraPositionX() float64 {
	if c.Viewport != nil && c.Viewport.Space.Width > 0 {
		scale := float32(c.LocalCoordinateWidth()) / float32(c.Viewport.Space.Width)
		return float64(float32(c.Viewport.CameraX) * scale)
	}
	return c.CameraX
}

func (c *Character) LookupIdentifier(name string) (evaluator.Value, bool) {
	if value, known := c.lookupEdges(name); known {
		return value, true
	}
	if value, known := c.lookupEntity(name); known {
		return value, true
	}
	if value, known := c.lookupViewport(name); known {
		return value, true
	}
	switch name {
	case "random":
		if err := c.ensureWorld(); err != nil {
			c.EvaluationFailure(err)
			return evaluator.Value{}, true
		}
		return evaluator.Int(int32(c.World.random.Next() % 1000)), true
	case "hitpausetime":
		return evaluator.Int(int32(max(0, c.HitPauseTime))), true
	case "hitvel x", "hitvel y":
		// The legacy trigger returns game-space units, even in a redirect or
		// foreign state. X follows the receiver's current facing; the stored
		// GetHitVar X remains world-oriented. Y keeps its stored sign.
		value, _ := c.hitVelocityInUnits("yvel", c.hitVelocityGameWidth())
		if name == "hitvel x" {
			value, _ = c.hitVelocityInUnits("xvel", c.hitVelocityGameWidth())
			value *= float32(c.Facing)
		}
		return evaluator.Float(value), true
	case "alive":
		if c.updateActive {
			return evaluator.Bool(c.aliveAtTickStart), true
		}
		return evaluator.Bool(c.Life > 0), true
	case "win", "winko", "wintime", "winperfect", "lose", "loseko", "losetime", "drawgame":
		root := c.rootPlayer()
		var result bool
		switch name {
		case "win":
			result = root.RoundWon
		case "winko":
			result = root.RoundWon && !root.RoundTimeOver
		case "wintime":
			result = root.RoundWon && root.RoundTimeOver
		case "winperfect":
			result = root.RoundWon && root.Life == root.MaxLife
		case "lose":
			result = root.RoundLost
		case "loseko":
			result = root.RoundLost && !root.RoundTimeOver
		case "losetime":
			result = root.RoundLost && root.RoundTimeOver
		case "drawgame":
			result = root.RoundDraw
		}
		return evaluator.Bool(result), true
	case "hitcount":
		return evaluator.Int(int32(c.HitCount)), true
	case "uniqhitcount":
		return evaluator.Int(int32(c.UniqHitCount)), true
	case "name", "p1name":
		return evaluator.Text(c.Name), true
	case "authorname":
		return evaluator.Text(c.AuthorName), true
	case "p2name":
		if len(c.Opponents) > 0 {
			return evaluator.Text(c.Opponents[0].Name), true
		}
		return evaluator.Value{}, true
	case "gametime":
		if c.Timeline == nil {
			c.EvaluationWarning("GameTime requires a timeline")
			return evaluator.Value{}, true
		}
		return evaluator.Int(c.Timeline.GameTime), true
	case "tickspersecond":
		if c.Timeline != nil && c.Timeline.TicksPerSecond > 0 {
			return evaluator.Int(c.Timeline.TicksPerSecond), true
		}
		return evaluator.Int(60), true
	case "matchno":
		// The current production lifecycle is one-on-one Versus. Arcade
		// progression and its increment/continue rules remain a mode gate.
		return evaluator.Int(1), true
	case "p2life", "p2stateno", "p2statetype", "p2movetype":
		if len(c.Opponents) == 0 {
			return evaluator.Value{}, true
		}
		other := c.Opponents[0]
		switch name {
		case "p2life":
			return evaluator.Int(int32(other.Life)), true
		case "p2stateno":
			return evaluator.Int(int32(other.StateNo)), true
		case "p2statetype":
			return evaluator.EvaluateValue(&evaluator.Identifier{Name: "statetype"}, other), true
		default:
			return evaluator.EvaluateValue(&evaluator.Identifier{Name: "movetype"}, other), true
		}
	case "numproj":
		return evaluator.Int(int32(c.numProjectiles(0, false))), true
	case "palno":
		return evaluator.Int(int32(c.PaletteNo)), true
	case "ailevel":
		return evaluator.Int(int32(c.AILevel)), true
	case "lifemax":
		return evaluator.Int(int32(c.MaxLife)), true
	case "powermax":
		return evaluator.Int(int32(c.MaxPower)), true
	}
	return evaluator.Value{}, false
}

func (c *Character) lookupViewport(name string) (evaluator.Value, bool) {
	switch name {
	case "gamewidth", "gameheight", "screenwidth", "screenheight", "screenpos x", "screenpos y", "camerapos x", "camerapos y", "camerazoom":
	default:
		return evaluator.Value{}, false
	}
	if c.Viewport == nil || c.Viewport.Space.Width <= 0 || c.Viewport.Space.Height <= 0 {
		c.EvaluationWarning(name + " requires a viewport context")
		return evaluator.Value{}, true
	}
	v := c.Viewport
	scale := float32(c.LocalCoordinateWidth()) / float32(v.Space.Width)
	width, height := float32(v.Space.Width)*scale, float32(v.Space.Height)*scale
	zoom := float32(v.Zoom)
	if zoom <= 0 {
		zoom = 1
	}
	cameraX, cameraY := float32(v.CameraX)*scale, float32(v.CameraY)*scale
	var value float32
	switch name {
	case "gamewidth":
		value = width / zoom
	case "gameheight":
		value = height / zoom
	case "screenwidth":
		value = width
	case "screenheight":
		value = height
	case "camerapos x":
		value = cameraX
	case "camerapos y":
		value = cameraY
	case "camerazoom":
		value = zoom
	case "screenpos x":
		value = float32(c.X - float64(cameraX) + float64(width)/float64(zoom)/2)
	case "screenpos y":
		value = float32(c.Y - float64(cameraY) + v.GroundY*float64(scale)/float64(zoom))
		if c.Timeline != nil {
			value += float32(c.Timeline.ShakeOffset()) * float32(c.LocalCoordinateWidth()) / 320
		}
	}
	return evaluator.Float(value), true
}
