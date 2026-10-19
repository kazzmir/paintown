package match

import (
	"path/filepath"
	"strings"
	"testing"
)

func TestNativeRoundResultPrograms(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/roundresult-versus-*.json")
	var captures []string
	for _, path := range paths {
		if !strings.HasSuffix(path, ".round.json") {
			captures = append(captures, path)
		}
	}
	paths = captures
	if err != nil || len(paths) != 16 {
		t.Fatal("missing native result corpus", err)
	}
	for _, path := range paths {
		t.Run(filepath.Base(path), func(t *testing.T) { replayNativeRound(t, path) })
	}
}

func TestNativeRoundPersistence(t *testing.T) {
	replayNativeRound(t, "../tests/testdata/reference/roundreset-persistence-v2.json")
}

func TestNativeControllerDeath(t *testing.T) {
	for _, name := range []string{"rounddeath-controller", "rounddeath-latch"} {
		t.Run(name, func(t *testing.T) { replayNativeRound(t, "../tests/testdata/reference/"+name+".json") })
	}
}

func TestNativeLateRoundContact(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/roundlate-projectile-*-recover.json")
	if err != nil || len(paths) != 7 {
		t.Fatal("missing native late contact corpus", err)
	}
	for _, path := range paths {
		t.Run(filepath.Base(path), func(t *testing.T) { replayNativeRound(t, path) })
	}
}
