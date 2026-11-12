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

func TestNativePaletteColors(t *testing.T) {
	var reference struct {
		Inputs map[string]string `json:"native_input_sha256"`
		Cases  []struct {
			Fixture                       string
			Params                        palette.Effect
			Age                           int
			Source, Destination, Expected [3]uint8
			SourceAlpha                   int `json:"source_alpha"`
			DestinationAlpha              int `json:"destination_alpha"`
		}
	}
	root := "../tests/testdata/reference"
	raw, err := os.ReadFile(filepath.Join(root, "palette-native-colors.json"))
	if err != nil {
		t.Fatal(err)
	}
	if err := json.Unmarshal(raw, &reference); err != nil {
		t.Fatal(err)
	}
	if len(reference.Cases) != 252 || len(reference.Inputs) != 37 {
		t.Fatal("incomplete native palette corpus", len(reference.Cases), len(reference.Inputs))
	}
	for name, want := range reference.Inputs {
		raw, err := os.ReadFile(filepath.Join(root, name))
		if err != nil {
			t.Fatal(err)
		}
		h := sha256.Sum256(raw)
		if hex.EncodeToString(h[:]) != want {
			t.Fatal("native palette input changed", name)
		}
	}
	var nearest Nearest
	var linear Linear
	for _, sampler := range []struct {
		name string
		draw func(*ebiten.Image, *ebiten.Image, *ebiten.DrawImageOptions, DrawStyle) error
	}{{"indexed", nearest.DrawStyledImage}, {"rgb", linear.DrawStyledImage}} {
		t.Run(sampler.name, func(t *testing.T) {
			for i, sample := range reference.Cases {
				state := palette.New(nil)
				state.SetEffect(sample.Params)
				for age := 0; age < sample.Age; age++ {
					state.AdvanceHitEffect()
				}
				pixels := image.NewRGBA(image.Rect(0, 0, 1, 1))
				pixels.SetRGBA(0, 0, color.RGBA{sample.Source[0], sample.Source[1], sample.Source[2], 255})
				src, dst := ebiten.NewImageFromImage(pixels), ebiten.NewImage(1, 1)
				dst.Fill(color.RGBA{sample.Destination[0], sample.Destination[1], sample.Destination[2], 255})
				op := &ebiten.DrawImageOptions{}
				style := DrawStyle{DestinationFactor: ApplyAlpha(op, sample.SourceAlpha, sample.DestinationAlpha), Palette: state.Transform()}
				if err := sampler.draw(dst, src, op, style); err != nil {
					t.Fatal(err)
				}
				var got [4]byte
				dst.ReadPixels(got[:])
				src.Deallocate()
				dst.Deallocate()
				if [3]uint8{got[0], got[1], got[2]} != sample.Expected || got[3] != 255 {
					t.Errorf("sample %d %s source=%v age=%d: got %v native %v", i, sample.Fixture, sample.Source, sample.Age, got, sample.Expected)
				}
			}
		})
	}
}
