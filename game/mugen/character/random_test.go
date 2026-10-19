package character

import (
	"github.com/kazzmir/paintown/game/mugen/evaluator"
	"testing"
)

func TestRandomWorldSurvivesRootResetAndStaysIsolated(t *testing.T) {
	w := NewWorld(56)
	w.SeedRandom(1856838958)
	a, b := &Character{}, &Character{}
	if err := w.AddRoots(a, b); err != nil {
		t.Fatal(err)
	}
	expr, err := evaluator.ParseReference("Random")
	if err != nil {
		t.Fatal(err)
	}
	draw := func(c *Character, want int) {
		t.Helper()
		got := evaluator.EvaluateValue(expr, c)
		if got.Kind() != evaluator.IntKind || got.Number() != float64(want) {
			t.Fatal(got, want)
		}
	}
	draw(a, 902)
	draw(b, 159)
	c, d := &Character{}, &Character{}
	if err := w.ResetRoots(c, d); err != nil {
		t.Fatal(err)
	}
	draw(c, 781)
	other := NewWorld(56)
	other.SeedRandom(1856838958)
	fresh := &Character{}
	if err := other.AddRoots(fresh); err != nil {
		t.Fatal(err)
	}
	draw(fresh, 902)
	draw(d, 213)
}
