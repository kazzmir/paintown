package match

import (
	"bytes"
	"encoding/json"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestNativeEffectOrderPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "effectorder-*.json", 16)
}

func TestNativeEffectOrderIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/effectorder-*.json")
	if err != nil || len(paths) != 16 {
		t.Fatal(len(paths), err)
	}
	for _, path := range paths {
		var meta struct {
			States     map[int]map[string]int `json:"controllers_by_state"`
			IDs        []int                  `json:"effect_ids"`
			Priorities []int                  `json:"effect_priorities"`
		}
		raw, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		if err := json.Unmarshal(raw, &meta); err != nil {
			t.Fatal(err)
		}
		if len(meta.States) == 0 || len(meta.IDs) == 0 || len(meta.IDs) != len(meta.Priorities) {
			t.Fatal(path, meta)
		}
		raw, err = os.ReadFile(strings.TrimSuffix(path, ".json") + ".cns")
		if err != nil {
			t.Fatal(err)
		}
		file, err := cns.ParseNamed(bytes.NewReader(raw), path)
		if err != nil {
			t.Fatal(err)
		}
		for state, want := range meta.States {
			if state != 700 && state != 701 && state != 710 {
				t.Fatal(path, state)
			}
			counts := map[string]int{}
			for _, ctrl := range file.States[state].Controllers {
				switch strings.ToLower(ctrl.Type) {
				case "explod", "sprpriority", "modifyexplod", "allpalfx", "helper":
					counts[strings.ToLower(ctrl.Type)]++
				}
			}
			if len(counts) != len(want) {
				t.Fatal(path, state, counts, want)
			}
			for kind, count := range want {
				if count < 1 || counts[strings.ToLower(kind)] != count {
					t.Fatal(path, state, counts, want)
				}
			}
		}
	}
}
