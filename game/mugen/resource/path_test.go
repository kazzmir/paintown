package resource

import (
	"errors"
	"io/fs"
	"os"
	"path/filepath"
	"testing"
)

func put(t *testing.T, path string) {
	t.Helper()
	if err := os.MkdirAll(filepath.Dir(path), 0755); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(path, []byte("asset"), 0600); err != nil {
		t.Fatal(err)
	}
}

func TestExplicitSearchAndWindowsNames(t *testing.T) {
	root := t.TempDir()
	first, second := filepath.Join(root, "first"), filepath.Join(root, "second")
	local := filepath.Join(first, "Assets", "A File.SFF")
	put(t, local)
	put(t, filepath.Join(second, "assets", "a file.sff"))
	got, err := Resolve(`"assets\a FILE.sff"`, first, second)
	if err != nil || got != local {
		t.Fatalf("path=%q err=%v", got, err)
	}
	got, err = Resolve(`assets\a file.sff`, filepath.Join(root, "missing"), second)
	if err != nil || got != filepath.Join(second, "assets", "a file.sff") {
		t.Fatal(got, err)
	}
	if _, err := Resolve("absent.sff", first, second); !errors.Is(err, fs.ErrNotExist) {
		t.Fatal(err)
	}
	if _, err := Resolve("", first); !errors.Is(err, fs.ErrNotExist) {
		t.Fatal(err)
	}
}

func TestAmbiguousCaseAndExactPreference(t *testing.T) {
	root := t.TempDir()
	put(t, filepath.Join(root, "One.SFF"))
	put(t, filepath.Join(root, "one.sff"))
	if _, err := Resolve("ONE.sff", root); err == nil || errors.Is(err, fs.ErrNotExist) {
		t.Fatal("ambiguous path was silently chosen", err)
	}
	if got, err := Resolve("one.sff", root); err != nil || got != filepath.Join(root, "one.sff") {
		t.Fatal(got, err)
	}
	if _, err := Resolve("One.SFF"); !errors.Is(err, fs.ErrNotExist) {
		t.Fatal("implicit current-directory search", err)
	}
	// An exact parent must win even when only the child's case needs folding.
	put(t, filepath.Join(root, "Assets", "Sprite.SFF"))
	put(t, filepath.Join(root, "assets", "Sprite.SFF"))
	if got, err := Resolve("Assets/sprite.sff", root); err != nil || got != filepath.Join(root, "Assets", "Sprite.SFF") {
		t.Fatal(got, err)
	}
}
