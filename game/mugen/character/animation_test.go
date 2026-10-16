package character

import (
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
)

func animationFixture(t *testing.T, controllers string) *Character {
	c := fixtureCharacter(t, controllers)
	c.AirFile = &air.Data{Actions: map[int]*air.Action{0: {ID: 0, LoopStart: 1, Elements: []air.Element{{Time: 2}, {Time: 3}, {Time: 4}}}}}
	c.ChangeAnim(0)
	return c
}

func TestCharacterAnimationTriggersAndHitPause(t *testing.T) {
	c := animationFixture(t, `[State 7, element event]
type=VarAdd
trigger1=AnimElem=2
v=0
value=1
[State 7, clock]
type=VarSet
trigger1=1
v=1
value=AnimTime
`)
	if c.GetAnimTime() != -9 {
		t.Fatal("initial animation time")
	}
	for i := 0; i < 9; i++ {
		c.Update()
	}
	if c.RuntimeError != nil {
		t.Fatal(c.RuntimeError)
	}
	if c.GetAnimTime() != 0 || c.AnimElem != 1 || c.IntVars[0] != 1 {
		t.Fatalf("loop boundary: time=%d element=%d events=%d", c.GetAnimTime(), c.AnimElem, c.IntVars[0])
	}
	c.ShakeTime = 2
	for i := 0; i < 2; i++ {
		c.Update()
	}
	if c.GetAnimTime() != 0 || c.AnimElemTime != 0 {
		t.Fatal("animation advanced during hitpause")
	}
	c.Update()
	if c.GetAnimTime() != -6 || c.IntVars[0] != 1 || c.IntVars[1] != 0 {
		t.Fatal("boundary query or event repeated")
	}
	if len(c.RuntimeWarnings) != 0 {
		t.Fatal(c.RuntimeWarnings)
	}
}

func TestChangeAnimElementAndMissingAction(t *testing.T) {
	c := animationFixture(t, `[State 7, select element]
type=ChangeAnim
trigger1=time=0
value=var(0)
elem=1+1
`)
	c.Update()
	if c.RuntimeError != nil {
		t.Fatal(c.RuntimeError)
	}
	if got, ok := c.GetAnimElemTime(2); !ok || got != 1 {
		t.Fatalf("element time=%d,%v", got, ok)
	}
	if c.AnimElem != 1 || c.GetAnimTime() != -6 {
		t.Fatal("ChangeAnim elem was ignored")
	}
	c.ChangeAnim(404)
	if c.Anim != 0 || c.RuntimeError != nil || len(c.RuntimeWarnings) != 1 {
		t.Fatal("missing action must warn and preserve animation")
	}
}
