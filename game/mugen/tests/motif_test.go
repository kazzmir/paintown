package tests

import (
	"os"
	"path/filepath"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/motif"
)

func TestLoadMotif(t *testing.T) {
	// Create a temporary data directory structure
	tmpDir, err := os.MkdirTemp("", "mugen-motif-test")
	if err != nil {
		t.Fatal(err)
	}
	defer os.RemoveAll(tmpDir)

	dataDir := filepath.Join(tmpDir, "data")
	if err := os.MkdirAll(dataDir, 0755); err != nil {
		t.Fatal(err)
	}

	motifContent := `
[Info]
name = "Test Motif"
author = "Antigravity"

[Files]
spr = system.sff
snd = system.snd
font1 = font/f-4x6.fnt

[Music]
title.bgm = sound/title.mp3
title.bgm.loop = 1

[Title Info]
fadein.time = 10
menu.pos = 160,150
menu.itemname.arcade = "ARCADE"
`
	motifDefPath := filepath.Join(dataDir, "system.def")
	if err := os.WriteFile(motifDefPath, []byte(motifContent), 0644); err != nil {
		t.Fatal(err)
	}

	for target, source := range map[string]string{
		filepath.Join(dataDir, "system.sff"):       referencePath(t, "data", "system.sff"),
		filepath.Join(dataDir, "system.snd"):       referencePath(t, "data", "system.snd"),
		filepath.Join(tmpDir, "font", "f-4x6.fnt"): referencePath(t, "font", "f-4x6.fnt"),
	} {
		data, err := os.ReadFile(source)
		if err != nil {
			t.Fatal(err)
		}
		writeResource(t, target, data)
	}

	m, err := motif.LoadMotif(motifDefPath, tmpDir)
	if err != nil {
		t.Fatalf("failed to load motif: %v", err)
	}

	if m.Info.Name != "Test Motif" {
		t.Errorf("expected Name 'Test Motif', got '%s'", m.Info.Name)
	}
	if m.SpriteData == nil || m.SoundBank == nil || m.Fonts[1] == nil {
		t.Fatal("declared sprite, sound and font assets must actually load")
	}
	if m.Files.Spr != "system.sff" {
		t.Errorf("expected Spr 'system.sff', got '%s'", m.Files.Spr)
	}
	if m.Music.TitleBGM != "sound/title.mp3" || !m.Music.TitleBGMLoop {
		t.Errorf("expected TitleBGM 'sound/title.mp3' looped, got '%s' loop=%v", m.Music.TitleBGM, m.Music.TitleBGMLoop)
	}
	if m.TitleInfo.FadeInTime != 10 {
		t.Errorf("expected FadeInTime 10, got %d", m.TitleInfo.FadeInTime)
	}
	if m.TitleInfo.MenuPos[0] != 160 || m.TitleInfo.MenuPos[1] != 150 {
		t.Errorf("expected MenuPos 160,150, got %v", m.TitleInfo.MenuPos)
	}
	if m.TitleInfo.ItemNames["arcade"] != "ARCADE" {
		t.Errorf("expected ItemName arcade 'ARCADE', got '%s'", m.TitleInfo.ItemNames["arcade"])
	}
}
