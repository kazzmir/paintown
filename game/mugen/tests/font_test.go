package tests

import (
	"image/color"
	"os"
	"path/filepath"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/font"
)

func TestAllStockFonts(t *testing.T) {
	entries, err := os.ReadDir(referencePath(t, "font"))
	if err != nil {
		t.Fatal(err)
	}
	count := 0
	for _, entry := range entries {
		ext := filepath.Ext(entry.Name())
		if ext != ".fnt" && ext != ".def" {
			continue
		}
		t.Run(entry.Name(), func(t *testing.T) {
			f, err := font.LoadFont(referencePath(t, "font", entry.Name()))
			if err != nil {
				t.Fatal(err)
			}
			if f.Height <= 0 || f.TextWidth("123") <= 0 {
				t.Fatal("font has no metrics")
			}
			if g := f.Glyph('1', 0); g == nil || g.Image == nil || g.Image.Bounds().Empty() {
				t.Fatal("numeric glyph missing")
			}
		})
		count++
	}
	if count != 19 {
		t.Fatalf("stock font files=%d want 19", count)
	}
}

func TestBitmapFontBanks(t *testing.T) {
	legacy, err := font.LoadFont(referencePath(t, "font", "f-6x8f.fnt"))
	if err != nil {
		t.Fatal(err)
	}
	if g := legacy.Glyph('|', 0); g == nil || g.Width != 6 || g.Image.Bounds().Dx() != 5 {
		t.Fatal("legacy padded final cell")
	}
	f, err := font.LoadFont(referencePath(t, "font", "name14.def"))
	if err != nil {
		t.Fatal(err)
	}
	if f.TextWidth(" I") != 16+1+7+1 {
		t.Fatalf("space/variable width=%d", f.TextWidth(" I"))
	}
	base, red := f.Glyph('A', 0), f.Glyph('A', 1)
	if base == nil || red == nil {
		t.Fatal("palette glyphs")
	}
	different := false
	for y := 0; y < base.Image.Bounds().Dy(); y++ {
		for x := 0; x < base.Image.Bounds().Dx(); x++ {
			if base.Image.At(x, y) != red.Image.At(x, y) {
				different = true
			}
		}
	}
	if !different {
		t.Fatal("color banks have identical pixels")
	}
	if f.Glyph('A', 999) == nil || f.Glyph('世', 0) != nil {
		t.Fatal("invalid bank fallback or ASCII restriction")
	}
	rgba, err := font.LoadFont(referencePath(t, "font", "num72.def"))
	if err != nil {
		t.Fatal(err)
	}
	if rgba.Glyph('0', 0) == nil || rgba.Glyph('0', 1) == nil || rgba.Glyph('0', 99) != rgba.Glyph('0', 0) {
		t.Fatal("sprite bank selection/fallback")
	}
	if c := color.NRGBAModel.Convert(rgba.Glyph('0', 0).Image.At(0, 0)).(color.NRGBA); c.A != 0 {
		t.Fatal("RGBA glyph alpha was lost")
	}
}
