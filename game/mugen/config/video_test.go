package config

import (
	"os"
	"path/filepath"
	"testing"
)

func TestStockVideoFitAndPresenceAwareOverrides(t *testing.T) {
	cfg, err := LoadConfigWithOverrides("../temp/mugen-1.1b1")
	if err != nil {
		t.Fatal(err)
	}
	if !cfg.Video.StageFit || !cfg.Video.SystemFit || !cfg.Video.KeepAspect || cfg.Video.RenderMode != "OpenGL" {
		t.Fatal("stock fit settings lost", cfg.Video)
	}
	cfg.Video.FullScreen, cfg.Video.VRetrace = true, true
	mergeExtendedConfig(cfg, &ExtendedConfig{})
	if !cfg.Video.FullScreen || !cfg.Video.VRetrace || !cfg.Video.StageFit || !cfg.Video.KeepAspect {
		t.Fatal("absent YAML values reset video settings")
	}
	path := filepath.Join(t.TempDir(), "video.yaml")
	if err := os.WriteFile(path, []byte("video:\n  stage_fit: false\n  system_fit: false\n  keep_aspect: false\n  fullscreen: false\n  vsync: false\n"), 0600); err != nil {
		t.Fatal(err)
	}
	ext, err := LoadExtendedConfig(path)
	if err != nil {
		t.Fatal(err)
	}
	mergeExtendedConfig(cfg, ext)
	if cfg.Video.FullScreen || cfg.Video.VRetrace || cfg.Video.StageFit || cfg.Video.SystemFit || cfg.Video.KeepAspect {
		t.Fatal("explicit false video overrides ignored", cfg.Video)
	}
}

func TestLegacyExplicitVideoFitFlags(t *testing.T) {
	root := t.TempDir()
	if err := os.Mkdir(filepath.Join(root, "data"), 0700); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(filepath.Join(root, "data/mugen.cfg"), []byte("[Video]\nStageFit=0\nSystemFit=0\nKeepAspect=0\nRenderMode=OpenGL\n"), 0600); err != nil {
		t.Fatal(err)
	}
	cfg, err := LoadConfigWithOverrides(root)
	if err != nil {
		t.Fatal(err)
	}
	if cfg.Video.StageFit || cfg.Video.SystemFit || cfg.Video.KeepAspect {
		t.Fatal("legacy zero fit flags lost", cfg.Video)
	}
}
