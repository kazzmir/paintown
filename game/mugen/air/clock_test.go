package air

import (
	"encoding/json"
	"os"
	"testing"
)

func loopAction() *Action {
	return &Action{ID: 9990, LoopStart: 1, Elements: []Element{{Time: 2}, {Time: 3}, {Time: 4}}}
}

func TestClockReferenceTrace(t *testing.T) {
	data, err := os.ReadFile("../tests/testdata/reference/animation-loop.json")
	if err != nil {
		t.Fatal(err)
	}
	var trace struct {
		Ticks []struct {
			Tick, Element, AnimTime int
			ElementTimes            [3]int
		}
	}
	if err := json.Unmarshal(data, &trace); err != nil {
		t.Fatal(err)
	}
	if len(trace.Ticks) != 24 {
		t.Fatal("reference trace is incomplete")
	}
	p := NewPlayer(loopAction())
	for _, want := range trace.Ticks {
		if p.Error != nil {
			t.Fatal(p.Error)
		}
		if p.CurrentIdx+1 != want.Element || p.AnimTime() != want.AnimTime {
			t.Fatalf("tick %d element=%d animtime=%d; want element=%d animtime=%d", want.Tick, p.CurrentIdx+1, p.AnimTime(), want.Element, want.AnimTime)
		}
		for i, elapsed := range want.ElementTimes {
			if got, valid := p.ElemTime(i + 1); !valid || got != elapsed {
				t.Fatalf("tick %d ElemTime(%d)=%d,%v want %d", want.Tick, i+1, got, valid, elapsed)
			}
		}
		if got, valid := p.ElemNo(0); !valid || got != want.Element {
			t.Fatalf("tick %d ElemNo(0)=%d,%v", want.Tick, got, valid)
		}
		p.Update()
	}
}

func TestClockDurationsAndReset(t *testing.T) {
	p := NewPlayer(&Action{LoopStart: -1, Elements: []Element{{Time: 0}, {Time: 1}, {Time: 0}, {Time: 2}}})
	for tick, want := range []int{2, 4, 4, 2, 4, 4} {
		if p.Error != nil || p.CurrentIdx+1 != want {
			t.Fatalf("tick %d: element=%d error=%v", tick, p.CurrentIdx+1, p.Error)
		}
		p.Update()
	}
	p = NewPlayer(loopAction())
	p.Reset(2)
	if p.CurrentIdx != 1 || p.AnimTime() != -7 {
		t.Fatal("element selection did not reset clock")
	}
	if n, ok := p.ElemTime(2); !ok || n != 0 {
		t.Fatal("selected element elapsed time")
	}
	p = NewPlayer(&Action{LoopStart: -1, Elements: []Element{{Time: 2}, {Time: -1}, {Time: 4}}})
	for i := 0; i < 10; i++ {
		p.Update()
	}
	if p.CurrentIdx != 1 || p.TickCounter != 8 || p.AnimTime() != -1 {
		t.Fatal("infinite frame does not advance elapsed time")
	}
	if _, ok := p.ElemTime(3); ok {
		t.Fatal("unreachable element start is not a valid time")
	}
	for _, action := range []*Action{{LoopStart: -1, Elements: []Element{{Time: 0}}}, {LoopStart: 1, Elements: []Element{{Time: 1}, {Time: 0}}}, {LoopStart: 2, Elements: []Element{{Time: 1}}}, {LoopStart: -1, Elements: []Element{{Time: -2}}}} {
		p := NewPlayer(action)
		if p.Error == nil {
			t.Fatal("invalid animation schedule accepted")
		}
		p.Update()
	}
}

func TestClockQueries(t *testing.T) {
	p := NewPlayer(loopAction())
	if _, ok := p.ElemTime(0); ok {
		t.Fatal("element zero")
	}
	if _, ok := p.ElemTime(4); ok {
		t.Fatal("element beyond action")
	}
	if _, ok := p.ElemNo(-1); ok {
		t.Fatal("queried before nonlooping prefix")
	}
	if got, ok := p.ElemNo(5); !ok || got != 3 {
		t.Fatal("future element query")
	}
	p.Update()
	p.Update()
	if got, ok := p.ElemNo(-3); !ok || got != 3 {
		t.Fatal("looping history query")
	}
	if got, ok := p.ElemNo(1000000); !ok || got < 2 || got > 3 {
		t.Fatal("large looping future query")
	}
}
