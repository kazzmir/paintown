package background

import (
	"image"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

type drawnElement struct {
	Element
	layer int
	seen  map[string]*SpriteImages
	calls int
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
