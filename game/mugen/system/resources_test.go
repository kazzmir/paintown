package main

import (
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/motif"
)

func TestSelectionUsesResolvedMotifFile(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "theme", "select.def")
	if err := os.MkdirAll(filepath.Dir(path), 0755); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(path, []byte("[Characters]\nrandomselect\n"), 0600); err != nil {
		t.Fatal(err)
	}
	m := &motif.Motif{}
	m.Files.Select = path
	state := NewSelectState(&Engine{dataDir: dir, motif: m}, "Versus Mode")
	if len(state.chars) != 1 || state.chars[0].Name != "Random" {
		t.Fatal("resolved selection file was prefixed with the data directory again")
	}
}

func TestStoryboardStatePropagatesLoadFailureAndSkipsBlank(t *testing.T) {
	dir := t.TempDir()
	next := &TitleState{}
	state := &StoryboardState{engine: &Engine{dataDir: dir}, storyboard: "missing.def", nextState: func() State { return next }}
	got, err := state.Update()
	if got != state || err == nil || !strings.Contains(err.Error(), "missing.def") {
		t.Fatalf("storyboard load failure was skipped: state=%T err=%v", got, err)
	}
	state = &StoryboardState{engine: &Engine{dataDir: dir}, nextState: func() State { return next }}
	got, err = state.Update()
	if got != next || err != nil {
		t.Fatalf("blank optional storyboard did not advance: state=%T err=%v", got, err)
	}
}
