package main

import (
	"github.com/kazzmir/paintown/game/mugen/engine"
	"testing"
)

func TestSessionSelectsSeparateMatchStreams(t *testing.T) {
	e := &Engine{random: engine.NewRandomStream(1)}
	first, second := e.nextMatchSeed(), e.nextMatchSeed()
	if first != 16807 || second != 282475249 {
		t.Fatal(first, second)
	}
	a, b := engine.NewRandomStream(first), engine.NewRandomStream(second)
	if a.Next() != 282475249 || b.Next() != 1622650073 {
		t.Fatal(a, b)
	}
	// Match draws must not consume the session stream.
	for i := 0; i < 20; i++ {
		a.Next()
	}
	if got := e.nextMatchSeed(); got != 1622650073 {
		t.Fatal(got)
	}
}
