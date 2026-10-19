package character

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"os"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
)

func TestReferenceCoordinateConstants(t *testing.T) {
	stem := "../tests/testdata/reference/coordinate-constants"
	source, err := os.ReadFile(stem + ".cns")
	if err != nil {
		t.Fatal(err)
	}
	data, err := os.ReadFile(stem + ".json")
	if err != nil {
		t.Fatal(err)
	}
	var trace struct {
		Cases map[string]struct {
			Localcoord string
			CNSHash    string `json:"cns_sha256"`
			Captures   []struct{ Variables map[string]float64 }
		}
	}
	if err := json.Unmarshal(data, &trace); err != nil {
		t.Fatal(err)
	}
	for name, test := range trace.Cases {
		t.Run(name, func(t *testing.T) {
			sum := sha256.Sum256(source)
			if hex.EncodeToString(sum[:]) != test.CNSHash {
				t.Fatal("coordinate fixture changed since native measurement")
			}
			file, err := cns.ParseNamed(strings.NewReader(string(source)), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			c := NewCharacter(file, nil, nil)
			c.LocalCoord, err = coord.Parse(test.Localcoord)
			if err != nil {
				t.Fatal(err)
			}
			c.ChangeState(0, -1, -1)
			for tick := 0; tick < 40; tick++ {
				c.Update()
			}
			if c.RuntimeError != nil {
				t.Fatal(c.RuntimeError)
			}
			for _, capture := range test.Captures {
				for index, expected := range capture.Variables {
					i, err := strconv.Atoi(index)
					if err != nil {
						t.Fatal(err)
					}
					if actual := c.GetFVar(i); float32(actual) != float32(expected) {
						t.Errorf("fvar(%d)=%v reference=%v", i, actual, expected)
					}
				}
			}
		})
	}
}

func TestCharacterUsesDeclaredMaximumPower(t *testing.T) {
	for _, source := range []string{"[Data]\npower=1500", "[Data]\npower=0", "[Data]\nlife=1000"} {
		file, err := cns.Parse(strings.NewReader(source))
		if err != nil {
			t.Fatal(err)
		}
		c := NewCharacter(file, nil, nil)
		want := file.Data.Power
		c.SetPower(9999)
		if c.MaxPower != want || c.Power != want {
			t.Fatalf("%s: maximum=%d power=%d want %d", source, c.MaxPower, c.Power, want)
		}
	}
}
