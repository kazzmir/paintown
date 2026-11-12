package main

import (
	"bytes"
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"math"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
)

// Add a real CNS observation without changing stock moves or their inputs.
func watchGameTime(t *testing.T, fight *FightState) {
	t.Helper()
	file, err := cns.ParseNamed(strings.NewReader("[Statedef -2]\n[State -2, clock witness]\ntype=VarSet\ntrigger1=1\nignorehitpause=1\nv=59\nvalue=GameTime\n"), "process-clock-witness.cns")
	if err != nil {
		t.Fatal(err)
	}
	for _, player := range fight.match.Players {
		state := player.Character.StateFile.States[-2]
		if state == nil {
			player.Character.StateFile.States[-2] = file.States[-2]
		} else {
			state.Controllers = append(state.Controllers, file.States[-2].Controllers...)
		}
	}
}

func TestNativeGameTimeProgramAcrossEngineMenusAndNewMatches(t *testing.T) {
	stem := "../tests/testdata/reference/engine-gametime"
	data, err := os.ReadFile(stem + ".json")
	if err != nil {
		t.Fatal(err)
	}
	var native struct {
		Hashes   map[string]string `json:"fixture_sha256"`
		Captures []struct{ Variables map[string]int }
	}
	if err := json.Unmarshal(data, &native); err != nil || len(native.Captures) != 2 {
		t.Fatal("missing same-process native clock observations", err)
	}
	checked := func(ext string) []byte {
		t.Helper()
		b, err := os.ReadFile(stem + ext)
		if err != nil {
			t.Fatal(err)
		}
		h := sha256.Sum256(b)
		if hex.EncodeToString(h[:]) != native.Hashes[ext] {
			t.Fatal("native clock input changed", ext)
		}
		return b
	}
	file, err := cns.ParseNamed(bytes.NewReader(checked(".cns")), stem+".cns")
	if err != nil {
		t.Fatal(err)
	}
	negative, err := cns.ParseNamed(bytes.NewReader(checked(".cmd")), stem+".cmd")
	if err != nil {
		t.Fatal(err)
	}
	for number, state := range negative.States {
		file.States[number] = state
	}
	file.SetLanguageVersion("1.1")
	animations, err := air.Parse(bytes.NewReader(checked(".air")))
	if err != nil {
		t.Fatal(err)
	}
	checked(".clock.json")
	e := soundTestEngine(t)
	previous := input.GlobalManager
	input.GlobalManager = input.NewInputManager(e.cfg)
	input.GlobalManager.Keyboard = &flowKeyboard{keys: nil}
	t.Cleanup(func() { input.GlobalManager = previous })
	request := match.Request{StagePath: "stages/kfm.def"}
	for slot := range request.Players {
		request.Players[slot] = match.Selection{DefPath: filepath.Join(e.dataDir, "chars/kfm/kfm.def"), Palette: 1}
	}
	for _, capture := range native.Captures {
		want := capture.Variables
		e.currentState = NewTitleState(e)
		// Reproduce native tick counts with real Engine updates. Do not seed or
		// align a Match timeline: its origin must come from the production handoff.
		for e.gameTime < int32(want["0"]) {
			if err := e.Update(); err != nil {
				t.Fatal(err)
			}
		}
		fight, err := NewFightState(e, request)
		if err != nil {
			t.Fatal(err)
		}
		for _, player := range fight.match.Players {
			player.Character.StateFile = file
			player.Character.AirFile = animations
			player.Character.ChangeState(5900, -1, -1)
		}
		e.currentState = fight
		for e.gameTime <= int32(want["1"]) {
			if err := e.Update(); err != nil {
				t.Fatal(err)
			}
		}
		for index, value := range want {
			i, err := strconv.Atoi(index)
			if err != nil {
				t.Fatal(err)
			}
			if got := fight.match.Players[0].Character.IntVars[i]; got != value {
				t.Errorf("Var(%d)=%d native=%d", i, got, value)
			}
		}
	}
}

func TestEngineGameTimeWrapsAcrossMenuOnlyUpdates(t *testing.T) {
	previous := input.GlobalManager
	input.GlobalManager = nil
	t.Cleanup(func() { input.GlobalManager = previous })
	e := &Engine{gameTime: math.MaxInt32 - 1}
	t.Cleanup(func() {
		if err := e.closeSound(); err != nil {
			t.Error(err)
		}
	})
	for _, want := range []int32{math.MaxInt32, math.MinInt32, math.MinInt32 + 1} {
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
		if e.gameTime != want {
			t.Fatal("process clock lost signed 32-bit semantics", e.gameTime, want)
		}
	}
}

func TestNativeClockRateAndVersusMatchNumber(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/clockrate-*.rate.json")
	if err != nil || len(paths) != 5 {
		t.Fatal("missing native rate controls", err)
	}
	for _, recipePath := range paths {
		t.Run(filepath.Base(recipePath), func(t *testing.T) {
			stem := strings.TrimSuffix(recipePath, ".rate.json")
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var native struct {
				Hashes map[string]string `json:"fixture_sha256"`
				Recipe struct {
					FPS   int `json:"config_game_speed"`
					Speed int `json:"options_game_speed"`
				}
				Capture struct{ Variables map[string]float64 }
			}
			if err := json.Unmarshal(data, &native); err != nil {
				t.Fatal(err)
			}
			inputs := map[string][]byte{}
			for ext, want := range native.Hashes {
				b, err := os.ReadFile(stem + ext)
				if err != nil {
					t.Fatal(err)
				}
				h := sha256.Sum256(b)
				if hex.EncodeToString(h[:]) != want {
					t.Fatal("native rate input changed", ext)
				}
				inputs[ext] = b
			}
			file, err := cns.ParseNamed(bytes.NewReader(inputs[".cns"]), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			file.SetLanguageVersion("1.1")
			animations, err := air.Parse(bytes.NewReader(inputs[".air"]))
			if err != nil {
				t.Fatal(err)
			}
			e := soundTestEngine(t)
			e.cfg.Config.GameSpeed, e.cfg.Options.GameSpeed = native.Recipe.FPS, native.Recipe.Speed
			previous := input.GlobalManager
			input.GlobalManager = input.NewInputManager(e.cfg)
			input.GlobalManager.Keyboard = &flowKeyboard{keys: nil}
			t.Cleanup(func() { input.GlobalManager = previous })
			request := match.Request{StagePath: "stages/kfm.def"}
			for slot := range request.Players {
				request.Players[slot] = match.Selection{DefPath: filepath.Join(e.dataDir, "chars/kfm/kfm.def"), Palette: 1}
			}
			fight, err := NewFightState(e, request)
			if err != nil {
				t.Fatal(err)
			}
			for _, player := range fight.match.Players {
				player.Character.StateFile, player.Character.AirFile = file, animations
				player.Character.ChangeState(5900, -1, -1)
			}
			e.currentState = fight
			for tick := 0; tick < 300; tick++ {
				if err := e.Update(); err != nil {
					t.Fatal(err)
				}
			}
			if len(native.Capture.Variables) != 12 {
				t.Fatal("incomplete native rate observation")
			}
			for key, want := range native.Capture.Variables {
				i, err := strconv.Atoi(key)
				if err != nil {
					t.Fatal(err)
				}
				if got := fight.match.Players[0].Character.FloatVars[i]; float32(got) != float32(want) {
					t.Errorf("FVar(%d)=%v native=%v", i, got, want)
				}
			}
		})
	}
}
