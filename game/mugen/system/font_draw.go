package main

import (
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/palette"
)

// Typed motif fields retain index/bank/alignment. Read optional color channels
// from the same first-authored font tuple instead of dropping its tail.
func (e *Engine) drawMotifFont(screen *ebiten.Image, f *font.Font, text string, x, y, bank int, align font.Alignment, section, key string) {
	values := e.themeValues(section, key, 0, 0, 0, 256, 256, 256)
	_ = f.DrawWithColor(screen, text, x, y, bank, align, [3]float32{float32(values[3]), float32(values[4]), float32(values[5])}, palette.Transform{})
}
