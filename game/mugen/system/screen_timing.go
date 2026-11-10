package main

import (
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
)

// Screen holds include their scheduled fade-out. An early dismissal starts
// that fade at the current tick while the screen's own animation keeps running.
type screenTransition struct {
	leaving  bool
	exitTick int
	image    *ebiten.Image
}

func (s *screenTransition) update(tick, duration, fadeOut int, dismiss bool) bool {
	fadeOut = max(0, fadeOut)
	if !s.leaving {
		switch {
		case dismiss:
			s.leaving, s.exitTick = true, tick
		case duration >= 0 && tick >= max(0, duration-fadeOut):
			s.leaving, s.exitTick = true, max(0, duration-fadeOut)
		}
	}
	return s.leaving && tick-s.exitTick >= fadeOut
}

func (s *screenTransition) draw(screen *ebiten.Image, tick, fadeIn, fadeOut int) {
	alpha := float32(0)
	if fadeIn > 0 && tick < fadeIn {
		alpha = 1 - float32(tick)/float32(fadeIn)
	}
	if s.leaving && fadeOut > 0 {
		alpha = max(alpha, float32(tick-s.exitTick)/float32(fadeOut))
	}
	if alpha <= 0 {
		return
	}
	if s.image == nil {
		s.image = ebiten.NewImage(1, 1)
		s.image.Fill(color.Black)
	}
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Scale(float64(screen.Bounds().Dx()), float64(screen.Bounds().Dy()))
	op.GeoM.Translate(float64(screen.Bounds().Min.X), float64(screen.Bounds().Min.Y))
	op.ColorScale.ScaleAlpha(min(1, alpha))
	screen.DrawImage(s.image, op)
}
