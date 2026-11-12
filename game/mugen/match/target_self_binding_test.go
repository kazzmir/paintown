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

func TestNativeTargetSelfBindingPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "targetself-*.json", 73)
}

func TestNativeTargetSelfBindingIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/targetself-*.json")
	if err != nil || len(paths) != 73 {
		t.Fatal(len(paths), err)
	}
	for _, path := range paths {
		raw, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		var meta struct {
			Kind  string `json:"controller_kind"`
			State int    `json:"controller_state"`
			Count int    `json:"controller_count"`
		}
		if err := json.Unmarshal(raw, &meta); err != nil {
			t.Fatal(err)
		}
		if meta.Kind != "BindToTarget" || meta.State != 700 || meta.Count < 1 {
			t.Fatal(path, meta)
		}
		raw, err = os.ReadFile(strings.TrimSuffix(path, ".json") + ".cns")
		if err != nil {
			t.Fatal(err)
		}
		data, err := cns.ParseNamed(bytes.NewReader(raw), path)
		if err != nil {
			t.Fatal(err)
		}
		count := 0
		for _, ctrl := range data.States[meta.State].Controllers {
			if strings.EqualFold(ctrl.Type, meta.Kind) {
				count++
			}
		}
		if count != meta.Count {
			t.Fatal(path, "wrong actual controller count", count, meta.Count)
		}
	}
}
