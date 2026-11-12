package match

import (
	"bytes"
	"encoding/json"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"os"
	"path/filepath"
	"strings"
	"testing"
)

func TestNativeActorDrawingPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "actordraw-*.json", 62)
}
func TestNativeActorDrawingIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/actordraw-*.json")
	if err != nil || len(paths) != 62 {
		t.Fatal(len(paths), err)
	}
	for _, path := range paths {
		raw, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		var meta struct {
			State  int            `json:"controller_state"`
			Counts map[string]int `json:"controller_counts"`
		}
		if err := json.Unmarshal(raw, &meta); err != nil {
			t.Fatal(err)
		}
		if meta.State != 700 || len(meta.Counts) != 5 {
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
		for kind, want := range meta.Counts {
			switch kind {
			case "AngleSet", "AngleAdd", "AngleMul", "AngleDraw", "Offset":
			default:
				t.Fatal(path, kind)
			}
			count := 0
			for _, ctrl := range file.States[meta.State].Controllers {
				if strings.EqualFold(ctrl.Type, kind) {
					count++
				}
			}
			if count != want {
				t.Fatal(path, kind, count, want)
			}
		}
	}
}
