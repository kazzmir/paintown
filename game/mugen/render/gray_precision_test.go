package render

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"image"
	"image/color"
	"os"
	"path/filepath"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/palette"
)

func TestNativeGrayOperationPrecision(t *testing.T) {
	var reference struct {
		Inputs map[string]string `json:"native_input_sha256"`
		Cases  []struct {
			Passes           int
			Source, Expected [3]uint8
		}
	}
	root := "../tests/testdata/reference"
	raw, err := os.ReadFile(filepath.Join(root, "global-gray-colors.json"))
	if err != nil {
		t.Fatal(err)
	}
	if err := json.Unmarshal(raw, &reference); err != nil {
		t.Fatal(err)
	}
	if len(reference.Cases) != 232 || len(reference.Inputs) != 6 {
		t.Fatal("incomplete native gray precision corpus", len(reference.Cases), len(reference.Inputs))
	}
	for name, want := range reference.Inputs {
		raw, err := os.ReadFile(filepath.Join(root, name))
		if err != nil {
			t.Fatal(err)
		}
		h := sha256.Sum256(raw)
		if hex.EncodeToString(h[:]) != want {
			t.Fatal("native gray input changed", name)
		}
	}
	state := palette.New(nil)
	state.SetEffect(palette.Effect{Time: -1, Color: 0, Mul: [3]int{256, 256, 256}})
	var nearest Nearest
	var linear Linear
	for _, sampler := range []struct {
		name string
		draw func(*ebiten.Image, *ebiten.Image, *ebiten.DrawImageOptions, DrawStyle) error
	}{{"indexed", nearest.DrawStyledImage}, {"rgb", linear.DrawStyledImage}} {
		t.Run(sampler.name, func(t *testing.T) {
			for i, sample := range reference.Cases {
				if sample.Passes < 1 || sample.Passes > 2 {
					t.Fatal(sample.Passes)
				}
				pixels := image.NewRGBA(image.Rect(0, 0, 1, 1))
				pixels.SetRGBA(0, 0, color.RGBA{sample.Source[0], sample.Source[1], sample.Source[2], 255})
				src := ebiten.NewImageFromImage(pixels)
				for pass := 0; pass < sample.Passes; pass++ {
					dst := ebiten.NewImage(1, 1)
					if err := sampler.draw(dst, src, &ebiten.DrawImageOptions{}, DrawStyle{DestinationFactor: -1, Palette: state.Transform()}); err != nil {
						t.Fatal(err)
					}
					src.Deallocate()
					src = dst
				}
				var got [4]byte
				src.ReadPixels(got[:])
				src.Deallocate()
				if [3]uint8{got[0], got[1], got[2]} != sample.Expected {
					t.Errorf("sample %d source=%v passes=%d: got=%v native=%v", i, sample.Source, sample.Passes, got, sample.Expected)
				}
			}
		})
	}
}
