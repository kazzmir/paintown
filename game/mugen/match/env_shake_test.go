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

func TestNativeEnvironmentShakePrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "envshake-*.json", 72)
}

func TestNativeEnvironmentShakeIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/envshake-*.json")
	if err != nil || len(paths) != 72 {
		t.Fatal(len(paths), err)
	}
	for _, path := range paths {
		var meta struct {
			Kind      string         `json:"controller_kind"`
			Count     int            `json:"controller_count"`
			States    []int          `json:"controller_states"`
			Dependent map[string]int `json:"dependent_kinds"`
		}
		raw, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		if err := json.Unmarshal(raw, &meta); err != nil {
			t.Fatal(err)
		}
		if meta.Kind != "EnvShake" || meta.Count < 1 || len(meta.States) != 1 {
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
		counts := map[string]int{}
		for number, state := range file.States {
			for _, ctrl := range state.Controllers {
				kind := strings.ToLower(ctrl.Type)
				switch kind {
				case "envshake":
					if number != meta.States[0] || number != 700 && number != 710 {
						t.Fatal(path, number)
					}
					counts[kind]++
				case "envcolor", "allpalfx":
					counts[kind]++
				}
			}
		}
		if counts["envshake"] != meta.Count {
			t.Fatal(path, counts, meta)
		}
		delete(counts, "envshake")
		if len(counts) != len(meta.Dependent) {
			t.Fatal(path, counts, meta.Dependent)
		}
		for kind, want := range meta.Dependent {
			if want < 1 || counts[strings.ToLower(kind)] != want {
				t.Fatal(path, counts, meta.Dependent)
			}
		}
		if strings.Contains(filepath.Base(path), "envshake-helper-") {
			helperCount := 0
			for _, ctrl := range file.States[700].Controllers {
				if strings.EqualFold(ctrl.Type, "Helper") {
					helperCount++
				}
			}
			if helperCount != 1 || meta.States[0] != 710 {
				t.Fatal(path, "helper intent", helperCount, meta)
			}
		}
	}
}
