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

func TestNativeGlobalPaletteFXPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "globalfx-*.json", 83)
}

func TestNativeGlobalPaletteFXIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/globalfx-*.json")
	if err != nil || len(paths) != 83 {
		t.Fatal(len(paths), err)
	}
	for _, path := range paths {
		var meta struct {
			Kinds          map[string]int `json:"controller_kinds"`
			States         []int          `json:"controller_states"`
			Dependent      string         `json:"dependent_kind"`
			DependentCount int            `json:"dependent_count"`
		}
		raw, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		if err := json.Unmarshal(raw, &meta); err != nil {
			t.Fatal(err)
		}
		if len(meta.Kinds) < 1 || len(meta.Kinds) > 2 || len(meta.States) != 1 {
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
		dependentCount := 0
		for _, state := range meta.States {
			if state != 700 && state != 710 {
				t.Fatal(path, state)
			}
			for _, ctrl := range file.States[state].Controllers {
				if meta.Dependent != "" && strings.EqualFold(ctrl.Type, meta.Dependent) {
					dependentCount++
				}
				switch strings.ToLower(ctrl.Type) {
				case "allpalfx", "bgpalfx":
					counts[strings.ToLower(ctrl.Type)]++
				}
			}
		}
		if meta.Dependent != "" && (meta.Dependent != "Explod" && meta.Dependent != "Projectile" || meta.DependentCount != 1 || dependentCount != 1) {
			t.Fatal(path, "dependent consumer intent", meta.Dependent, dependentCount)
		}
		if len(counts) != len(meta.Kinds) {
			t.Fatal(path, counts, meta.Kinds)
		}
		for kind, want := range meta.Kinds {
			if (kind != "AllPalFX" && kind != "BGPalFX") || want < 1 || counts[strings.ToLower(kind)] != want {
				t.Fatal(path, kind, counts, meta.Kinds)
			}
		}
	}
}
