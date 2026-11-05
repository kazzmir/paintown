package character

import (
	"testing"

	"github.com/kazzmir/paintown/game/mugen/engine"
)

func TestAfterImageRecordingExpiresAndResumesWithoutClearingHistory(t *testing.T) {
	c := &Character{}
	p := engine.DefaultAfterImageParameters()
	p.Time, p.Length, p.FrameGap = 3, 60, 1
	if err := c.SetAfterImage(p); err != nil {
		t.Fatal(err)
	}
	for frame := 0; frame < 4; frame++ {
		c.X = float64(frame)
		c.advanceAfterImages()
	}
	if len(c.afterImages.frames) != 4 || !c.afterImages.frames[0].visible || !c.afterImages.frames[1].visible || c.afterImages.frames[2].visible || c.afterImages.frames[3].visible {
		t.Fatal(c.afterImages)
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
}
