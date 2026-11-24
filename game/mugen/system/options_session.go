package main

import (
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
)

// Load/Save is deliberately session-only. These value snapshots include live
// input edits without owning a device, rewriting files or sharing mutable lists.
type optionSnapshot struct {
	difficulty, life, time, speed, master, wav, bgm, teamLife int
	teamLose                                                  bool
	keys                                                      [2]config.Keys
	pads                                                      [2]config.GamepadConfig
	keyboardDisabled                                          [2]bool
}

func (e *Engine) snapshotOptions() *optionSnapshot {
	c := e.cfg
	s := &optionSnapshot{difficulty: c.Options.Difficulty, life: c.Options.Life, time: c.Options.Time, speed: c.Options.GameSpeed,
		master: c.Sound.MasterVolume, wav: c.Sound.WavVolume, bgm: c.Sound.BGMVolume, teamLife: c.Options.Team1Vs2Life, teamLose: c.Options.TeamLoseOnKO,
		keys: [2]config.Keys{c.Input.Player1Keyboard, c.Input.Player2Keyboard}, pads: c.Input.Gamepads, keyboardDisabled: c.Input.KeyboardDisabled}
	if im := input.GlobalManager; im != nil {
		s.keys = [2]config.Keys{im.P1Keyboard, im.P2Keyboard}
		s.pads = [2]config.GamepadConfig{im.P1Gamepad, im.P2Gamepad}
		s.keyboardDisabled = im.KeyboardDisabled
	}
	return s
}

func (e *Engine) restoreOptions(s *optionSnapshot) {
	c := e.cfg
	c.Options.Difficulty, c.Options.Life, c.Options.Time, c.Options.GameSpeed = s.difficulty, s.life, s.time, s.speed
	c.Sound.MasterVolume, c.Sound.WavVolume, c.Sound.BGMVolume = s.master, s.wav, s.bgm
	c.Options.WavVolume = s.wav
	c.Options.Team1Vs2Life, c.Options.TeamLoseOnKO = s.teamLife, s.teamLose
	c.Input.Player1Keyboard, c.Input.Player2Keyboard = s.keys[0], s.keys[1]
	c.Input.Gamepads, c.Input.KeyboardDisabled = s.pads, s.keyboardDisabled
	if im := input.GlobalManager; im != nil {
		im.P1Keyboard, im.P2Keyboard = s.keys[0], s.keys[1]
		im.P1Gamepad, im.P2Gamepad = s.pads[0], s.pads[1]
		im.KeyboardDisabled = s.keyboardDisabled
	}
	e.applyScheduler()
}
