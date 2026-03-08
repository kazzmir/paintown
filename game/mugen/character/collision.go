package character

import (
	"github.com/kazzmir/paintown/game/mugen/air"
)

// BoxToWorld transforms a local animation box to world coordinates.
func BoxToWorld(box air.Box, x, y float64, facing int, scaleX, scaleY float64) (float64, float64, float64, float64) {
	// Facing 1 = Right, -1 = Left
	// Scale applies to the local coordinates

	x1 := float64(box.X1) * scaleX
	x2 := float64(box.X2) * scaleX
	y1 := float64(box.Y1) * scaleY
	y2 := float64(box.Y2) * scaleY

	if facing == -1 {
		// Flip X across the vertical axis (x=0 locally)
		x1, x2 = -x2, -x1
	}

	// Translate to world position
	return x + x1, y + y1, x + x2, y + y2
}

// Intersects checks if two world-space rectangles overlap.
func Intersects(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2 float64) bool {
	// Ensure coordinates are normalized (min to max)
	if ax1 > ax2 {
		ax1, ax2 = ax2, ax1
	}
	if ay1 > ay2 {
		ay1, ay2 = ay2, ay1
	}
	if bx1 > bx2 {
		bx1, bx2 = bx2, bx1
	}
	if by1 > by2 {
		by1, by2 = by2, by1
	}

	return ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1
}

// CheckCollision evaluates if the attacker's hitboxes (Clsn1) intersect
// with the defender's hurtboxes (Clsn2).
func CheckCollision(attacker, defender *Character) bool {
	// Attaker must have a HitDef active and boxes to check
	if attacker.MoveType != "A" {
		return false
	}

	el1 := attacker.CurrentElement()
	if el1 == nil || len(el1.Clsn1) == 0 {
		return false
	}

	el2 := defender.CurrentElement()
	if el2 == nil || len(el2.Clsn2) == 0 {
		return false
	}

	// Get scales from CNS Size
	s1x, s1y := attacker.StateFile.Size.XScale, attacker.StateFile.Size.YScale
	s2x, s2y := defender.StateFile.Size.XScale, defender.StateFile.Size.YScale
	if s1x == 0 {
		s1x = 1
	}
	if s1y == 0 {
		s1y = 1
	}
	if s2x == 0 {
		s2x = 1
	}
	if s2y == 0 {
		s2y = 1
	}

	for _, b1 := range el1.Clsn1 {
		ax1, ay1, ax2, ay2 := BoxToWorld(b1, attacker.X, attacker.Y, attacker.Facing, s1x, s1y)
		for _, b2 := range el2.Clsn2 {
			bx1, by1, bx2, by2 := BoxToWorld(b2, defender.X, defender.Y, defender.Facing, s2x, s2y)
			if Intersects(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2) {
				return true
			}
		}
	}

	return false
}
