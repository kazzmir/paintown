package palette

import (
	"image"
	"image/color"
	"os"
	"path/filepath"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/sff"
)

func table() *State {
	archive := &sff.SFF{}
	for item := 1; item <= 10; item++ {
		archive.Palettes = append(archive.Palettes, sff.Palette{GroupNumber: 1, ItemNumber: uint16(item), Colors: color.Palette{color.Black, color.RGBA{R: uint8(item), A: 255}}})
	}
	return New(archive)
}

func TestNonTransitiveRemovalAndCapacity(t *testing.T) {
	s := table()
	if err := s.Remap(ID{1, 1}, ID{1, 2}); err != nil {
		t.Fatal(err)
	}
	if err := s.Remap(ID{1, 2}, ID{1, 3}); err != nil {
		t.Fatal(err)
	}
	if s.Destination(ID{1, 1}) != (ID{1, 2}) {
		t.Fatal("mapping became transitive")
	}
	for item := 3; item <= 8; item++ {
		if err := s.Remap(ID{1, item}, ID{1, 10}); err != nil {
			t.Fatal(err)
		}
	}
	before := s.Revision
	if err := s.Remap(ID{1, 9}, ID{1, 10}); err == nil || s.Revision != before {
		t.Fatal("ninth mapping changed state")
	}
	if err := s.Remap(ID{1, 1}, ID{1, 3}); err != nil {
		t.Fatal("existing mapping could not change", err)
	}
	if err := s.Remap(ID{1, 2}, ID{-1, 0}); err != nil {
		t.Fatal(err)
	}
	if err := s.Remap(ID{1, 9}, ID{1, 10}); err != nil {
		t.Fatal("removed slot was not reusable", err)
	}
	s.Remap(ID{1, 1}, ID{1, 1})
	if s.Destination(ID{1, 1}) != (ID{1, 1}) {
		t.Fatal("identity did not remove mapping")
	}
}

func TestGlobalDepthAndIndependentCopy(t *testing.T) {
	s := table()
	s.Archive.Palettes = append(s.Archive.Palettes, sff.Palette{GroupNumber: 2, ItemNumber: 1, Colors: make(color.Palette, 32)})
	if err := s.Remap(ID{1, 1}, ID{2, 1}); err == nil {
		t.Fatal("different palette depths accepted")
	}
	s.Remap(ID{1, 1}, ID{1, 2})
	if err := s.Remap(ID{-1, 0}, ID{1, 3}); err != nil {
		t.Fatal(err)
	}
	if s.Destination(ID{1, 1}) != (ID{1, 3}) || s.Destination(ID{2, 1}) != (ID{2, 1}) {
		t.Fatal("global mapping depth or old-map removal")
	}
	copy := s.Clone()
	copy.Remap(ID{-1, 0}, ID{-1, 0})
	if copy.Destination(ID{1, 1}) != (ID{1, 1}) || s.Destination(ID{1, 1}) != (ID{1, 3}) {
		t.Fatal("independent map copy changed parent")
	}
	before := s.Revision
	if err := s.Remap(ID{-1, 0}, ID{9, 9}); err == nil || s.Revision != before {
		t.Fatal("invalid global destination erased mappings")
	}
}

func TestImageRemapPreservesSourceAndMask(t *testing.T) {
	s := table()
	base := image.NewPaletted(image.Rect(0, 0, 2, 1), s.Archive.Palettes[0].Colors)
	base.Pix = []byte{0, 1}
	s.Archive.Sprites = []sff.Sprite{{PaletteIndex: 0, Image: base, ImageOpaque: base}}
	s.Remap(ID{1, 1}, ID{1, 3})
	img, err := s.Image(0, true)
	if err != nil {
		t.Fatal(err)
	}
	if _, _, _, alpha := img.At(0, 0).RGBA(); alpha != 0 {
		t.Fatal("index zero lost masking")
	}
	if red, _, _, _ := img.At(1, 0).RGBA(); red != 3*257 {
		t.Fatal("destination palette not applied", red)
	}
	if red, _, _, _ := base.At(1, 0).RGBA(); red != 257 {
		t.Fatal("source palette changed")
	}
}

func TestStockSelectedPalettePixels(t *testing.T) {
	for _, name := range []string{"kfm", "kfm720"} {
		t.Run(name, func(t *testing.T) {
			path := filepath.Join("../temp/mugen-1.1b1/chars", name, name+".sff")
			file, err := os.Open(path)
			if err != nil {
				t.Fatal(err)
			}
			archive, err := sff.Parse(file)
			file.Close()
			if err != nil {
				t.Fatal(err)
			}
			s := New(archive)
			if err := s.Remap(ID{1, 1}, ID{1, 2}); err != nil {
				t.Fatal(err)
			}
			destIndex, _ := s.index(ID{1, 2})
			destination := archive.Palettes[destIndex].Colors
			checked := 0
			for index, sprite := range archive.Sprites {
				indexed, ok := sprite.Image.(*image.Paletted)
				if !ok {
					continue
				}
				original := archive.Palettes[sprite.PaletteIndex]
				if original.GroupNumber != 1 || original.ItemNumber != 1 {
					continue
				}
				mapped, err := s.Image(index, true)
				if err != nil {
					t.Fatal(err)
				}
				for y := 0; y < indexed.Bounds().Dy(); y++ {
					for x := 0; x < indexed.Bounds().Dx(); x++ {
						pixel := indexed.ColorIndexAt(x, y)
						want := color.Color(destination[pixel])
						if pixel == 0 {
							want = color.RGBA{}
						}
						if color.RGBAModel.Convert(mapped.At(x, y)) != color.RGBAModel.Convert(want) {
							t.Fatalf("sprite %d pixel %d,%d did not use selected palette", index, x, y)
						}
					}
				}
				checked++
			}
			if checked < 100 {
				t.Fatal("stock palette coverage unexpectedly small", checked)
			}
		})
	}
}
