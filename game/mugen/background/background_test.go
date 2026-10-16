package background

import (
	"image"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

type drawnElement struct {
	Element
	layer int
	seen  map[string]*SpriteImages
	calls int
}

func TestBackgroundDefaultWindowUsesLocalSpace(t *testing.T) {
	ast, err := parsers.Parse(strings.NewReader("[TitleBG Image]\ntype=normal\nspriteno=0,0\n"))
	if err != nil {
		t.Fatal(err)
	}
	bg := &Background{LocalCoord: coord.Space{Width: 1280, Height: 720}}
	bg.LoadFromAST(ast, "titlebg", nil, nil)
	if len(bg.Elements) != 1 {
		t.Fatal("background element was not built")
	}
	image, ok := bg.Elements[0].(*NormalElement)
	if !ok || image.Window != [4]int{0, 0, 1279, 719} {
		t.Fatalf("high-resolution background retained a 320x240 clip: %+v", image)
	}
}

func (e *drawnElement) GetLayerNo() int { return e.layer }
func (e *drawnElement) Draw(_ *ebiten.Image, _, _, _ float64, sprites map[string]*SpriteImages) {
	e.seen, e.calls = sprites, e.calls+1
}

func TestBackgroundArchiveOwnershipAndLayers(t *testing.T) {
	sharedImage := ebiten.NewImage(1, 1)
	defer sharedImage.Dispose()
	shared := map[string]*SpriteImages{"0-0": {Masked: sharedImage, Opaque: sharedImage}}
	back, front := &drawnElement{layer: 0}, &drawnElement{layer: 1}
	archive := &sff.SFF{Sprites: []sff.Sprite{{Image: image.NewRGBA(image.Rect(0, 0, 3, 1)), ImageOpaque: image.NewRGBA(image.Rect(0, 0, 3, 1))}}}
	b := &Background{SpriteData: archive, Elements: []Element{back, front}}
	b.DrawLayer(nil, 0, 0, 0, 0, shared)
	if back.calls != 1 || front.calls != 0 || back.seen["0-0"] == shared["0-0"] || back.seen["0-0"].Masked.Bounds().Dx() != 3 {
		t.Fatal("background did not select its archive for the requested layer")
	}
	owned := back.seen["0-0"]
	b.DrawLayer(nil, 1, 0, 0, 0, shared)
	b.DrawLayer(nil, 0, 0, 0, 0, shared)
	if front.calls != 1 || back.calls != 2 || back.seen["0-0"] != owned || front.seen["0-0"] != owned || owned.Opaque == owned.Masked {
		t.Fatal("background phases did not share their own masked/opaque cache")
	}
	owned.Masked.Dispose()
	owned.Opaque.Dispose()
	plain := &Background{Elements: []Element{back}}
	plain.DrawLayer(nil, 0, 0, 0, 0, shared)
	if back.seen["0-0"] != shared["0-0"] {
		t.Fatal("background without an override lost shared sprites")
	}
}
