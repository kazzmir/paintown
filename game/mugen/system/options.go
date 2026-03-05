package main

import (
	"fmt"
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/inpututil"
	"github.com/kazzmir/paintown/game/mugen/font"
)

// optionItem represents a single configurable option in the options menu.
type optionItem struct {
	label    string
	getValue func() int
	setValue func(int)
	// Display strings for each value, indexed by (value - min)
	// If nil, just shows the integer.
	choices  []string
	min, max int
}

func (o *optionItem) displayValue() string {
	v := o.getValue()
	if o.choices != nil {
		idx := v - o.min
		if idx >= 0 && idx < len(o.choices) {
			return o.choices[idx]
		}
	}
	return fmt.Sprintf("%d", v)
}

func (o *optionItem) increment() {
	v := o.getValue() + 1
	if v > o.max {
		v = o.max
	}
	o.setValue(v)
}

func (o *optionItem) decrement() {
	v := o.getValue() - 1
	if v < o.min {
		v = o.min
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
}

func NewOptionsState(engine *Engine) *OptionsState {
	cfg := engine.cfg.Options

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
			label:    "Life",
			getValue: func() int { return engine.cfg.Options.Life },
			setValue: func(v int) { engine.cfg.Options.Life = v },
			min:      0, max: 300,
		},
		{
			label:    "Time Limit",
			getValue: func() int { return engine.cfg.Options.Time },
			setValue: func(v int) { engine.cfg.Options.Time = v },
			min:      -1, max: 99,
		},
		{
			label:    "Game Speed",
			getValue: func() int { return engine.cfg.Options.GameSpeed },
			setValue: func(v int) { engine.cfg.Options.GameSpeed = v },
			choices:  speedChoices,
			min:      -2, max: 2,
		},
		{
			label:    "Sound Volume",
			getValue: func() int { return engine.cfg.Options.WavVolume },
			setValue: func(v int) { engine.cfg.Options.WavVolume = v },
			min:      0, max: 100,
		},
		{
			label:    "Music Volume",
			getValue: func() int { return engine.cfg.Options.MidiVolume },
			setValue: func(v int) { engine.cfg.Options.MidiVolume = v },
			min:      0, max: 100,
		},
	}

	_ = cfg
	return &OptionsState{
		engine:         engine,
		items:          items,
		cursorAlpha:    0,
		cursorAlphaMod: 2.0 / 255.0,
	}
}

func (s *OptionsState) Update() (State, error) {
	s.ticks++

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
	if inpututil.IsKeyJustPressed(ebiten.KeyUp) {
		s.cursor--
		if s.cursor < 0 {
			s.cursor = len(s.items) - 1
		}
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CursorMoveSnd[0], s.engine.motif.OptionInfo.CursorMoveSnd[1])
	}
	if inpututil.IsKeyJustPressed(ebiten.KeyDown) {
		s.cursor = (s.cursor + 1) % len(s.items)
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CursorMoveSnd[0], s.engine.motif.OptionInfo.CursorMoveSnd[1])
	}

	// Adjust value
	if inpututil.IsKeyJustPressed(ebiten.KeyLeft) {
		s.items[s.cursor].decrement()
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CursorMoveSnd[0], s.engine.motif.OptionInfo.CursorMoveSnd[1])
	}
	if inpututil.IsKeyJustPressed(ebiten.KeyRight) {
		s.items[s.cursor].increment()
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CursorMoveSnd[0], s.engine.motif.OptionInfo.CursorMoveSnd[1])
	}

	// Accept / close
	if inpututil.IsKeyJustPressed(ebiten.KeyEscape) {
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CancelSnd[0], s.engine.motif.OptionInfo.CancelSnd[1])
		return NewTitleState(s.engine), nil
	}
	if inpututil.IsKeyJustPressed(ebiten.KeyEnter) {
		s.engine.PlaySnd(s.engine.motif.OptionInfo.CursorDoneSnd[0], s.engine.motif.OptionInfo.CursorDoneSnd[1])
		// "Done" selects back to title
		return NewTitleState(s.engine), nil
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
		for _, el := range s.engine.motif.OptionBG.Elements {
			if el.GetLayerNo() == 0 {
				el.Draw(screen, 0, 0, s.engine.sprites)
			}
		}
	}

	// Use the same font setup as the title menu
	fi := s.engine.motif.TitleInfo.ItemFont
	fiActive := s.engine.motif.TitleInfo.ItemActiveFont
	spacing := s.engine.motif.TitleInfo.MenuSpacing[1]
	if spacing == 0 {
		spacing = 15
	}

	// Center the options list
	startX := 160.0
	startY := 60.0

	for i, item := range s.items {
		fontInfo := fi
		if i == s.cursor {
			fontInfo = fiActive
		}

		yPos := startY + float64(i)*spacing

		var f *font.Font
		if fontInfo.Index >= 1 && fontInfo.Index < len(s.engine.motif.Fonts) {
			f = s.engine.motif.Fonts[fontInfo.Index]
		}

		if f != nil {
			label := item.label
			valueStr := item.displayValue()

			// Cursor highlight: draw a faint box behind the active row
			if i == s.cursor && s.engine.motif.TitleInfo.BoxCursorVisible {
				cursorImg := ebiten.NewImage(240, int(spacing)+2)
				cursorImg.Fill(color.RGBA{R: 128, G: 200, B: 255, A: 255})
				op := &ebiten.DrawImageOptions{}
				op.GeoM.Translate(startX-120, yPos-2)
				op.Blend = ebiten.BlendSourceOver
				op.ColorScale.ScaleAlpha(float32(s.cursorAlpha))
				screen.DrawImage(cursorImg, op)
			}

			// Draw "Label   < Value >"
			f.Draw(screen, label, int(startX)-110, int(yPos), fontInfo.Bank, font.AlignLeft)
			// Draw left arrow, value, right arrow in the right column
			valDisplay := "< " + valueStr + " >"
			f.Draw(screen, valDisplay, int(startX)+110, int(yPos), fontInfo.Bank, font.AlignRight)
		}
	}

	// Hint text
	if fi.Index >= 1 && fi.Index < len(s.engine.motif.Fonts) {
		f := s.engine.motif.Fonts[fi.Index]
		if f != nil {
			f.Draw(screen, "Left/Right to change  Esc to exit", 160, 220, fi.Bank, font.AlignCenter)
		}
	}

	// Draw Layer 1 BG (foreground)
	if s.engine.motif.OptionBG != nil {
		for _, el := range s.engine.motif.OptionBG.Elements {
			if el.GetLayerNo() == 1 {
				el.Draw(screen, 0, 0, s.engine.sprites)
			}
		}
	}
}

// --- Stubs for future screens ---

// WinScreenState stub.
type WinScreenState struct{ engine *Engine }

func NewWinScreenState(e *Engine) *WinScreenState { return &WinScreenState{engine: e} }
func (s *WinScreenState) Update() (State, error) {
	if inpututil.IsKeyJustPressed(ebiten.KeyEnter) {
		return NewTitleState(s.engine), nil
	}
	return s, nil
}
func (s *WinScreenState) Draw(screen *ebiten.Image) { screen.Fill(color.Black) }

// ContinueScreenState stub.
type ContinueScreenState struct{ engine *Engine }

func NewContinueScreenState(e *Engine) *ContinueScreenState { return &ContinueScreenState{engine: e} }
func (s *ContinueScreenState) Update() (State, error) {
	if inpututil.IsKeyJustPressed(ebiten.KeyEnter) {
		return NewTitleState(s.engine), nil
	}
	return s, nil
}
func (s *ContinueScreenState) Draw(screen *ebiten.Image) { screen.Fill(color.Black) }

// GameOverScreenState stub.
type GameOverScreenState struct{ engine *Engine }

func NewGameOverScreenState(e *Engine) *GameOverScreenState {
	return &GameOverScreenState{engine: e}
}
func (s *GameOverScreenState) Update() (State, error) {
	if inpututil.IsKeyJustPressed(ebiten.KeyEnter) {
		return NewTitleState(s.engine), nil
	}
	return s, nil
}
func (s *GameOverScreenState) Draw(screen *ebiten.Image) { screen.Fill(color.Black) }
