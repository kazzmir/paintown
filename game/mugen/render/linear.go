// Package render implements the measured fight-surface texture samplers.
package render

import "github.com/hajimehoshi/ebiten/v2"

// Linear retains fractional geometry, clamps to the texture's edge and uses
// the eight-bit interpolation weights of the reference OpenGL renderer.
type Linear struct {
	shader *ebiten.Shader
}

const linearShader = `//kage:unit pixels
package main

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
	top := floor((a * (256-f.x) + b * f.x + vec4(128)) / 256)
	bottom := floor((c * (256-f.x) + d * f.x + vec4(128)) / 256)
	return floor((top * (256-f.y) + bottom * f.y + vec4(128)) / 256) / 255 * tint
}
`

func (r *Linear) Draw(dst, src *ebiten.Image, geometry ebiten.GeoM, tint ebiten.ColorScale) error {
	if r.shader == nil {
		shader, err := ebiten.NewShader([]byte(linearShader))
		if err != nil {
			return err
		}
		r.shader = shader
	}
	// Rectangle APIs quantize vertices to a coarse subpixel grid. Triangles
	// preserve the source-to-destination mapping through rasterization.
	vertices := quad(src, geometry, tint)
	dst.DrawTrianglesShader(vertices[:], []uint16{0, 1, 2, 1, 3, 2}, r.shader,
		&ebiten.DrawTrianglesShaderOptions{Images: [4]*ebiten.Image{src}})
	return nil
}
