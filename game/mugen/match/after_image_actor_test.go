package match

import (
	"bytes"
	"encoding/json"
	"os"
	"path/filepath"
	"sort"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestNativeAfterImageActorPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "trailactor-*.json", 48)
}

func TestNativeAfterImageActorIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/trailactor-*.json")
	if err != nil || len(paths) != 48 {
		t.Fatal(len(paths), err)
	}
	for _, path := range paths {
		raw, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		var meta struct {
			Count     int            `json:"controller_count"`
			States    []int          `json:"controller_states"`
			TimeCount int            `json:"afterimage_time_count"`
			Kinds     map[string]int `json:"actual_controller_counts"`
		}
		if err = json.Unmarshal(raw, &meta); err != nil {
			t.Fatal(err)
		}
		raw, err = os.ReadFile(strings.TrimSuffix(path, ".json") + ".cns")
		if err != nil {
			t.Fatal(err)
		}
		file, err := cns.ParseNamed(bytes.NewReader(raw), path)
		if err != nil {
			t.Fatal(err)
		}
		kinds := map[string]int{}
		states := []int{}
		for number, state := range file.States {
			for _, ctrl := range state.Controllers {
				name := strings.ToLower(ctrl.Type)
				kinds[name]++
				if name == "afterimage" {
					found := false
					for _, n := range states {
						found = found || n == number
					}
					if !found {
						states = append(states, number)
					}
				}
			}
		}
		sort.Ints(states)
		sort.Ints(meta.States)
		if kinds["afterimage"] != meta.Count || kinds["afterimagetime"] != meta.TimeCount || len(states) != len(meta.States) {
			t.Fatal(path, kinds, states, meta)
		}
		for i, n := range states {
			if n != meta.States[i] || n != 700 && n != 701 && n != 710 && n != 720 {
				t.Fatal(path, states, meta.States)
			}
		}
		if len(kinds) != len(meta.Kinds) {
			t.Fatal(path, kinds, meta.Kinds)
		}
		for name, want := range meta.Kinds {
			if kinds[strings.ToLower(name)] != want {
				t.Fatal(path, kinds, meta.Kinds)
			}
		}
	}
}
