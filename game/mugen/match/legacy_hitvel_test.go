package match

import (
	"path/filepath"
	"testing"
)

func TestNativeLegacyHitVelocity(t *testing.T) {
	replayNativeGeometryPrograms(t, "legacyhitvel-*.json", 15)
}

func TestNativeLegacyHitVelocityProjectileCounterexamples(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/known-divergences/roundlate-projectile-*-detail.json")
	if err != nil || len(paths) != 3 {
		t.Fatal("missing preserved HitVel counterexamples", err)
	}
	for _, path := range paths {
		t.Run(filepath.Base(path), func(t *testing.T) { replayNativeRound(t, path) })
	}
}
