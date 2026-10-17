package character

import (
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

func TestOrphanExplodClockExpiryBindingAndRoundReset(t *testing.T) {
	file, err := cns.Parse(strings.NewReader("[Statedef 0]\nphysics=N\nanim=0\n[State 0, wait]\ntype=Null\ntrigger1=1\n"))
	if err != nil {
		t.Fatal(err)
	}
	frames, err := air.Parse(strings.NewReader("[Begin Action 0]\n-1,-1,0,0,-1\n"))
	if err != nil {
		t.Fatal(err)
	}
	root, other := NewCharacter(file, frames, nil), NewCharacter(file, frames, nil)
	world := NewWorld(56)
	if err := world.AddRoots(root, other); err != nil {
		t.Fatal(err)
	}
	spawn := func(lifetime int) (*Character, *ExplodData) {
		t.Helper()
		if err := root.CreateHelper(engine.HelperParameters{PosType: "p1", Type: "normal"}); err != nil {
			t.Fatal(err)
		}
		h := world.Helpers[len(world.Helpers)-1]
		if err := h.CreateExplod(engine.ExplodParameters{Action: 0, PosType: "p1", BindID: -2, BindTime: -1, RemoveTime: lifetime, Facing: 1, VFacing: 1}); err != nil {
			t.Fatal(err)
		}
		return h, h.Explods[0]
	}
	helper, e := spawn(4)
	other.binding = &actorBinding{anchor: helper, remaining: -1, generation: other.hitGeneration}
	if err := helper.DestroyHelper(engine.DestroySelfParameters{}); err != nil {
		t.Fatal(err)
	}
	if other.binding != nil || e.Anchor != nil || !e.Visual.Active {
		t.Fatal("retirement did not release bindings and retain the orphan")
	}
	for tick := 0; tick < 5; tick++ {
		if err := TickCharacters([]*Character{root, other}); err != nil {
			t.Fatal(err)
		}
		if tick < 4 && !e.Visual.Active {
			t.Fatal("orphan expired before its remaining lifetime", tick, e.Visual.Age)
		}
	}
	if e.Visual.Active || e.Visual.Age != 4 || len(world.orphanEffectOwners) != 0 {
		t.Fatal("retired helper froze or leaked its finite Explod", e.Visual, world.orphanEffectOwners)
	}
	helper, e = spawn(-1)
	if err := helper.DestroyHelper(engine.DestroySelfParameters{}); err != nil {
		t.Fatal(err)
	}
	if err := world.ResetRoots(NewCharacter(file, frames, nil), NewCharacter(file, frames, nil)); err != nil {
		t.Fatal(err)
	}
	if e.Visual.Active || len(world.orphanEffectOwners) != 0 {
		t.Fatal("round reset retained the orphan")
	}
}
