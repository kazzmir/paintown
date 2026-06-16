package main

import (
	"image/color"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
)

// SelectState handles character and stage selection.
type SelectState struct {
	engine *Engine
	title  string
	chars  []*CharacterPortrait
	showP2 bool // whether P2 cursor is active (VS/Team VS/Co-op only)

	p1CursorX int
	p1CursorY int
	p2CursorX int
	p2CursorY int
	p1Done    bool
	p2Done    bool
	ticks     int
}

func NewSelectState(engine *Engine, title string) *SelectState {
	si := engine.motif.SelectInfo

	// Resolve select.def path
	selectDefPath := engine.motif.Files.Select
	if selectDefPath == "" {
		selectDefPath = "select.def"
	}
	selectDefPath = filepath.Join(engine.dataDir, "data", selectDefPath)
	charsDir := filepath.Join(engine.dataDir, "chars")

	chars := loadCharacters(selectDefPath, charsDir)

	// Determine if P2 cursor should be shown
	// Only in modes that involve a second human player
	p2Modes := map[string]bool{
		"Versus Mode": true,
		"Team Versus": true,
		"Team Co-op":  true,
	}

	s := &SelectState{
		engine:    engine,
		title:     title,
		chars:     chars,
		showP2:    p2Modes[title],
		p1CursorX: si.P1StartCell[0],
		p1CursorY: si.P1StartCell[1],
		p2CursorX: si.P2StartCell[0],
		p2CursorY: si.P2StartCell[1],
	}

	return s
}

// drawCursorOutline draws a hollow rectangle border on screen at (x, y) with size (w, h).
func drawCursorOutline(screen *ebiten.Image, x, y, w, h float64, c color.RGBA, alpha float32) {
	thickness := 1.0
	lines := [][4]float64{
		{x, y, w, thickness},         // top
		{x, y + h - 1, w, thickness}, // bottom
		{x, y, thickness, h},         // left
		{x + w - 1, y, thickness, h}, // right
	}
	pix := ebiten.NewImage(1, 1)
	pix.Fill(c)
	for _, l := range lines {
		op := &ebiten.DrawImageOptions{}
		op.GeoM.Scale(l[2], l[3])
		op.GeoM.Translate(l[0], l[1])
		op.Blend = ebiten.BlendSourceOver
		op.ColorScale.ScaleAlpha(alpha)
		screen.DrawImage(pix, op)
	}
}

func (s *SelectState) rows() int {
	if s.engine.motif.SelectInfo.Rows > 0 {
		return s.engine.motif.SelectInfo.Rows
	}
	return 6
}

func (s *SelectState) cols() int {
	if s.engine.motif.SelectInfo.Columns > 0 {
		return s.engine.motif.SelectInfo.Columns
	}
	return 10
}

func (s *SelectState) wrap(x, max int) int {
	if s.engine.motif.SelectInfo.Wrapping {
		x = (x + max) % max
	} else {
		if x < 0 {
			x = 0
		} else if x >= max {
			x = max - 1
		}
	}
	return x
}

// charAt returns the character at grid position (col, row), or nil.
func (s *SelectState) charAt(col, row int) *CharacterPortrait {
	idx := row*s.cols() + col
	if idx < 0 || idx >= len(s.chars) {
		return nil
	}
	return s.chars[idx]
}

func (s *SelectState) Update() (State, error) {
	s.ticks++

	if s.engine.motif.SelectBG != nil {
		s.engine.motif.SelectBG.Update()
	}

	snd := s.engine.motif.SelectInfo

	// P1 cursor
	if !s.p1Done {
		if input.GlobalManager.IsJustPressed(1, input.ActionLeftUI) {
			s.p1CursorX = s.wrap(s.p1CursorX-1, s.cols())
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(1, input.ActionRightUI) {
			s.p1CursorX = s.wrap(s.p1CursorX+1, s.cols())
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(1, input.ActionUp) {
			s.p1CursorY = s.wrap(s.p1CursorY-1, s.rows())
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(1, input.ActionDown) {
			s.p1CursorY = s.wrap(s.p1CursorY+1, s.rows())
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(1, input.ActionEnter) || input.GlobalManager.IsJustPressed(1, input.ActionZ) {
			s.p1Done = true
			s.engine.PlaySnd(snd.CursorDoneSnd[0], snd.CursorDoneSnd[1])
		}
	}

	// P2 cursor (only in two-player modes)
	if s.showP2 && !s.p2Done {
		if input.GlobalManager.IsJustPressed(2, input.ActionLeftUI) {
			s.p2CursorX = s.wrap(s.p2CursorX-1, s.cols())
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(2, input.ActionRightUI) {
			s.p2CursorX = s.wrap(s.p2CursorX+1, s.cols())
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(2, input.ActionUp) {
			s.p2CursorY = s.wrap(s.p2CursorY-1, s.rows())
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(2, input.ActionDown) {
			s.p2CursorY = s.wrap(s.p2CursorY+1, s.rows())
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(2, input.ActionA) || input.GlobalManager.IsJustPressed(2, input.ActionEnter) {
			s.p2Done = true
			s.engine.PlaySnd(snd.CursorDoneSnd[0], snd.CursorDoneSnd[1])
		}
	}

	if input.GlobalManager.IsJustPressed(1, input.ActionEscape) {
		s.engine.PlaySnd(snd.CancelSnd[0], snd.CancelSnd[1])
		return NewTitleState(s.engine), nil
	}

	// Transition to VS when player(s) are done
	if s.p1Done && (!s.showP2 || s.p2Done) {
		// In Arcade mode, play the selected character's intro storyboard first
		if s.title == "Arcade Mode" {
			ch := s.charAt(s.p1CursorX, s.p1CursorY)
			if ch != nil && ch.IntroStoryboard != "" {
				return &StoryboardState{
					engine:     s.engine,
					storyboard: ch.IntroStoryboard,
					nextState:  func() State { return NewVersusState(s.engine) },
				}, nil
			}
		}
		return NewVersusState(s.engine), nil
	}

	return s, nil
}

func (s *SelectState) Draw(screen *ebiten.Image) {
	// Clear
	if s.engine.motif.SelectBG != nil && s.engine.motif.SelectBG.BGClearColor != nil {
		screen.Fill(s.engine.motif.SelectBG.BGClearColor)
	} else {
		screen.Fill(color.Black)
	}

	// Layer 0 BG
	if s.engine.motif.SelectBG != nil {
		for _, el := range s.engine.motif.SelectBG.Elements {
			if el.GetLayerNo() == 0 {
				el.Draw(screen, 0, 0, 0, s.engine.sprites)
			}
		}
	}

	si := s.engine.motif.SelectInfo
	cellW := si.CellSize[0]
	cellH := si.CellSize[1]
	if cellW == 0 {
		cellW = 27
	}
	if cellH == 0 {
		cellH = 27
	}
	spacing := si.CellSpacing
	if spacing == 0 {
		spacing = 2
	}
	gridX := si.Pos[0]
	gridY := si.Pos[1]

	// Cursor pulse alpha
	pulse := float32(0.5 + 0.5*float64(s.ticks%60)/60.0)

	for row := 0; row < s.rows(); row++ {
		for col := 0; col < s.cols(); col++ {
			cx := float64(gridX + col*(cellW+spacing))
			cy := float64(gridY + row*(cellH+spacing))

			ch := s.charAt(col, row)

			// Cell background
			cellBG := ebiten.NewImage(cellW, cellH)
			cellBG.Fill(color.RGBA{30, 30, 60, 200})
			bgOp := &ebiten.DrawImageOptions{}
			bgOp.GeoM.Translate(cx, cy)
			screen.DrawImage(cellBG, bgOp)

			// Small portrait (group 9000, image 0)
			if ch != nil && ch.SmallPortrait != nil {
				pw := float64(ch.SmallPortrait.Bounds().Dx())
				ph := float64(ch.SmallPortrait.Bounds().Dy())
				scaleX := float64(cellW) / pw
				scaleY := float64(cellH) / ph
				op := &ebiten.DrawImageOptions{}
				op.GeoM.Scale(scaleX, scaleY)
				op.GeoM.Translate(cx, cy)
				screen.DrawImage(ch.SmallPortrait, op)
			}

			// P1 cursor outline (blue)
			if row == s.p1CursorY && col == s.p1CursorX {
				drawCursorOutline(screen, cx, cy, float64(cellW), float64(cellH), color.RGBA{80, 160, 255, 220}, pulse)
			}

			// P2 cursor outline (red) — only in two-player modes
			if s.showP2 && row == s.p2CursorY && col == s.p2CursorX {
				p2Alpha := pulse
				if row == s.p1CursorY && col == s.p1CursorX {
					// Blink when on same cell as P1
					if (s.ticks/15)%2 == 0 {
						p2Alpha = 0
					}
				}
				drawCursorOutline(screen, cx, cy, float64(cellW), float64(cellH), color.RGBA{255, 80, 80, 220}, p2Alpha)
			}
		}
	}

	// Big portrait panels — P1 left, P2 right
	s.drawFacePanel(screen, s.p1CursorX, s.p1CursorY,
		si.P1FaceOffset, si.P1FaceScale,
		si.P1NameOffset, si.P1NameFont)
	s.drawFacePanel(screen, s.p2CursorX, s.p2CursorY,
		si.P2FaceOffset, si.P2FaceScale,
		si.P2NameOffset, si.P2NameFont)

	// Mode title (e.g. "Arcade Mode")
	if s.title != "" {
		tFont := si.TitleFont
		if tFont[0] >= 1 && tFont[0] < len(s.engine.motif.Fonts) {
			f := s.engine.motif.Fonts[tFont[0]]
			if f != nil {
				align := font.AlignCenter
				switch tFont[2] {
				case 1:
					align = font.AlignLeft
				case -1:
					align = font.AlignRight
				}
				f.Draw(screen, s.title, si.TitleOffset[0], si.TitleOffset[1], tFont[1], align)
			}
		}
	}

	// Layer 1 BG (foreground)
	if s.engine.motif.SelectBG != nil {
		for _, el := range s.engine.motif.SelectBG.Elements {
			if el.GetLayerNo() == 1 {
				el.Draw(screen, 0, 0, 0, s.engine.sprites)
			}
		}
	}
}

// drawFacePanel draws the scaled big portrait and character name for one player.
// faceOff is the top-left corner to draw from; faceScale is the (sx, sy) multiplier.
// nameOff and nameFont are the position+font for the character name.
func (s *SelectState) drawFacePanel(screen *ebiten.Image, col, row int,
	faceOff [2]float64, faceScale [2]float64,
	nameOff [2]int, nameFont [3]int) {

	ch := s.charAt(col, row)
	if ch == nil {
		return
	}

	// Apply default scale of 1 if not set
	sx := faceScale[0]
	sy := faceScale[1]
	if sx == 0 {
		sx = 1
	}
	if sy == 0 {
		sy = 1
	}

	// Big portrait (group 9000, image 1), fall back to small (image 0)
	portrait := ch.BigPortrait
	if portrait == nil {
		portrait = ch.SmallPortrait
	}
	if portrait != nil {
		op := &ebiten.DrawImageOptions{}
		op.GeoM.Scale(sx, sy)
		op.GeoM.Translate(faceOff[0], faceOff[1])
		op.Filter = ebiten.FilterNearest
		screen.DrawImage(portrait, op)
	}

	// Character name using per-player font
	if nameFont[0] >= 1 && nameFont[0] < len(s.engine.motif.Fonts) {
		f := s.engine.motif.Fonts[nameFont[0]]
		if f != nil && ch.Name != "" {
			align := font.AlignLeft
			switch nameFont[2] {
			case 0:
				align = font.AlignCenter
			case -1:
				align = font.AlignRight
			}
			f.Draw(screen, ch.Name, nameOff[0], nameOff[1], nameFont[1], align)
		}
	}
}
