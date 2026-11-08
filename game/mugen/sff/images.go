package sff

import (
	"fmt"
	"image"
	"image/color"
)

// SpriteImage returns pixels with an optional v2 palette bank, without changing
// the archive or its shared indexed pixels. paletteIndex=-1 retains the sprite's
// own palette. Unmasked true-color sprites force opacity after source RGB
// premultiplication, matching the beta's RGBA background shader.
func (s *SFF) SpriteImage(spriteIndex, paletteIndex int, masked bool) (image.Image, error) {
	if spriteIndex < 0 || spriteIndex >= len(s.Sprites) {
		return nil, fmt.Errorf("sprite index %d outside archive", spriteIndex)
	}
	sprite := s.Sprites[spriteIndex]
	if paletteIndex < 0 {
		if masked {
			return sprite.Image, nil
		}
		return sprite.ImageOpaque, nil
	}
	if paletteIndex >= len(s.Palettes) {
		return nil, fmt.Errorf("palette index %d outside archive", paletteIndex)
	}
	indexed, ok := sprite.Image.(*image.Paletted)
	if !ok {
		if !masked {
			return sprite.ImageOpaque, nil
		}
		return sprite.Image, nil
	}
	img, opaque, err := indexedImages(indexed.Pix, indexed.Bounds().Dx(), indexed.Bounds().Dy(), s.Palettes[paletteIndex].Colors)
	if err != nil {
		return nil, err
	}
	if masked {
		return img, nil
	}
	return opaque, nil
}

// Keep an opaque view, not a second decoded pixel plane. The native upload
// premultiplies straight RGBA bytes with integer truncation before mask=0
// forces alpha to one. In particular, transparent pixels become opaque black.
type opaqueRGBA struct{ image.Image }

func (*opaqueRGBA) ColorModel() color.Model { return color.RGBAModel }

func (i *opaqueRGBA) At(x, y int) color.Color {
	if !image.Pt(x, y).In(i.Bounds()) {
		return color.RGBA{}
	}
	c := nativeRGBA(i.Image.At(x, y))
	c.A = 255
	return c
}

func nativeRGBA(c color.Color) color.RGBA {
	if n, ok := c.(color.NRGBA); ok {
		return color.RGBA{uint8(uint16(n.R) * uint16(n.A) / 255), uint8(uint16(n.G) * uint16(n.A) / 255), uint8(uint16(n.B) * uint16(n.A) / 255), n.A}
	}
	r, g, b, a := c.RGBA()
	return color.RGBA{uint8(r >> 8), uint8(g >> 8), uint8(b >> 8), uint8(a >> 8)}
}

type uploadRGBA struct{ image.Image }

func (*uploadRGBA) ColorModel() color.Model   { return color.RGBAModel }
func (i *uploadRGBA) At(x, y int) color.Color { return nativeRGBA(i.Image.At(x, y)) }

// UploadPixels keeps source pixels unchanged while using native byte-truncated
// RGBA premultiplication. Go's general image conversion instead premultiplies
// at 16-bit precision, changing some eight-bit colors by one. Indexed and
// already-premultiplied views pass through without conversion.
func UploadPixels(pixels image.Image) image.Image {
	if _, straight := pixels.(*image.NRGBA); straight {
		return &uploadRGBA{pixels}
	}
	return pixels
}

func trueColorImages(img image.Image) (image.Image, image.Image, error) {
	if opaque, ok := img.(interface{ Opaque() bool }); ok && opaque.Opaque() {
		return img, img, nil
	}
	return img, &opaqueRGBA{img}, nil
}
