package input

import (
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/config"
)

type testKeyboard struct {
	held, edges map[ebiten.Key]bool
	queries     int
}

func (k *testKeyboard) IsPressed(key ebiten.Key) bool     { k.queries++; return k.held[key] }
func (k *testKeyboard) IsJustPressed(key ebiten.Key) bool { k.queries++; return k.edges[key] }

func TestInjectedKeyboardZeroKeyAndPlayerConfirm(t *testing.T) {
	cfg := &config.MugenConfig{}
	cfg.Input.Player1Keyboard.SetBinding("a", ebiten.KeyA)
	cfg.Input.Player1Keyboard.SetBinding("b", config.NoKey)
	cfg.Input.Player2Keyboard.SetBinding("start", ebiten.KeyU)
	manager := NewInputManager(cfg)
	source := &testKeyboard{held: map[ebiten.Key]bool{ebiten.KeyA: true}, edges: map[ebiten.Key]bool{ebiten.KeyA: true, ebiten.KeyU: true}}
	manager.Keyboard = source
	if !manager.IsPressed(1, ActionA) || !manager.IsJustPressed(1, ActionA) || !manager.IsJustPressed(2, ActionEnter) || manager.IsJustPressed(1, ActionEnter) {
		t.Fatal("explicit KeyA or player-specific confirmation was lost")
	}
	before := source.queries
	if manager.IsPressed(1, ActionB) || manager.IsJustPressed(1, Action(999)) || source.queries != before {
		t.Fatal("unbound/invalid action polled the device")
	}
	if manager.getActionKey(config.Keys{}, ActionX) != ebiten.KeyA || manager.getActionKey(config.Keys{}, ActionA) != ebiten.KeyZ {
		t.Fatal("legacy defaults for absent bindings changed")
	}
}
