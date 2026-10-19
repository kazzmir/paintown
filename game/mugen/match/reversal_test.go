package match

import (
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestNativeReversalPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "reversal-*.json", 90)
}

func TestNativeReversalProjectileControllers(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/reversal-ps-*.cns")
	if err != nil || len(paths) != 10 {
		t.Fatalf("projectile fixture inventory: %d, %v", len(paths), err)
	}
	for _, path := range paths {
		file, err := os.Open(path)
		if err != nil {
			t.Fatal(err)
		}
		program, err := cns.ParseNamed(file, path)
		file.Close()
		if err != nil {
			t.Fatal(err)
		}
		count := 0
		for _, ctrl := range program.States[701].Controllers {
			switch strings.ToLower(ctrl.Type) {
			case "projectile":
				count++
			case "hitdef":
				t.Fatalf("%s substitutes HitDef for Projectile", path)
			}
		}
		if count != 1 {
			t.Fatalf("%s has %d Projectile controllers", path, count)
		}
	}
}
