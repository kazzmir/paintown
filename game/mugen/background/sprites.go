package background

import (
	"image"
	"reflect"

	"github.com/hajimehoshi/ebiten/v2"
)

// SpriteCache belongs to one loaded background/storyboard. Linked immutable
// pixel views share uploads; unseen frames and unused mask modes upload nothing.
// All access stays on the drawing thread.
type SpriteCache struct {
	images map[image.Image]*ebiten.Image
}

func (c *SpriteCache) Pair(masked, opaque image.Image) *SpriteImages {
	return &SpriteImages{maskedPixels: masked, opaquePixels: opaque, cache: c}
}

func (c *SpriteCache) upload(pixels image.Image) *ebiten.Image {
	if pixels == nil {
		return nil
	}
	// SFF views are comparable pointers. External image implementations may
	// contain slices; those still upload, without using an invalid map key.
	if !reflect.TypeOf(pixels).Comparable() {
		return ebiten.NewImageFromImage(pixels)
	}
	if c.images == nil {
		c.images = make(map[image.Image]*ebiten.Image)
	}
	if cached := c.images[pixels]; cached != nil {
		return cached
	}
	img := ebiten.NewImageFromImage(pixels)
	c.images[pixels] = img
	return img
}

// Image also accepts prebuilt Masked/Opaque pairs supplied by existing callers.
func (s *SpriteImages) Image(masked bool) *ebiten.Image {
	if masked {
		if s.Masked == nil && s.maskedPixels != nil {
			s.Masked = s.cache.upload(s.maskedPixels)
		}
		return s.Masked
	}
	if s.Opaque == nil {
		if s.opaquePixels == nil {
			s.Opaque = s.Image(true)
		} else {
			s.Opaque = s.cache.upload(s.opaquePixels)
		}
	}
	return s.Opaque
}
