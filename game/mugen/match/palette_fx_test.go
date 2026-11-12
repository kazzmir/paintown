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

func TestNativePaletteFXPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "palettefx-*.json", 53)
}

func TestNativePaletteFXIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/palettefx-*.json")
	if err != nil || len(paths) != 53 {
		t.Fatal(len(paths), err)
	}
	for _, path := range paths {
		var meta struct {
			Kind  string `json:"controller_kind"`
			State int    `json:"controller_state"`
			Count int    `json:"controller_count"`
		}
		raw, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		if err := json.Unmarshal(raw, &meta); err != nil {
			t.Fatal(err)
		}
		if meta.Kind != "PalFX" || (meta.State != 700 && meta.State != 710) || meta.Count < 0 || meta.Count > 2 {
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
		count := 0
		for _, ctrl := range file.States[meta.State].Controllers {
			if strings.EqualFold(ctrl.Type, "PalFX") {
				count++
			}
		}
		if count != meta.Count {
			t.Fatal(path, count, meta.Count)
		}
	}
}
