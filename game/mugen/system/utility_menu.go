package main

import (
	"fmt"
	"image"
	"image/color"
	"math"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
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

// Utility pages inherit the title menu's fonts, banks, colors and fitted size.
// Long pages scroll; an absent menu font retains the readable utility fallback.
type utilityMenu struct {
	canvas                                   *ebiten.Image
	glyph                                    *ebiten.Image
	font                                     *font.Font
	loaded                                   bool
	footer                                   []utilityRow
	scaleHint, scale                         float64
	first, last, previousCursor, pageRequest int
	initialized, manual, themed              bool
}

func (u *utilityMenu) updateScroll(im *input.InputManager) {
	if im.KeyJustPressed(ebiten.KeyPageUp) {
		u.pageRequest--
	}
	if im.KeyJustPressed(ebiten.KeyPageDown) {
		u.pageRequest++
	}
}

func (u *utilityMenu) faces(e *Engine) (utilityFace, utilityFace) {
	white := [3]float32{256, 256, 256}
	normal := utilityFace{font: u.font, rgb: white}
	u.themed = false
	if info := e.motif.TitleInfo.ItemFont; info.Index >= 1 && info.Index < len(e.motif.Fonts) && e.motif.Fonts[info.Index] != nil {
		f := e.motif.Fonts[info.Index]
		if f.Glyph('A', info.Bank) != nil {
			rgb := e.themeValues("Title Info", "menu.item.font", 0, 0, 0, 256, 256, 256)
			normal = utilityFace{font: f, bank: info.Bank, rgb: [3]float32{float32(rgb[3]), float32(rgb[4]), float32(rgb[5])}}
			u.themed = true
		}
	}
	active := normal
	if info := e.motif.TitleInfo.ItemActiveFont; u.themed && info.Index >= 1 && info.Index < len(e.motif.Fonts) && e.motif.Fonts[info.Index] != nil {
		rgb := e.themeValues("Title Info", "menu.item.active.font", 0, 0, 0, 256, 256, 256)
		active = utilityFace{font: e.motif.Fonts[info.Index], bank: info.Bank, rgb: [3]float32{float32(rgb[3]), float32(rgb[4]), float32(rgb[5])}}
	}
	return normal, active
}

type utilityLine struct {
	label, value string
	row          int
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
	normal, active := u.faces(e)
	u.scale = 1
	step := max(10, normal.height()+2, active.height()+2)
	if u.themed {
		space := e.motif.Info.LocalCoord
		if e.logicalWidth > 0 && e.logicalHeight > 0 {
			space = coord.Space{Width: e.logicalWidth, Height: e.logicalHeight}
		}
		if space.Width <= 0 || space.Height <= 0 {
			space = coord.Default()
		}
		u.scale, _, _, _ = coord.Fit(space, coord.Space{Width: screen.Bounds().Dx(), Height: screen.Bounds().Dy()})
		if u.scaleHint > 0 {
			u.scale = u.scaleHint
		}
		step = max(step, int(e.motif.TitleInfo.MenuSpacing[1]))
	} else if title != "" {
		step = max(20, step)
	}
	measure := func(text string) int { return max(u.textWidth(normal, text), u.textWidth(active, text)) }
	available := max(1, int(float64(screen.Bounds().Dx()-8)/u.scale)-24)
	labelWidth, valueWidth := 0, 0
	for _, row := range append(append([]utilityRow(nil), rows...), u.footer...) {
		labelWidth = max(labelWidth, measure(row.label))
		valueWidth = max(valueWidth, measure(row.value))
	}
	gap := 24
	if labelWidth+valueWidth+gap > available {
		labelWidth = min(labelWidth, available*3/5)
		valueWidth = max(1, available-labelWidth-gap)
	}
	width := min(available+24, max(190, labelWidth+valueWidth+gap+24, measure(title)+24))
	flatten := func(source []utilityRow) []utilityLine {
		var lines []utilityLine
		for index, row := range source {
			face := normal
			if index == cursor {
				face = active
			}
			labels, values := u.wrapText(face, row.label, max(1, labelWidth)), u.wrapText(face, row.value, max(1, valueWidth))
			for n := 0; n < max(len(labels), len(values)); n++ {
				line := utilityLine{row: index}
				if n < len(labels) {
					line.label = labels[n]
				}
				if n < len(values) {
					line.value = values[n]
				}
				lines = append(lines, line)
			}
		}
		return lines
	}
	lines, footer := flatten(rows), flatten(u.footer)
	width = min(available+24, max(width, measure(fmt.Sprintf("%d-%d OF %d  PGUP PGDN", len(lines), len(lines), len(lines)))+24))
	header := 0
	if title != "" {
		header = step
	}
	space := max(1, int(float64(screen.Bounds().Dy()-max(4, top)-4)/u.scale)-8-header)
	capacity := max(1, space/step-len(footer))
	overflow := len(lines) > capacity
	if overflow {
		capacity = max(1, capacity-1)
	}
	if !u.initialized || cursor != u.previousCursor {
		u.manual = false
	}
	if u.pageRequest != 0 {
		u.first += u.pageRequest * max(1, capacity-1)
		u.manual, u.pageRequest = true, 0
	}
	if cursor >= 0 && !u.manual {
		start, end := -1, -1
		for i, line := range lines {
			if line.row == cursor {
				if start < 0 {
					start = i
				}
				end = i
			}
		}
		if start >= 0 && start < u.first {
			u.first = start
		}
		if end >= u.first+capacity {
			u.first = max(start, end-capacity+1)
		}
	}
	u.first = max(0, min(u.first, len(lines)-capacity))
	u.last = min(len(lines), u.first+capacity)
	u.previousCursor, u.initialized = cursor, true
	count := u.last - u.first + len(footer)
	if overflow {
		count++
	}
	height := 8 + header + count*step
	if u.canvas == nil || u.canvas.Bounds().Size() != image.Pt(width, height) {
		if u.canvas != nil {
			u.canvas.Deallocate()
		}
		u.canvas = ebiten.NewImage(width, height)
	}
	u.canvas.Fill(color.RGBA{R: 32, G: 48, B: 80, A: 192})
	if title != "" {
		u.drawText(normal, title, (width-u.textWidth(normal, title))/2, normal.height()+4)
	}
	y := header + 4
	drawLine := func(line utilityLine, selected bool) {
		face := normal
		if selected {
			face = active
			u.canvas.SubImage(image.Rect(4, y-2, width-4, y+step-2)).(*ebiten.Image).Fill(color.RGBA{R: 112, G: 152, B: 200, A: 192})
		}
		u.drawText(face, line.label, 12, y+face.height())
		u.drawText(face, line.value, width-12-u.textWidth(face, line.value), y+face.height())
		y += step
	}
	for _, line := range lines[u.first:u.last] {
		drawLine(line, line.row == cursor)
	}
	if overflow {
		text := fmt.Sprintf("%d-%d OF %d  PGUP PGDN", u.first+1, u.last, len(lines))
		u.drawText(normal, text, 12, y+normal.height())
		y += step
	}
	for _, line := range footer {
		drawLine(line, false)
	}
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Scale(u.scale, u.scale)
	op.GeoM.Translate(float64(screen.Bounds().Min.X)+(float64(screen.Bounds().Dx())-float64(width)*u.scale)/2,
		float64(screen.Bounds().Min.Y)+math.Max(0, math.Min(float64(top), float64(screen.Bounds().Dy())-float64(height)*u.scale-4)))
	screen.DrawImage(u.canvas, op)
}
