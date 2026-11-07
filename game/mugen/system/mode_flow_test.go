package main

import (
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

// Real round completion drives loss/continue/retry, ordered arcade victory and
// ending, then survival carry/recovery/results and training/watch mode rules.
func TestModeProgressionContinueSurvivalAndTrainingFlow(t *testing.T) {
	e := soundTestEngine(t)
	e.pendingMatchSeed = 16807
	oldInput := input.GlobalManager
	t.Cleanup(func() { input.GlobalManager = oldInput })
	input.GlobalManager = input.NewInputManager(e.cfg)
	keyboard := &flowKeyboard{}
	input.GlobalManager.Keyboard = keyboard
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
	canvas := ebiten.NewImage(1280, 720)
	defer canvas.Deallocate()
	enterFight := func() *FightState {
		t.Helper()
		for n := 0; n < 64; n++ {
			if f, ok := e.currentState.(*FightState); ok {
				return f
			}
			step(ebiten.KeyEnter)
		}
		t.Fatalf("mode did not reach fight: %T", e.currentState)
		return nil
	}
	selectMode := func(name string) *FightState {
		t.Helper()
		e.currentState = NewSelectState(e, name)
		step(ebiten.KeyEnter)
		step(ebiten.KeyEnter)
		return enterFight()
	}
	complete := func(f *FightState, winner int) {
		t.Helper()
		found := false
		for _, section := range f.match.Fight.AST.Sections {
			if section.Name != "Round" {
				continue
			}
			for _, a := range section.Attributes {
				if a.ID.String() == "match.wins" {
					a.Value = parsers.NumberValue{Val: 1}
					a.RawValue = "1"
					found = true
				}
			}
		}
		if !found {
			t.Fatal("stock round target missing")
		}
		f.match.Options.Input = func(int, int) []string { return nil }
		for _, p := range f.match.Actors() {
			p.Character.InputSource = f.match.Options.Input
		}
		for n := 0; n < 700 && f.match.Phase != match.Active; n++ {
			step()
		}
		if f.match.Phase != match.Active {
			t.Fatal("mode fight did not release control")
		}
		for _, p := range f.match.SidePlayers(1 - winner) {
			p.Character.SetLife(0)
			p.Character.ChangeState(5150, 0, -1)
		}
		for n := 0; n < 800 && e.currentState == f; n++ {
			step()
		}
		if e.currentState == f {
			t.Fatal("completed mode match did not route its result", f.match.Phase)
		}
	}
	f := selectMode("Arcade Mode")
	r := e.modeRun
	if r == nil || len(r.opponents) < 2 || f.match.Request.MatchNo != 1 {
		t.Fatal("arcade schedule/match context missing")
	}
	firstOpponent := f.match.Request.Players[1].DefPath
	complete(f, 1)
	if _, ok := e.currentState.(*ContinueScreenState); !ok {
		t.Fatalf("arcade loss did not offer continue: %T", e.currentState)
	}
	e.Draw(canvas)
	step(ebiten.KeyEnter)
	f = enterFight()
	if f.match.Request.Players[1].DefPath != firstOpponent || f.match.Request.MatchNo != 1 || e.modeRun != r {
		t.Fatal("continue advanced/replaced the current opponent")
	}
	complete(f, 0)
	if _, ok := e.currentState.(*WinScreenState); !ok {
		t.Fatalf("nonfinal arcade win omitted victory: %T", e.currentState)
	}
	e.Draw(canvas)
	step(ebiten.KeyEnter)
	f = enterFight()
	if f.match.Request.MatchNo != 2 || f.match.World.MatchNo != 2 || r.index != 1 {
		t.Fatal("arcade did not advance MatchNo/opponent schedule")
	}
	complete(f, 0)
	if _, ok := e.currentState.(*StoryboardState); !ok {
		t.Fatalf("selected character ending was not entered: %T", e.currentState)
	}
	for n := 0; n < 64; n++ {
		if _, ok := e.currentState.(*TitleState); ok {
			break
		}
		step(ebiten.KeyEnter)
	}
	if _, ok := e.currentState.(*TitleState); !ok {
		t.Fatalf("ending did not return to title: %T", e.currentState)
	}
	step() // Releases the completed campaign.
	f = selectMode("Survival")
	f.match.Players[0].Character.SetLife(600)
	f.match.Players[0].Character.SetPower(500)
	complete(f, 0)
	f = enterFight()
	if f.match.Request.MatchNo != 2 || f.match.Players[0].Character.Life <= 600 || f.match.Players[0].Character.Life > f.match.Players[0].Character.MaxLife || f.match.Players[0].Character.GetPower() != 500 {
		t.Fatal("survival did not carry/recover life and power", f.match.Players[0].Character.Life, f.match.Players[0].Character.GetPower())
	}
	complete(f, 1)
	result, ok := e.currentState.(*GameOverScreenState)
	if !ok || !result.survival || result.score != 1 {
		t.Fatalf("survival loss did not show the actual streak: %T", e.currentState)
	}
	e.Draw(canvas)
	step(ebiten.KeyEnter)
	step()
	f = selectMode("Training")
	for n := 0; n < 700 && f.match.Phase != match.Active; n++ {
		step()
	}
	root := f.match.Players[0].Character
	root.SetLife(0)
	step()
	if f.match.TimeLeft != -1 || f.match.Phase != match.Active || !root.IsAlive() || root.Life != root.MaxLife || root.GetPower() != root.MaxPower {
		t.Fatal("training timer/KO/refill rules missing")
	}
	step(ebiten.KeyEscape)
	step()
	f = selectMode("Watch Mode")
	for _, p := range f.match.Actors() {
		if p.Character.AILevel <= 0 {
			t.Fatal("watch mode left a human-controlled side")
		}
	}
	step(ebiten.KeyEscape)
}
