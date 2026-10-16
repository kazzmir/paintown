package sff

import (
	"fmt"
	"image"
)

// SpriteImage returns pixels with an optional v2 palette bank, without changing
// the archive or its shared indexed pixels. paletteIndex=-1 retains the sprite's
// own palette. True-color sprites retain their per-pixel alpha in either mode.
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
