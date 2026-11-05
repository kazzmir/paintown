package character

import (
	"math"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/palette"
)

type afterImageFrame struct {
	x, y         float64
	facing       int
	anim, elem   int
	frames       *air.Data
	constants    *cns.CNS
	angleEnabled bool
	angle        float64
	scale        [2]float32
	offset       []float64
	visible      bool
}

type afterImageHistory struct {
	parameters engine.AfterImageParameters
	time       int
	gapClock   int
	frames     []afterImageFrame // oldest first, including disabled recording slots
}

func (c *Character) SetAfterImage(p engine.AfterImageParameters) error {
	if p.Length <= 0 {
		p.Length = 20
	}
	p.Length = min(60, p.Length)
	if p.TimeGap <= 0 {
		p.TimeGap = 1
	}
	if p.FrameGap <= 0 {
		p.FrameGap = 4
	}
	c.afterImages = &afterImageHistory{parameters: p, time: p.Time}
	return nil
}

func (c *Character) SetAfterImageTime(duration int) error {
	if c.afterImages != nil {
		// Expired recording may resume while the existing history is retained.
		c.afterImages.time = duration
	}
	return nil
}

func (c *Character) advanceAfterImages() {
	h := c.afterImages
	if h == nil || c.worldFrozen || c.pausedThisTick {
		return
	}
	if h.time > 0 {
		h.time--
	}
	record := h.gapClock == 0
	h.gapClock = (h.gapClock + 1) % h.parameters.TimeGap
	if !record {
		return
	}
	capacity := h.parameters.Length - 1
	if capacity <= 0 {
		h.frames = nil
		return
	}
	f := afterImageFrame{x: c.X, y: c.Y, facing: c.Facing, anim: c.GetAnim(), elem: c.AnimElem,
		frames: c.AnimationFile(), constants: c.StateFile, angleEnabled: c.AngleDrawingEnabled,
		angle: c.DrawAngle, scale: c.DrawScale, offset: append([]float64(nil), c.DrawOffset...),
		visible: h.time > 0 || h.time == -1}
	h.frames = append(h.frames, f)
	if len(h.frames) > capacity {
		copy(h.frames, h.frames[len(h.frames)-capacity:])
		h.frames = h.frames[:capacity]
	}
}

func (h *afterImageHistory) transform(age int) palette.Transform {
	p := h.parameters
	t := palette.Transform{Enabled: true, Color: float32(max(0, min(256, p.Color))) / 256, Invert: p.Invert}
	for i := range t.Add {
		t.Add[i] = float32(p.Bright[i]+age*p.Add[i]) / 255
		// Native palette coefficients are quantized before sprite drawing.
		aged := math.Trunc(math.Pow(float64(p.Mul[i]), float64(age)) * 256)
		coefficient := math.Trunc(float64(p.Contrast[i]) * aged / 256)
		t.Mul[i] = float32(coefficient / 256)
	}
	return t
}

func (p *Player) drawAfterImages(screen *ebiten.Image, c *Character, x, y, sx, sy float64) {
	h := c.afterImages
	if h == nil {
		return
	}
	gap := h.parameters.FrameGap
	// Select newest first so disabled slots do not age the palette. Enabled
	// records still count when their saved pose is clipped offscreen.
	var selected [60]int // SetAfterImage bounds history to length-1 <= 59.
	count := 0
	for age := gap - 1; age < len(h.frames); age += gap {
		index := len(h.frames) - 1 - age
		if h.frames[index].visible {
			selected[count] = index
			count++
		}
	}
	// Older selected records draw first; palette age follows enabled selection.
	for paletteAge := count - 1; paletteAge >= 0; paletteAge-- {
		f := h.frames[selected[paletteAge]]

		snapshot := *c
		snapshot.X, snapshot.Y, snapshot.Facing = f.x, f.y, f.facing
		snapshot.Anim, snapshot.AnimElem, snapshot.animationFile = f.anim, f.elem, f.frames
		snapshot.StateFile = f.constants
		snapshot.AngleDrawingEnabled, snapshot.DrawAngle, snapshot.DrawScale = f.angleEnabled, f.angle, f.scale
		snapshot.DrawOffset = f.offset
		snapshot.DrawTransparency = engine.Transparency{Enabled: true, Mode: "none"}
		switch h.parameters.Trans {
		case "add":
			snapshot.DrawTransparency.Mode, snapshot.DrawTransparency.Source, snapshot.DrawTransparency.Destination = "alpha", 256, 256
		case "add1":
			snapshot.DrawTransparency.Mode, snapshot.DrawTransparency.Source, snapshot.DrawTransparency.Destination = "alpha", 256, 128
		case "sub":
			snapshot.DrawTransparency.Mode = "sub"
		}
		transform := h.transform(paletteAge)
		p.drawEntityWithPalette(screen, &snapshot, x+(f.x-c.X)*sx, y+(f.y-c.Y)*sy, sx, sy, &transform)
		if snapshot.RuntimeError != nil {
			c.RuntimeError = snapshot.RuntimeError
		}
	}
}
