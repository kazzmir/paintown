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

func TestStoryboardLayoutUsesWindowInsteadOfMotifCanvas(t *testing.T) {
	engine := &Engine{logicalWidth: 1280, logicalHeight: 720}
	engine.currentState = &StoryboardState{engine: engine}
	if w, h := engine.Layout(640, 480); w != 640 || h != 480 {
		t.Fatalf("storyboard output canvas=%dx%d", w, h)
	}
	if w, h := engine.Layout(0, 0); w != 1 || h != 1 {
		t.Fatalf("minimized storyboard output canvas=%dx%d", w, h)
	}
	engine.currentState = &TitleState{}
	if w, h := engine.Layout(640, 480); w != 1280 || h != 720 {
		t.Fatalf("title motif canvas=%dx%d", w, h)
	}
}
