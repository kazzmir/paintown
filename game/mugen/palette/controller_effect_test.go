package palette

import (
	"encoding/json"
	"image"
	"image/color"
	"os"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/sff"
)

func TestNativePaletteImageColors(t *testing.T) {
	var reference struct {
		Cases []struct {
			Fixture          string
			Params           Effect
			Age              int
			Source, Expected [3]uint8
			SourceAlpha      int `json:"source_alpha"`
		}
	}
	raw, err := os.ReadFile("../tests/testdata/reference/palette-native-colors.json")
	if err != nil {
		t.Fatal(err)
	}
	if err := json.Unmarshal(raw, &reference); err != nil {
		t.Fatal(err)
	}
	if len(reference.Cases) != 252 {
		t.Fatal(len(reference.Cases))
	}
	for i, sample := range reference.Cases {
		// A byte image cannot represent the alpha cases' unclamped intermediate.
		if sample.SourceAlpha != 256 {
			continue
		}
		colors := color.Palette{color.NRGBA{}, color.NRGBA{sample.Source[0], sample.Source[1], sample.Source[2], 255}}
		pixels := image.NewPaletted(image.Rect(0, 0, 2, 1), colors)
		pixels.SetColorIndex(1, 0, 1)
		state := New(&sff.SFF{Sprites: []sff.Sprite{{Image: pixels, PaletteIndex: 0}}, Palettes: []sff.Palette{{GroupNumber: 1, ItemNumber: 1, Colors: colors}}})
		state.SetEffect(sample.Params)
		for age := 0; age < sample.Age; age++ {
			state.AdvanceHitEffect()
		}
		result, err := state.Image(0, true)
		if err != nil {
			t.Fatal(err)
		}
		got := color.NRGBAModel.Convert(result.At(1, 0)).(color.NRGBA)
		if [3]uint8{got.R, got.G, got.B} != sample.Expected || got.A != 255 {
			t.Errorf("sample %d %s got %v native %v", i, sample.Fixture, got, sample.Expected)
		}
		_, _, _, a := result.At(0, 0).RGBA()
		if a != 0 {
			t.Fatal("palette effect destroyed index-zero mask", i, a)
		}
	}
}
