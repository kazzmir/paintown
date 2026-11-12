package character

import (
	"github.com/kazzmir/paintown/game/mugen/evaluator"
	"testing"
)

func TestHitPauseTimeBindingUsesAttackClock(t *testing.T) {
	expr, err := evaluator.ParseReference("HitPauseTime")
	if err != nil {
		t.Fatal(err)
	}
	c := &Character{ShakeTime: 19}
	for _, clock := range []int{5, 1, 0, -3} {
		c.HitPauseTime = clock
		got := evaluator.EvaluateValue(expr, c)
		if got.Kind() != evaluator.IntKind || got.Number() != float64(max(0, clock)) {
			t.Fatal(clock, got)
		}
	}
}
