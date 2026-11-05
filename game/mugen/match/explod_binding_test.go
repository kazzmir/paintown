package match

import (
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestNativeExplodBindTimePrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "explodbind-*.json", 28)
}

// Labels are not controller types. Keep the observed binding corpus honest.
func TestNativeExplodBindTimeIntent(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/explodbind-*.json")
	if err != nil || len(paths) != 28 {
		t.Fatal(len(paths), err)
	}
	for _, path := range paths {
		path = strings.TrimSuffix(path, ".json") + ".cns"
		f, err := os.Open(path)
		if err != nil {
			t.Fatal(err)
		}
		data, err := cns.ParseNamed(f, path)
		f.Close()
		if err != nil {
			t.Fatal(err)
		}
		count := 0
		for _, ctrl := range data.States[700].Controllers {
			if strings.EqualFold(ctrl.Type, "ExplodBindTime") {
				count++
			}
		}
		if count != 1 {
			t.Fatal(path, "expected one real ExplodBindTime", count)
		}
	}
}
