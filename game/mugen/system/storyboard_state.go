package main

import (
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/input"
	sb "github.com/kazzmir/paintown/game/mugen/storyboard"
)

// StoryboardState plays a storyboard .def file and then transitions to nextState.
type StoryboardState struct {
	engine     *Engine
	storyboard string // absolute path to the .def file
	nextState  func() State

	player    *sb.Player
	loaded    bool
	loadError error
}

func (s *StoryboardState) load() {
	s.loaded = true
	if s.storyboard == "" {
		return
	}
	story, err := sb.Load(s.storyboard, s.engine.dataDir)
	if err != nil {
		s.loadError = err
		return
	}
	s.player = sb.NewPlayer(story)
	s.loadError = s.player.Error
}

func (s *StoryboardState) Update() (State, error) {
	if !s.loaded {
		s.load()
	}

	if s.loadError != nil {
		return s, s.loadError
	}

	if s.player == nil {
		return s.nextState(), nil
	}

	skip := false
	if input.GlobalManager != nil {
		skip = input.GlobalManager.IsJustPressed(1, input.ActionEnter) ||
			input.GlobalManager.IsJustPressed(1, input.ActionEscape) ||
			input.GlobalManager.IsJustPressed(1, input.ActionSpace)
	}

	// Skip advances one scene; the final scene exits the storyboard.
	done := s.player.Update(sb.InputState{Skip: skip})
	if s.player.Error != nil {
		return s, s.player.Error
	}
	if done {
		return s.nextState(), nil
	}
	return s, nil
}

func (s *StoryboardState) Draw(screen *ebiten.Image) {
	screen.Fill(color.Black)
	if s.player != nil {
		s.player.Draw(screen)
	}
}
