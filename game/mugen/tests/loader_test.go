package tests

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"image/color"
	"io/fs"
	"os"
	"path/filepath"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/sff"
)

func TestKFMReferencePixels(t *testing.T) {
	var manifest struct {
		Source  string
		Sprites []struct {
			Index, Group, Item, X, Y, Width, Height int
			Hash                                    string `json:"rgba_sha256"`
		}
	}
	data, err := os.ReadFile("testdata/kfm-sprites.json")
	if err != nil {
		t.Fatal(err)
	}
	if err = json.Unmarshal(data, &manifest); err != nil {
		t.Fatal(err)
	}
	f, err := os.Open(referencePath(t, manifest.Source))
	if err != nil {
		t.Fatal(err)
	}
	defer f.Close()
	archive, err := sff.Parse(f)
	if err != nil {
		t.Fatal(err)
	}
	if len(archive.Sprites) != len(manifest.Sprites) {
		t.Fatal("sprite count differs from extraction")
	}
	for _, want := range manifest.Sprites {
		got := archive.Sprites[want.Index]
		if int(got.GroupNumber) != want.Group || int(got.ImageNumber) != want.Item || int(got.XAxis) != want.X || int(got.YAxis) != want.Y {
			t.Fatalf("sprite %d metadata differs", want.Index)
		}
		bounds := got.ImageOpaque.Bounds()
		if bounds.Dx() != want.Width || bounds.Dy() != want.Height {
			t.Fatalf("sprite %d bounds=%v", want.Index, bounds)
		}
		hash := sha256.New()
		for y := 0; y < bounds.Dy(); y++ {
			for x := 0; x < bounds.Dx(); x++ {
				c := color.NRGBAModel.Convert(got.ImageOpaque.At(x, y)).(color.NRGBA)
				hash.Write([]byte{c.R, c.G, c.B, c.A})
			}
		}
		if hex.EncodeToString(hash.Sum(nil)) != want.Hash {
			t.Errorf("sprite %d (%d,%d) pixels differ from Elecbyte extraction", want.Index, want.Group, want.Item)
		}
	}
}

func TestAllStockSFF(t *testing.T) {
	root := referenceRoot(t)
	files, total := 0, 0
	err := filepath.WalkDir(root, func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			return err
		}
		if d.IsDir() || filepath.Ext(path) != ".sff" {
			return nil
		}
		rel, _ := filepath.Rel(root, path)
		t.Run(rel, func(t *testing.T) {
			f, err := os.Open(path)
			if err != nil {
				t.Fatal(err)
			}
			defer f.Close()
			archive, err := sff.Parse(f)
			if err != nil {
				t.Fatal(err)
			}
			if len(archive.Sprites) == 0 {
				t.Fatal("empty archive")
			}
			for i, s := range archive.Sprites {
				if s.Image == nil || s.ImageOpaque == nil || s.Image.Bounds().Empty() {
					t.Fatalf("sprite %d (%d,%d) missing pixels", i, s.GroupNumber, s.ImageNumber)
				}
			}
			total += len(archive.Sprites)
		})
		files++
		return nil
	})
	if err != nil {
		t.Fatal(err)
	}
	if files != 27 {
		t.Fatalf("stock SFF files=%d, want 27", files)
	}
	t.Logf("decoded %d sprites in %d stock archives", total, files)
}

func TestParseSystemSFF(t *testing.T) {
	sffPath := referencePath(t, "data", "system.sff")

	f, err := os.Open(sffPath)
	if err != nil {
		t.Fatalf("required reference fixture: %v", err)
	}
	defer f.Close()

	sffFile, err := sff.Parse(f)
	if err != nil {
		t.Fatalf("Failed to parse SFF: %v", err)
	}

	if len(sffFile.Sprites) == 0 {
		t.Errorf("Parsed 0 sprites, expected many")
	}

	successCount := 0
	for idx, spr := range sffFile.Sprites {
		if spr.Image != nil {
			bounds := spr.Image.Bounds()
			if bounds.Dx() > 0 && bounds.Dy() > 0 {
				successCount++
			} else {
				t.Errorf("Sprite %d (Group %d, Item %d) has 0-size bounds", idx, spr.GroupNumber, spr.ImageNumber)
			}
		} else if !spr.Linked {
			t.Errorf("Sprite %d (Group %d, Item %d) is not linked but has no image", idx, spr.GroupNumber, spr.ImageNumber)
		}
	}

	t.Logf("Successfully parsed %d total sprites (%d with valid PCX images).", len(sffFile.Sprites), successCount)
}
