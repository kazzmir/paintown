package main

import (
	"image"
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/hajimehoshi/ebiten/v2/inpututil"
	"github.com/kazzmir/paintown/game/mugen/font"
)

type State interface {
	Update() (State, error)
	Draw(screen *ebiten.Image)
}

// TODO: Implement specific states like TitleState, SelectState, etc.

type StoryboardState struct {
	engine     *Engine
	storyboard string
	nextState  func() State
	ticks      int
}

func (s *StoryboardState) Update() (State, error) {
	if s.storyboard == "" {
		return s.nextState(), nil
	}

	s.ticks++
	// Basic implementation: skip on any key or after some time if we don't have a real storyboard player yet
	if inpututil.IsKeyJustPressed(ebiten.KeyEnter) || inpututil.IsKeyJustPressed(ebiten.KeySpace) || s.ticks > 120 {
		return s.nextState(), nil
	}

	return s, nil
}

func (s *StoryboardState) Draw(screen *ebiten.Image) {
	// Placeholder draw for storyboard
	ebitenutil.DebugPrint(screen, "Playing Storyboard: "+s.storyboard+"\n(Press Enter to skip)")
}

type TitleState struct {
	engine         *Engine
	selectIndex    int
	topIndex       int
	items          []string
	cursorAlpha    float64
	cursorAlphaMod float64
}

func NewTitleState(engine *Engine) *TitleState {
	s := &TitleState{
		engine:         engine,
		cursorAlpha:    0,
		cursorAlphaMod: 2.0 / 60.0, // Pulse over 1 second? No, let's use a faster rate. Reference used 2/255 per tick? No, 2 per check.
	}
	// Reference used 60 FPS. 2/255 * 60 = 0.47 per second.
	// Let's use 2.0/255.0 as the mod.
	s.cursorAlphaMod = 2.0 / 255.0
	// Extract active menu items from motif
	// MUGEN order is typically fixed: arcade, versus, teamarcade, teamversus, teamcoop, survival, survivalcoop, training, watch, options, exit
	order := []string{"arcade", "versus", "teamarcade", "teamversus", "teamcoop", "survival", "survivalcoop", "training", "watch", "options", "exit"}
	for _, id := range order {
		if name, ok := engine.motif.TitleInfo.ItemNames[id]; ok && name != "" {
			s.items = append(s.items, id)
		}
	}
	return s
}

func (s *TitleState) Update() (State, error) {
	s.cursorAlpha += s.cursorAlphaMod
	if s.cursorAlpha > 42.0/255.0 {
		s.cursorAlpha = 42.0 / 255.0
		s.cursorAlphaMod = -2.0 / 255.0
	} else if s.cursorAlpha < 0 {
		s.cursorAlpha = 0
		s.cursorAlphaMod = 2.0 / 255.0
	}
	if s.engine.motif.TitleBG != nil {
		for _, el := range s.engine.motif.TitleBG.Elements {
			el.Update()
		}
	}

	if len(s.items) == 0 {
		return s, nil
	}

	if inpututil.IsKeyJustPressed(ebiten.KeyUp) {
		s.selectIndex--
		if s.selectIndex < 0 {
			s.selectIndex = len(s.items) - 1
		}
		s.engine.PlaySnd(s.engine.motif.TitleInfo.CursorMoveSnd[0], s.engine.motif.TitleInfo.CursorMoveSnd[1])
	}
	if inpututil.IsKeyJustPressed(ebiten.KeyDown) {
		s.selectIndex = (s.selectIndex + 1) % len(s.items)
		s.engine.PlaySnd(s.engine.motif.TitleInfo.CursorMoveSnd[0], s.engine.motif.TitleInfo.CursorMoveSnd[1])
	}

	if inpututil.IsKeyJustPressed(ebiten.KeyEscape) {
		return nil, ebiten.Termination
	}

	visibleItems := s.engine.motif.TitleInfo.VisibleItems
	if visibleItems > 0 && len(s.items) > visibleItems {
		if s.selectIndex < s.topIndex {
			s.topIndex = s.selectIndex
		} else if s.selectIndex >= s.topIndex+visibleItems {
			s.topIndex = s.selectIndex - visibleItems + 1
		}
	} else {
		s.topIndex = 0
	}

	if inpututil.IsKeyJustPressed(ebiten.KeyEnter) {
		s.engine.PlaySnd(s.engine.motif.TitleInfo.CursorDoneSnd[0], s.engine.motif.TitleInfo.CursorDoneSnd[1])
		selected := s.items[s.selectIndex]
		if selected == "exit" {
			return nil, ebiten.Termination
		}
		// TODO: Transition to other states
	}

	return s, nil
}

func (s *TitleState) Draw(screen *ebiten.Image) {
	// 0. Clear screen with bgclearcolor
	if s.engine.motif.TitleBG != nil && s.engine.motif.TitleBG.BGClearColor != nil {
		screen.Fill(s.engine.motif.TitleBG.BGClearColor)
	} else {
		screen.Fill(color.Black)
	}

	// 1. Draw Layer 0 Backgrounds
	if s.engine.motif.TitleBG != nil {
		for _, el := range s.engine.motif.TitleBG.Elements {
			if el.GetLayerNo() == 0 {
				el.Draw(screen, 0, 0, s.engine.sprites)
			}
		}
	}

	menuX := s.engine.motif.TitleInfo.MenuPos[0]
	menuY := s.engine.motif.TitleInfo.MenuPos[1]

	// MUGEN menu item spacing
	spacingY := s.engine.motif.TitleInfo.MenuSpacing[1]
	if spacingY == 0 {
		spacingY = 15 // Fallback
	}

	visibleItems := s.engine.motif.TitleInfo.VisibleItems
	if visibleItems <= 0 {
		visibleItems = len(s.items)
	}

	// 2. Draw Box Cursor (if visible)
	if s.engine.motif.TitleInfo.BoxCursorVisible {
		// BoxCursor is drawn for the selected item
	}

	// 3. Draw Menu Items
	// Determine menu window for clipping
	visibleItems = s.engine.motif.TitleInfo.VisibleItems
	if visibleItems <= 0 {
		visibleItems = len(s.items)
	}

	margins := s.engine.motif.TitleInfo.VisibleMargins
	menuTarget := screen
	minY := 0.0
	if margins[0] != 0 || margins[1] != 0 {
		// margins.y = 12, 8
		minY = menuY - float64(margins[0])
		maxY := menuY + float64(visibleItems-1)*spacingY + float64(margins[1])
		menuTarget = screen.SubImage(image.Rect(0, int(minY), screen.Bounds().Dx(), int(maxY))).(*ebiten.Image)
	}

	// Draw Box Cursor to menuTarget so it's clipped by the same window
	if s.engine.motif.TitleInfo.BoxCursorVisible {
		box := s.engine.motif.TitleInfo.BoxCursorCoords
		bx := menuX + float64(box[0])
		by := menuY - 2 + float64(s.selectIndex-s.topIndex)*spacingY + float64(box[1])
		bw := float64(box[2] - box[0])
		bh := float64(box[3] - box[1])

		// Use a 1x1 image and scale it to avoid FillRect potential alpha issues on some platforms/drivers
		cursorImg := ebiten.NewImage(1, 1)
		cursorImg.Fill(color.RGBA{R: 128, G: 200, B: 255, A: 255})
		op := &ebiten.DrawImageOptions{}
		op.GeoM.Scale(bw, bh)
		op.GeoM.Translate(bx, by)
		op.Blend = ebiten.BlendSourceOver
		op.ColorScale.ScaleAlpha(float32(s.cursorAlpha))
		menuTarget.DrawImage(cursorImg, op)
	}

	// Loop all items, they will get clipped by the menu's rect or drawspace
	for idx, id := range s.items {
		name := s.engine.motif.TitleInfo.ItemNames[id]

		fontInfo := s.engine.motif.TitleInfo.ItemFont
		if idx == s.selectIndex {
			fontInfo = s.engine.motif.TitleInfo.ItemActiveFont
		}

		if fontInfo.Index >= 1 && fontInfo.Index < len(s.engine.motif.Fonts) {
			f := s.engine.motif.Fonts[fontInfo.Index]
			if f != nil {
				align := font.AlignLeft
				if fontInfo.Align == 0 {
					align = font.AlignCenter
				} else if fontInfo.Align == -1 {
					align = font.AlignRight
				}

				// Shift font up by ascent estimated from box cursor
				yShift := 0.0
				if s.engine.motif.TitleInfo.BoxCursorVisible {
					yShift = float64(s.engine.motif.TitleInfo.BoxCursorCoords[1])
				} else {
					yShift = -float64(f.Height) + 2
				}

				// Draw at y = menu_pos.y + (idx - topIndex) * spacing.y
				f.Draw(menuTarget, name, int(menuX), int(menuY+float64(idx-s.topIndex)*spacingY+yShift), fontInfo.Bank, align)
			}
		}
	}

	// 4. Draw Layer 1 Backgrounds
	if s.engine.motif.TitleBG != nil {
		for _, el := range s.engine.motif.TitleBG.Elements {
			if el.GetLayerNo() == 1 {
				el.Draw(screen, 0, 0, s.engine.sprites)
			}
		}
	}
}
