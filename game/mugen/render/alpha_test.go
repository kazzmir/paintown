package render

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"image"
	"image/color"
	"os"
	"path/filepath"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
)

// Run the test runner inside the public game loop so GPU readback is available,
// following Ebitengine's own image-test arrangement.
type shaderTestGame struct {
	tests *testing.M
	code  int
}

func (g *shaderTestGame) Update() error {
	g.code = g.tests.Run()
	return ebiten.Termination
}
func (*shaderTestGame) Draw(*ebiten.Image)         {}
func (*shaderTestGame) Layout(int, int) (int, int) { return 32, 32 }

func TestMain(m *testing.M) {
	g := &shaderTestGame{tests: m, code: 1}
	ebiten.SetWindowSize(32, 32)
	ebiten.SetWindowTitle("MUGEN sampler tests")
	if err := ebiten.RunGame(g); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	os.Exit(g.code)
}

func TestNativeAlphaNormalization(t *testing.T) {
	var reference struct {
		Inputs map[string]string `json:"native_input_sha256"`
		Cases  []struct {
			Source, Destination, Expected [3]uint8
			SourceAlpha                   int `json:"source_alpha"`
			DestinationAlpha              int `json:"destination_alpha"`
		}
	}
	root := "../tests/testdata/reference"
	raw, err := os.ReadFile(filepath.Join(root, "indexed-alpha-colors.json"))
	if err != nil {
		t.Fatal(err)
	}
	if err := json.Unmarshal(raw, &reference); err != nil {
		t.Fatal(err)
	}
	if len(reference.Cases) != 270 || len(reference.Inputs) != 15 {
		t.Fatal("incomplete frozen native color corpus", len(reference.Cases), len(reference.Inputs))
	}
	for name, want := range reference.Inputs {
		raw, err := os.ReadFile(filepath.Join(root, name))
		if err != nil {
			t.Fatal(err)
		}
		h := sha256.Sum256(raw)
		if hex.EncodeToString(h[:]) != want {
			t.Fatal("native color input changed", name)
		}
	}
	var nearest Nearest
	var linear Linear
	for _, sampler := range []struct {
		name string
		draw func(*ebiten.Image, *ebiten.Image, *ebiten.DrawImageOptions, ...float32) error
	}{{"indexed", nearest.DrawImage}, {"rgb", linear.DrawImage}} {
		t.Run(sampler.name, func(t *testing.T) {
			for i, sample := range reference.Cases {
				pixels := image.NewRGBA(image.Rect(0, 0, 1, 1))
				pixels.SetRGBA(0, 0, color.RGBA{sample.Source[0], sample.Source[1], sample.Source[2], 255})
				src := ebiten.NewImageFromImage(pixels)
				dst := ebiten.NewImage(1, 1)
				dst.Fill(color.RGBA{sample.Destination[0], sample.Destination[1], sample.Destination[2], 255})
				op := &ebiten.DrawImageOptions{}
				destination := ApplyAlpha(op, sample.SourceAlpha, sample.DestinationAlpha)
				if err := sampler.draw(dst, src, op, destination); err != nil {
					t.Fatal(err)
				}
				var got [4]byte
				dst.ReadPixels(got[:])
				src.Deallocate()
				dst.Deallocate()
				if [3]uint8{got[0], got[1], got[2]} != sample.Expected || got[3] != 255 {
					t.Errorf("sample %d source=%v AS%d D%d: got %v, native %v", i, sample.Source, sample.SourceAlpha, sample.DestinationAlpha, got, sample.Expected)
				}
			}
		})
	}
}
