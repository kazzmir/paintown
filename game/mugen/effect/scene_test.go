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

func TestSceneHonorsSeparatePauseAllowances(t *testing.T) {
	scene := NewScene(8)
	request := testRequest()
	request.Source.Animations.Actions[7] = &air.Action{ID: 7, LoopStart: -1, Elements: []air.Element{{Time: 10}, {Time: -1, XOffset: 20}}}
	for _, allowances := range [][2]int{{0, 0}, {0, 30}, {2, 0}} {
		request.PauseMoveTime, request.SuperMoveTime = allowances[0], allowances[1]
		if err := scene.Queue.Push(request); err != nil {
			t.Fatal(err)
		}
	}
	if err := scene.Flush(); err != nil {
		t.Fatal(err)
	}
	for tick := 0; tick < 10; tick++ {
		if err := scene.AdvanceWithPause(true, true); err != nil {
			t.Fatal(err)
		}
	}
	if scene.Active[0].Clock.Elapsed != 0 || scene.Active[1].Clock.CurrentElement().XOffset != 20 || scene.Active[2].Clock.Elapsed != 0 {
		t.Fatal("superpause did not separate effect allowances")
	}
	for tick := 0; tick < 3; tick++ {
		if err := scene.AdvanceWithPause(true, false); err != nil {
			t.Fatal(err)
		}
	}
	if scene.Active[1].Clock.Elapsed != 10 || scene.Active[2].Clock.Elapsed != 2 {
		t.Fatal("ordinary pause used the super allowance")
	}
	if err := scene.Advance(); err != nil {
		t.Fatal(err)
	}
	if scene.Active[0].Clock.Elapsed != 1 || scene.Active[1].Clock.Elapsed != 11 || scene.Active[2].Clock.Elapsed != 3 {
		t.Fatal("effect clocks did not resume")
	}
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

func TestOwnedEffectSharesClockAndRetiresImmediately(t *testing.T) {
	scene := NewScene(8)
	r := testRequest()
	lifetime := -1
	r.RemoveTime = &lifetime
	owned := &Instance{Request: r, Clock: air.NewPlayer(r.Source.Animations.Actions[7]), Active: true, OwnerDriven: true}
	if err := scene.Queue.PushOwned(owned); err != nil {
		t.Fatal(err)
	}
	if err := scene.Flush(); err != nil || scene.Active[0] != owned {
		t.Fatal("owned instance was copied instead of shared", err)
	}
	for tick := 0; tick < 10; tick++ {
		if err := scene.Advance(); err != nil {
			t.Fatal(err)
		}
		if owned.Age != tick {
			t.Fatal("render scene advanced an owner-driven clock", owned.Age, tick)
		}
		if err := owned.Advance(false, false); err != nil {
			t.Fatal(err)
		}
	}
	if !owned.Active || owned.Age != 10 {
		t.Fatal("indefinite owned animation expired on its first pass")
	}
	owned.Active = false
	if err := scene.Advance(); err != nil || len(scene.Active) != 0 {
		t.Fatal("owner removal left a visible instance", err)
	}
	queued := &Instance{Request: r, Clock: air.NewPlayer(r.Source.Animations.Actions[7]), Active: true, OwnerDriven: true}
	if err := scene.Queue.PushOwned(queued); err != nil {
		t.Fatal(err)
	}
	scene.Clear()
	if queued.Active || len(scene.Queue.Drain()) != 0 {
		t.Fatal("scene retirement retained an owned request")
	}
}
