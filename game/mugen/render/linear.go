// Package render implements the measured fight-surface texture samplers.
package render

import "github.com/hajimehoshi/ebiten/v2"

// Linear retains fractional geometry, clamps to the texture's edge and uses
// the eight-bit interpolation weights of the reference OpenGL renderer.
type Linear struct {
	shader    *ebiten.Shader
	projected *ebiten.Shader
}

const linearShader = `//kage:unit pixels
package main

var NativeDestination float

// Round the weighted difference, then add the first byte. Rounding the final
// sum gives different ties when the first byte is odd. This is the reference
// Linux GL sampler's widened-UNORM interpolation (Mesa lp_build_lerp_simple).
func lerpBytes(a, b vec4, weight float) vec4 {
	delta := (b-a) * (weight / 256)
	rounded := floor(delta + vec4(0.5))
	tie := vec4(1) - sign(abs(fract(delta) - vec4(0.5)))
	odd := rounded - 2 * floor(rounded / 2)
	return a + rounded - tie * odd
}

func Fragment(dst vec4, src vec2, tint vec4, local vec4) vec4 {
	lo := vec2(0.5)
	hi := imageSrc0Size() - vec2(0.5)
	p := clamp(local.xy, lo, hi)
	base := floor(p - vec2(0.5)) + vec2(0.5)
	f := floor((p - base) * 256 + vec2(0.5))
	origin := imageSrc0Origin()
	a := floor(imageSrc0At(origin + clamp(base, lo, hi)) * 255 + vec4(0.5))
	b := floor(imageSrc0At(origin + clamp(base + vec2(1, 0), lo, hi)) * 255 + vec4(0.5))
	c := floor(imageSrc0At(origin + clamp(base + vec2(0, 1), lo, hi)) * 255 + vec4(0.5))
	d := floor(imageSrc0At(origin + clamp(base + vec2(1, 1), lo, hi)) * 255 + vec4(0.5))
	top := lerpBytes(a, b, f.x)
	bottom := lerpBytes(c, d, f.x)
	texel := lerpBytes(top, bottom, f.y)
	color := applyPalette(texel) * tint
	if NativeDestination >= 0 {
		if texel.a == 0 { discard() }
		color.a = NativeDestination
	}
	return color
}
`

func (r *Linear) Draw(dst, src *ebiten.Image, geometry ebiten.GeoM, tint ebiten.ColorScale) error {
	return r.DrawImage(dst, src, &ebiten.DrawImageOptions{GeoM: geometry, ColorScale: tint})
}

// DrawImage retains the caller's blend operation as well as its transform/tint.
func (r *Linear) DrawImage(dst, src *ebiten.Image, options *ebiten.DrawImageOptions, destination ...float32) error {
	return r.DrawStyledImage(dst, src, options, DrawStyle{DestinationFactor: alphaDestination(destination)})
}

func (r *Linear) DrawStyledImage(dst, src *ebiten.Image, options *ebiten.DrawImageOptions, style DrawStyle) error {
	if options == nil {
		options = &ebiten.DrawImageOptions{}
	}
	if r.shader == nil {
		shader, err := ebiten.NewShader([]byte(withPaletteShader(linearShader)))
		if err != nil {
			return err
		}
		r.shader = shader
	}
	// Rectangle APIs quantize vertices to a coarse subpixel grid. Triangles
	// preserve the source-to-destination mapping through rasterization.
	vertices := quad(src, options.GeoM, options.ColorScale)
	dst.DrawTrianglesShader(vertices[:], []uint16{0, 1, 2, 1, 3, 2}, r.shader,
		&ebiten.DrawTrianglesShaderOptions{Images: [4]*ebiten.Image{src},
			Blend: options.Blend, CompositeMode: options.CompositeMode,
			Uniforms: style.uniforms()})
	return nil
}
