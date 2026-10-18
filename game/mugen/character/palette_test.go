package character

import (
	"image"
	"image/color"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

func TestSpriteCacheTracksPaletteChangesAndRoundOwners(t *testing.T) {
	colors := color.Palette{color.Black, color.White}
	img := image.NewPaletted(image.Rect(0, 0, 1, 1), colors)
	archive := &sff.SFF{Sprites: []sff.Sprite{{Image: img, PaletteIndex: 0}}, Palettes: []sff.Palette{{GroupNumber: 1, ItemNumber: 1, Colors: colors}, {GroupNumber: 1, ItemNumber: 2, Colors: color.Palette{color.Black, color.RGBA{R: 255, A: 255}}}}}
	c := NewCharacter(&cns.CNS{}, nil, nil)
	c.Palettes = palette.New(archive)
	p := &Player{Character: c, SFF: archive}
	first := p.getEbitenImage(c, 0, 0)
	if err := c.SetPaletteMapping([2]int{1, 1}, [2]int{1, 2}); err != nil {
		t.Fatal(err)
	}
	second := p.getEbitenImage(c, 0, 0)
	if first == second {
		t.Fatal("palette change reused stale GPU image")
	}
	c.SetPaletteMapping([2]int{1, 1}, [2]int{1, 2})
	if p.getEbitenImage(c, 0, 0) != second {
		t.Fatal("idempotent mapping rebuilt image")
	}
	c.Palettes = palette.New(archive)
	p.pruneSpriteCache()
	if len(p.spriteCache) != 0 {
		t.Fatal("retired round owner retained cached sprites")
	}
}

func TestInvalidPaletteMappingWarnsWithoutChangingState(t *testing.T) {
	c := NewCharacter(&cns.CNS{}, nil, nil)
	c.Palettes = palette.New(&sff.SFF{})
	if err := c.SetPaletteMapping([2]int{1, 1}, [2]int{1, 2}); err != nil {
		t.Fatal(err)
	}
	if c.Palettes.Revision != 0 || len(c.RuntimeWarnings) != 1 {
		t.Fatal("invalid palette failed silently or changed state")
	}
}

func TestSpriteCacheSeparatesHitEffectsFromPortraits(t *testing.T) {
	colors := color.Palette{color.Black, color.White}
	img := image.NewPaletted(image.Rect(0, 0, 1, 1), colors)
	archive := &sff.SFF{Sprites: []sff.Sprite{{Image: img, PaletteIndex: 0}}, Palettes: []sff.Palette{{GroupNumber: 1, ItemNumber: 1, Colors: colors}}}
	c := NewCharacter(&cns.CNS{}, nil, nil)
	c.Palettes = palette.New(archive)
	c.Palettes.SetHitEffect(-1, [3]int{}, [3]int{})
	p := &Player{Character: c, SFF: archive}
	body := p.getEbitenImage(c, 0, 0)
	portrait := p.getMappedSpriteImage(c, 0, 0, false)
	if body == nil || portrait == nil || body == portrait || len(p.spriteCache) != 2 {
		t.Fatal("body and portrait shared a filtered cache entry")
	}
}
