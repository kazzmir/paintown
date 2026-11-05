package render

import (
	"github.com/hajimehoshi/ebiten/v2"
	"math"
)

// Nearest preserves sprite-local sampling precision and fractional geometry.
type Nearest struct {
	shader    *ebiten.Shader
	projected *ebiten.Shader
}

const nearestShader = `//kage:unit pixels
package main

var Reverse vec2
var NativeDestination float

func Fragment(dst vec4, src vec2, tint vec4, local vec4) vec4 {
	// Match the measured indexed source-address boundary bias.
	p := floor(local.xy * 16 + vec2(0.5)) / 16
	// Native indexes texels before reversing the source axis. At an exact
	// boundary the reversed axis selects the texel before the boundary.
	p = mix(floor(p), ceil(p) - vec2(1), Reverse) + vec2(0.5)
	// Recover palette bytes before normalization. A texture unit's UNORM
	// conversion can round below the float32 reciprocal product, changing
	// an eight-bit alpha blend at half-integer color boundaries.
	texel := floor(imageSrc0At(imageSrc0Origin() + p) * 255 + vec4(0.5))
	color := applyPalette(texel) * tint
	if NativeDestination >= 0 {
		// Native's independent blend uses a constant destination factor and
		// discards fully transparent texels rather than multiplying that factor
		// by texture alpha. Preserve that behavior on Ebitengine's blend API.
		if texel.a == 0 { discard() }
		color.a = NativeDestination
	}
	return color
}
`

func (r *Nearest) Draw(dst, src *ebiten.Image, geometry ebiten.GeoM, tint ebiten.ColorScale) error {
	return r.DrawImage(dst, src, &ebiten.DrawImageOptions{GeoM: geometry, ColorScale: tint})
}

func (r *Nearest) DrawImage(dst, src *ebiten.Image, options *ebiten.DrawImageOptions, destination ...float32) error {
	return r.DrawStyledImage(dst, src, options, DrawStyle{DestinationFactor: alphaDestination(destination)})
}

func (r *Nearest) DrawStyledImage(dst, src *ebiten.Image, options *ebiten.DrawImageOptions, style DrawStyle) error {
	if options == nil {
		options = &ebiten.DrawImageOptions{}
	}
	geometry, tint := options.GeoM, options.ColorScale
	if !geometry.IsInvertible() {
		return nil
	}
	if r.shader == nil {
		shader, err := ebiten.NewShader([]byte(withPaletteShader(nearestShader)))
		if err != nil {
			return err
		}
		r.shader = shader
	}
	vertices := paddedQuad(src, geometry, tint)
	reverse := sourceReversal(geometry)
	uniforms := style.uniforms()
	uniforms["Reverse"] = reverse
	dst.DrawTrianglesShader(vertices[:], []uint16{0, 1, 2, 1, 3, 2}, r.shader,
		&ebiten.DrawTrianglesShaderOptions{Images: [4]*ebiten.Image{src},
			Blend: options.Blend, CompositeMode: options.CompositeMode,
			Uniforms: uniforms})
	return nil
}

// Source addressing follows each local axis along its dominant screen direction.
// A quarter turn can reverse an axis even when its diagonal component is zero.
func sourceReversal(geometry ebiten.GeoM) (reverse [2]float32) {
	for axis := 0; axis < 2; axis++ {
		a, b := geometry.Element(0, axis), geometry.Element(1, axis)
		if math.Abs(b) > math.Abs(a) {
			a = b
		}
		if a < 0 {
			reverse[axis] = 1
		}
	}
	return reverse
}
