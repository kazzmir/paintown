package engine

import (
	"encoding/json"
	"os"
	"strconv"
	"testing"
)

func TestNativeKOSlowdownCadence(t *testing.T) {
	for _, fixture := range []struct {
		name             string
		duration, offset int
	}{
		{"ko-ready", 60, 0}, {"ko-ready-six", 6, 0}, {"ko-ready-twelve", 12, 0}, {"ko-widewait-late", 60, 40},
	} {
		t.Run(fixture.name, func(t *testing.T) {
			data, err := os.ReadFile("../tests/testdata/reference/roundclock-" + fixture.name + "-stable.json")
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				Captures []struct{ Variables map[string]int }
			}
			if err = json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			want := map[int]int{}
			for _, c := range reference.Captures {
				for key, value := range c.Variables {
					index, err := strconv.Atoi(key)
					if err != nil {
						t.Fatal(err)
					}
					want[index] = value
				}
			}
			var clock RoundSlowdown
			clock.Start(fixture.duration)
			var actual []int
			for tick := 0; len(actual) < fixture.offset+40 && tick < 1000; tick++ {
				if clock.BeginFrame() {
					clock.Advance()
					actual = append(actual, tick)
				}
			}
			if len(actual) != fixture.offset+40 {
				t.Fatal("slowdown stalled")
			}
			for sample := 0; sample < 40; sample++ {
				if got := actual[fixture.offset+sample]; got != want[20+sample] {
					t.Fatalf("sample%d: %d native%d", sample, got, want[20+sample])
				}
			}
		})
	}
}
