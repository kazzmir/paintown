package main

import (
	"fmt"
	"image"
	"image/color"
	"os"
	"path/filepath"
	"sort"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

func (e *Engine) themeAttribute(section, key string) *parsers.Attribute {
	if e.motif == nil || e.motif.AST == nil {
		return nil
	}
	for _, s := range e.motif.AST.Sections {
		if !strings.EqualFold(s.Name, section) {
			continue
		}
		for _, a := range s.Attributes {
			if strings.EqualFold(a.ID.String(), key) {
				return a
			}
		}
	}
	return nil
}

func (e *Engine) themeNumber(section, key string, fallback int) int {
	if a := e.themeAttribute(section, key); a != nil && strings.TrimSpace(a.RawValue) != "" {
		return int(background.GetFloat(a.Value))
	}
	return fallback
}

func (e *Engine) themeText(section, key, fallback string) string {
	if a := e.themeAttribute(section, key); a != nil {
		return background.GetString(a.Value)
	}
	return fallback
}

func (e *Engine) themeValues(section, key string, fallback ...float64) []float64 {
	values := append([]float64(nil), fallback...)
	if a := e.themeAttribute(section, key); a != nil {
		for i, part := range strings.Split(a.RawValue, ",") {
			if i >= len(values) {
				break
			}
			var n float64
			if _, err := fmt.Sscanf(strings.TrimSpace(part), "%f", &n); err == nil {
				values[i] = n
			}
		}
	}
	return values
}

func (e *Engine) modeLabel(screen *ebiten.Image, section, key, text string, x, y int, fonts []*font.Font) {
	info := e.themeValues(section, key+".font", 1, 0, 0, 256, 256, 256)
	index := int(info[0])
	if index < 1 || index >= len(fonts) || fonts[index] == nil {
		return
	}
	offset := e.themeValues(section, key+".offset", 0, 0)
	align := font.AlignCenter
	if info[2] > 0 {
		align = font.AlignLeft
	} else if info[2] < 0 {
		align = font.AlignRight
	}
	_ = fonts[index].DrawWithColor(screen, text, x+int(offset[0]), y+int(offset[1]), int(info[1]), align, [3]float32{float32(info[3]), float32(info[4]), float32(info[5])}, palette.Transform{})
}

type WinScreenState struct {
	engine     *Engine
	winner     *character.Player
	quote      string
	quoteSet   bool
	ticks      int
	next       func() (State, error)
	transition screenTransition
}

func NewWinScreenState(e *Engine) *WinScreenState { return &WinScreenState{engine: e} }
func (s *WinScreenState) Update() (State, error) {
	s.ticks++
	if s.engine.motif.VictoryBG != nil {
		s.engine.motif.VictoryBG.Update()
	}
	dismiss := input.GlobalManager.IsJustPressed(1, input.ActionEnter) || input.GlobalManager.IsJustPressed(1, input.ActionEscape)
	if s.transition.update(s.ticks, s.engine.themeNumber("Victory Screen", "time", 300), s.engine.themeNumber("Victory Screen", "fadeout.time", 0), dismiss) {
		if s.next != nil {
			return s.next()
		}
		return NewTitleState(s.engine), nil
	}
	return s, nil
}
func (s *WinScreenState) Draw(screen *ebiten.Image) {
	screen.Fill(color.Black)
	if bg := s.engine.motif.VictoryBG; bg != nil {
		if bg.BGClearColor != nil {
			screen.Fill(bg.BGClearColor)
		}
		bg.DrawLayer(screen, 0, 0, 0, 0, s.engine.sprites)
	}
	name := "Winner"
	if p := s.winner; p != nil {
		name = p.Character.Name
		pos := s.engine.themeValues("Victory Screen", "p1.offset", 400, -40)
		ids := s.engine.themeValues("Victory Screen", "p1.spr", 9000, 2)
		scale := s.engine.themeValues("Victory Screen", "p1.scale", 1, 1)
		if findSprite(p.SFF, int(ids[0]), int(ids[1])) == nil {
			ids = []float64{9000, 1}
		}
		ratio := float64(screen.Bounds().Dx()) / float64(p.LocalCoord.Width)
		target := screen
		if s.engine.themeAttribute("Victory Screen", "p1.window") != nil {
			box := s.engine.themeValues("Victory Screen", "p1.window", 0, 0, float64(screen.Bounds().Max.X-1), float64(screen.Bounds().Max.Y-1))
			bounds := image.Rect(int(box[0]), int(box[1]), int(box[2])+1, int(box[3])+1).Intersect(screen.Bounds())
			if bounds.Empty() {
				target = nil
			} else {
				target = screen.SubImage(bounds).(*ebiten.Image)
			}
		}
		if target != nil {
			// Clip in motif coordinates after deriving the scale from the full
			// canvas. A narrow portrait window must not shrink/recenter the art.
			p.DrawSprite(target, p.Character, int(ids[0]), int(ids[1]), pos[0], pos[1], scale[0]*ratio*s.engine.themeValues("Victory Screen", "p1.facing", 1)[0], scale[1]*ratio)
		}
	}
	s.engine.modeLabel(screen, "Victory Screen", "p1.name", name, 0, 0, s.engine.motif.Fonts)
	if !s.quoteSet {
		s.quote = s.victoryQuote()
		s.quoteSet = true
	}
	s.drawQuote(screen)
	if bg := s.engine.motif.VictoryBG; bg != nil {
		bg.DrawLayer(screen, 1, 0, 0, 0, s.engine.sprites)
	}
	s.transition.draw(screen, s.ticks, s.engine.themeNumber("Victory Screen", "fadein.time", 0), s.engine.themeNumber("Victory Screen", "fadeout.time", 0))
}

func (s *WinScreenState) drawQuote(screen *ebiten.Image) {
	e := s.engine
	spec := e.themeValues("Victory Screen", "winquote.font", 1, 0, 1)
	index := int(spec[0])
	if index < 1 || index >= len(e.motif.Fonts) || e.motif.Fonts[index] == nil {
		return
	}
	f := e.motif.Fonts[index]
	bank := int(spec[1])
	box := e.themeValues("Victory Screen", "winquote.window", 0, 0, float64(screen.Bounds().Dx()-1), float64(screen.Bounds().Dy()-1))
	rect := image.Rect(int(box[0]), int(box[1]), int(box[2])+1, int(box[3])+1).Intersect(screen.Bounds())
	if rect.Empty() {
		return
	}
	target := screen.SubImage(rect).(*ebiten.Image)
	text := s.quote
	if strings.EqualFold(e.themeText("Victory Screen", "winquote.textwrap", ""), "w") {
		// The quote anchor can sit well inside its clipping window (the HD
		// motif starts at x200 in a window beginning at x38). Wrap against
		// the remaining space in its alignment direction, not the full box.
		anchor := int(e.themeValues("Victory Screen", "winquote.offset", 0, 0)[0]) + f.OffsetX
		width := rect.Dx()
		switch {
		case spec[2] > 0:
			width = rect.Max.X - max(rect.Min.X, anchor)
		case spec[2] < 0:
			width = min(rect.Max.X, anchor) - rect.Min.X
		default:
			width = 2 * min(anchor-rect.Min.X, rect.Max.X-anchor)
		}
		if width <= 0 {
			return
		}
		var lines []string
		for _, paragraph := range strings.Split(text, "\n") {
			line := ""
			for _, word := range strings.Fields(paragraph) {
				candidate := word
				if line != "" {
					candidate = line + " " + word
				}
				if f.TextWidthForBank(candidate, bank) <= width {
					line = candidate
					continue
				}
				if line != "" {
					lines = append(lines, line)
					line = ""
				}
				for _, r := range word {
					candidate = line + string(r)
					if line != "" && f.TextWidthForBank(candidate, bank) > width {
						lines = append(lines, line)
						line = ""
					}
					line += string(r)
				}
			}
			lines = append(lines, line)
		}
		text = strings.Join(lines, "\n")
	}
	for line, value := range strings.Split(text, "\n") {
		e.modeLabel(target, "Victory Screen", "winquote", value, 0, line*(f.Height+f.SpacingY), e.motif.Fonts)
	}
}

func (s *WinScreenState) victoryQuote() string {
	fallback := s.engine.themeText("Victory Screen", "winquote.text", "Winner!")
	if s.winner == nil || s.winner.Character.StateFile == nil {
		return fallback
	}
	c := s.winner.Character
	quotes := c.StateFile.Quotes
	language := s.engine.cfg.Config.Language
	if language == "" {
		language = os.Getenv("LC_ALL")
		if language == "" {
			language = os.Getenv("LANG")
		}
		language = strings.ToLower(strings.SplitN(strings.SplitN(language, "_", 2)[0], ".", 2)[0])
	}
	lookup := func(index int) string {
		if text := quotes[language][index]; text != "" {
			return text
		}
		return quotes["en"][index]
	}
	if c.VictoryQuoteIndex >= 0 {
		if text := lookup(c.VictoryQuoteIndex); text != "" {
			return text
		}
	}
	keys := map[int]bool{}
	for index := range quotes["en"] {
		keys[index] = true
	}
	for index := range quotes[language] {
		keys[index] = true
	}
	indexes := make([]int, 0, len(keys))
	for index := range keys {
		indexes = append(indexes, index)
	}
	sort.Ints(indexes)
	if len(indexes) > 0 {
		return lookup(indexes[max(0, c.Time)%len(indexes)])
	}
	return fallback
}

type ContinueScreenState struct {
	engine *Engine
	retry  match.Request
	ticks  int
	no     bool
}

func NewContinueScreenState(e *Engine) *ContinueScreenState { return &ContinueScreenState{engine: e} }
func (s *ContinueScreenState) Update() (State, error) {
	s.ticks++
	if input.GlobalManager.IsJustPressed(1, input.ActionLeftUI) || input.GlobalManager.IsJustPressed(1, input.ActionRightUI) {
		s.no = !s.no
	}
	if input.GlobalManager.IsJustPressed(1, input.ActionEscape) || s.ticks >= s.engine.themeNumber("Continue Screen", "time", 600) {
		return NewGameOverScreenState(s.engine), nil
	}
	if input.GlobalManager.IsJustPressed(1, input.ActionEnter) {
		if s.no || s.retry.Campaign == 0 {
			return NewGameOverScreenState(s.engine), nil
		}
		return NewVersusState(s.engine, s.retry), nil
	}
	return s, nil
}
func (s *ContinueScreenState) Draw(screen *ebiten.Image) {
	screen.Fill(color.Black)
	pos := s.engine.themeValues("Continue Screen", "pos", float64(screen.Bounds().Dx()/2), float64(screen.Bounds().Dy()/3))
	fonts := s.engine.motif.Fonts
	// The beta's supplied motif explicitly uses fight fonts here.
	if s.engine.modeRun != nil && s.engine.modeRun.lastFight != nil {
		fonts = s.engine.modeRun.lastFight.match.Fight.Fonts[:]
	}
	s.engine.modeLabel(screen, "Continue Screen", "continue", s.engine.themeText("Continue Screen", "continue.text", "CONTINUE?"), int(pos[0]), int(pos[1]), fonts)
	yes, no := "yes", "no"
	if s.no {
		no = "no.active"
	} else {
		yes = "yes.active"
	}
	s.engine.modeLabel(screen, "Continue Screen", yes, s.engine.themeText("Continue Screen", yes+".text", s.engine.themeText("Continue Screen", "yes.text", "YES")), int(pos[0]), int(pos[1]), fonts)
	s.engine.modeLabel(screen, "Continue Screen", no, s.engine.themeText("Continue Screen", no+".text", s.engine.themeText("Continue Screen", "no.text", "NO")), int(pos[0]), int(pos[1]), fonts)
}

type GameOverScreenState struct {
	engine       *Engine
	ticks, score int
	survival     bool
	fight        *FightState
	transition   screenTransition
}

func NewGameOverScreenState(e *Engine) *GameOverScreenState { return &GameOverScreenState{engine: e} }
func (s *GameOverScreenState) Update() (State, error) {
	s.ticks++
	if s.survival && s.engine.themeNumber("Survival Results Screen", "enabled", 1) == 0 {
		return NewTitleState(s.engine), nil
	}
	if !s.survival && s.engine.themeNumber("Game Over Screen", "enabled", 0) == 0 {
		return NewTitleState(s.engine), nil
	}
	if !s.survival && s.ticks == 1 && s.engine.themeText("Game Over Screen", "storyboard", "") != "" {
		return s.engine.configuredStory("Game Over Screen", NewTitleState(s.engine))
	}
	section := "Game Over Screen"
	if s.survival {
		section = "Survival Results Screen"
	}
	dismiss := input.GlobalManager.IsJustPressed(1, input.ActionEnter) || input.GlobalManager.IsJustPressed(1, input.ActionEscape)
	if s.transition.update(s.ticks, s.engine.themeNumber(section, "show.time", 300), s.engine.themeNumber(section, "fadeout.time", 0), dismiss) {
		return NewTitleState(s.engine), nil
	}
	return s, nil
}
func (s *GameOverScreenState) Draw(screen *ebiten.Image) {
	screen.Fill(color.Black)
	text := "Game Over"
	if s.survival {
		if s.fight != nil {
			s.fight.Draw(screen)
		}
		text = strings.ReplaceAll(s.engine.themeText("Survival Results Screen", "winstext.text", "Rounds survived: %i"), "%i", fmt.Sprint(s.score))
		if s.engine.resultTextVisible("Survival Results Screen", "winstext", s.ticks) {
			s.engine.modeLabel(screen, "Survival Results Screen", "winstext", text, 0, 0, s.engine.motif.Fonts)
		}
		s.transition.draw(screen, s.ticks, s.engine.themeNumber("Survival Results Screen", "fadein.time", 0), s.engine.themeNumber("Survival Results Screen", "fadeout.time", 0))
		return
	}
	s.engine.modeLabel(screen, "Continue Screen", "continue", text, screen.Bounds().Dx()/2, screen.Bounds().Dy()/2, s.engine.motif.Fonts)
}

func (e *Engine) configuredStory(section string, next State) (State, error) {
	name := e.themeText(section, "storyboard", "")
	if e.themeNumber(section, "enabled", 0) == 0 || name == "" {
		return next, nil
	}
	return &StoryboardState{engine: e, storyboard: name, sourceDir: filepath.Dir(e.motif.Source), nextState: func() State { return next }}, nil
}

type completionScreen struct {
	engine     *Engine
	fight      *FightState
	ticks      int
	next       State
	transition screenTransition
}

func (s *completionScreen) Update() (State, error) {
	s.ticks++
	dismiss := input.GlobalManager.IsJustPressed(1, input.ActionEnter) || input.GlobalManager.IsJustPressed(1, input.ActionEscape)
	if s.transition.update(s.ticks, s.engine.themeNumber("Win Screen", "pose.time", 300), s.engine.themeNumber("Win Screen", "fadeout.time", 0), dismiss) {
		return s.next, nil
	}
	return s, nil
}
func (s *completionScreen) Draw(screen *ebiten.Image) {
	if s.fight != nil {
		s.fight.Draw(screen)
	} else {
		screen.Fill(color.Black)
	}
	if s.engine.resultTextVisible("Win Screen", "wintext", s.ticks) {
		s.engine.modeLabel(screen, "Win Screen", "wintext", s.engine.themeText("Win Screen", "wintext.text", "Congratulations!"), 0, 0, s.engine.motif.Fonts)
	}
	s.transition.draw(screen, s.ticks, s.engine.themeNumber("Win Screen", "fadein.time", 0), s.engine.themeNumber("Win Screen", "fadeout.time", 0))
}

func (e *Engine) resultTextVisible(section, key string, tick int) bool {
	duration := e.themeNumber(section, key+".displaytime", -1)
	return duration < 0 || tick < duration
}
