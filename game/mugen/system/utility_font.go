package main

import (
	"image/color"
	"strings"
	"unicode"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/palette"
)

type utilityFace struct {
	font *font.Font
	bank int
	rgb  [3]float32
}

func (f utilityFace) height() int {
	if f.font == nil {
		return 16
	}
	return max(1, f.font.Height)
}

// Uppercase-only menu banks still render ordinary utility labels. Missing
// punctuation uses the readable fallback at the same height, without replacing
// the motif's lettering for the rest of the row.
func (f utilityFace) character(r rune) rune {
	if f.font != nil && f.font.Glyph(r, f.bank) == nil && f.font.Glyph(unicode.ToUpper(r), f.bank) != nil {
		return unicode.ToUpper(r)
	}
	return r
}

func (u *utilityMenu) glyphFace(f utilityFace, r rune) (utilityFace, rune, float64) {
	r = f.character(r)
	if f.font != nil && (r == ' ' || f.font.Glyph(r, f.bank) != nil) {
		return f, r, 1
	}
	fallback := utilityFace{font: u.font, rgb: f.rgb}
	return fallback, fallback.character(r), float64(f.height()) / float64(fallback.height())
}

func (u *utilityMenu) textWidth(f utilityFace, text string) int {
	normalized := strings.Map(f.character, text)
	complete := f.font != nil
	for _, r := range normalized {
		complete = complete && (r == ' ' || f.font != nil && f.font.Glyph(r, f.bank) != nil)
	}
	if complete {
		return f.font.TextWidthForBank(normalized, f.bank)
	}
	width := 0.
	for _, r := range text {
		face, r, scale := u.glyphFace(f, r)
		advance := 6
		if face.font != nil {
			advance = face.font.TextWidthForBank(string(r), face.bank)
		}
		width += float64(advance) * scale
	}
	return int(width + .5)
}

func (u *utilityMenu) drawText(f utilityFace, text string, x, y int) {
	normalized := strings.Map(f.character, text)
	complete := f.font != nil
	for _, r := range normalized {
		complete = complete && (r == ' ' || f.font != nil && f.font.Glyph(r, f.bank) != nil)
	}
	if complete {
		_ = f.font.DrawWithColor(u.canvas, normalized, x, y, f.bank, font.AlignLeft, f.rgb, palette.Transform{})
		return
	}
	position := float64(x)
	for _, r := range text {
		face, r, scale := u.glyphFace(f, r)
		advance := 6
		if face.font != nil {
			advance = face.font.TextWidthForBank(string(r), face.bank)
		}
		if scale == 1 && face.font != nil {
			_ = face.font.DrawWithColor(u.canvas, string(r), int(position), y, face.bank, font.AlignLeft, face.rgb, palette.Transform{})
		} else {
			width, height := max(1, advance+8), face.height()+8
			if u.glyph == nil || u.glyph.Bounds().Dx() < width || u.glyph.Bounds().Dy() < height {
				if u.glyph != nil {
					u.glyph.Deallocate()
				}
				u.glyph = ebiten.NewImage(width, height)
			}
			u.glyph.Fill(color.Transparent)
			if face.font != nil {
				_ = face.font.DrawWithColor(u.glyph, string(r), 4, face.height(), face.bank, font.AlignLeft, face.rgb, palette.Transform{})
			} else {
				ebitenutil.DebugPrintAt(u.glyph, string(r), 4, 0)
			}
			op := &ebiten.DrawImageOptions{}
			op.GeoM.Scale(scale, scale)
			op.GeoM.Translate(position-4*scale, float64(y)-float64(face.height())*scale)
			u.canvas.DrawImage(u.glyph, op)
		}
		position += float64(advance) * scale
	}
}

func (u *utilityMenu) wrapText(f utilityFace, text string, width int) []string {
	var lines []string
	runes := []rune(text)
	for len(runes) > 0 {
		count, space := 1, 0
		for count < len(runes) && u.textWidth(f, string(runes[:count+1])) <= width {
			if runes[count] == ' ' {
				space = count
			}
			count++
		}
		if count < len(runes) && space > 0 {
			count = space
		}
		lines = append(lines, strings.TrimSpace(string(runes[:count])))
		runes = []rune(strings.TrimLeftFunc(string(runes[count:]), unicode.IsSpace))
	}
	if len(lines) == 0 {
		return []string{""}
	}
	return lines
}
