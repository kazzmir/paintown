package palette

import (
	"image"
	"image/color"
)

type HitEffect struct {
	Time     int
	Mul, Add [3]int
}

// HitDefs with zero or a negative duration other than -1 leave an existing
// effect alone. This differs from the standalone PalFX controller's cancel.
func (s *State) SetHitEffect(time int, mul, add [3]int) {
	if time <= 0 && time != -1 {
		return
	}
	s.hitFX = HitEffect{Time: time, Mul: mul, Add: add}
	s.Revision++
}

func (s *State) AdvanceHitEffect() {
	if s.hitFX.Time > 0 {
		s.hitFX.Time--
		if s.hitFX.Time == 0 {
			s.Revision++
		}
	}
}

func (fx HitEffect) Color(source color.Color) color.Color {
	c := color.NRGBAModel.Convert(source).(color.NRGBA)
	channel := func(index int, value uint8) uint8 {
		// The beta accepts signed multipliers. Add before multiplying;
		// clamp only the result and round half values upward to a byte.
		product := (int64(value) + int64(fx.Add[index])) * int64(fx.Mul[index])
		if product <= 0 {
			return 0
		}
		return uint8(min(int64(255), (product+128)/256))
	}
	return color.NRGBA{R: channel(0, c.R), G: channel(1, c.G), B: channel(2, c.B), A: c.A}
}

func (fx HitEffect) Image(source image.Image) image.Image {
	if indexed, ok := source.(*image.Paletted); ok {
		colors := make(color.Palette, len(indexed.Palette))
		for i, c := range indexed.Palette {
			colors[i] = fx.Color(c)
		}
		result := image.NewPaletted(indexed.Bounds(), colors)
		for y := indexed.Bounds().Min.Y; y < indexed.Bounds().Max.Y; y++ {
			start := indexed.PixOffset(indexed.Bounds().Min.X, y)
			dest := result.PixOffset(result.Bounds().Min.X, y)
			copy(result.Pix[dest:dest+result.Bounds().Dx()], indexed.Pix[start:start+indexed.Bounds().Dx()])
		}
		return result
	}
	result := image.NewNRGBA(source.Bounds())
	for y := source.Bounds().Min.Y; y < source.Bounds().Max.Y; y++ {
		for x := source.Bounds().Min.X; x < source.Bounds().Max.X; x++ {
			result.Set(x, y, fx.Color(source.At(x, y)))
		}
	}
	return result
}
