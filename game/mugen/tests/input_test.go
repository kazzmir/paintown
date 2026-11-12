package tests

import (
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
)

type heldKeyboard map[ebiten.Key]bool

func (k heldKeyboard) IsPressed(key ebiten.Key) bool     { return k[key] }
func (k heldKeyboard) IsJustPressed(key ebiten.Key) bool { return k[key] }

func TestStockKeyboardProducesFacingRelativeGameplayInput(t *testing.T) {
	cfg, err := config.LoadConfigWithOverrides(referenceRoot(t))
	if err != nil {
		t.Fatal(err)
	}
	manager := input.NewInputManager(cfg)
	manager.Keyboard = heldKeyboard{ebiten.KeyArrowRight: true, ebiten.KeyComma: true, ebiten.KeyA: true, ebiten.KeyU: true}
	previous := input.GlobalManager
	input.GlobalManager = manager
	defer func() { input.GlobalManager = previous }()
	if got := strings.Join(character.GetRawInputs(1, 1), ","); got != "DirForward,ButtonA" {
		t.Fatal("P1 stock symbols did not produce gameplay controls:", got)
	}
	if got := strings.Join(character.GetRawInputs(2, -1), ","); got != "DirForward,ButtonStart" {
		t.Fatal("P2 KeyA/facing conversion did not produce gameplay controls:", got)
	}
}
