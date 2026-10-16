package tests

import (
	"fmt"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/storyboard"
)

func TestStockStoryboardResourcesAndPlayback(t *testing.T) {
	for _, path := range []string{"chars/kfm/intro.def", "chars/kfm/ending.def", "chars/kfm720/intro.def", "chars/kfm720/ending.def", "data/kfm/logo.def", "data/kfm/intro.def", "data/kfm/gameover.def", "data/kfm/credits.def"} {
		t.Run(path, func(t *testing.T) {
			story, err := storyboard.Load(path, referenceRoot(t))
			if err != nil {
				t.Fatal(err)
			}
			if story.SpriteData == nil || len(story.Sprites) == 0 || len(story.Scenes) == 0 || story.SpriteMap != nil {
				t.Fatal("CPU resources absent or images uploaded during loading")
			}
			if strings.HasSuffix(path, "data/kfm/intro.def") && (story.Scenes[0].Background == nil || len(story.Scenes[0].Background.Elements) == 0) {
				t.Fatal("storyboard background was discarded")
			}
			budget := 0
			for _, scene := range story.Scenes[story.StartScene:] {
				budget += scene.EndTime
			}
			player := storyboard.NewPlayer(story)
			done, ticks := false, 0
			for !done && ticks <= budget {
				done = player.Update(storyboard.InputState{})
				ticks++
			}
			if player.Error != nil || !done || ticks != budget {
				t.Fatalf("playback ticks=%d budget=%d done=%v error=%v", ticks, budget, done, player.Error)
			}
		})
	}
}

func storyboardSource(t *testing.T) string {
	t.Helper()
	return fmt.Sprintf("[SceneDef]\nspr=%s\n[Scene 0]\nend.time=3\nlayer99.anim=0\n[Begin Action 0]\n0,0,0,0,-1\n", referencePath(t, "chars", "kfm", "kfm.sff"))
}

func TestStoryboardResourcesLayersAndDefaults(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "scene.def")
	source := strings.Replace(storyboardSource(t), "[Scene 0]", "snd="+referencePath(t, "data", "system.snd")+"\nfont0=f-4x6.def\n[Scene 0]\nclearcolor=1,2,3\nlayerall.pos=12,34", 1)
	source += "[Scene 1]\nend.time=2\n[Scene 2]\nend.time=1\nclearcolor=-1\n"
	writeResource(t, path, []byte(source))
	story, err := storyboard.Load(path, referenceRoot(t))
	if err != nil {
		t.Fatal(err)
	}
	if story.Fonts[0] == nil || story.SoundBank == nil || len(story.Scenes[0].Layers) != 1 || story.Scenes[0].Layers[0].Index != 99 {
		t.Fatal("declared font/sound/layer99 resources were discarded")
	}
	if story.Scenes[1].DefaultPosition != [2]float64{12, 34} || !story.Scenes[1].ClearColorSet || story.Scenes[1].ClearColor != story.Scenes[0].ClearColor || story.Scenes[2].ClearColorSet || story.Scenes[0].MusicLoop {
		t.Fatal("documented inheritance, no-clear flag or music default lost")
	}
}

func TestStoryboardResourceAndSceneFailures(t *testing.T) {
	for _, test := range []struct{ name, old, replacement, want string }{
		{"missing sprite", "spr=" + referencePath(t, "chars", "kfm", "kfm.sff"), "spr=missing.sff", "[SceneDef] spr (line 2)"},
		{"missing sound", "[Scene 0]", "snd=missing.snd\n[Scene 0]", "[SceneDef] snd (line 3)"},
		{"missing font", "[Scene 0]", "font0=missing.fnt\n[Scene 0]", "[SceneDef] font0 (line 3)"},
		{"negative start", "[Scene 0]", "startscene=-1\n[Scene 0]", "startscene -1"},
		{"past last start", "[Scene 0]", "startscene=1\n[Scene 0]", "startscene 1"},
		{"missing end", "end.time=3", "", "end.time is required"},
		{"negative end", "end.time=3", "end.time=-1", "end.time is required"},
		{"missing action", "layer99.anim=0", "layer99.anim=42", "layer99.anim (line 5): action 42"},
	} {
		t.Run(test.name, func(t *testing.T) {
			path := filepath.Join(t.TempDir(), "scene.def")
			writeResource(t, path, []byte(strings.Replace(storyboardSource(t), test.old, test.replacement, 1)))
			_, err := storyboard.Load(path, referenceRoot(t))
			if err == nil || !strings.Contains(err.Error(), path) || !strings.Contains(err.Error(), test.want) {
				t.Fatalf("expected contextual failure containing %q: %v", test.want, err)
			}
		})
	}
}
