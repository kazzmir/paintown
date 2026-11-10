package main

import (
	"fmt"
	"image/color"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
)

type optionItem struct {
	label      string
	shortLabel string
	getValue   func() int
	setValue   func(int)
	action     func() State
	// Display strings for each value, indexed by (value - min)
	// If nil, just shows the integer.
	choices   []string
	min, max  int
	increment int // amount to change per press (default 1)
}

func (o *optionItem) displayValue() string {
	if o.getValue == nil {
		return ""
	}
	v := o.getValue()
	if o.choices != nil {
		idx := v - o.min
		if idx >= 0 && idx < len(o.choices) {
			return o.choices[idx]
		}
	}
	// Special case for time limit
	if o.label == "Time Limit" && v == -1 {
		return "Unlimited"
	}
	// Special case for life
	if o.label == "Life" {
		return fmt.Sprintf("%d%%", v)
	}
	return fmt.Sprintf("%d", v)
}

func (o *optionItem) doIncrement() {
	if o.getValue == nil || o.setValue == nil {
		return
	}
	v := o.getValue()
	inc := o.increment
	if inc == 0 {
		inc = 1
	}
	v += inc
	if v > o.max {
		//v = o.max
		v = o.min
	}
	o.setValue(v)
}

func (o *optionItem) doDecrement() {
	if o.getValue == nil || o.setValue == nil {
		return
	}
	v := o.getValue()
	inc := o.increment
	if inc == 0 {
		inc = 1
	}
	v -= inc
	if v < o.min {
		//v = o.min
		v = o.max
	}
	o.setValue(v)
}

// OptionsState shows the options menu with live value editing.
type OptionsState struct {
	engine         *Engine
	items          []*optionItem
	cursor         int
	ticks          int
	cursorAlpha    float64
	cursorAlphaMod float64
	cursorImage    *ebiten.Image
	fallbackCanvas *ebiten.Image
}

func NewOptionsState(engine *Engine) *OptionsState {
	diffChoices := []string{"1", "2", "3", "4", "5", "6", "7", "8"}
	speedChoices := []string{"Slow 2", "Slow 1", "Normal", "Fast 1", "Fast 2"}

	items := []*optionItem{
		{
			label:    "Difficulty",
			getValue: func() int { return engine.cfg.Options.Difficulty },
			setValue: func(v int) { engine.cfg.Options.Difficulty = v },
			choices:  diffChoices,
			min:      1, max: 8,
		},
		{
			label:     "Life",
			getValue:  func() int { return engine.cfg.Options.Life },
			setValue:  func(v int) { engine.cfg.Options.Life = v },
			min:       10,
			max:       300,
			increment: 10,
		},
		{
			label:     "Time Limit",
			getValue:  func() int { return engine.cfg.Options.Time },
			setValue:  func(v int) { engine.cfg.Options.Time = v },
			min:       -1,
			max:       99,
			increment: 1,
		},
		{
			label:    "Game Speed",
			getValue: func() int { return engine.cfg.Options.GameSpeed },
			setValue: func(v int) {
				engine.cfg.Options.GameSpeed = v
				engine.applyScheduler()
			},
			choices: speedChoices,
			min:     -2, max: 2,
		},
		{
			label:    "Sound Volume",
			getValue: func() int { return engine.cfg.Sound.WavVolume },
			setValue: func(v int) { engine.cfg.Sound.WavVolume = v },
			min:      0, max: 100, increment: 5,
		},
		{
			label:    "Music Volume",
			getValue: func() int { return engine.cfg.Sound.BGMVolume },
			setValue: func(v int) { engine.cfg.Sound.BGMVolume = v },
			min:      0, max: 100, increment: 5,
		},
	}

	s := &OptionsState{
		engine:         engine,
		items:          items,
		cursorAlpha:    0,
		cursorAlphaMod: 2.0 / 255.0,
	}
	s.items = append(s.items, &optionItem{label: "Input Configuration", shortLabel: "Input Config", action: func() State { return NewInputConfigState(engine, s) }})
	s.items = append(s.items, &optionItem{label: "Return to Main Menu", shortLabel: "Back", action: func() State { return NewTitleState(engine) }})
	return s
}

func (s *OptionsState) Update() (State, error) {
	s.ticks++
	if input.GlobalManager.KeyJustPressed(ebiten.KeyF1) {
		return NewInputConfigState(s.engine, s), nil
	}

	if s.engine.motif.OptionBG != nil {
		s.engine.motif.OptionBG.Update()
	}

	// Pulse cursor alpha
	s.cursorAlpha += s.cursorAlphaMod
	if s.cursorAlpha > 42.0/255.0 {
		s.cursorAlpha = 42.0 / 255.0
		s.cursorAlphaMod = -2.0 / 255.0
	} else if s.cursorAlpha < 0 {
		s.cursorAlpha = 0
		s.cursorAlphaMod = 2.0 / 255.0
	}

	// Navigate
	if input.GlobalManager.IsJustPressed(1, input.ActionUp) {
		s.cursor--
		if s.cursor < 0 {
			s.cursor = len(s.items) - 1
		}
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CursorMoveSnd[0], s.engine.motif.OptionInfo.CursorMoveSnd[1])
	}
	if input.GlobalManager.IsJustPressed(1, input.ActionDown) {
		s.cursor = (s.cursor + 1) % len(s.items)
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CursorMoveSnd[0], s.engine.motif.OptionInfo.CursorMoveSnd[1])
	}

	// Adjust value
	if input.GlobalManager.IsJustPressed(1, input.ActionLeftUI) {
		s.items[s.cursor].doDecrement()
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CursorMoveSnd[0], s.engine.motif.OptionInfo.CursorMoveSnd[1])
	}
	if input.GlobalManager.IsJustPressed(1, input.ActionRightUI) {
		s.items[s.cursor].doIncrement()
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CursorMoveSnd[0], s.engine.motif.OptionInfo.CursorMoveSnd[1])
	}

	// Accept / close
	if input.GlobalManager.IsJustPressed(1, input.ActionEscape) {
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CancelSnd[0], s.engine.motif.OptionInfo.CancelSnd[1])
		return NewTitleState(s.engine), nil
	}
	if input.GlobalManager.IsJustPressed(1, input.ActionEnter) {
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CursorDoneSnd[0], s.engine.motif.OptionInfo.CursorDoneSnd[1])
		if action := s.items[s.cursor].action; action != nil {
			return action(), nil
		}
		s.items[s.cursor].doIncrement()
	}

	return s, nil
}

func (s *OptionsState) Draw(screen *ebiten.Image) {
	// Clear
	if s.engine.motif.OptionBG != nil && s.engine.motif.OptionBG.BGClearColor != nil {
		screen.Fill(s.engine.motif.OptionBG.BGClearColor)
	} else {
		screen.Fill(color.Black)
	}

	// Draw Layer 0 BG
	if s.engine.motif.OptionBG != nil {
		s.engine.motif.OptionBG.DrawLayer(screen, 0, 0, 0, 0, s.engine.sprites)
	}

	// Use the same font setup as the title menu
	fi := s.engine.motif.TitleInfo.ItemFont
	fiActive := s.engine.motif.TitleInfo.ItemActiveFont
	spacing := s.engine.motif.TitleInfo.MenuSpacing[1]
	width, height := s.engine.logicalWidth, s.engine.logicalHeight
	sx, sy := float64(width)/320, float64(height)/240
	if spacing == 0 {
		spacing = 15 * sy
	}
	// Screenpacks may draw their title-menu labels as background art and
	// deliberately omit text fonts. Their Options still needs readable rows.
	fallbackRows := false
	for _, info := range []struct{ Index, Bank, Align int }{fi, fiActive} {
		if info.Index < 1 || info.Index >= len(s.engine.motif.Fonts) || s.engine.motif.Fonts[info.Index] == nil || float64(s.engine.motif.Fonts[info.Index].Height) > spacing {
			fallbackRows = true
		}
	}

	// Center the options list
	startX := float64(width) / 2
	startY := float64(height) / 4
	title := s.engine.themeValues("Option Info", "title.font", float64(fi.Index), float64(fi.Bank), 0)
	position := s.engine.themeValues("Option Info", "title.offset", startX, 36*sy)
	if index := int(title[0]); index >= 1 && index < len(s.engine.motif.Fonts) && s.engine.motif.Fonts[index] != nil {
		align := font.AlignCenter
		if title[2] > 0 {
			align = font.AlignLeft
		} else if title[2] < 0 {
			align = font.AlignRight
		}
		s.engine.drawMotifFont(screen, s.engine.motif.Fonts[index], s.engine.themeText("Option Info", "title.text", "OPTIONS"), int(position[0]), int(position[1]), int(title[1]), align, "Option Info", "title.font")
	}

	if fallbackRows {
		s.drawFallbackRows(screen)
	}
	for i, item := range s.items {
		if fallbackRows {
			break
		}
		fontInfo := fi
		fontKey := "menu.item.font"
		if i == s.cursor {
			fontInfo = fiActive
			fontKey = "menu.item.active.font"
		}

		yPos := startY + float64(i)*spacing

		var f *font.Font
		if fontInfo.Index >= 1 && fontInfo.Index < len(s.engine.motif.Fonts) {
			f = s.engine.motif.Fonts[fontInfo.Index]
		}

		if f != nil {
			// Title bitmap banks can contain uppercase glyphs only.
			label := strings.ToUpper(item.label)
			valueStr := strings.ToUpper(item.displayValue())

			// Cursor highlight: draw a faint box behind the active row
			if i == s.cursor && s.engine.motif.TitleInfo.BoxCursorVisible {
				if s.cursorImage == nil {
					s.cursorImage = ebiten.NewImage(1, 1)
					s.cursorImage.Fill(color.RGBA{R: 128, G: 200, B: 255, A: 255})
				}
				op := &ebiten.DrawImageOptions{}
				op.GeoM.Scale(240*sx, spacing+2*sy)
				op.GeoM.Translate(startX-120*sx, yPos-spacing)
				op.Blend = ebiten.BlendSourceOver
				op.ColorScale.ScaleAlpha(float32(s.cursorAlpha))
				screen.DrawImage(s.cursorImage, op)
			}

			valDisplay := "< " + valueStr + " >"
			if item.action != nil {
				valDisplay = "ENTER"
			}
			if item.shortLabel != "" && float64(f.TextWidthForBank(label, fontInfo.Bank)+f.TextWidthForBank(valDisplay, fontInfo.Bank)) > 210*sx {
				label = strings.ToUpper(item.shortLabel)
			}
			s.engine.drawMotifFont(screen, f, label, int(startX-110*sx), int(yPos), fontInfo.Bank, font.AlignLeft, "Title Info", fontKey)
			s.engine.drawMotifFont(screen, f, valDisplay, int(startX+110*sx), int(yPos), fontInfo.Bank, font.AlignRight, "Title Info", fontKey)
		}
	}

	// Hint text
	if !fallbackRows && fi.Index >= 1 && fi.Index < len(s.engine.motif.Fonts) {
		f := s.engine.motif.Fonts[fi.Index]
		if f != nil {
			hint := "LEFT/RIGHT CHANGE   F1 INPUT   ESC EXIT"
			if float64(f.TextWidthForBank(hint, fi.Bank)) > float64(width)-16*sx {
				hint = "F1 INPUT   ESC EXIT"
			}
			s.engine.drawMotifFont(screen, f, hint, int(startX), int(220*sy), fi.Bank, font.AlignCenter, "Title Info", "menu.item.font")
		}
	}

	// Draw Layer 1 BG (foreground)
	if s.engine.motif.OptionBG != nil {
		s.engine.motif.OptionBG.DrawLayer(screen, 1, 0, 0, 0, s.engine.sprites)
	}
}

// Use the same portable built-in lettering as Input Configuration when the
// motif's title font cannot represent an Options list. The motif background
// and authored heading remain outside this fitted, bounded settings panel.
func (s *OptionsState) drawFallbackRows(screen *ebiten.Image) {
	if s.fallbackCanvas == nil {
		s.fallbackCanvas = ebiten.NewImage(400, 210)
	}
	panel := s.fallbackCanvas
	panel.Fill(color.RGBA{R: 12, G: 18, B: 32, A: 240})
	for index, item := range s.items {
		marker := "  "
		if index == s.cursor {
			marker = "> "
		}
		y := 12 + index*18
		ebitenutil.DebugPrintAt(panel, marker+strings.ToUpper(item.label), 12, y)
		value := "< " + strings.ToUpper(item.displayValue()) + " >"
		if item.action != nil {
			value = "ENTER"
		}
		ebitenutil.DebugPrintAt(panel, value, 388-len(value)*6, y)
	}
	ebitenutil.DebugPrintAt(panel, "LEFT/RIGHT ADJUST   ENTER CHANGE/OPEN", 12, 170)
	ebitenutil.DebugPrintAt(panel, "F1 INPUT   ESC BACK   SESSION SETTINGS", 12, 188)
	w, h := float64(screen.Bounds().Dx()), float64(screen.Bounds().Dy())
	scale := min(w/400, h/280)
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Scale(scale, scale)
	op.GeoM.Translate((w-400*scale)/2, min(h/4, h-222*scale))
	screen.DrawImage(panel, op)
}
