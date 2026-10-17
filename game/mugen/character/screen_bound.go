package character

import "github.com/kazzmir/paintown/game/mugen/engine"

func (c *Character) SetScreenBound(p engine.ScreenBoundParameters) error {
	c.screenBoundsDisabled = !p.Bound
	c.cameraDisabled = [2]bool{!p.MoveX, !p.MoveY}
	return nil
}

func (c *Character) CameraMovementEnabled(axis int) bool {
	return axis >= 0 && axis < 2 && !c.cameraDisabled[axis]
}

func (c *Character) constrainPosition() {
	if c.screenBoundsDisabled {
		return
	}
	c.X = max(c.LeftBound, min(c.RightBound, c.X))
	if c.ScreenWidth > 0 {
		half := float64(c.ScreenWidth) / 2
		camera := c.cameraPositionX()
		front, back := c.edgeWidths()
		left, right := back, front
		if c.Facing < 0 {
			left, right = front, back
		}
		c.X = max(camera-half+left, min(camera+half-right, c.X))
	}
}
