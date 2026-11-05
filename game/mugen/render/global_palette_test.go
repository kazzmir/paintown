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

func TestNativeGlobalPaletteColors(t *testing.T) {
	runNativeGlobalPaletteColors(t, "global-native-colors.json", 1155, 33, 67)
}

func TestNativeGlobalPhotoColors(t *testing.T) {
	runNativeGlobalPaletteColors(t, "global-photo-colors.json", 3234, 0, 69)
}

func runNativeGlobalPaletteColors(t *testing.T, artifact string, cases, clears, inputs int) {
	t.Helper()
	var reference struct {
		Profiles int
		Inputs   map[string]string `json:"native_input_sha256"`
		Cases    []struct {
			Fixture, Kind    string
			Params           struct{ All, BG, Local palette.Effect }
			SourceAlpha      int `json:"source_alpha"`
			Source, Expected [3]uint8
		}
		ClearCases []struct {
			Fixture  string
			Params   palette.Effect
			Expected [3]uint8
		} `json:"clear_cases"`
	}
	root := "../tests/testdata/reference"
	raw, err := os.ReadFile(filepath.Join(root, artifact))
	if err != nil {
		t.Fatal(err)
	}
	if err := json.Unmarshal(raw, &reference); err != nil {
		t.Fatal(err)
	}
	if reference.Profiles != 33 || len(reference.Cases) != cases || len(reference.ClearCases) != clears || len(reference.Inputs) != inputs {
		t.Fatal("incomplete native global color corpus", reference.Profiles, len(reference.Cases), len(reference.ClearCases), len(reference.Inputs))
	}
	for name, want := range reference.Inputs {
		raw, err := os.ReadFile(filepath.Join(root, name))
		if err != nil {
			t.Fatal(err)
		}
		h := sha256.Sum256(raw)
		if hex.EncodeToString(h[:]) != want {
			t.Fatal("native input changed", name)
		}
	}
	transform := func(effect palette.Effect) palette.Transform {
		state := palette.New(nil)
		state.SetEffect(effect)
		return state.Transform()
	}
	pixel := func(rgb [3]uint8) *ebiten.Image {
		pixels := image.NewRGBA(image.Rect(0, 0, 1, 1))
		pixels.SetRGBA(0, 0, color.RGBA{rgb[0], rgb[1], rgb[2], 255})
		return ebiten.NewImageFromImage(pixels)
	}
	var nearest Nearest
	var linear Linear
	for _, sampler := range []struct {
		name string
		draw func(*ebiten.Image, *ebiten.Image, *ebiten.DrawImageOptions, DrawStyle) error
	}{{"indexed", nearest.DrawStyledImage}, {"rgb", linear.DrawStyledImage}} {
		t.Run(sampler.name, func(t *testing.T) {
			for i, sample := range reference.Cases {
				all := transform(sample.Params.All)
				first := all
				switch sample.Kind {
				case "actor":
					first = palette.Combine(transform(sample.Params.Local), all)
				case "stage":
					first = palette.Combine(transform(sample.Params.BG), all)
				case "hud":
				default:
					t.Fatal(sample.Kind)
				}
				src, stage, dst := pixel(sample.Source), ebiten.NewImage(1, 1), ebiten.NewImage(1, 1)
				stage.Fill(color.RGBA{255, 0, 255, 255})
				op := &ebiten.DrawImageOptions{}
				style := DrawStyle{DestinationFactor: ApplyAlpha(op, sample.SourceAlpha, 0), Palette: first}
				if err := sampler.draw(stage, src, op, style); err != nil {
					t.Fatal(err)
				}
				if err := sampler.draw(dst, stage, &ebiten.DrawImageOptions{}, DrawStyle{DestinationFactor: -1, Palette: all}); err != nil {
					t.Fatal(err)
				}
				var got [4]byte
				dst.ReadPixels(got[:])
				src.Deallocate()
				stage.Deallocate()
				dst.Deallocate()
				if [3]uint8{got[0], got[1], got[2]} != sample.Expected {
					t.Errorf("sample %d %s/%s source=%v got=%v native=%v", i, sample.Fixture, sample.Kind, sample.Source, got, sample.Expected)
				}
			}
			for _, sample := range reference.ClearCases {
				all := transform(sample.Params)
				c := color.NRGBAModel.Convert(all.ClearColor(color.NRGBA{255, 0, 255, 255})).(color.NRGBA)
				src, dst := pixel([3]uint8{c.R, c.G, c.B}), ebiten.NewImage(1, 1)
				if err := sampler.draw(dst, src, &ebiten.DrawImageOptions{}, DrawStyle{DestinationFactor: -1, Palette: all}); err != nil {
					t.Fatal(err)
				}
				var got [4]byte
				dst.ReadPixels(got[:])
				src.Deallocate()
				dst.Deallocate()
				if [3]uint8{got[0], got[1], got[2]} != sample.Expected {
					t.Errorf("clear %s first=%v got=%v native=%v", sample.Fixture, c, got, sample.Expected)
				}
			}
		})
	}
}
