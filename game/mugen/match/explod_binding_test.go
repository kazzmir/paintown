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
	assertNativeControllerKind(t, "explodbind-*.json", "ExplodBindTime", 28)
}

func assertNativeControllerKind(t *testing.T, pattern, kind string, total int) {
	t.Helper()
	assertNativeControllerCount(t, pattern, kind, total, 1)
}

func assertNativeControllerCount(t *testing.T, pattern, kind string, total, expected int) {
	t.Helper()
	paths, err := filepath.Glob(filepath.Join("../tests/testdata/reference", pattern))
	if err != nil || len(paths) != total {
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
			if strings.EqualFold(ctrl.Type, kind) {
				count++
			}
		}
		if count != expected {
			t.Fatal(path, "expected", expected, "real", kind, "got", count)
		}
	}
}
