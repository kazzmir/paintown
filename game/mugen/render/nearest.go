package render

import "github.com/hajimehoshi/ebiten/v2"

// Nearest preserves sprite-local sampling precision and fractional geometry.
type Nearest struct {
	shader *ebiten.Shader
}

const nearestShader = `//kage:unit pixels
package main

var Reverse vec2

func Fragment(dst vec4, src vec2, tint vec4, local vec4) vec4 {
	// Match the measured indexed source-address boundary bias.
	p := floor(local.xy * 16 + vec2(0.5)) / 16
	// Native indexes texels before reversing the source axis. At an exact
	// boundary the reversed axis selects the texel before the boundary.
	p = mix(floor(p), ceil(p) - vec2(1), Reverse) + vec2(0.5)
	return imageSrc0At(imageSrc0Origin() + p) * tint
}
`

func (r *Nearest) Draw(dst, src *ebiten.Image, geometry ebiten.GeoM, tint ebiten.ColorScale) error {
	if !geometry.IsInvertible() {
		return nil
	}
	if r.shader == nil {
		shader, err := ebiten.NewShader([]byte(nearestShader))
		if err != nil {
			return err
		}
		r.shader = shader
	}
	vertices := paddedQuad(src, geometry, tint)
	reverse := [2]float32{}
	if geometry.Element(0, 0) < 0 {
		reverse[0] = 1
	}
	if geometry.Element(1, 1) < 0 {
		reverse[1] = 1
	}
	dst.DrawTrianglesShader(vertices[:], []uint16{0, 1, 2, 1, 3, 2}, r.shader,
		&ebiten.DrawTrianglesShaderOptions{Images: [4]*ebiten.Image{src},
			Uniforms: map[string]any{"Reverse": reverse}})
	return nil
}
