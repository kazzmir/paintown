package main

import (
	"fmt"
	"math"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
)

type hudCombo struct {
	revision         uint64
	count, age, hold int
	x                float64
	active, exiting  bool
}

func (h *FightHUD) comboUnit() float64 {
	width := h.match.Options.MotifSpace.Width
	if width <= 0 {
		width = 320
	}
	return float64(width) / 320
}

func (h *FightHUD) updateCombos() {
	for slot, player := range h.match.Players {
		view := &h.combos[slot]
		prefix := fmt.Sprintf("team%d", slot+1)
		position := h.values("Combo", prefix+".pos", 0, 0)
		start := h.values("Combo", prefix+".start.x", position[0])[0]
		if player.Character.ComboRevision != view.revision {
			view.revision = player.Character.ComboRevision
			view.count = player.Character.ComboCount
			view.age, view.hold, view.exiting = 0, 0, false
			if !view.active {
				view.x = start
			}
			view.active = view.count >= 2
		} else {
			view.age++
		}
		if !view.active {
			continue
		}
		target := position[0]
		if view.exiting {
			target = start
		}
		// A small fixed step in 240p units keeps the authored start/target
		// positions meaningful at either stock motif resolution.
		step := 2 * h.comboUnit()
		if math.Abs(target-view.x) <= step {
			view.x = target
			if view.exiting {
				view.active = false
			} else {
				view.hold++
				duration := h.match.Setting("Combo", prefix+".displaytime", 90)
				if duration >= 0 && view.hold >= duration {
					view.exiting = true
				}
			}
		} else {
			view.x += math.Copysign(step, target-view.x)
		}
	}
}

func (h *FightHUD) drawCombos(screen *ebiten.Image) {
	for slot, view := range h.combos {
		if !view.active {
			continue
		}
		prefix := fmt.Sprintf("team%d", slot+1)
		position := h.values("Combo", prefix+".pos", 0, 0)
		x, y := view.x, position[1]
		if h.match.Setting("Combo", prefix+".counter.shake", 0) != 0 && view.age < 10 {
			x += float64(view.age%3-1) * h.comboUnit()
		}
		h.component(screen, "Combo", prefix+".counter", x, y)
		h.label(screen, "Combo", prefix+".counter", fmt.Sprint(view.count), x, y)
		text := strings.ReplaceAll(h.text("Combo", prefix+".text.text", "%i Hit!"), "%i", fmt.Sprint(view.count))
		textX := view.x
		counterFont := h.values("Combo", prefix+".counter.font", 0, 0, 0)
		fontID := int(counterFont[0])
		if fontID > 0 && fontID < len(h.match.Fight.Fonts) && h.match.Fight.Fonts[fontID] != nil {
			width := float64(h.match.Fight.Fonts[fontID].TextWidthForBank(fmt.Sprint(view.count), int(counterFont[1])))
			if counterFont[2] < 0 {
				textX -= width + 2*h.values("Combo", prefix+".text.offset", 0, 0)[0]
			} else {
				textX += width
			}
		}
		h.component(screen, "Combo", prefix+".text", textX, position[1])
		h.label(screen, "Combo", prefix+".text", text, textX, position[1])
	}
}
