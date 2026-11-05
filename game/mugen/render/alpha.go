package render

import "github.com/hajimehoshi/ebiten/v2"

// ApplyAlpha selects the native complementary or independent blend path.
// The returned destination factor is passed to DrawImage/DrawProjected so an
// independent factor is not represented by a complemented source alpha.
func ApplyAlpha(options *ebiten.DrawImageOptions, source, destination int) float32 {
	source, destination = max(0, min(256, source)), max(0, min(256, destination))
	s, d := float32(source)/256, float32(destination)/256
	options.ColorScale.Scale(s, s, s, 1-d)
	destinationFactor := ebiten.BlendFactorOneMinusSourceAlpha
	native := float32(-1)
	if source+destination != 256 {
		destinationFactor, native = ebiten.BlendFactorSourceAlpha, d
	}
	options.Blend = ebiten.Blend{
		BlendOperationRGB: ebiten.BlendOperationAdd, BlendOperationAlpha: ebiten.BlendOperationAdd,
		BlendFactorSourceRGB: ebiten.BlendFactorOne, BlendFactorDestinationRGB: destinationFactor,
		BlendFactorSourceAlpha: ebiten.BlendFactorOne, BlendFactorDestinationAlpha: ebiten.BlendFactorOneMinusSourceAlpha,
	}
	return native
}

func alphaDestination(values []float32) float32 {
	if len(values) > 0 {
		return values[0]
	}
	return -1
}
