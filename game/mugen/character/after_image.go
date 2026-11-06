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
	palettes     *palette.State
}

type afterImageHistory struct {
	parameters      engine.AfterImageParameters
	time            int
	gapClock        int
	frames          []afterImageFrame // oldest first, including disabled recording slots
	paletteSource   *palette.State
	paletteRevision uint64
	paletteSnapshot *palette.State
}

func (c *Character) SetAfterImage(p engine.AfterImageParameters) error {
	allowed, err := c.CanSetAfterImage()
	if err != nil || !allowed {
		return err
	}
	if c.World.afterImageOwners == nil {
		c.World.afterImageOwners = map[*Character]struct{}{}
	}
	c.World.afterImageOwners[c] = struct{}{}
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

// Actor slots last until destruction or round reset. Projectiles additionally
// retire a finite recording context after its configured drain interval.
// Admission precedes expression evaluation, so denied controllers have no
// parameter side effects. Reissuing one's own context needs no additional slot.
func (c *Character) CanSetAfterImage() (bool, error) {
	if err := c.ensureWorld(); err != nil {
		return false, err
	}
	_, owned := c.World.afterImageOwners[c]
	return !c.destroyed && (owned || len(c.World.afterImageOwners) < c.World.AfterImageMax), nil
}

func (c *Character) HasAfterImage() bool { return c.afterImages != nil }

func (c *Character) SetAfterImageTime(duration int) error {
	if c.afterImages != nil {
		// Expired recording may resume while the existing history is retained.
		c.afterImages.time = duration
	}
	return nil
}

func (c *Character) advanceAfterImages() {
	h := c.afterImages
	if h == nil || c.worldFrozen || c.pausedThisTick && !c.isProjectile {
		return
	}
	// Duration observes the precontact pause state. A new hit's own pass still
	// consumes duration; subsequent frozen passes retain it.
	durationPaused := c.isProjectile && c.projectile != nil && c.projectile.historyPaused
	if h.time > 0 && !durationPaused {
		h.time--
	}
	// Projectile hitpause forces a pose regardless of the advancing sample phase.
	// Its allocation drain clock freezes separately from both recording clocks.
	record := h.gapClock == 0 || c.isProjectile && c.pausedThisTick
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
	if f.visible && c.Palettes != nil {
		// Each pose retains the mappings used when recorded. Reuse immutable
		// snapshots until the source changes; local PalFX is replaced at draw.
		if h.paletteSource != c.Palettes || h.paletteSnapshot == nil || h.paletteRevision != c.Palettes.Revision {
			h.paletteSource, h.paletteRevision = c.Palettes, c.Palettes.Revision
			h.paletteSnapshot = c.Palettes.Clone()
		}
		f.palettes = h.paletteSnapshot
	}
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
		f := h.frames[index]
		// Blank AIR elements consume recording slots, but do not age the
		// selected palette sequence of older drawable records.
		if f.visible && f.frames != nil {
			action := f.frames.Actions[f.anim]
			if action == nil || f.elem < 0 || f.elem >= len(action.Elements) || action.Elements[f.elem].Group < 0 {
				continue
			}
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
		snapshot.Palettes = f.palettes
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
