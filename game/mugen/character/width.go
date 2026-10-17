package character

import (
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func (c *Character) SetWidth(p engine.WidthParameters) error {
	c.frameWidths = p
	return nil
}

func (c *Character) edgeWidths() (front, back float64) {
	front, back = c.ScreenRightMargin, c.ScreenLeftMargin
	if c.Facing < 0 {
		front, back = back, front
	}
	return front + c.frameWidths.Edge[0], back + c.frameWidths.Edge[1]
}

func (c *Character) pushWidths() (front, back float64) {
	front, back = c.bodyWidths()
	return front + c.frameWidths.Player[0], back + c.frameWidths.Player[1]
}

func (c *Character) lookupEdges(name string) (evaluator.Value, bool) {
	switch name {
	case "leftedge", "rightedge", "frontedge", "backedge", "frontedgedist", "backedgedist", "frontedgebodydist", "backedgebodydist":
	default:
		return evaluator.Value{}, false
	}
	if c.Viewport == nil || c.Viewport.Space.Width <= 0 {
		c.EvaluationWarning(name + " requires a viewport context")
		return evaluator.Value{}, true
	}
	v := c.Viewport
	scale := float32(c.LocalCoordinateWidth()) / float32(v.Space.Width)
	zoom := float32(v.Zoom)
	if zoom <= 0 {
		zoom = 1
	}
	half := float32(v.Space.Width) * scale / zoom / 2
	camera := float32(c.cameraPositionX())
	left, right := camera-half, camera+half
	front, back := right, left
	frontMargin, backMargin := c.ScreenRightMargin, c.ScreenLeftMargin
	if c.Facing < 0 {
		front, back = left, right
		frontMargin, backMargin = backMargin, frontMargin
	}
	frontDistance := (front-float32(c.X))*float32(c.Facing) - float32(frontMargin)
	backDistance := (float32(c.X)-back)*float32(c.Facing) - float32(backMargin)
	var value float32
	switch name {
	case "leftedge":
		value = left
	case "rightedge":
		value = right
	case "frontedge":
		value = front
	case "backedge":
		value = back
	case "frontedgedist":
		value = frontDistance
	case "backedgedist":
		value = backDistance
	case "frontedgebodydist":
		value = frontDistance - float32(c.frameWidths.Edge[0])
	case "backedgebodydist":
		value = backDistance - float32(c.frameWidths.Edge[1])
	}
	return evaluator.Float(value), true
}
