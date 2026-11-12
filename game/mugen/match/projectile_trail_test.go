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

func TestNativeProjectileTrailPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "trailprojectile-*.json", 49)
}

func TestNativeProjectileTrailIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/trailprojectile-*.json")
	if err != nil || len(paths) != 49 {
		t.Fatal(len(paths), err)
	}
	for _, path := range paths {
		data, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		var meta struct {
			Count  int               `json:"controller_count"`
			States []int             `json:"controller_states"`
			Fields map[string]string `json:"projectile_fields"`
			Kinds  map[string]int    `json:"actual_controller_counts"`
		}
		if err = json.Unmarshal(data, &meta); err != nil {
			t.Fatal(err)
		}
		name := strings.TrimSuffix(path, ".json") + ".cns"
		data, err = os.ReadFile(name)
		if err != nil {
			t.Fatal(err)
		}
		file, err := cns.ParseNamed(bytes.NewReader(data), name)
		if err != nil {
			t.Fatal(err)
		}
		kinds := map[string]int{}
		for number, state := range file.States {
			for _, ctrl := range state.Controllers {
				kind := strings.ToLower(ctrl.Type)
				kinds[kind]++
				if kind != "projectile" {
					continue
				}
				if number != 700 || len(ctrl.Params) != len(meta.Fields) {
					t.Fatal(path, number, ctrl.Params, meta.Fields)
				}
				for key, want := range meta.Fields {
					if ctrl.Params[key] != want {
						t.Fatal(path, key, ctrl.Params[key], want)
					}
				}
			}
		}
		if meta.Count != 1 || len(meta.States) != 1 || meta.States[0] != 700 || kinds["projectile"] != 1 || kinds["afterimage"] != 0 || kinds["hitdef"] != 0 || len(kinds) != len(meta.Kinds) {
			t.Fatal(path, meta, kinds)
		}
		for key, want := range meta.Kinds {
			if kinds[strings.ToLower(key)] != want {
				t.Fatal(path, key, kinds, meta.Kinds)
			}
		}
	}
}

func TestNativeProjectileTrailOwnershipAndClocks(t *testing.T) {
	replayNativeGeometryPrograms(t, "trailpool-*.json", 58)
	replayNativeGeometryPrograms(t, "trailphase-*.json", 52)
}

func TestNativeProjectileTrailAdditionalIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/trailp*.json")
	if err != nil {
		t.Fatal(err)
	}
	count := 0
	for _, path := range paths {
		if !strings.Contains(path, "/trailpool-") && !strings.Contains(path, "/trailphase-") {
			continue
		}
		count++
		data, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		var meta struct {
			Controllers []struct {
				State  int
				Type   string
				Params map[string]string
			} `json:"full_controller_intent"`
			Kinds  map[string]int `json:"actual_controller_counts"`
			States []int          `json:"declared_states"`
		}
		if err := json.Unmarshal(data, &meta); err != nil {
			t.Fatal(err)
		}
		source := strings.TrimSuffix(path, ".json") + ".cns"
		data, err = os.ReadFile(source)
		if err != nil {
			t.Fatal(err)
		}
		file, err := cns.ParseNamed(bytes.NewReader(data), source)
		if err != nil {
			t.Fatal(err)
		}
		kinds := map[string]int{}
		total := 0
		for number, state := range file.States {
			expected := 0
			for _, want := range meta.Controllers {
				if want.State != number {
					continue
				}
				if expected >= len(state.Controllers) {
					t.Fatal(path, number, "missing controller", want)
				}
				ctrl := state.Controllers[expected]
				expected++
				total++
				kinds[strings.ToLower(ctrl.Type)]++
				if !strings.EqualFold(ctrl.Type, want.Type) || len(ctrl.Params) != len(want.Params) {
					t.Fatal(path, number, ctrl, want)
				}
				for key, value := range want.Params {
					if ctrl.Params[key] != value {
						t.Fatal(path, number, key, ctrl.Params[key], value)
					}
				}
			}
			if expected == 0 || expected != len(state.Controllers) {
				t.Fatal(path, number, "empty or extra controllers")
			}
		}
		if total != len(meta.Controllers) || len(meta.States) != len(file.States) || len(kinds) != len(meta.Kinds) {
			t.Fatal(path, total, kinds, meta)
		}
		for key, value := range meta.Kinds {
			if kinds[strings.ToLower(key)] != value {
				t.Fatal(path, key, kinds, meta.Kinds)
			}
		}
	}
	if count != 110 {
		t.Fatal("incomplete additional trail intent corpus", count)
	}
}
