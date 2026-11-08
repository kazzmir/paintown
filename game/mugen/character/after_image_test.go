package character

import (
	"image/color"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

func TestAfterImageRecordingExpiresAndResumesWithoutClearingHistory(t *testing.T) {
	archive := &sff.SFF{Palettes: []sff.Palette{{GroupNumber: 1, ItemNumber: 1, Colors: color.Palette{color.Black, color.White}}, {GroupNumber: 1, ItemNumber: 2, Colors: color.Palette{color.Black, color.White}}}}
	c := &Character{Palettes: palette.New(archive)}
	p := engine.DefaultAfterImageParameters()
	p.Time, p.Length, p.FrameGap = 3, 60, 1
	if err := c.SetAfterImage(p); err != nil {
		t.Fatal(err)
	}
	for frame := 0; frame < 4; frame++ {
		c.X = float64(frame)
		c.DrawOffset = []float64{float64(frame), 3}
		c.Palettes.SetEffect(palette.Effect{Time: 10, Color: 256, Mul: [3]int{128, 128, 128}})
		c.advanceAfterImages()
	}
	if len(c.afterImages.frames) != 4 || !c.afterImages.frames[0].visible || !c.afterImages.frames[1].visible || c.afterImages.frames[2].visible || c.afterImages.frames[3].visible {
		t.Fatal(c.afterImages)
	}
	first := c.afterImages.frames[0].palettes
	if first.Transform().Enabled || first != c.afterImages.frames[1].palettes || c.afterImages.frames[0].offset != ([2]float64{0, 3}) {
		t.Fatal("recording inherited local PalFX, duplicated mappings or changed saved offsets")
	}
	if err := c.SetAfterImageTime(2); err != nil {
		t.Fatal(err)
	}
	c.X = 4
	c.advanceAfterImages()
	c.X = 5
	c.advanceAfterImages()
	if len(c.afterImages.frames) != 6 || !c.afterImages.frames[4].visible || c.afterImages.frames[5].visible || !c.afterImages.frames[0].visible {
		t.Fatal(c.afterImages)
	}
	c.worldFrozen = true
	c.advanceAfterImages()
	if len(c.afterImages.frames) != 6 {
		t.Fatal("paused recording advanced")
	}
	c.worldFrozen = false
	c.SetAfterImageTime(-1)
	if err := c.Palettes.Remap(palette.ID{1, 1}, palette.ID{1, 2}); err != nil {
		t.Fatal(err)
	}
	c.advanceAfterImages()
	last := c.afterImages.frames[len(c.afterImages.frames)-1].palettes
	if last == first || last.Destination(palette.ID{1, 1}) != (palette.ID{1, 2}) || first.Destination(palette.ID{1, 1}) != (palette.ID{1, 1}) || last.Transform().Enabled {
		t.Fatal("remapped recording changed historical outfit or inherited local PalFX")
	}
}
