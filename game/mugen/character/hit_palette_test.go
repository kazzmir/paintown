package character

import (
	"image"
	"image/color"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

func TestSharedHitPaletteClocksOncePerWorldFrame(t *testing.T) {
	states, err := cns.Parse(strings.NewReader("[Data]\nlife=1000\n[Statedef 0]\ntype=S\nmovetype=I\nphysics=N\nctrl=0\n[State 0, idle]\ntype=Null\ntrigger1=0\n"))
	if err != nil {
		t.Fatal(err)
	}
	colors := color.Palette{color.NRGBA{}, color.NRGBA{247, 239, 231, 255}}
	sprite := image.NewPaletted(image.Rect(0, 0, 2, 1), colors)
	sprite.SetColorIndex(1, 0, 1)
	archive := &sff.SFF{Sprites: []sff.Sprite{{Image: sprite, PaletteIndex: 0}}, Palettes: []sff.Palette{{GroupNumber: 1, ItemNumber: 1, Colors: colors}}}
	shared := palette.New(archive)
	shared.SetHitEffect(2, [3]int{}, [3]int{})
	players := []*Character{NewCharacter(states, nil, nil), NewCharacter(states, nil, nil)}
	for _, player := range players {
		player.Palettes = shared
	}
	for frame := 0; frame < 2; frame++ {
		if err := TickCharacters(players); err != nil {
			t.Fatal(err)
		}
		pixels, err := shared.Image(0, true)
		if err != nil {
			t.Fatal(err)
		}
		r, _, _, a := pixels.At(1, 0).RGBA()
		if a != 65535 || (frame == 0 && r != 0) || (frame == 1 && r != 247*257) {
			t.Fatal("shared palette clock advanced per actor or failed to expire", frame, r, a)
		}
	}
}
