package main

import (
	"bytes"
	"path/filepath"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/motif"
)

type flowKeyboard struct{ keys map[ebiten.Key]bool }

func (k *flowKeyboard) IsPressed(key ebiten.Key) bool     { return k.keys[key] }
func (k *flowKeyboard) IsJustPressed(key ebiten.Key) bool { return k.keys[key] }

func TestTitleSelectionVersusFightResultRematchAndExit(t *testing.T) {
	root, err := filepath.Abs("../temp/mugen-1.1b1")
	if err != nil {
		t.Fatal(err)
	}
	cfg, err := config.LoadConfigWithOverrides(root)
	if err != nil {
		t.Fatal(err)
	}
	m, err := motif.LoadMotif("data/mugen1/system.def", root)
	if err != nil {
		t.Fatal(err)
	}
	previous := input.GlobalManager
	t.Cleanup(func() { input.GlobalManager = previous })
	input.GlobalManager = input.NewInputManager(cfg)
	keyboard := &flowKeyboard{}
	input.GlobalManager.Keyboard = keyboard
	e := &Engine{dataDir: root, cfg: cfg, motif: m, logicalWidth: m.Info.LocalCoord.Width, logicalHeight: m.Info.LocalCoord.Height}
	t.Cleanup(func() {
		if err := e.closeSound(); err != nil {
			t.Error(err)
		}
	})
	e.currentState = NewTitleState(e)
	audible := false
	var ticks int32
	step := func(keys ...ebiten.Key) {
		t.Helper()
		keyboard.keys = map[ebiten.Key]bool{}
		for _, key := range keys {
			keyboard.keys[key] = true
		}
		previousFight, inFight := e.currentState.(*FightState)
		var roots [2]*character.Character
		var previousStates [2]int
		if inFight {
			for slot, player := range previousFight.match.Players {
				roots[slot] = player.Character
				previousStates[slot] = player.Character.StateNo
			}
		}
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
		if inFight && e.currentState == previousFight && !previousFight.match.SimulationSkipped {
			for slot, root := range roots {
				// Stock 5900 clears user variables after the negative pass.
				if previousStates[slot] == 5900 {
					continue
				}
				if root.IntVars[59] != int(ticks) {
					t.Fatal("CNS GameTime lost the process tick", root.IntVars[59], ticks)
				}
			}
		}
		ticks++
		if e.gameTime != ticks {
			t.Fatal("process clock skipped or double-counted an update", e.gameTime, ticks)
		}
		if fight, ok := e.currentState.(*FightState); ok && fight.match.Timeline.GameTime != ticks {
			t.Fatal("match handoff lost the next process tick", fight.match.Timeline.GameTime, ticks)
		}
		pcm := make([]byte, 735*4)
		if _, err := e.sound.mixer.Read(pcm); err != nil {
			t.Fatal(err)
		}
		audible = audible || !bytes.Equal(pcm, make([]byte, len(pcm)))
	}
	for i := 0; i < 37; i++ {
		step()
	}
	step(ebiten.KeyArrowDown)
	step(ebiten.KeyEnter)
	s, ok := e.currentState.(*SelectState)
	if !ok {
		t.Fatalf("title did not open selection: %T", e.currentState)
	}
	// Start both on the first stock cell, then move P2 with the stock D key.
	s.p1CursorX, s.p1CursorY, s.p2CursorX, s.p2CursorY = 0, 0, 0, 0
	step(ebiten.KeyD)
	step(ebiten.KeyComma)
	if s.p2Done {
		t.Fatal("P1 confirmation also confirmed P2")
	}
	step(ebiten.KeyG)
	if !s.choosingStage {
		t.Fatal("confirmed players did not reach stage selection")
	}
	step(ebiten.KeyArrowRight)
	step(ebiten.KeyEnter)
	v, ok := e.currentState.(*VersusState)
	if !ok {
		t.Fatalf("stage selection did not open versus: %T", e.currentState)
	}
	if !strings.HasSuffix(v.request.Players[0].DefPath, "kfm/kfm.def") || !strings.HasSuffix(v.request.Players[1].DefPath, "kfm720/kfm720.def") || v.request.Players[1].Palette != 2 || v.request.StagePath != "stages/stage0-720.def" {
		t.Fatal("selection handoff lost metadata", v.request)
	}
	step(ebiten.KeyEnter)
	f, ok := e.currentState.(*FightState)
	if !ok {
		t.Fatalf("versus did not enter fight: %T", e.currentState)
	}
	watchGameTime(t, f)
	for i := 0; i < 600 && f.match.Phase != match.Active; i++ {
		step()
	}
	if f.match.Phase != match.Active {
		t.Fatal("fight did not grant control")
	}
	if !audible || !f.hud.played["round1"] || !f.hud.played["fight"] {
		t.Fatal("menu/round/fight sound did not reach production PCM", audible, f.hud.played)
	}
	for _, p := range f.match.Players {
		if p.Character.OwnSounds != p.SND || p.Character.CommonSounds != f.match.Fight.CommonSounds || p.Character.SoundQueue != e.sound.queue {
			t.Fatal("match sound resources were lost")
		}
	}
	// Win both rounds through the configured keyboard adapter and stock CNS.
	// Keep initial positions, states, life, timer and round timing intact.
	damagedRounds := map[int]bool{}
	for tick := 0; tick < 12000 && f.match.Phase != match.Finished; tick++ {
		if f.match.Phase != match.Active {
			step()
			continue
		}
		c := f.match.Players[0].Character
		distance, _ := c.GetOpponentBodyDistance()
		if distance > 16*float64(c.LocalCoordinateWidth())/320 {
			if c.Facing == 1 {
				step(ebiten.KeyArrowRight)
			} else {
				step(ebiten.KeyArrowLeft)
			}
		} else if f.match.Tick%40 < 2 {
			step(ebiten.KeyL) // P1 X in the bundled keyboard configuration
		} else {
			step()
		}
		victim := f.match.Players[1].Character
		if victim.Life < victim.MaxLife {
			damagedRounds[f.match.Round] = true
		}
	}
	if !damagedRounds[1] || !damagedRounds[2] || f.match.Wins != [2]int{2, 0} {
		t.Fatal("configured keyboard did not win both rounds", damagedRounds, f.match.Wins)
	}
	if f.match.Phase != match.Finished {
		t.Fatal("match did not reach result")
	}
	if !f.hud.played["round2"] || !f.hud.played["KO"] {
		t.Fatal("round reset lost announcement lifecycle", f.hud.played)
	}
	for i := 0; i < 23; i++ {
		step() // Finished matches keep the process clock running.
	}
	step(ebiten.KeyEnter)
	rematch, ok := e.currentState.(*FightState)
	if !ok || rematch == f || rematch.match.Round != 1 || rematch.match.Wins != [2]int{} {
		t.Fatal("rematch did not create fresh match")
	}
	if rematch.match.Timeline == f.match.Timeline || rematch.match.Timeline.GameTime == 0 {
		t.Fatal("rematch must retain process time with a fresh simulation timeline")
	}
	watchGameTime(t, rematch)
	for i := 0; i < 150; i++ {
		step()
	}
	step(ebiten.KeyEscape)
	if _, ok := e.currentState.(*TitleState); !ok {
		t.Fatalf("escape did not return to title: %T", e.currentState)
	}
	for i := 0; i < 41; i++ {
		step()
	}
}

func TestSelectionReportsRosterFailure(t *testing.T) {
	m := &motif.Motif{}
	m.Files.Select = filepath.Join(t.TempDir(), "absent-select.def")
	s := NewSelectState(&Engine{motif: m}, "Versus Mode")
	if _, err := s.Update(); err == nil || !strings.Contains(err.Error(), "absent-select.def") {
		t.Fatal("roster failure was silently dropped", err)
	}
}
