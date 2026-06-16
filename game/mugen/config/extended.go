package config

import (
	"os"

	"gopkg.in/yaml.v3"
)

// PlayerConfig holds both keyboard and joystick settings for a player.
type PlayerConfig struct {
	Keyboard map[string]string `yaml:"keyboard"`
	Joy      map[string]string `yaml:"joy"`
}

// ExtendedConfig provides modern overrides for MUGEN settings.
type ExtendedConfig struct {
	Input struct {
		P1 PlayerConfig `yaml:"p1"`
		P2 PlayerConfig `yaml:"p2"`
	} `yaml:"input"`
	Video struct {
		Width      int  `yaml:"width"`
		Height     int  `yaml:"height"`
		FullScreen bool `yaml:"fullscreen"`
		VSync      bool `yaml:"vsync"`
	} `yaml:"video"`
}

// LoadExtendedConfig reads and parses the YAML configuration at the given path.
func LoadExtendedConfig(path string) (*ExtendedConfig, error) {
	f, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	cfg := &ExtendedConfig{}
	decoder := yaml.NewDecoder(f)
	if err := decoder.Decode(cfg); err != nil {
		return nil, err
	}

	return cfg, nil
}
