package main

import (
	"path/filepath"
	"slices"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/config"
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
	// Command-line partners use the same simul roster/AI/HUD consumers as the
	// selection flow below, including Debug.StartStage when --stage is absent.
	direct, err := quickMatchRequest("chars/kfm/kfm.def,chars/kfm720/kfm720.def", "watch", "", "chars/kfm/kfm.def", "chars/kfm720/kfm720.def", e.cfg)
	if err != nil {
		t.Fatal(err)
	}
	for side := range direct.Teams {
		for index := range direct.Teams[side].Members {
			direct.Teams[side].Members[index].DefPath = filepath.Join(e.dataDir, direct.Teams[side].Members[index].DefPath)
		}
	}
	for side := range direct.Players {
		direct.Players[side].DefPath = filepath.Join(e.dataDir, direct.Players[side].DefPath)
	}
	preview, err := NewFightState(e, direct)
	if err != nil {
		t.Fatal(err)
	}
	if len(preview.match.Actors()) != 4 || preview.match.Request.StagePath != e.cfg.Debug.StartStage {
		t.Fatal("direct partner roster/stage handoff was lost")
	}
	for _, p := range preview.match.Actors() {
		if p.Character.AILevel < 1 {
			t.Fatal("direct watch left a human-owned participant")
		}
	}
	previewCanvas := ebiten.NewImage(1280, 720)
	preview.Draw(previewCanvas)
	previewCanvas.Deallocate()
	if preview.renderError != nil || preview.hud.renderError != nil {
		t.Fatal("direct simul preview lost presentation", preview.renderError, preview.hud.renderError)
	}
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
	// DEF identity does not identify a select.def row. The selected roster
	// must retain its own music even when other matching entries change.
	for _, ch := range s.chars {
		if ch.DefPath == s.rosters[1][0].DefPath {
			ch.Entry.Music = "unselected-row.mp3"
		}
	}
	step(ebiten.KeyEnter)
	vs, ok := e.currentState.(*VersusState)
	if !ok {
		t.Fatalf("team selection did not enter versus: %T", e.currentState)
	}
	if vs.request.Music != "" {
		t.Fatal("selected row music was replaced during handoff", vs.request.Music)
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

	// Both co-op humans choose first; P1 then chooses the CPU roster type.
	s = NewSelectState(e, "Team Co-op")
	e.currentState = s
	step(ebiten.KeyEnter)
	step(ebiten.KeyD)
	step(ebiten.KeyU)
	if !s.teamMenu || !s.p1Done || !s.p2Done || !s.teamReady[0] {
		t.Fatal("co-op did not offer the opponent menu after human selections")
	}
	step(ebiten.KeyArrowDown)
	step(ebiten.KeyArrowDown)
	step(ebiten.KeyArrowRight)
	step(ebiten.KeyEnter)
	if s.teamMenu || !s.opponentTeamChosen || s.teamModes[1] != match.Turns || s.teamCounts[1] != 3 {
		t.Fatal("P1 did not configure the co-op opposing roster")
	}
	enterFight := func() *FightState {
		t.Helper()
		for attempt := 0; attempt < 8; attempt++ {
			if f, ok := e.currentState.(*FightState); ok {
				return f
			}
			step(ebiten.KeyEnter)
		}
		t.Fatalf("co-op did not reach fight: %T", e.currentState)
		return nil
	}
	f = enterFight()
	if f.match.Teams[0].Mode != match.Simul || f.match.Teams[1].Mode != match.Turns || len(f.match.Teams[1].Members) != 3 || len(f.match.World.Roots) != 3 {
		t.Fatal("co-op selected mode/count did not reach match")
	}
	for index, p := range f.match.SidePlayers(0) {
		if p.Character.AILevel != 0 || f.match.Request.Teams[0].Members[index].DefPath != s.rosters[index][0].DefPath {
			t.Fatal("co-op human choice/control ownership was replaced")
		}
	}
	if f.match.Players[1].Character.AILevel < 1 {
		t.Fatal("co-op opposing roster lacks CPU ownership")
	}
	checkCoopInput := func() {
		t.Helper()
		step(ebiten.KeyD)
		read := f.match.Options.Input
		if !slices.Contains(read(3, 1), "DirForward") || slices.Contains(read(1, 1), "DirForward") {
			t.Fatal("second human input did not reach only the co-op partner slot")
		}
		pad := &flowPad{standard: true}
		pad.release()
		pad.axes[ebiten.StandardGamepadAxisLeftStickHorizontal] = 0.9
		manager := input.GlobalManager
		manager.P2Gamepad = config.DefaultGamepadConfig()
		manager.P2Gamepad.Enabled = true
		manager.KeyboardDisabled[1] = true
		manager.Gamepads = flowGamepads{2: pad}
		step()
		if !slices.Contains(read(3, 1), "DirForward") || slices.Contains(read(1, 1), "DirForward") || slices.Contains(read(3, -1), "DirForward") {
			t.Fatal("controller-only second human lost co-op slot/facing ownership")
		}
		manager.KeyboardDisabled[1], manager.P2Gamepad.Enabled = false, false
	}
	checkCoopInput()
	e.Draw(canvas)
	if f.renderError != nil || f.hud.renderError != nil {
		t.Fatal("co-op team HUD/actor drawing", f.renderError, f.hud.renderError)
	}
	step(ebiten.KeyEscape)

	// Survival co-op uses the same two human slots but always a Single CPU.
	s = NewSelectState(e, "Survival Co-op")
	e.currentState = s
	step(ebiten.KeyEnter)
	step(ebiten.KeyU)
	if s.teamMenu || !s.choosingStage {
		t.Fatal("Survival co-op unexpectedly asks for an opponent type")
	}
	f = enterFight()
	if f.match.Teams[0].Mode != match.Simul || f.match.Teams[1].Mode != match.Single || len(f.match.World.Roots) != 3 {
		t.Fatal("Survival co-op roster")
	}
	for _, p := range f.match.SidePlayers(0) {
		if p.Character.AILevel != 0 || p.Character.MaxLife != 667 {
			t.Fatal("Survival co-op human control/life", p.Character.AILevel, p.Character.MaxLife)
		}
	}
	if f.match.Players[1].Character.MaxLife != 1000 || f.match.Players[1].Character.AILevel < 1 {
		t.Fatal("Survival co-op Single CPU")
	}
	checkCoopInput()
	step(ebiten.KeyEscape)
	if _, ok := e.currentState.(*TitleState); !ok {
		t.Fatalf("co-op fight did not return to title: %T", e.currentState)
	}

	// Watch lets P1 choose each team's mode and roster in sequence. Both
	// sides then use CPU controls, including the selected Simul partner.
	s = NewSelectState(e, "Watch Mode")
	e.currentState = s
	step(ebiten.KeyArrowDown)
	step(ebiten.KeyArrowDown)
	step(ebiten.KeyEnter) // Own Turns team of two.
	step(ebiten.KeyEnter)
	step(ebiten.KeyArrowRight)
	step(ebiten.KeyEnter)
	if !s.teamMenu || !s.p1Done || s.p2Done {
		t.Fatal("Watch did not defer the opposing menu until after the first roster")
	}
	step(ebiten.KeyArrowDown)
	step(ebiten.KeyEnter) // Opposing Simul team.
	step(ebiten.KeyEnter)
	step(ebiten.KeyArrowRight)
	step(ebiten.KeyEnter)
	if len(s.rosters[0]) != 2 || len(s.rosters[1]) != 2 || !s.choosingStage {
		t.Fatal("P1 could not select both Watch rosters")
	}
	f = enterFight()
	if f.match.Teams[0].Mode != match.Turns || f.match.Teams[1].Mode != match.Simul || len(f.match.World.Roots) != 3 {
		t.Fatal("Watch selected team modes did not reach match")
	}
	for _, team := range f.match.Request.Teams {
		for _, selected := range team.Members {
			if selected.AILevel < 1 {
				t.Fatal("Watch left a selected member human-controlled")
			}
		}
	}
	step(ebiten.KeyEscape)

	// Team Arcade uses the same deferred enemy menu but schedules its roster.
	s = NewSelectState(e, "Team Arcade")
	e.currentState = s
	step(ebiten.KeyEnter) // Own Single team.
	if s.teamMenu {
		t.Fatal("Team Arcade requested the enemy mode before choosing its own roster")
	}
	step(ebiten.KeyEnter)
	if !s.teamMenu || !s.p1Done || s.teamReady[1] {
		t.Fatal("Team Arcade omitted its deferred enemy menu")
	}
	step(ebiten.KeyArrowDown)
	step(ebiten.KeyEnter) // CPU Simul team.
	f = enterFight()
	if f.match.Teams[0].Mode != match.Single || f.match.Teams[1].Mode != match.Simul || len(f.match.World.Roots) != 3 || f.match.Request.Campaign == 0 {
		t.Fatal("Team Arcade lost its selected CPU roster/schedule")
	}
	step(ebiten.KeyEscape)
}
