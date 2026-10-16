package character

import "strings"

// ScaleFrom converts another player's script units into this player's units.
// Zero-valued profiles are retained for legacy callers constructing Characters.
func (c *Character) ScaleFrom(other *Character) float64 {
	return float64(c.LocalCoordinateWidth()) / float64(other.LocalCoordinateWidth())
}

func (c *Character) PositionOf(other *Character) (float64, float64) {
	scale := c.ScaleFrom(other)
	return other.X * scale, other.Y * scale
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

// GetOpponentBodyDistance returns front-to-front horizontal distance in the
// caller's facing and local coordinate space. Vertical distance is axis-based.
func (c *Character) GetOpponentBodyDistance() (float64, float64) {
	if len(c.Target) == 0 {
		return 0, 0
	}
	other := c.Target[0]
	x, y := c.PositionOf(other)
	front, _ := c.bodyWidths()
	otherFront, _ := other.bodyWidths()
	x += float64(other.Facing) * otherFront * c.ScaleFrom(other)
	x -= c.X + float64(c.Facing)*front
	return x * float64(c.Facing), y - c.Y
}
