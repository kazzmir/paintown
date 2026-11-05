package match

import (
	"bytes"
	"encoding/json"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestNativeShakeFrequencyPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "shakefreq-*.json", 12)
}

func TestNativeShakeFrequencyIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/shakefreq-*.json")
	if err != nil || len(paths) != 12 {
		t.Fatal(paths, err)
	}
	for _, path := range paths {
		var meta struct {
			Kind  string `json:"controller_kind"`
			Count int    `json:"controller_count"`
			Input struct {
				Source, Frequency string
				Phase             *string
				Amplitude, Time   int
			} `json:"frequency_control"`
		}
		raw, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		if err := json.Unmarshal(raw, &meta); err != nil {
			t.Fatal(err)
		}
		if meta.Kind != "HitDef" || meta.Count != 1 || meta.Input.Time != 8 {
			t.Fatal(path, meta)
		}
		prefix := "envshake."
		if meta.Input.Source == "fall" {
			prefix = "fall." + prefix
		} else if meta.Input.Source != "hit" {
			t.Fatal(path, meta.Input.Source)
		}
		raw, err = os.ReadFile(strings.TrimSuffix(path, ".json") + ".cns")
		if err != nil {
			t.Fatal(err)
		}
		file, err := cns.ParseNamed(bytes.NewReader(raw), path)
		if err != nil {
			t.Fatal(err)
		}
		hits, falls := 0, 0
		for number, state := range file.States {
			for _, ctrl := range state.Controllers {
				switch strings.ToLower(ctrl.Type) {
				case "hitdef":
					hits++
					if number != 700 || ctrl.Params[prefix+"freq"] != meta.Input.Frequency || ctrl.Params[prefix+"time"] != "8" || ctrl.Params[prefix+"ampl"] != strconv.Itoa(meta.Input.Amplitude) {
						t.Fatal(path, number, ctrl.Params)
					}
					phase, present := ctrl.Params[prefix+"phase"]
					if present != (meta.Input.Phase != nil) || present && phase != *meta.Input.Phase {
						t.Fatal(path, "phase", phase, meta.Input.Phase)
					}
				case "fallenvshake":
					falls++
					if meta.Input.Source != "fall" || number == 700 {
						t.Fatal(path, number, "unexpected FallEnvShake")
					}
				}
			}
		}
		wantFalls := 0
		if meta.Input.Source == "fall" {
			wantFalls = 8
		}
		if hits != 1 || falls != wantFalls {
			t.Fatal(path, hits, falls)
		}
	}
}
