package air

import (
	"crypto/sha256"
	"encoding/json"
	"fmt"
	"os"
	"strings"
	"testing"
)

func TestNativeInfiniteAnimationClocks(t *testing.T) {
	for _, label := range []string{"infinite", "prefix-infinite", "suffix-infinite"} {
		t.Run(label, func(t *testing.T) {
			stem := "../tests/testdata/reference/customhit-animation-" + label
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				AIRHash  string `json:"air_sha256"`
				Captures []struct{ Variables map[string]int }
			}
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			source, err := os.ReadFile(stem + ".air")
			if err != nil || fmt.Sprintf("%x", sha256.Sum256(source)) != reference.AIRHash {
				t.Fatal("native AIR hash mismatch", err)
			}
			frames, err := Parse(strings.NewReader(string(source)))
			if err != nil {
				t.Fatal(err)
			}
			values := map[string]int{}
			for _, capture := range reference.Captures {
				for key, value := range capture.Variables {
					values[key] = value
				}
			}
			p := NewPlayer(frames.Actions[9300])
			for tick := 0; tick < 6; tick++ {
				want, present := values[fmt.Sprint(tick)]
				if !present || p.Error != nil || p.AnimTime() != want {
					t.Fatalf("tick %d AnimTime=%d native=%d present=%v error=%v", tick, p.AnimTime(), want, present, p.Error)
				}
				elapsed, valid := p.ElemTime(1)
				if want, present := values[fmt.Sprint(tick+6)]; !present || !valid || elapsed != want {
					t.Fatalf("tick %d ElemTime(1)=%d,%v native=%d present=%v", tick, elapsed, valid, want, present)
				}
				p.Update()
			}
		})
	}
}

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
	p = NewPlayer(&Action{LoopStart: -1, Elements: []Element{{Time: 2}, {Time: -1}}})
	for i := 0; i < 10; i++ {
		p.Update()
	}
	if p.CurrentIdx != 1 || p.TickCounter != 8 || p.AnimTime() != 11 {
		t.Fatal("infinite frame does not advance elapsed time")
	}
	if _, ok := p.ElemTime(3); ok {
		t.Fatal("element beyond the action is not a valid time")
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
