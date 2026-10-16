package tests

import (
	"os"
	"path/filepath"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/motif"
	"github.com/kazzmir/paintown/game/mugen/stage"
)

func TestStockCoordinateProfiles(t *testing.T) {
	for _, test := range []struct {
		name          string
		width, height int
	}{{"kfm", 320, 240}, {"kfm720", 1280, 720}} {
		player, err := character.LoadPlayer(referencePath(t, "chars", test.name), test.name+".def")
		if err != nil {
			t.Fatal(err)
		}
		if player.LocalCoord != (coord.Space{Width: test.width, Height: test.height}) || player.Character.LocalCoordinateWidth() != test.width {
			t.Fatalf("%s profile was lost: %v", test.name, player.LocalCoord)
		}
	}
	for _, test := range []struct {
		path          string
		width, height int
	}{{"data/system.def", 320, 240}, {"data/kfm/system.def", 320, 240}, {"data/mugen1/system.def", 1280, 720}, {"data/big/system.def", 1280, 720}} {
		m, err := motif.LoadMotif(test.path, referenceRoot(t))
		if err != nil {
			t.Fatal(err)
		}
		if m.Info.LocalCoord != (coord.Space{Width: test.width, Height: test.height}) || m.TitleBG.LocalCoord != m.Info.LocalCoord {
			t.Fatalf("%s motif/background profile was lost: %v", test.path, m.Info.LocalCoord)
		}
	}
	loaded, err := stage.LoadStage("stages/stage0-720.def", referenceRoot(t))
	if err != nil {
		t.Fatal(err)
	}
	if loaded.StageInfo.LocalCoord != (coord.Space{Width: 1280, Height: 720}) || loaded.Background.LocalCoord != loaded.StageInfo.LocalCoord {
		t.Fatal("stage profile was lost")
	}
	for _, images := range loaded.SpriteImages {
		if images.Opaque != nil && images.Opaque != images.Masked {
			images.Opaque.Dispose()
		}
		if images.Masked != nil {
			images.Masked.Dispose()
		}
	}
	cfg, err := config.LoadConfigWithOverrides(referenceRoot(t))
	if err != nil || cfg.Config.GameWidth != 1280 || cfg.Config.GameHeight != 720 {
		t.Fatalf("game space was confused with video window dimensions: %v %v", cfg, err)
	}
}

func TestInvalidLocalCoordinateSources(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "invalid.def")
	writeResource(t, path, []byte("[Info]\nlocalcoord=0,240\n"))
	if _, err := motif.LoadMotif(path, dir); err == nil {
		t.Fatal("invalid motif dimensions accepted")
	}
	if _, err := character.LoadPlayer(dir, filepath.Base(path)); err == nil {
		t.Fatal("invalid player dimensions accepted")
	}
	writeResource(t, path, []byte("[StageInfo]\nlocalcoord=320,-1\n"))
	if _, err := stage.LoadStage(path, dir); err == nil {
		t.Fatal("invalid stage dimensions accepted")
	}
	if err := os.MkdirAll(filepath.Join(dir, "data"), 0755); err != nil {
		t.Fatal(err)
	}
	writeResource(t, filepath.Join(dir, "data", "mugen.cfg"), []byte("[Config]\nGameWidth=0\nGameHeight=720\n"))
	if _, err := config.LoadConfigWithOverrides(dir); err == nil {
		t.Fatal("invalid game dimensions accepted")
	}
}
