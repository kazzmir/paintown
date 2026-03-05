package tests

import (
	"os"
	"path/filepath"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/config"
)

func TestLoadConfig(t *testing.T) {
	// Create a temporary data directory structure
	tmpDir, err := os.MkdirTemp("", "mugen-test")
	if err != nil {
		t.Fatal(err)
	}
	defer os.RemoveAll(tmpDir)

	dataDir := filepath.Join(tmpDir, "data")
	if err := os.MkdirAll(dataDir, 0755); err != nil {
		t.Fatal(err)
	}

	cfgContent := `
[Options]
Difficulty = 4
Life = 100
Time = 99
GameSpeed = 0
Motif = data/system.def

[Rules]
GameType = VS
Default.Attack.LifeToPowerMul = .7
Default.GetHit.LifeToPowerMul = .6
Super.TargetDefenceMul = 1.5

[Config]
GameSpeed = 60
DrawShadows = 1
AfterImageMax = 8

[Video Linux]
Width  = 320
Height = 240
Stretch = 1
`
	if err := os.WriteFile(filepath.Join(dataDir, "mugen.cfg"), []byte(cfgContent), 0644); err != nil {
		t.Fatal(err)
	}

	cfg, err := config.LoadConfig(tmpDir)
	if err != nil {
		t.Fatalf("failed to load config: %v", err)
	}

	if cfg.Options.Difficulty != 4 {
		t.Errorf("expected Difficulty 4, got %d", cfg.Options.Difficulty)
	}
	if cfg.Options.Motif != "data/system.def" {
		t.Errorf("expected Motif data/system.def, got %s", cfg.Options.Motif)
	}
	if cfg.Rules.DefaultAttackLifeToPowerMul != 0.7 {
		t.Errorf("expected DefaultAttackLifeToPowerMul 0.7, got %f", cfg.Rules.DefaultAttackLifeToPowerMul)
	}
	if cfg.Config.GameSpeed != 60 {
		t.Errorf("expected Config.GameSpeed 60, got %d", cfg.Config.GameSpeed)
	}
	if !cfg.Config.DrawShadows {
		t.Errorf("expected DrawShadows true, got false")
	}
	if cfg.Video.Width != 320 || cfg.Video.Height != 240 {
		t.Errorf("expected Video 320x240, got %dx%d", cfg.Video.Width, cfg.Video.Height)
	}
}
