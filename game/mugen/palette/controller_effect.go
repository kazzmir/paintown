package palette

import (
	"image"
	"image/color"
	"math"
)

// Transform remains unclamped through sprite blending. Palette byte images
// cannot represent an overbright effect combined with partial transparency.
type Transform struct {
	Enabled  bool
	Add, Mul [3]float32
	Color    float32
	Invert   bool
}

type Effect struct {
	Time, Color, Period int
	Add, Mul, SinAdd    [3]int
	Invert              bool
	age                 uint64
}

func (s *State) SetEffect(effect Effect) {
	if effect.Time < -1 {
		effect.Time = 0
	}
	effect.age = 0
	s.controllerFX = effect
	s.hitFX = HitEffect{}
	s.Revision++
}

// New own-palette actors copy mappings, but start with no inherited effect.
func (s *State) ClearEffects() {
	s.controllerFX = Effect{}
	s.hitFX = HitEffect{}
	s.Revision++
}

func (s *State) Transform() Transform {
	if s == nil {
		return Transform{}
	}
	fx := s.controllerFX
	if fx.Time == 0 {
		if s.hitFX.Time == 0 {
			return Transform{}
		}
		fx = Effect{Time: s.hitFX.Time, Color: 256, Add: s.hitFX.Add, Mul: s.hitFX.Mul}
	}
	t := Transform{Enabled: true, Color: float32(fx.Color) / 256, Invert: fx.Invert}
	wave := 0.0
	if fx.Period > 0 {
		wave = math.Sin(2 * math.Pi * float64(fx.age%uint64(fx.Period)) / float64(fx.Period))
	}
	for i := 0; i < 3; i++ {
		t.Add[i] = float32(float64(fx.Add[i])+float64(fx.SinAdd[i])*wave) / 255
		t.Mul[i] = float32(fx.Mul[i]) / 256
	}
	return t
}

// Image is a clamped CPU preview. Production sprites use MappedImage plus the
// unclamped GPU transform, so alpha blending retains overbright values.
func (t Transform) Image(source image.Image) image.Image {
	if !t.Enabled {
		return source
	}
	convert := func(source color.Color) color.Color {
		c := color.NRGBAModel.Convert(source).(color.NRGBA)
		const reciprocal = float32(1.0 / 255)
		rgb := [3]float32{float32(c.R) * reciprocal, float32(c.G) * reciprocal, float32(c.B) * reciprocal}
		gray := rgb[0]*0.3125 + rgb[1]*0.4375 + rgb[2]*0.25
		var result [3]uint8
		for i, channel := range rgb {
			channel = gray + (channel-gray)*t.Color
			if t.Invert {
				channel = 1 - channel
			}
			channel = (channel + t.Add[i]) * t.Mul[i]
			result[i] = uint8(max(0, min(255, math.RoundToEven(float64(channel*255)))))
		}
		return color.NRGBA{R: result[0], G: result[1], B: result[2], A: c.A}
	}
	if indexed, ok := source.(*image.Paletted); ok {
		colors := make(color.Palette, len(indexed.Palette))
		for i, c := range indexed.Palette {
			colors[i] = convert(c)
		}
		result := image.NewPaletted(indexed.Bounds(), colors)
		for y := indexed.Bounds().Min.Y; y < indexed.Bounds().Max.Y; y++ {
			start, dest := indexed.PixOffset(indexed.Bounds().Min.X, y), result.PixOffset(result.Bounds().Min.X, y)
			copy(result.Pix[dest:dest+result.Bounds().Dx()], indexed.Pix[start:start+indexed.Bounds().Dx()])
		}
		return result
	}
	result := image.NewNRGBA(source.Bounds())
	for y := source.Bounds().Min.Y; y < source.Bounds().Max.Y; y++ {
		for x := source.Bounds().Min.X; x < source.Bounds().Max.X; x++ {
			result.Set(x, y, convert(source.At(x, y)))
		}
	}
	return result
}
