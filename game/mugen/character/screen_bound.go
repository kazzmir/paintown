package character

import "github.com/kazzmir/paintown/game/mugen/engine"

func (c *Character) SetScreenBound(p engine.ScreenBoundParameters) error {
	c.screenBoundsDisabled = !p.Bound
	c.cameraDisabled = [2]bool{!p.MoveX, !p.MoveY}
	return nil
}

func (c *Character) CameraMovementEnabled(axis int) bool {
	return !c.destroyed && axis >= 0 && axis < 2 && !c.cameraDisabled[axis]
}

func (c *Character) constrainPosition() {
	if c.screenBoundsDisabled {
		return
	}
	c.X = max(c.LeftBound, min(c.RightBound, c.X))
	if c.ScreenWidth > 0 {
		left, right := c.screenPositionBounds()
		c.X = max(left, min(right, c.X))
	}
}

func (c *Character) screenPositionBounds() (left, right float64) {
	half := float64(c.ScreenWidth) / 2
	camera := c.cameraPositionX()
	front, back := c.edgeWidths()
	leftMargin, rightMargin := back, front
	if c.Facing < 0 {
		leftMargin, rightMargin = front, back
	}
	return camera - half + leftMargin, camera + half - rightMargin
}
