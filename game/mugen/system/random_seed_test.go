package main

import "testing"

func TestExplicitInitialMatchSeedAndFollowingMatches(t *testing.T) {
	e := &Engine{pendingMatchSeed: 16807}
	for _, want := range []int64{16807, 282475249, 1622650073} {
		if got := e.nextMatchSeed(); got != want {
			t.Fatalf("match seed: got %d, want %d", got, want)
		}
	}
}
