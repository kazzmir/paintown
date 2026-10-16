package config

import (
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

func TestBindingPresenceAndNamedKeys(t *testing.T) {
	var keys Keys
	if _, present := keys.Binding("a"); present {
		t.Fatal("zero-value configuration invents a binding")
	}
	if !keys.SetBinding("a", ebiten.KeyA) || !keys.SetBinding("up", NoKey) {
		t.Fatal("recognized actions were rejected")
	}
	copy := keys
	copy.SetBinding("b", ebiten.KeyF24)
	if key, present := copy.Binding("a"); key != ebiten.KeyA || !present {
		t.Fatal("explicit KeyA was lost on copy")
	}
	if key, present := copy.Binding("jump"); key != NoKey || !present {
		t.Fatal("explicit disabled direction was lost")
	}
	if _, present := keys.Binding("b"); present {
		t.Fatal("copy shared mutable binding presence")
	}
	for _, name := range []string{"a", "KeyA", "KeyF24", "NumpadEqual", "KeyDigit2", "none"} {
		if _, valid := stringToKey(name); !valid {
			t.Errorf("recognized Ebitengine/legacy key name %q rejected", name)
		}
	}
	if key, valid := stringToKey("not-a-key"); valid || key != NoKey {
		t.Fatal("unknown key became a valid KeyA")
	}
}

func TestSDLKeySymbolsAndRawJoystickNumbers(t *testing.T) {
	ast, err := parsers.Parse(strings.NewReader("[P1 Keys]\na=0\nb=160\nx=282\ny=256\n[P1 Joystick]\njump=9\na=13\n"))
	if err != nil {
		t.Fatal(err)
	}
	var keys, joystick Keys
	parseKeys(ast.Sections[0], &keys, true)
	parseKeys(ast.Sections[1], &joystick, false)
	if key, present := keys.Binding("a"); key != NoKey || !present {
		t.Fatal("SDL UNKNOWN must be an explicit disabled binding")
	}
	if key, present := keys.Binding("b"); key != NoKey || !present {
		t.Fatal("unsupported layout symbol silently became another key")
	}
	if keys.X != ebiten.KeyF1 || keys.Y != ebiten.KeyNumpad0 || int(joystick.Jump) != 9 || int(joystick.A) != 13 {
		t.Fatalf("keyboard symbols and joystick device numbers were mixed: %v %v", keys, joystick)
	}
}

func TestYAMLKeyAOverridesAndDisabledBinding(t *testing.T) {
	cfg := &MugenConfig{}
	cfg.Input.Player1Keyboard.SetBinding("a", ebiten.KeyComma)
	cfg.Input.Player1Keyboard.SetBinding("jump", ebiten.KeyArrowUp)
	ext := &ExtendedConfig{}
	ext.Input.P1.Keyboard = map[string]string{"a": "KeyA", "jump": "none", "start": "KeyF24"}
	mergeExtendedConfig(cfg, ext)
	keys := cfg.Input.Player1Keyboard
	if key, present := keys.Binding("a"); !present || key != ebiten.KeyA {
		t.Fatal("YAML KeyA override was discarded as zero")
	}
	if key, present := keys.Binding("jump"); !present || key != NoKey || keys.Start != ebiten.KeyF24 {
		t.Fatal("disabled or named-key override was lost")
	}
}
