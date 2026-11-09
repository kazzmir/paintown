package main

import (
	"fmt"
	"image"
	"image/color"
	"math"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/render"
	"github.com/kazzmir/paintown/game/mugen/resource"
)

// SelectState handles character and stage selection.
type SelectState struct {
	engine *Engine
	title  string
	chars  []*CharacterPortrait
	showP2 bool // whether both rosters are selected (Watch uses P1 for both)

	p1CursorX              int
	p1CursorY              int
	p2CursorX              int
	p2CursorY              int
	p1Done                 bool
	p2Done                 bool
	ticks                  int
	stages                 []string
	stageIndex             int
	choosingStage          bool
	loadError              error
	palettes               [2]int
	selectDefPath          string
	teamMenu               bool
	teamModes              [2]match.TeamMode
	teamCounts, teamCursor [2]int
	teamReady              [2]bool
	rosters                [2][]match.Selection
	rosterPortraits        [2][]CharacterPortrait
	opponentTeamChosen     bool
	cursorClocks           [2]*air.Player
	spriteCache            background.SpriteCache
	indexSampler           render.Nearest
	linearSampler          render.Linear
}

func NewSelectState(engine *Engine, title string) *SelectState {
	si := engine.motif.SelectInfo

	// Resolve select.def path
	selectDefPath := engine.motif.Files.Select
	if selectDefPath == "" {
		selectDefPath = "select.def"
	}
	motifDir := ""
	if engine.motif.Source != "" {
		motifDir = filepath.Dir(engine.motif.Source)
	}
	resolved, loadError := resource.Resolve(selectDefPath, motifDir, filepath.Join(engine.dataDir, "data"), engine.dataDir)
	if loadError == nil {
		selectDefPath = resolved
	}
	charsDir := filepath.Join(engine.dataDir, "chars")

	var chars []*CharacterPortrait
	var stages []string
	if loadError == nil {
		chars, stages, loadError = loadCharacters(selectDefPath, charsDir, &engine.characterPacks, engine.cfg.Config.Language)
	}

	// Watch selects both rosters with P1; versus/co-op use two humans.
	p2Modes := map[string]bool{
		"Versus Mode":    true,
		"Team Versus":    true,
		"Team Co-op":     true,
		"Survival Co-op": true,
		"Watch Mode":     true,
	}

	s := &SelectState{
		engine:        engine,
		selectDefPath: selectDefPath,
		title:         title,
		chars:         chars,
		stages:        stages,
		loadError:     loadError,
		palettes:      [2]int{1, 2},
		showP2:        p2Modes[title],
		p1CursorX:     si.P1StartCell[1],
		p1CursorY:     si.P1StartCell[0],
		p2CursorX:     si.P2StartCell[1],
		p2CursorY:     si.P2StartCell[0],
	}

	s.normalizeCursor(&s.p1CursorX, &s.p1CursorY)
	if s.showP2 {
		s.normalizeCursor(&s.p2CursorX, &s.p2CursorY)
	}
	s.initTeams()
	return s
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
	if col < 0 || col >= s.cols() || row < 0 || row >= s.rows() || idx < 0 || idx >= len(s.chars) || s.chars[idx] == nil || s.chars[idx].Empty {
		return nil
	}
	return s.chars[idx]
}

func (s *SelectState) Update() (State, error) {
	if s.loadError != nil {
		return s, s.loadError
	}
	s.ticks++
	for _, clock := range s.cursorClocks {
		if clock != nil {
			clock.Update()
		}
	}

	if s.engine.motif.SelectBG != nil {
		s.engine.motif.SelectBG.Update()
	}

	snd := s.engine.motif.SelectInfo
	if input.GlobalManager.IsJustPressed(1, input.ActionEscape) {
		if s.choosingStage {
			s.choosingStage = false
			s.p1Done = false
			s.p2Done = false
			s.rosters = [2][]match.Selection{}
			s.rosterPortraits = [2][]CharacterPortrait{}
			if s.title == "Team Co-op" || s.title == "Survival Co-op" || s.title == "Team Arcade" || s.title == "Watch Mode" {
				s.initTeams()
			}
			return s, nil
		}
		return NewTitleState(s.engine), nil
	}
	if s.teamMenu {
		s.updateTeamMenu()
		return s, nil
	}
	if s.choosingStage {
		if input.GlobalManager.IsJustPressed(1, input.ActionLeftUI) {
			s.stageIndex = (s.stageIndex + len(s.stages) - 1) % len(s.stages)
		}
		if input.GlobalManager.IsJustPressed(1, input.ActionRightUI) {
			s.stageIndex = (s.stageIndex + 1) % len(s.stages)
		}
		if _, ok := s.confirm(1); ok {
			return s.startMatch()
		}
		return s, nil
	}

	// P1 cursor
	if !s.p1Done {
		if input.GlobalManager.IsJustPressed(1, input.ActionLeftUI) {
			s.moveCursor(&s.p1CursorX, &s.p1CursorY, -1, 0)
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(1, input.ActionRightUI) {
			s.moveCursor(&s.p1CursorX, &s.p1CursorY, 1, 0)
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(1, input.ActionUp) {
			s.moveCursor(&s.p1CursorX, &s.p1CursorY, 0, -1)
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(1, input.ActionDown) {
			s.moveCursor(&s.p1CursorX, &s.p1CursorY, 0, 1)
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if palette, ok := s.confirm(1); ok && s.charAt(s.p1CursorX, s.p1CursorY) != nil {
			s.p1Done = s.pickMember(0, s.chosen(s.p1CursorX, s.p1CursorY), palette)
			s.palettes[0] = palette
			s.engine.PlaySnd(snd.CursorDoneSnd[0], snd.CursorDoneSnd[1])
		}
	}

	// Watch reuses P1 controls after choosing the opposing team's mode.
	coop := s.title == "Team Co-op" || s.title == "Survival Co-op"
	if s.showP2 && !s.p2Done && (!coop || s.p1Done) && (s.title != "Watch Mode" || s.opponentTeamChosen) {
		inputSlot := 2
		if s.title == "Watch Mode" {
			inputSlot = 1
		}
		if input.GlobalManager.IsJustPressed(inputSlot, input.ActionLeftUI) {
			s.moveCursor(&s.p2CursorX, &s.p2CursorY, -1, 0)
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(inputSlot, input.ActionRightUI) {
			s.moveCursor(&s.p2CursorX, &s.p2CursorY, 1, 0)
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(inputSlot, input.ActionUp) {
			s.moveCursor(&s.p2CursorX, &s.p2CursorY, 0, -1)
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if input.GlobalManager.IsJustPressed(inputSlot, input.ActionDown) {
			s.moveCursor(&s.p2CursorX, &s.p2CursorY, 0, 1)
			s.engine.PlaySnd(snd.CursorMoveSnd[0], snd.CursorMoveSnd[1])
		}
		if palette, ok := s.confirm(inputSlot); ok && s.charAt(s.p2CursorX, s.p2CursorY) != nil {
			s.p2Done = s.pickMember(1, s.chosen(s.p2CursorX, s.p2CursorY), palette)
			s.palettes[1] = palette
			s.engine.PlaySnd(snd.CursorDoneSnd[0], snd.CursorDoneSnd[1])
		}
	}

	if input.GlobalManager.IsJustPressed(1, input.ActionEscape) {
		s.engine.PlaySnd(snd.CancelSnd[0], snd.CancelSnd[1])
		return NewTitleState(s.engine), nil
	}

	if s.p1Done && !s.opponentTeamChosen && (s.title == "Team Arcade" || s.title == "Watch Mode" || (s.title == "Team Co-op" && s.p2Done)) {
		s.teamMenu = true
		s.teamReady = [2]bool{true, false}
		return s, nil
	}
	// Transition to stage selection when all manually selected rosters are done.
	if s.p1Done && (!s.showP2 || s.p2Done) {
		if len(s.stages) == 0 {
			return s, fmt.Errorf("selection contains no available stages")
		}
		s.choosingStage = true
	}

	return s, nil
}

func (s *SelectState) confirm(slot int) (int, bool) {
	for index, action := range []input.Action{input.ActionA, input.ActionB, input.ActionC, input.ActionX, input.ActionY, input.ActionZ, input.ActionEnter} {
		if input.GlobalManager.IsJustPressed(slot, action) {
			palette := index + 1
			if index == 6 {
				palette = 1
			}
			if input.GlobalManager.IsPressed(slot, input.ActionStart) && index < 6 {
				palette += 6
			}
			return palette, true
		}
	}
	return 0, false
}

func (s *SelectState) chosen(col, row int) *CharacterPortrait {
	ch := s.charAt(col, row)
	if ch == nil {
		return nil
	}
	if ch.DefPath != "" {
		return ch
	}
	return s.randomCharacter()
}

func (s *SelectState) randomCharacter() *CharacterPortrait {
	var available []*CharacterPortrait
	for _, candidate := range s.chars {
		if candidate.DefPath != "" {
			available = append(available, candidate)
		}
	}
	if len(available) == 0 {
		return nil
	}
	return available[s.ticks%len(available)]
}

func (s *SelectState) startMatch() (State, error) {
	// A chosen team is a snapshot, independent of the current cursor. Modes
	// with an automatic opponent must not depend on the hidden P2 cursor.
	picks := [2]*CharacterPortrait{s.chosen(s.p1CursorX, s.p1CursorY), s.chosen(s.p2CursorX, s.p2CursorY)}
	for side, roster := range s.rosters {
		if len(roster) == 0 {
			continue
		}
		if len(s.rosterPortraits[side]) > 0 {
			picks[side] = &s.rosterPortraits[side][0]
			continue
		}
		selected := roster[0]
		picks[side] = &CharacterPortrait{Name: selected.Name, DefPath: selected.DefPath}
		for _, ch := range s.chars {
			if ch.DefPath == selected.DefPath {
				picks[side] = ch
				break
			}
		}
	}
	if !s.showP2 && picks[1] == nil {
		picks[1] = s.randomCharacter()
	}
	p1, p2 := picks[0], picks[1]
	if p1 == nil || p2 == nil {
		return s, fmt.Errorf("selection has no playable characters")
	}
	request := match.Request{Teams: s.selectedTeams(p1, p2), Mode: s.title, StagePath: s.stages[s.stageIndex], Music: p2.Entry.Music,
		Players: [2]match.Selection{{DefPath: p1.DefPath, Name: p1.Name, Palette: s.palettes[0]}, {DefPath: p2.DefPath, Name: p2.Name, Palette: s.palettes[1]}}}
	if !s.showP2 && s.title != "Training" {
		request.Players[1].AILevel = max(1, min(8, s.engine.cfg.Options.Difficulty))
	}
	request, err := s.prepareMode(request)
	if err != nil {
		return s, err
	}
	versus := NewVersusState(s.engine, request)
	if s.title == "Arcade Mode" && p1.IntroStoryboard != "" {
		return &StoryboardState{engine: s.engine, storyboard: p1.IntroStoryboard, nextState: func() State { return versus }}, nil
	}
	return versus, nil
}

func (s *SelectState) Draw(screen *ebiten.Image) {
	defer s.drawTeamSelection(screen)
	// Clear
	if s.engine.motif.SelectBG != nil && s.engine.motif.SelectBG.BGClearColor != nil {
		screen.Fill(s.engine.motif.SelectBG.BGClearColor)
	} else {
		screen.Fill(color.Black)
	}

	if s.teamMenu {
		return
	}
	// Layer 0 BG
	if s.engine.motif.SelectBG != nil {
		s.engine.motif.SelectBG.DrawLayer(screen, 0, 0, 0, 0, s.engine.sprites)
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

	for row := 0; row < s.rows(); row++ {
		for col := 0; col < s.cols(); col++ {
			cx, cy := float64(gridX+col*(cellW+spacing)), float64(gridY+row*(cellH+spacing))
			ch := s.charAt(col, row)
			if ch != nil || si.ShowEmptyBoxes {
				s.drawSelectionSprite(screen, si.CellBgSpr, cx, cy, 1, 1, nil)
			}
			if ch != nil {
				if ch.SmallPortrait != nil {
					ch.drawPortrait(screen, [2]int{9000, 0}, [2]float64{cx + si.PortraitOffset[0], cy + si.PortraitOffset[1]}, si.PortraitScale, 1, s.engine.logicalWidth, nil)
				} else if ch.DefPath == "" {
					s.drawSelectionSprite(screen, si.CellRandSpr, cx, cy, 1, 1, nil)
				}
			}
			if row == s.p1CursorY && col == s.p1CursorX {
				s.drawSelectionCursor(screen, 0, cx, cy)
			}
			if s.showP2 && row == s.p2CursorY && col == s.p2CursorX {
				if !si.P2Cursor.Blink || row != s.p1CursorY || col != s.p1CursorX || s.ticks/15%2 != 0 {
					s.drawSelectionCursor(screen, 1, cx, cy)
				}
			}
		}
	}

	// Big portrait panels — P1 left, P2 right
	s.drawFacePanel(screen, s.p1CursorX, s.p1CursorY,
		si.P1FaceOffset, si.P1FaceScale, si.P1FaceFacing, si.P1FaceWindow,
		si.P1NameOffset, si.P1NameFont)
	if s.showP2 {
		s.drawFacePanel(screen, s.p2CursorX, s.p2CursorY,
			si.P2FaceOffset, si.P2FaceScale, si.P2FaceFacing, si.P2FaceWindow,
			si.P2NameOffset, si.P2NameFont)
	}

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
		s.engine.motif.SelectBG.DrawLayer(screen, 1, 0, 0, 0, s.engine.sprites)
	}
	if s.choosingStage {
		info := si.TitleFont
		if info[0] > 0 && info[0] < len(s.engine.motif.Fonts) && s.engine.motif.Fonts[info[0]] != nil {
			s.engine.motif.Fonts[info[0]].Draw(screen, "Stage: "+filepath.Base(s.stages[s.stageIndex]), s.engine.logicalWidth/2, s.engine.logicalHeight-12, info[1], font.AlignCenter)
		}
	}
}

// drawFacePanel draws the scaled big portrait and character name for one player.
// faceOff is the top-left corner to draw from; faceScale is the (sx, sy) multiplier.
// nameOff and nameFont are the position+font for the character name.
func (s *SelectState) drawFacePanel(screen *ebiten.Image, col, row int,
	faceOff [2]float64, faceScale [2]float64, facing int, window *[4]int,
	nameOff [2]int, nameFont [3]int) {

	ch := s.charAt(col, row)
	if ch == nil {
		return
	}

	pair := [2]int{9000, 1}
	if ch.BigPortrait == nil {
		pair[1] = 0
	}
	ch.drawPortrait(screen, pair, faceOff, faceScale, facing, s.engine.logicalWidth, window)

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

// Empty cells retain their authored indices, but are not automatic opponents.
func (s *SelectState) normalizeCursor(x, y *int) {
	*x, *y = max(0, min(*x, s.cols()-1)), max(0, min(*y, s.rows()-1))
	start := *y*s.cols() + *x
	for n := 0; n < s.rows()*s.cols(); n++ {
		index := (start + n) % (s.rows() * s.cols())
		cx, cy := index%s.cols(), index/s.cols()
		if s.charAt(cx, cy) != nil {
			*x, *y = cx, cy
			return
		}
	}
}

func (s *SelectState) moveCursor(x, y *int, dx, dy int) {
	oldX, oldY := *x, *y
	limit := s.cols()
	if dy != 0 {
		limit = s.rows()
	}
	for n := 0; n < limit; n++ {
		*x, *y = s.wrap(*x+dx, s.cols()), s.wrap(*y+dy, s.rows())
		if s.engine.motif.SelectInfo.MoveOverEmptyBoxes || s.charAt(*x, *y) != nil {
			return
		}
	}
	*x, *y = oldX, oldY
}

func (s *SelectState) drawSelectionCursor(screen *ebiten.Image, side int, x, y float64) {
	cursor, done := s.engine.motif.SelectInfo.P1Cursor, s.p1Done
	if side == 1 {
		cursor, done = s.engine.motif.SelectInfo.P2Cursor, s.p2Done
	}
	animation, sprite := cursor.ActiveAnimation, cursor.ActiveSprite
	if done {
		if cursor.DoneAnimation != nil {
			animation, sprite = cursor.DoneAnimation, nil
		} else if cursor.DoneSprite != nil {
			animation, sprite = nil, cursor.DoneSprite
		}
	}
	if animation != nil && s.engine.motif.SystemAIR != nil {
		action := s.engine.motif.SystemAIR.Actions[*animation]
		if action != nil {
			clock := s.cursorClocks[side]
			if clock == nil || clock.Action != action {
				clock = air.NewPlayer(action)
				s.cursorClocks[side] = clock
			}
			pose := clock.Visual()
			if pose.Element != nil {
				s.drawSelectionSprite(screen, [2]int{pose.Element.Group, pose.Element.Image}, x, y, 1, 1, &pose)
			}
		}
	} else if sprite != nil {
		s.drawSelectionSprite(screen, *sprite, x, y, 1, 1, nil)
	}
}

func (s *SelectState) drawSelectionSprite(screen *ebiten.Image, pair [2]int, x, y, sx, sy float64, pose *air.Visual) {
	archive := s.engine.motif.SpriteData
	if archive == nil || pair[0] < 0 || pair[1] < 0 {
		return
	}
	index, found := archive.Index(pair[0], pair[1])
	if !found {
		return
	}
	sprite := archive.Sprites[index]
	if sprite.Image == nil {
		return
	}
	if s.engine.sprites == nil {
		s.engine.sprites = map[string]*background.SpriteImages{}
	}
	key := fmt.Sprintf("%d-%d", pair[0], pair[1])
	cached := s.engine.sprites[key]
	if cached == nil {
		cached = s.spriteCache.Pair(sprite.Image, sprite.ImageOpaque)
		s.engine.sprites[key] = cached
	}
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Translate(-float64(sprite.XAxis), -float64(sprite.YAxis))
	destination := float32(-1)
	if pose != nil {
		if pose.Element.FlipX {
			op.GeoM.Scale(-1, 1)
		}
		if pose.Element.FlipY {
			op.GeoM.Scale(1, -1)
		}
		op.GeoM.Scale(pose.ScaleX, pose.ScaleY)
		op.GeoM.Rotate(-pose.Angle * math.Pi / 180)
		op.GeoM.Translate(pose.X, pose.Y)
		destination = render.ApplyBlend(op, pose.Element.Blend, pose.AlphaSrc, pose.AlphaDst)
	}
	op.GeoM.Scale(sx, sy)
	op.GeoM.Translate(x, y)
	if _, indexed := sprite.Image.(*image.Paletted); indexed {
		_ = s.indexSampler.DrawImage(screen, cached.Image(true), op, destination)
	} else {
		_ = s.linearSampler.DrawImage(screen, cached.Image(true), op, destination)
	}
}
