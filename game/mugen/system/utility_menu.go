package main

import (
	"image"
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/resource"
)

type utilityRow struct{ label, value string }

type menuRepeat struct{ held [4]int }

// Native utility navigation repeats after 320ms, then every 50ms. Keep this
// local to menus so held combat buttons retain their ordinary CMD edges.
func (r *menuRepeat) directions(im *input.InputManager, tps int) (pressed [4]bool) {
	delay, interval := max(1, tps*320/1000), max(1, tps*50/1000)
	for i, action := range []input.Action{input.ActionUp, input.ActionDown, input.ActionLeftUI, input.ActionRightUI} {
		if !im.IsPressed(1, action) {
			r.held[i] = 0
			continue
		}
		r.held[i]++
		pressed[i] = r.held[i] == 1 || r.held[i] > delay && (r.held[i]-delay-1)%interval == 0
	}
	return
}

// Utility menus use their own small lettering, independently of title-menu art
// and uppercase-only banks. A built-in fallback keeps incomplete motifs usable.
type utilityMenu struct {
	canvas *ebiten.Image
	font   *font.Font
	loaded bool
}

func (u *utilityMenu) draw(e *Engine, screen *ebiten.Image, rows []utilityRow, cursor int, title string, top int) {
	if len(rows) == 0 {
		return
	}
	if !u.loaded {
		u.loaded = true
		if path, err := resource.Resolve("font/f-6x9.def", e.dataDir); err == nil {
			u.font, _ = font.LoadFont(path, e.cfg.Config.Language)
		}
	}
	line, width := 16, 190
	if title != "" {
		width = 180
	}
	measure := func(s string) int { return len(s) * 6 }
	if u.font != nil {
		line = max(10, u.font.Height+u.font.SpacingY+1)
		measure = func(s string) int { return u.font.TextWidthForBank(s, 0) }
	}
	for _, row := range rows {
		width = max(width, measure(row.label)+measure(row.value)+24)
	}
	header, step, padding, count := 0, line, 0, len(rows)
	if title != "" {
		header, step, count = 20, 20, max(5, count)
	}
	height := 8 + header + count*step
	if u.canvas == nil || u.canvas.Bounds().Size() != image.Pt(width, height) {
		if u.canvas != nil {
			u.canvas.Deallocate()
		}
		u.canvas = ebiten.NewImage(width, height)
	}
	u.canvas.Fill(color.RGBA{R: 32, G: 48, B: 80, A: 192})
	draw := func(text string, x, y int, rgb [3]float32) {
		if u.font == nil {
			ebitenutil.DebugPrintAt(u.canvas, text, x, y-line+2)
		} else {
			_ = u.font.DrawWithColor(u.canvas, text, x, y, 0, font.AlignLeft, rgb, palette.Transform{})
		}
	}
	white := [3]float32{256, 256, 256}
	if title != "" {
		draw(title, (width-measure(title))/2, line+4, white)
	}
	for i, row := range rows {
		y := padding + header + i*step
		if i == cursor {
			u.canvas.SubImage(image.Rect(4, y-1, width-4, y+line-1)).(*ebiten.Image).Fill(color.RGBA{R: 112, G: 152, B: 200, A: 192})
		}
		draw(row.label, 12, y+line-2, white)
		rgb := white
		if title != "" {
			rgb = [3]float32{256, 240, 80}
		}
		draw(row.value, width-12-measure(row.value), y+line-2, rgb)
	}
	scale := min(1., float64(screen.Bounds().Dx()-8)/float64(width), float64(screen.Bounds().Dy()-8)/float64(height))
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Scale(scale, scale)
	op.GeoM.Translate(float64(screen.Bounds().Min.X)+(float64(screen.Bounds().Dx())-float64(width)*scale)/2,
		float64(screen.Bounds().Min.Y)+min(float64(top), float64(screen.Bounds().Dy())-float64(height)*scale-4))
	screen.DrawImage(u.canvas, op)
}
