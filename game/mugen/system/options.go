package main

import (
	"fmt"
	"image"
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
)

type optionItem struct {
	label               string
	getValue            func() int
	setValue            func(int)
	action              func() State
	choices             []string
	min, max, increment int
	format              func(int) string
	adjust              func(int)
}

func (o *optionItem) displayValue() string {
	if o.getValue == nil {
		return ""
	}
	v := o.getValue()
	if o.format != nil {
		return o.format(v)
	}
	if o.choices != nil && v-o.min >= 0 && v-o.min < len(o.choices) {
		return o.choices[v-o.min]
	}
	return fmt.Sprint(v)
}
func (o *optionItem) change(direction int) {
	if o.adjust != nil {
		o.adjust(direction)
		return
	}
	if o.getValue == nil || o.setValue == nil {
		return
	}
	step := o.increment
	if step == 0 {
		step = 1
	}
	o.setValue(max(o.min, min(o.max, o.getValue()+step*direction)))
}
func (o *optionItem) doIncrement() { o.change(1) }
func (o *optionItem) doDecrement() { o.change(-1) }

type OptionsState struct {
	repeat        menuRepeat
	engine        *Engine
	items         []*optionItem
	cursor, ticks int
	team          bool
	back          *OptionsState
	confirmation  bool
	yes           bool
	loadSave      int
	message       string
	messageTicks  int
	utility       utilityMenu
	dialog        utilityMenu
	background    *ebiten.Image
	transition    screenTransition
	volume        [3]int
}

func NewOptionsState(e *Engine) *OptionsState {
	if e.initialOptions == nil {
		e.initialOptions = e.snapshotOptions()
	}
	if e.sessionOptions == nil {
		e.sessionOptions = e.snapshotOptions()
	}
	s := &OptionsState{engine: e}
	s.buildItems()
	return s
}
func (s *OptionsState) buildItems() {
	e, c := s.engine, s.engine.cfg
	percent := func(v int) string { return fmt.Sprintf("%d%%", v) }
	if s.team {
		s.items = []*optionItem{
			{label: "Single VS Simul Advantage", getValue: func() int { return c.Options.Team1Vs2Life }, setValue: func(v int) { c.Options.Team1Vs2Life = v }, min: 100, max: 300, increment: 10, format: percent},
			{label: "If player KOed", getValue: func() int {
				if c.Options.TeamLoseOnKO {
					return 0
				}
				return 1
			}, setValue: func(v int) { c.Options.TeamLoseOnKO = v == 0 }, min: 0, max: 1, choices: []string{"Team loses", "AI keeps fighting"}},
			{label: "Default Values", action: func() State { s.confirmation = true; s.yes = false; return s }},
			{label: "Return to Options", action: func() State { return s.back }},
		}
		return
	}
	s.volume = [3]int{c.Sound.MasterVolume, c.Sound.WavVolume, c.Sound.BGMVolume}
	for i, v := range s.volume {
		if v == 0 {
			s.volume[i] = -1
		} else if v == 100 {
			s.volume[i] = 101
		}
	}
	s.items = []*optionItem{
		{label: "Difficulty", getValue: func() int { return c.Options.Difficulty }, setValue: func(v int) { c.Options.Difficulty = v }, min: 1, max: 8, format: func(v int) string {
			level := "Medium"
			if v < 3 {
				level = "Easy"
			} else if v > 5 {
				level = "Hard"
			}
			return fmt.Sprintf("%s %d", level, v)
		}},
		{label: "Life", getValue: func() int { return c.Options.Life }, setValue: func(v int) { c.Options.Life = v }, min: 30, max: 300, increment: 10, format: percent},
		{label: "Time Limit", getValue: func() int { return c.Options.Time }, format: func(v int) string {
			if v == -1 {
				return "None"
			}
			return fmt.Sprint(v)
		}, adjust: func(d int) {
			v := c.Options.Time
			if d < 0 {
				if v == -1 {
					v = 99
				} else if v == 99 {
					v = 80
				} else if v > 20 {
					v -= 20
				}
			} else {
				if v >= 80 && v < 99 {
					v = 99
				} else if v == 99 {
					v = -1
				} else if v < 80 && v != -1 {
					v += 20
				}
			}
			c.Options.Time = v
		}},
		{label: "Game Speed", getValue: func() int { return c.Options.GameSpeed }, setValue: func(v int) { c.Options.GameSpeed = v; e.applyScheduler() }, min: -9, max: 9, format: func(v int) string {
			if v == 0 {
				return "Normal"
			}
			if v < 0 {
				return fmt.Sprintf("Slow %d", -v)
			}
			return fmt.Sprintf("Fast %d", v)
		}},
	}
	for i, label := range []string{"Master Volume", "Wav/SFX Volume", "BGM Volume"} {
		s.items = append(s.items, &optionItem{label: label, getValue: func() int { return s.volume[i] }, setValue: func(v int) {
			s.volume[i] = v
			actual := max(0, min(100, v))
			switch i {
			case 0:
				c.Sound.MasterVolume = actual
			case 1:
				c.Sound.WavVolume = actual
				c.Options.WavVolume = actual
			case 2:
				c.Sound.BGMVolume = actual
			}
		}, min: -1, max: 101, format: func(v int) string {
			if v == -1 {
				return "Off"
			}
			if v == 101 {
				return "Max"
			}
			return fmt.Sprint(v)
		}})
	}
	s.items = append(s.items,
		&optionItem{label: "Input Configuration", action: func() State { return NewInputConfigState(e, s) }},
		&optionItem{label: "Team Mode Config", action: func() State { child := &OptionsState{engine: e, team: true, back: s}; child.buildItems(); return child }},
		&optionItem{label: "Session Load/Save", getValue: func() int { return s.loadSave }, setValue: func(v int) { s.loadSave = v }, min: 0, max: 1, choices: []string{"Load", "Save"}, action: func() State {
			if s.loadSave == 0 {
				e.restoreOptions(e.sessionOptions)
				s.buildItems()
				s.message = "Session options loaded"
			} else {
				e.sessionOptions = e.snapshotOptions()
				s.message = "Session options saved"
			}
			s.messageTicks = 90
			return s
		}},
		&optionItem{label: "Default Values", action: func() State { s.confirmation = true; s.yes = false; return s }},
		&optionItem{label: "Return to Main Menu", action: func() State { return NewTitleState(e) }},
	)
}
func (s *OptionsState) defaults() {
	c := s.engine.cfg
	if s.team {
		c.Options.Team1Vs2Life, c.Options.TeamLoseOnKO = 200, true
	} else {
		c.Options.Difficulty, c.Options.Life, c.Options.Time, c.Options.GameSpeed = 4, 100, 99, 0
		c.Sound.MasterVolume, c.Sound.WavVolume, c.Sound.BGMVolume = 100, 80, 75
		c.Options.WavVolume = 80
		s.engine.applyScheduler()
	}
	s.buildItems()
}
func (s *OptionsState) Update() (State, error) {
	s.ticks++
	if s.messageTicks > 0 {
		s.messageTicks--
	}
	e, im := s.engine, input.GlobalManager
	if e.motif.OptionBG != nil {
		e.motif.OptionBG.Update()
	}
	if s.transition.leaving {
		if s.transition.update(s.ticks, -1, e.themeNumber("Option Info", "fadeout.time", 0), false) {
			return NewTitleState(e), nil
		}
		return s, nil
	}
	direction := s.repeat.directions(im, e.ticksPerSecond())
	left, right := direction[2], direction[3]
	accept := im.IsJustPressed(1, input.ActionEnter)
	cancel := im.IsJustPressed(1, input.ActionEscape)
	if s.confirmation {
		if left || right {
			s.yes = !s.yes
		}
		if accept {
			if s.yes {
				s.defaults()
			}
			s.confirmation = false
		}
		if cancel {
			s.confirmation = false
		}
		return s, nil
	}
	if im.KeyJustPressed(ebiten.KeyF1) {
		if !s.team {
			s.cursor = 7
		}
		return NewInputConfigState(e, s), nil
	}
	if im.KeyJustPressed(ebiten.KeyF2) && !s.team {
		s.cursor = 8
		child := &OptionsState{engine: e, team: true, back: s}
		child.buildItems()
		return child, nil
	}
	if direction[0] {
		s.cursor = (s.cursor + len(s.items) - 1) % len(s.items)
		e.PlaySnd(e.motif.OptionInfo.CursorMoveSnd[0], e.motif.OptionInfo.CursorMoveSnd[1])
	}
	if direction[1] {
		s.cursor = (s.cursor + 1) % len(s.items)
		e.PlaySnd(e.motif.OptionInfo.CursorMoveSnd[0], e.motif.OptionInfo.CursorMoveSnd[1])
	}
	if left {
		s.items[s.cursor].doDecrement()
	}
	if right {
		s.items[s.cursor].doIncrement()
	}
	if left || right {
		e.PlaySnd(e.motif.OptionInfo.CursorMoveSnd[0], e.motif.OptionInfo.CursorMoveSnd[1])
	}
	exit := cancel
	if accept {
		e.PlaySnd(e.motif.OptionInfo.CursorDoneSnd[0], e.motif.OptionInfo.CursorDoneSnd[1])
		if s.cursor == len(s.items)-1 {
			exit = true
		} else if action := s.items[s.cursor].action; action != nil {
			return action(), nil
		} else {
			s.items[s.cursor].doIncrement()
		}
	}
	if exit {
		e.PlaySnd(e.motif.OptionInfo.CancelSnd[0], e.motif.OptionInfo.CancelSnd[1])
		if s.team {
			return s.back, nil
		}
		if s.transition.update(s.ticks, -1, e.themeNumber("Option Info", "fadeout.time", 0), true) {
			return NewTitleState(e), nil
		}
	}
	return s, nil
}
func (s *OptionsState) Draw(screen *ebiten.Image) {
	e := s.engine
	if s.background == nil {
		s.background = ebiten.NewImage(e.logicalWidth, e.logicalHeight)
	}
	heading := ""
	if s.team {
		heading = "TEAM MODE CONFIG"
	}
	content := e.drawOptionsBackground(screen, s.background, heading)
	rows := make([]utilityRow, 0, len(s.items)+2)
	for i, item := range s.items {
		if !s.team && (i == 9 || i == 11) {
			rows = append(rows, utilityRow{})
		}
		value := item.displayValue()
		if item.action != nil && item.getValue == nil {
			value = "Enter"
		}
		rows = append(rows, utilityRow{item.label, value})
	}
	cursor := s.cursor
	if !s.team {
		if cursor >= 9 {
			cursor++
		}
		if s.cursor >= 11 {
			cursor++
		}
	}
	if s.messageTicks > 0 {
		rows = append(rows, utilityRow{s.message, ""})
	}
	s.utility.draw(e, screen, rows, cursor, "", content.Min.Y-screen.Bounds().Min.Y)
	if s.confirmation {
		rows = []utilityRow{{"Reset default values?", "No"}}
		if s.yes {
			rows[0].value = "Yes"
		}
		s.dialog.draw(e, screen, rows, 0, "", 100)
	}
	s.transition.draw(screen, s.ticks, e.themeNumber("Option Info", "fadein.time", 0), e.themeNumber("Option Info", "fadeout.time", 0))
}

// All Options pages retain the active motif's background, title styling and
// fitted viewport. Utility lettering is composed afterward in window pixels.
func (e *Engine) drawOptionsBackground(screen, background *ebiten.Image, heading string) image.Rectangle {
	if e.motif.OptionBG != nil && e.motif.OptionBG.BGClearColor != nil {
		background.Fill(e.motif.OptionBG.BGClearColor)
	} else {
		background.Fill(color.Black)
	}
	if e.motif.OptionBG != nil {
		e.motif.OptionBG.DrawLayer(background, 0, 0, 0, 0, e.sprites)
		e.motif.OptionBG.DrawLayer(background, 1, 0, 0, 0, e.sprites)
	}
	fi := e.motif.TitleInfo.ItemFont
	title := e.themeValues("Option Info", "title.font", float64(fi.Index), float64(fi.Bank), 0)
	pos := e.themeValues("Option Info", "title.offset", float64(e.logicalWidth)/2, 36*float64(e.logicalHeight)/240)
	if heading == "" {
		heading = e.themeText("Option Info", "title.text", "OPTIONS")
	}
	if index := int(title[0]); index >= 1 && index < len(e.motif.Fonts) && e.motif.Fonts[index] != nil {
		align := font.AlignCenter
		if title[2] > 0 {
			align = font.AlignLeft
		} else if title[2] < 0 {
			align = font.AlignRight
		}
		e.drawMotifFont(background, e.motif.Fonts[index], heading, int(pos[0]), int(pos[1]), int(title[1]), align, "Option Info", "title.font")
	}
	screen.Fill(color.Black)
	scale, x, y, _ := coord.Fit(coord.Space{Width: e.logicalWidth, Height: e.logicalHeight}, coord.Space{Width: screen.Bounds().Dx(), Height: screen.Bounds().Dy()})
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Scale(scale, scale)
	op.GeoM.Translate(x+float64(screen.Bounds().Min.X), y+float64(screen.Bounds().Min.Y))
	screen.DrawImage(background, op)
	return image.Rect(int(x), int(y+50*scale), int(x+float64(e.logicalWidth)*scale), int(y+float64(e.logicalHeight)*scale)).Add(screen.Bounds().Min)
}
