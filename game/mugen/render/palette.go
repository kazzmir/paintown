package render

import (
	"strings"

	"github.com/kazzmir/paintown/game/mugen/palette"
)

type DrawStyle struct {
	DestinationFactor float32
	Palette           palette.Transform
}

// Texture RGB is premultiplied. Inversion and addition therefore use the
// texture alpha. Apply the effect before tint/source-alpha and framebuffer
// clamping, as in the supplied native indexed/RGBA shaders.
const paletteShader = `
var PaletteEnabled float
var PaletteAdd vec3
var PaletteMul vec3
var PaletteColor float
var PaletteInvert float
func applyPalette(texel vec4) vec4 {
 color := texel * (1.0 / 255.0)
 if PaletteEnabled != 0 {
  if PaletteColor != 1 {
   grey := dot(color.rgb, vec3(0.3125,0.4375,0.25))
   // Complete desaturation has distinct native rounding. Partial color
   // balance retains normalized interpolation; full gray uses byte luminance.
   if PaletteColor == 0 {
    grey = dot(texel.rgb, vec3(0.3125,0.4375,0.25)) * (1.0 / 255.0)
   }
   color.rgb = mix(vec3(grey),color.rgb,PaletteColor)
  }
  if PaletteInvert != 0 { color.rgb = vec3(color.a)-color.rgb }
  color.rgb = (color.rgb+color.a*PaletteAdd)*PaletteMul
 }
 return color
}
`

func withPaletteShader(source string) string {
	return strings.Replace(source, "package main", "package main\n"+paletteShader, 1)
}

func (style DrawStyle) uniforms() map[string]any {
	enabled, invert := float32(0), float32(0)
	if style.Palette.Enabled {
		enabled = 1
	}
	if style.Palette.Invert {
		invert = 1
	}
	return map[string]any{"NativeDestination": style.DestinationFactor,
		"PaletteEnabled": enabled, "PaletteAdd": style.Palette.Add,
		"PaletteMul": style.Palette.Mul, "PaletteColor": style.Palette.Color,
		"PaletteInvert": invert}
}
