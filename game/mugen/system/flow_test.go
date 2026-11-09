package main

import (
	"bytes"
	"os"
	"path/filepath"
	"slices"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/engine"
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
	override := filepath.Join(t.TempDir(), "mugen.yaml")
	if err := os.WriteFile(override, []byte(`input:
  p1:
    keyboard:
      right: [KeyArrowRight, KeyNumpad6]
      left: [KeyArrowLeft, KeyNumpad4]
      a: [KeyComma, KeyF6]
      x: [KeyL, KeyF7, L]
      start: [KeyEnter, KeyNumpadEnter]
      escape: [KeyEscape, KeyF8]
      y: [KeyF9, not-a-key]
      z: none
  p2:
    keyboard:
      a: [KeyF, KeyNumpad1]
      b: KeyG
      z: []
`), 0600); err != nil {
		t.Fatal(err)
	}
	cfg, err := config.LoadConfigWithOverrides(root, override)
	if err != nil {
		t.Fatal(err)
	}
	bindings, _ := cfg.Input.Player1Keyboard.Bindings("x")
	if !slices.Equal(bindings, []ebiten.Key{ebiten.KeyL, ebiten.KeyF7}) || cfg.Input.Player1Keyboard.Y != ebiten.KeySemicolon {
		t.Fatal("YAML alternatives did not deduplicate or reject a malformed override atomically")
	}
	// Snapshot/setter isolation matters because managers and round adapters copy
	// Keys by value. Changing a copy must not rewrite another user's aliases.
	bindings[1] = ebiten.KeyF9
	copied := cfg.Input.Player1Keyboard
	copied.SetBinding("x", ebiten.KeyF10)
	original, _ := cfg.Input.Player1Keyboard.Bindings("x")
	replaced, _ := copied.Bindings("x")
	if !slices.Equal(original, []ebiten.Key{ebiten.KeyL, ebiten.KeyF7}) || !slices.Equal(replaced, []ebiten.Key{ebiten.KeyF10}) {
		t.Fatal("keyboard binding snapshots/setters shared mutable aliases")
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
	keyboard.keys = map[ebiten.Key]bool{ebiten.KeyQuote: true, ebiten.KeyY: true}
	if input.GlobalManager.IsPressed(1, input.ActionZ) || input.GlobalManager.IsJustPressed(2, input.ActionZ) {
		t.Fatal("disabled scalar/list bindings fell back to legacy controls")
	}
	keyboard.keys = map[ebiten.Key]bool{ebiten.KeyF: true, ebiten.KeyNumpad1: true}
	if !input.GlobalManager.IsPressed(2, input.ActionA) {
		t.Fatal("both alternatives did not activate the same logical action")
	}
	delete(keyboard.keys, ebiten.KeyF)
	if !input.GlobalManager.IsPressed(2, input.ActionA) || !input.GlobalManager.IsJustPressed(2, input.ActionA) || input.GlobalManager.IsPressed(1, input.ActionA) {
		t.Fatal("releasing an alternative released the action or crossed player ownership")
	}
	delete(keyboard.keys, ebiten.KeyNumpad1)
	if input.GlobalManager.IsPressed(2, input.ActionA) {
		t.Fatal("logical action remained held after both alternatives released")
	}
	e := &Engine{dataDir: root, cfg: cfg, motif: m, logicalWidth: m.Info.LocalCoord.Width, logicalHeight: m.Info.LocalCoord.Height}
	e.random = engine.NewRandomStream(1)
	t.Cleanup(func() {
		if err := e.closeSound(); err != nil {
			t.Error(err)
		}
	})
	e.currentState = NewTitleState(e)
	previousTPS := ebiten.TPS()
	t.Cleanup(func() { ebiten.SetTPS(previousTPS) })
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
	title := e.currentState.(*TitleState)
	for index, item := range title.items {
		if item == "options" {
			title.selectIndex = index
			break
		}
	}
	step(ebiten.KeyEnter)
	options, ok := e.currentState.(*OptionsState)
	if !ok {
		t.Fatalf("title did not open options: %T", e.currentState)
	}
	for index, item := range options.items {
		if item.label == "Game Speed" {
			options.cursor = index
			break
		}
	}
	step(ebiten.KeyArrowRight)
	if ebiten.TPS() != e.ticksPerSecond() || ebiten.TPS() == previousTPS {
		t.Fatal("Game Speed option did not update the live scheduler", ebiten.TPS(), e.ticksPerSecond())
	}
	step(ebiten.KeyEscape)
	if _, ok := e.currentState.(*TitleState); !ok {
		t.Fatal("options did not return to title")
	}
	for i := 0; i < 37; i++ {
		step()
	}
	step(ebiten.KeyArrowDown)
	step(ebiten.KeyNumpadEnter)
	s, ok := e.currentState.(*SelectState)
	if !ok {
		t.Fatalf("title did not open selection: %T", e.currentState)
	}
	// The 320p KFM and 1280p KFM720 portraits must occupy the same 100px
	// authored cells in this 1280p motif, rather than shrinking the first.
	for _, portrait := range s.chars[:2] {
		canvas := ebiten.NewImage(128, 128)
		portrait.drawPortrait(canvas, [2]int{9000, 0}, [2]float64{4, 4}, [2]float64{1, 1}, 1, e.logicalWidth, nil)
		pixels := make([]byte, 128*128*4)
		canvas.ReadPixels(pixels)
		canvas.Deallocate()
		left, right := 128, 0
		for y := 0; y < 128; y++ {
			for x := 0; x < 128; x++ {
				if pixels[(y*128+x)*4+3] > 0 {
					left = min(left, x)
					right = max(right, x)
				}
			}
		}
		if right-left < 90 {
			t.Fatal("mixed-coordinate selection portrait shrank", portrait.Name, left, right)
		}
	}
	// Start both on the first stock cell, then move P2 with the stock D key.
	s.p1CursorX, s.p1CursorY, s.p2CursorX, s.p2CursorY = 0, 0, 0, 0
	step(ebiten.KeyD)
	step(ebiten.KeyF6)
	if s.p2Done {
		t.Fatal("P1 confirmation also confirmed P2")
	}
	step(ebiten.KeyG)
	if !s.choosingStage {
		t.Fatal("confirmed players did not reach stage selection")
	}
	step(ebiten.KeyNumpad6)
	step(ebiten.KeyNumpadEnter)
	v, ok := e.currentState.(*VersusState)
	if !ok {
		t.Fatalf("stage selection did not open versus: %T", e.currentState)
	}
	if !strings.HasSuffix(v.request.Players[0].DefPath, "kfm/kfm.def") || !strings.HasSuffix(v.request.Players[1].DefPath, "kfm720/kfm720.def") || v.request.Players[1].Palette != 2 || v.request.StagePath != "stages/stage0-720.def" {
		t.Fatal("selection handoff lost metadata", v.request)
	}
	// Exercise the actual VS renderer before handing off. The former flow
	// checked only its type, allowing an empty names-only screen to pass.
	canvas := ebiten.NewImage(e.logicalWidth, e.logicalHeight)
	v.ticks = m.VSScreen.FadeInTime + 1
	v.Draw(canvas)
	withPortraits := make([]byte, e.logicalWidth*e.logicalHeight*4)
	canvas.ReadPixels(withPortraits)
	portraits := v.portraits
	v.portraits = [2]*CharacterPortrait{}
	v.Draw(canvas)
	background := make([]byte, len(withPortraits))
	canvas.ReadPixels(background)
	canvas.Deallocate()
	v.portraits = portraits
	var changed [2]int
	backgroundPixels := 0
	for y := 0; y < e.logicalHeight; y++ {
		for x := 0; x < e.logicalWidth; x++ {
			i := (y*e.logicalWidth + x) * 4
			if background[i] != 0 || background[i+1] != 0 || background[i+2] != 0 {
				backgroundPixels++
			}
			if bytes.Equal(withPortraits[i:i+4], background[i:i+4]) {
				continue
			}
			side := 0
			if x >= e.logicalWidth/2 {
				side = 1
			}
			window := m.VSScreen.P1Portrait.Window
			if side == 1 {
				window = m.VSScreen.P2Portrait.Window
			}
			if window == nil || x < window[0] || x > window[2] || y < window[1] || y > window[3] {
				t.Fatal("VS portrait escaped its authored window", x, y)
			}
			changed[side]++
		}
	}
	if backgroundPixels < 50000 || changed[0] < 1000 || changed[1] < 1000 {
		t.Fatal("VS background or portraits missing", backgroundPixels, changed)
	}
	step(ebiten.KeyNumpadEnter)
	f, ok := e.currentState.(*FightState)
	if !ok {
		t.Fatalf("versus did not enter fight: %T", e.currentState)
	}
	watchGameTime(t, f)
	if f.match.Options.TicksPerSecond != int32(ebiten.TPS()) {
		t.Fatal("match did not inherit the live Game Speed scheduler")
	}
	if f.match.World.RandomSeed() != 16807 {
		t.Fatal("session seed was not handed to production match")
	}
	incoming, broken := false, false
	for i := 0; i < 600 && f.match.Phase != match.Active; i++ {
		step()
		for _, explod := range f.match.Players[0].Character.Explods {
			broken = broken || explod.Anim == 192
			if explod.Anim != 191 || explod.CurrentTime != 12 {
				continue
			}
			if explod.VelX >= -4 || explod.X >= f.match.Players[0].Character.StartX+260 {
				t.Fatal("incoming intro block ignored legacy velocity", explod.VelX, explod.X)
			}
			// Check the real stage/actor/effect composition before the kick,
			// rather than merely checking that the Explod was allocated.
			canvas := ebiten.NewImage(e.logicalWidth, e.logicalHeight)
			f.Draw(canvas)
			visible := make([]byte, e.logicalWidth*e.logicalHeight*4)
			canvas.ReadPixels(visible)
			explod.Visual.Active = false
			f.Draw(canvas)
			without := make([]byte, len(visible))
			canvas.ReadPixels(without)
			explod.Visual.Active = true
			canvas.Deallocate()
			if bytes.Equal(visible, without) {
				t.Fatal("incoming intro block was offscreen or covered before impact")
			}
			incoming = true
		}
	}
	if f.match.Phase != match.Active || !incoming || !broken {
		t.Fatal("intro flight/break/control handoff incomplete", f.match.Phase, incoming, broken)
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
				step(ebiten.KeyNumpad6)
			} else {
				step(ebiten.KeyNumpad4)
			}
		} else if f.match.Tick%40 < 2 {
			if f.match.Round == 1 {
				step(ebiten.KeyF7) // Alternative P1 X through stock CNS.
			} else {
				step(ebiten.KeyL) // Existing single-key path remains usable.
			}
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
	if f.match.World.RandomSeed() != 16807 {
		t.Fatal("round reset reseeded the match")
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
	if rematch.match.World.RandomSeed() != 282475249 {
		t.Fatal("rematch did not advance the session RNG")
	}
	for i := 0; i < 150; i++ {
		step()
	}
	step(ebiten.KeyF8)
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
