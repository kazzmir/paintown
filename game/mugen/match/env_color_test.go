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

func TestNativeEnvironmentColorPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "envcolor-*.json", 76)
}

func TestNativeEnvironmentColorIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/envcolor-*.json")
	if err != nil || len(paths) != 76 {
		t.Fatal(len(paths), err)
	}
	for _, path := range paths {
		var meta struct {
			States map[int]map[string]int `json:"controllers_by_state"`
		}
		raw, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		if err := json.Unmarshal(raw, &meta); err != nil {
			t.Fatal(err)
		}
		if len(meta.States) == 0 {
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
				case "envcolor", "explod", "projectile", "helper", "allpalfx", "bgpalfx":
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
