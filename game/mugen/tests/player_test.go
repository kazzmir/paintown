package tests

import (
	"sort"
	"strings"
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
			// These three stock states require the still-missing HitOverride.
			// This is a regression inventory, not a complete-compilation gate.
			gaps := map[int]bool{1300: true, 1320: true, 1340: true}
			for _, id := range ids {
				err := p.Character.SM.CompileState(p.Character.StateFile.States[id])
				if gaps[id] {
					if err == nil || !strings.Contains(err.Error(), `unsupported controller type "HitOverride"`) {
						t.Errorf("state %d expected diagnosed HitOverride gap, got %v; update inventory when closed", id, err)
					}
				} else if err != nil {
					t.Errorf("%s state %d: %v", name, id, err)
				}
			}
			t.Logf("compiled %d/%d states (%d known HitOverride gaps), loaded %d sprites and %d animation actions", len(ids)-len(gaps), len(ids), len(gaps), len(p.SFF.Sprites), len(p.AIR.Actions))
		})
	}
}
