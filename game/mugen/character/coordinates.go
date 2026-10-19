package character

import "strings"

func (c *Character) spriteScale() (x, y float64) {
	if c.projectile != nil {
		return c.projectile.scale[0], c.projectile.scale[1]
	}
	if c.StateFile == nil {
		return 1, 1
	}
	x, y = c.StateFile.Size.XScale, c.StateFile.Size.YScale
	if x == 0 && len(c.StateFile.ConstantFields["size.xscale"]) == 0 {
		x = 1
	}
	if y == 0 && len(c.StateFile.ConstantFields["size.yscale"]) == 0 {
		y = 1
	}
	return x, y
}

// ScaleFrom converts another player's script units into this player's units.
// Zero-valued profiles are retained for legacy callers constructing Characters.
func (c *Character) ScaleFrom(other *Character) float64 {
	return float64(c.LocalCoordinateWidth()) / float64(other.LocalCoordinateWidth())
}

func (c *Character) PositionOf(other *Character) (float64, float64) {
	scale := c.ScaleFrom(other)
	return other.X * scale, other.Y * scale
}

func (c *Character) hasPushBox() bool {
	element := c.CurrentElement()
	return element != nil && len(element.Clsn2) > 0
}

func (c *Character) bodyWidths() (front, back float64) {
	if c.StateFile == nil {
		return 0, 0
	}
	size := c.StateFile.Size
	if strings.EqualFold(c.StateType, "A") {
		return float64(size.AirFront), float64(size.AirBack)
	}
	return float64(size.GroundFront), float64(size.GroundBack)
}

// GetOpponentBodyDistance measures from the caller's front to the opponent's
// nearer base edge, in the caller's facing and units. Width's push offsets do
// not affect this query. Vertical distance is axis-based.
func (c *Character) GetOpponentBodyDistance() (float64, float64) {
	if len(c.Opponents) == 0 {
		return 0, 0
	}
	other := c.Opponents[0]
	x, y := c.PositionOf(other)
	front, _ := c.bodyWidths()
	otherFront, otherBack := other.bodyWidths()
	otherEdge := otherFront
	direction := float64(other.Facing)
	if (c.X-x)*direction < 0 {
		otherEdge, direction = otherBack, -direction
	}
	x += direction * otherEdge * c.ScaleFrom(other)
	x -= c.X + float64(c.Facing)*front
	return x * float64(c.Facing), y - c.Y
}
