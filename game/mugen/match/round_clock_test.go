package match

import (
	"bytes"
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/fight"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

func TestNativeRoundClockPrograms(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/roundclock-*-stable.json")
	if err != nil || len(paths) != 29 {
		t.Fatal("missing native round-clock corpus", err)
	}
	for _, path := range paths {
		t.Run(filepath.Base(path), func(t *testing.T) { replayNativeRound(t, path) })
	}
}

func replayNativeRound(t *testing.T, path string) {
	t.Helper()
	var expected struct {
		FixtureHashes map[string]string `json:"fixture_sha256"`
		OriginalFight map[string]string `json:"original_fight_sha256"`
		Captures      []struct{ Variables map[string]int }
	}
	data, err := os.ReadFile(path)
	if err != nil {
		t.Fatal(err)
	}
	if err = json.Unmarshal(data, &expected); err != nil {
		t.Fatal(err)
	}
	stem := strings.TrimSuffix(path, ".json")
	checked := func(path, want string) []byte {
		t.Helper()
		b, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		h := sha256.Sum256(b)
		if hex.EncodeToString(h[:]) != want {
			t.Fatal("native round input changed", path)
		}
		return b
	}
	var recipe struct {
		RoundTime   int `json:"round_time"`
		Fight       map[string]int
		ReplayTicks int    `json:"replay_ticks"`
		NativeMode  string `json:"native_mode"`
	}
	if err = json.Unmarshal(checked(stem+".round.json", expected.FixtureHashes[".round.json"]), &recipe); err != nil {
		t.Fatal(err)
	}
	file, err := cns.ParseNamed(bytes.NewReader(checked(stem+".cns", expected.FixtureHashes[".cns"])), stem+".cns")
	if err != nil {
		t.Fatal(err)
	}
	file.SetLanguageVersion("1.1")
	commandsBytes := checked(stem+".cmd", expected.FixtureHashes[".cmd"])
	commands, err := cmd.Parse(bytes.NewReader(commandsBytes))
	if err != nil {
		t.Fatal(err)
	}
	negative, err := cns.ParseNamed(bytes.NewReader(commandsBytes), stem+".cmd")
	if err != nil {
		t.Fatal(err)
	}
	negative.SetLanguageVersion("1.1")
	for number, state := range negative.States {
		if file.States[number] == nil {
			file.States[number] = state
		}
	}
	animations, err := air.Parse(bytes.NewReader(checked(stem+".air", expected.FixtureHashes[".air"])))
	if err != nil {
		t.Fatal(err)
	}
	m := stockMatch(t, recipe.RoundTime)
	for relative, want := range expected.OriginalFight {
		checked(filepath.Join(m.Options.RootDir, relative), want)
	}
	m.Fight, err = fight.Load("data/mugen1/fight.def", m.Options.RootDir)
	if err != nil {
		t.Fatal(err)
	}
	for field, value := range recipe.Fight {
		section, key, ok := strings.Cut(field, ".")
		if !ok {
			t.Fatal(field)
		}
		ast, err := parsers.Parse(strings.NewReader("[" + section + "]\n" + key + "=" + strconv.Itoa(value) + "\n"))
		if err != nil {
			t.Fatal(err)
		}
		m.Fight.AST.Sections = append(m.Fight.AST.Sections, ast.Sections...)
	}
	for _, p := range m.Players {
		p.Character.StateFile, p.AIR, p.CMD = file, animations, commands
		p.LocalCoord = coord.Default()
	}
	if err = m.resetRound(); err != nil {
		t.Fatal(err)
	}
	if recipe.ReplayTicks < 1 || recipe.ReplayTicks > 2400 {
		t.Fatal("unbounded replay")
	}
	if recipe.NativeMode == "versus" {
		found := false
		for _, capture := range expected.Captures {
			if start, ok := capture.Variables["0"]; ok {
				m.Timeline.GameTime = int32(start - 1)
				found = true
			}
		}
		if !found {
			t.Fatal("missing native first-pass GameTime")
		}
	}
	for tick := 0; tick < recipe.ReplayTicks; tick++ {
		if err = m.Update(); err != nil {
			t.Fatal(err)
		}
	}
	for _, capture := range expected.Captures {
		for raw, want := range capture.Variables {
			index, err := strconv.Atoi(raw)
			if err != nil {
				t.Fatal(err)
			}
			if got := m.Players[0].Character.IntVars[index]; got != want {
				t.Errorf("Var(%d)=%d native=%d", index, got, want)
			}
		}
	}
}
