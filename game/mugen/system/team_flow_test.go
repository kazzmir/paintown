package main

import (
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
)

func TestTeamSelectionFightHUDAndExitFlow(t *testing.T) {
	e := soundTestEngine(t)
	oldInput := input.GlobalManager
	t.Cleanup(func() { input.GlobalManager = oldInput })
	input.GlobalManager = input.NewInputManager(e.cfg)
	keyboard := &flowKeyboard{}
	input.GlobalManager.Keyboard = keyboard
	s := NewSelectState(e, "Team Versus")
	e.currentState = s
	step := func(keys ...ebiten.Key) {
		t.Helper()
		keyboard.keys = map[ebiten.Key]bool{}
		for _, key := range keys {
			keyboard.keys[key] = true
		}
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
	}
	step(ebiten.KeyArrowDown) // P1 simul.
	step(ebiten.KeyEnter)
	step(ebiten.KeyS) // P2 turns (WASD menu controls).
	step(ebiten.KeyS)
	step(ebiten.KeyU)
	if s.teamMenu || s.teamModes != ([2]match.TeamMode{match.Simul, match.Turns}) {
		t.Fatal("team menu did not choose simul/turns", s.teamModes)
	}
	step(ebiten.KeyEnter)
	step(ebiten.KeyArrowRight)
	step(ebiten.KeyEnter)
	step(ebiten.KeyU)
	step(ebiten.KeyD)
	step(ebiten.KeyU)
	if !s.choosingStage || len(s.rosters[0]) != 2 || len(s.rosters[1]) != 2 {
		t.Fatal("team grid did not collect complete rosters")
	}
	step(ebiten.KeyEnter)
	if _, ok := e.currentState.(*VersusState); !ok {
		t.Fatalf("team selection did not enter versus: %T", e.currentState)
	}
	step(ebiten.KeyEnter)
	f, ok := e.currentState.(*FightState)
	if !ok {
		t.Fatalf("team versus did not enter fight: %T", e.currentState)
	}
	if len(f.match.World.Roots) != 3 || f.match.Teams[0].Mode != match.Simul || f.match.Teams[1].Mode != match.Turns {
		t.Fatal("selected rosters did not reach production match")
	}
	canvas := ebiten.NewImage(1280, 720)
	defer canvas.Deallocate()
	e.Draw(canvas)
	if f.renderError != nil || f.hud.renderError != nil {
		t.Fatal("team HUD/actor drawing", f.renderError, f.hud.renderError)
	}
	if f.hud.clocks["Simul Face/p3.bg1"] == nil || f.hud.clocks["Turns Face/p2.teammate.bg1"] == nil {
		t.Fatal("partner/benched portrait consumers were not drawn")
	}
	step(ebiten.KeyEscape)
	if _, ok := e.currentState.(*TitleState); !ok {
		t.Fatalf("team fight did not return to title: %T", e.currentState)
	}
}
