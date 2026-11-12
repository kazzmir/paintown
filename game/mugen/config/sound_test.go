package config

import (
	"os"
	"path/filepath"
	"strings"
	"testing"
)

func TestStockSoundSettingsAndExplicitZeroOverrides(t *testing.T) {
	cfg, err := LoadConfigWithOverrides("../temp/mugen-1.1b1")
	if err != nil {
		t.Fatal(err)
	}
	if cfg.Sound != DefaultSoundConfig() {
		t.Fatalf("stock settings lost: %+v", cfg.Sound)
	}
	path := filepath.Join(t.TempDir(), "sound.yaml")
	if err := os.WriteFile(path, []byte("sound:\n  enabled: false\n  master_volume: 0\n  wav_volume: 0\n  bgm_volume: 0\n"), 0600); err != nil {
		t.Fatal(err)
	}
	ext, err := LoadExtendedConfig(path)
	if err != nil {
		t.Fatal(err)
	}
	mergeExtendedConfig(cfg, ext)
	if cfg.Sound.Sound || cfg.Sound.EffectsVolume() != 0 || cfg.Sound.MasterVolume != 0 || cfg.Sound.WavVolume != 0 || cfg.Sound.BGMVolume != 0 {
		t.Fatal("explicit false/zero overrides lost", cfg.Sound)
	}
	if cfg.Sound.SampleRate != 44100 || cfg.Sound.BufferSize != 11 || cfg.Sound.WavChannels != 32 {
		t.Fatal("absent overrides changed device settings")
	}
}

func TestInvalidSoundConfigurationHasSourceContext(t *testing.T) {
	root := t.TempDir()
	if err := os.Mkdir(filepath.Join(root, "data"), 0700); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(filepath.Join(root, "data/mugen.cfg"), []byte("[Sound]\nBufferSize=31\n"), 0600); err != nil {
		t.Fatal(err)
	}
	_, err := LoadConfigWithOverrides(root)
	if err == nil || !strings.Contains(err.Error(), "mugen.cfg [Sound]: BufferSize") {
		t.Fatal("missing contextual device validation", err)
	}
}
