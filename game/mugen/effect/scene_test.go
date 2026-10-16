package effect

import (
	"image"
	"math"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

func testRequest() Request {
	return Request{Source: Source{Sprites: &sff.SFF{Sprites: []sff.Sprite{{Image: image.NewRGBA(image.Rect(0, 0, 1, 1))}}}, Animations: &air.Data{Actions: map[int]*air.Action{7: {ID: 7, LoopStart: 1, Elements: []air.Element{{Time: 2}, {Time: 3}}}}}, Scale: 1}, Action: 7, Facing: -1, X: 10, Y: -20}
}

func TestEffectInstancesKeepIndependentClocksAndExpireAfterOnePass(t *testing.T) {
	s := NewScene(8)
	r := testRequest()
	if err := s.Queue.Push(r); err != nil {
		t.Fatal(err)
	}
	if err := s.Flush(); err != nil {
		t.Fatal(err)
	}
	if err := s.Advance(); err != nil {
		t.Fatal(err)
	}
	if err := s.Queue.Push(r); err != nil {
		t.Fatal(err)
	}
	if err := s.Flush(); err != nil {
		t.Fatal(err)
	}
	if s.Active[0].Clock.Elapsed != 1 || s.Active[1].Clock.Elapsed != 0 {
		t.Fatal("new effect advanced early or clocks were shared")
	}
	for i := 0; i < 4; i++ {
		if err := s.Advance(); err != nil {
			t.Fatal(err)
		}
	}
	if len(s.Active) != 1 || s.Active[0].Clock.Elapsed != 4 {
		t.Fatal("finite effect looped past its first pass", len(s.Active))
	}
	if err := s.Advance(); err != nil || len(s.Active) != 0 {
		t.Fatal("effect did not expire", err)
	}
	if len(r.Source.Animations.Actions[7].Elements) != 2 {
		t.Fatal("scene changed shared AIR")
	}
}

func TestInfiniteEffectAndSceneRetirement(t *testing.T) {
	s := NewScene(8)
	r := testRequest()
	r.Source.Animations.Actions[7] = &air.Action{ID: 7, LoopStart: -1, Elements: []air.Element{{Time: -1}}}
	if err := s.Queue.Push(r); err != nil {
		t.Fatal(err)
	}
	if err := s.Flush(); err != nil {
		t.Fatal(err)
	}
	for i := 0; i < 100; i++ {
		if err := s.Advance(); err != nil {
			t.Fatal(err)
		}
	}
	if len(s.Active) != 1 {
		t.Fatal("infinite marker expired")
	}
	if err := s.Queue.Push(r); err != nil {
		t.Fatal(err)
	}
	s.Clear()
	if len(s.Active) != 0 || len(s.Queue.Drain()) != 0 {
		t.Fatal("retired scene retained queued/active resources")
	}
}

func TestEffectErrorsAreDiagnosed(t *testing.T) {
	for _, mutate := range []func(*Request){func(r *Request) { r.Source.Scale = 0 }, func(r *Request) { r.X = math.Inf(1) }, func(r *Request) { r.Action = 404 }, func(r *Request) { r.Source.Sprites = nil }, func(r *Request) { r.Facing = 0 }} {
		s := NewScene(8)
		r := testRequest()
		mutate(&r)
		if err := s.Queue.Push(r); err != nil {
			t.Fatal(err)
		}
		if err := s.Flush(); err == nil {
			t.Fatal("invalid effect silently accepted", r)
		}
	}
}
