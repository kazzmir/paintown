package engine

import "testing"

func TestRandomStreamSeedNormalizationAndPeriodBoundary(t *testing.T) {
	for _, seed := range []int64{-1, 0, 2147483647, 4294967295} {
		r := NewRandomStream(seed)
		if r.InitialSeed() != 1 || r.Next() != 16807 {
			t.Fatal(seed, r)
		}
	}
	r := NewRandomStream(2147483646)
	if got := r.Next(); got != 2147466840 {
		t.Fatal(got)
	}
	// This uses the widened product; int32 multiplication would overflow.
	if got := r.Next(); got != 1865008398 {
		t.Fatal(got)
	}
}
