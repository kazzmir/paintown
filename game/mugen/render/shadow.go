package render

import "github.com/hajimehoshi/ebiten/v2"

// Shadow projects only the current sprite onto the stage floor. Fade endpoints
// are in common game units, independently of the stage and character profiles.
type Shadow struct {
	Intensity              int
	YScale, XShear, FloorY float64
	FadeRange              [2]float64
	Reflection             bool // colored floor reflection, preserving the sprite palette
}

func (s Shadow) Opacity(y float64, blend string, destination int) float32 {
	opacity := float64(max(0, min(256, s.Intensity))) / 256
	if s.Reflection {
		return float32(opacity)
	}
	if s.YScale == 0 || blend == "sub" {
		return 0
	}
	if blend == "alpha" {
		opacity *= float64(256-max(0, min(256, destination))) / 256
	}
	low, high := s.FadeRange[0], s.FadeRange[1]
	if low < high {
		opacity *= max(0, min(1, (y-low)/(high-low)))
	}
	return float32(opacity)
}

func (s Shadow) Projection(offset float64) ebiten.GeoM {
	var projection ebiten.GeoM
	projection.SetElement(0, 1, s.XShear)
	projection.SetElement(0, 2, -s.XShear*s.FloorY)
	projection.SetElement(1, 1, -s.YScale)
	projection.SetElement(1, 2, (s.FloorY+offset)*(1+s.YScale))
	return projection
}

// SpriteGeometry applies pixel-center and reversed-Y biases in the projected
// surface, independently of those used for the ordinary sprite.
func (s Shadow) SpriteGeometry(geometry ebiten.GeoM, offset float64, indexed, flippedY bool) ebiten.GeoM {
	if flippedY {
		geometry.Translate(0, -1)
	}
	if indexed {
		geometry.Translate(-IndexedOffset, -IndexedOffset)
	}
	geometry.Concat(s.Projection(offset))
	if flippedY != (s.YScale > 0) {
		geometry.Translate(0, 1)
	}
	if indexed {
		// Indexed shear has a half-game-pixel horizontal origin correction.
		geometry.Translate(IndexedOffset-s.XShear/2, IndexedOffset)
	}
	return geometry
}

// SamplingReversal is determined before the stage shear. A strong shear may
// change an axis's dominant screen direction without reversing its texel order.
func (s Shadow) SamplingReversal(geometry ebiten.GeoM, offset float64, indexed, flippedY bool) [2]float32 {
	s.XShear = 0
	return sourceReversal(s.SpriteGeometry(geometry, offset, indexed, flippedY))
}
