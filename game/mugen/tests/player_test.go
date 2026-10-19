package tests

import (
	"sort"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/character"
)

func TestStockPlayerLoadAndCompilationInventory(t *testing.T) {
	for _, name := range []string{"kfm", "kfm720"} {
		t.Run(name, func(t *testing.T) {
			p, err := character.LoadPlayer(referencePath(t, "chars", name), name+".def")
			if err != nil {
				t.Fatal(err)
			}
			for _, id := range []int{-1, 0, 40, 200, 5000, 5900} {
				if _, ok := p.Character.StateFile.States[id]; !ok {
					t.Errorf("missing required stock state %d", id)
				}
			}
			ids := make([]int, 0, len(p.Character.StateFile.States))
			for id := range p.Character.StateFile.States {
				ids = append(ids, id)
			}
			sort.Ints(ids)
			for _, id := range ids {
				if err := p.Character.SM.CompileState(p.Character.StateFile.States[id]); err != nil {
					t.Errorf("%s state %d: %v", name, id, err)
				}
			}
			t.Logf("compiled %d/%d states, loaded %d sprites and %d animation actions", len(ids), len(ids), len(p.SFF.Sprites), len(p.AIR.Actions))
		})
	}
}
