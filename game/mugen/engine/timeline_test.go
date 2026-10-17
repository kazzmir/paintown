package engine

import (
	"encoding/json"
	"math"
	"os"
	"strconv"
	"testing"
)

func TestNativePauseTimelineWithoutDisplay(t *testing.T) {
	for _, test := range []struct {
		name         string
		time, move   int
		other, super bool
	}{
		{"pause-three", 3, 0, false, false}, {"pause-one", 1, 0, false, false},
		{"pause-move-one", 3, 1, false, false}, {"pause-move-all", 3, 3, false, false},
		{"pause-zero", 0, 0, false, false}, {"pause-missing", 0, 0, false, false},
		{"pause-negative", -2, 0, false, false}, {"pause-overmove", 3, 5, false, false},
		{"pause-negmove", 3, -1, false, false}, {"pause-other", 3, 0, true, false},
		{"superpause-three", 3, 0, false, true},
	} {
		t.Run(test.name, func(t *testing.T) {
			data, err := os.ReadFile("../tests/testdata/reference/" + test.name + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var trace struct {
				Captures []struct{ Variables map[string]float64 }
			}
			if err := json.Unmarshal(data, &trace); err != nil {
				t.Fatal(err)
			}
			clock := &Timeline{}
			actor, owner := NewPauseActor(), NewPauseActor()
			if !test.other {
				owner = actor
			}
			local := 0
			observed := map[int]int32{}
			for frame := 0; frame < 20; frame++ {
				clock.BeginFrame()
				if !clock.Frozen(actor) {
					observed[local] = clock.GameTime
					local++
				}
				if frame == 0 {
					clock.RequestPause(owner, PauseParameters{Time: test.time, MoveTime: test.move}, test.super)
					if clock.Paused() {
						t.Fatal("request froze its issuing tick")
					}
				}
				clock.EndFrame()
			}
			for _, capture := range trace.Captures {
				for key, want := range capture.Variables {
					index, err := strconv.Atoi(key)
					if err != nil {
						t.Fatal(err)
					}
					if index%6 == 0 && float64(observed[index/6]) != want {
						t.Errorf("local time %d first runs at game tick %d native=%v", index/6, observed[index/6], want)
					}
				}
			}
		})
	}
}

func TestTimelineNestedActorsAndIntegerClock(t *testing.T) {
	timeline := &Timeline{GameTime: math.MaxInt32}
	parent, child := NewPauseActor(), NewPauseActor()
	timeline.BeginFrame()
	timeline.RequestPause(parent, PauseParameters{Time: 2, MoveTime: 1}, false)
	timeline.BeginFrame()
	timeline.EndFrame()
	if timeline.GameTime != math.MaxInt32 || timeline.Paused() {
		t.Fatal("nested adapter ended the world frame")
	}
	timeline.EndFrame()
	if timeline.GameTime != math.MinInt32 || timeline.Frozen(parent) || !timeline.Frozen(child) {
		t.Fatal("clock wrapping or independent actor allowance failed")
	}
}
