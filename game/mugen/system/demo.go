package main

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
)

// demoRun owns an attract sequence without changing the player's configured
// controls, debug mode, sound volumes or normal match rules.
type demoRun struct {
	request    match.Request
	versus     bool
	fightTicks int
}

func demoInputPressed() bool {
	if input.GlobalManager == nil {
		return false
	}
	for slot := 1; slot <= 2; slot++ {
		for action := input.ActionJump; action <= input.ActionSpace; action++ {
			if input.GlobalManager.IsJustPressed(slot, action) {
				return true
			}
		}
	}
	return false
}

func (e *Engine) beginDemo() (State, error) {
	selection := NewSelectState(e, "Watch Mode")
	if selection.loadError != nil {
		return nil, selection.loadError
	}
	var available []int
	for index, ch := range selection.chars {
		if ch.DefPath != "" {
			available = append(available, index)
		}
	}
	if len(available) == 0 || len(selection.stages) == 0 {
		return nil, fmt.Errorf("demo selection has no playable characters or stages")
	}
	random := engine.NewRandomStream(e.nextMatchSeed())
	request := match.Request{Mode: "Demo"}
	for side := range request.Players {
		index := available[int(random.Next()%int64(len(available)))]
		ch := selection.chars[index]
		selected := match.Selection{DefPath: ch.DefPath, Name: ch.Name, Palette: side + 1, AILevel: max(1, min(8, e.cfg.Options.Difficulty))}
		request.Players[side] = selected
		request.Teams[side] = match.TeamSelection{Mode: match.Single, Members: []match.Selection{selected}}
		if side == 0 {
			selection.p1CursorX, selection.p1CursorY = index%selection.cols(), index/selection.cols()
		} else {
			selection.p2CursorX, selection.p2CursorY = index%selection.cols(), index/selection.cols()
		}
		selection.pickMember(side, ch, selected.Palette)
	}
	selection.stageIndex = int(random.Next() % int64(len(selection.stages)))
	request.StagePath = selection.stages[selection.stageIndex]
	selection.teamMenu = false
	selection.p1Done, selection.p2Done = true, true
	e.demo = &demoRun{request: request, versus: e.themeNumber("Demo Mode", "vsscreen.enabled", 0) != 0}
	if e.themeNumber("Demo Mode", "select.enabled", 0) != 0 {
		return selection, nil
	}
	return e.nextDemoScreen()
}

func (e *Engine) nextDemoScreen() (State, error) {
	if e.demo.versus {
		e.demo.versus = false
		return NewVersusState(e, e.demo.request), nil
	}
	return NewFightState(e, e.demo.request)
}

func (e *Engine) endDemo(completed bool) (State, error) {
	if fight, ok := e.currentState.(*FightState); ok {
		if err := fight.match.ClearSounds(); err != nil {
			return fight, err
		}
		fight.match.Effects.Clear()
	}
	e.demo = nil
	title := NewTitleState(e)
	if !completed {
		e.demoCycles = 0
		return title, nil
	}
	e.demoCycles++
	cycles := e.themeNumber("Demo Mode", "intro.waitcycles", 1)
	if cycles > 0 && e.demoCycles >= cycles && e.motif.Files.IntroStoryboard != "" {
		e.demoCycles = 0
		return &StoryboardState{engine: e, storyboard: e.motif.Files.IntroStoryboard, nextState: func() State { return title }}, nil
	}
	return title, nil
}

func (e *Engine) demoShowsBars() bool {
	return e.demo == nil || e.themeNumber("Demo Mode", "fight.bars.display", 0) != 0
}
func (e *Engine) demoShowsDebug() bool {
	return e.demo == nil || e.themeNumber("Demo Mode", "debuginfo", 0) != 0
}
