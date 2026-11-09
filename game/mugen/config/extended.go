package config

import (
	"fmt"
	"os"

	"gopkg.in/yaml.v3"
)

// KeyBinding accepts either the original single-key scalar or alternative keys.
type KeyBinding []string

func (binding *KeyBinding) UnmarshalYAML(node *yaml.Node) error {
	switch node.Kind {
	case yaml.ScalarNode:
		var name string
		if err := node.Decode(&name); err != nil {
			return err
		}
		*binding = KeyBinding{name}
		return nil
	case yaml.SequenceNode:
		var names []string
		if err := node.Decode(&names); err != nil {
			return err
		}
		*binding = names
		return nil
	default:
		return fmt.Errorf("line %d: input binding must be a control name or a list of control names", node.Line)
	}
}

// PlayerConfig holds both keyboard and joystick settings for a player.
type PlayerConfig struct {
	KeyboardEnabled *bool                 `yaml:"keyboard_enabled"`
	Keyboard        map[string]KeyBinding `yaml:"keyboard"`
	Joy             map[string]KeyBinding `yaml:"joy"`
	Gamepad         *GamepadOptions       `yaml:"gamepad"`
}

type GamepadOptions struct {
	Enabled  *bool    `yaml:"enabled"`
	Deadzone *float64 `yaml:"deadzone"`
}

// ExtendedConfig provides modern overrides for MUGEN settings.
type ExtendedConfig struct {
	Input struct {
		P1 PlayerConfig `yaml:"p1"`
		P2 PlayerConfig `yaml:"p2"`
	} `yaml:"input"`
	Video struct {
		Width      int   `yaml:"width"`
		Height     int   `yaml:"height"`
		FullScreen *bool `yaml:"fullscreen"`
		VSync      *bool `yaml:"vsync"`
		StageFit   *bool `yaml:"stage_fit"`
		SystemFit  *bool `yaml:"system_fit"`
		KeepAspect *bool `yaml:"keep_aspect"`
	} `yaml:"video"`
	Sound struct {
		Enabled      *bool `yaml:"enabled"`
		MasterVolume *int  `yaml:"master_volume"`
		WavVolume    *int  `yaml:"wav_volume"`
		BGMVolume    *int  `yaml:"bgm_volume"`
	} `yaml:"sound"`
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
