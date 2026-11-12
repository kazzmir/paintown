package character

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"os"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestReferenceGuardEndAndControlGating(t *testing.T) {
	for _, name := range []string{"guard-end", "guard-start", "control-gating"} {
		t.Run(name, func(t *testing.T) {
			stem := "../tests/testdata/reference/" + name
			source, err := os.ReadFile(stem + ".cns")
			if err != nil {
				t.Fatal(err)
			}
			frames, err := os.ReadFile(stem + ".air")
			if err != nil {
				t.Fatal(err)
			}
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var trace struct {
				CNSHash  string `json:"cns_sha256"`
				AIRHash  string `json:"air_sha256"`
				Captures []struct{ Variables map[string]int }
			}
			if err := json.Unmarshal(data, &trace); err != nil {
				t.Fatal(err)
			}
			for _, input := range []struct {
				bytes []byte
				hash  string
			}{{source, trace.CNSHash}, {frames, trace.AIRHash}} {
				sum := sha256.Sum256(input.bytes)
				if hex.EncodeToString(sum[:]) != input.hash {
					t.Fatal("fixture changed since native measurement")
				}
			}
			file, err := cns.ParseNamed(strings.NewReader(string(source)), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			actions, err := air.Parse(strings.NewReader(string(frames)))
			if err != nil {
				t.Fatal(err)
			}
			c := NewCharacter(file, actions, nil)
			c.ChangeState(0, -1, -1)
			// Both probes wait for the real round control grant before entering
			// their diagnostic state. This is the runner's corresponding boundary.
			c.Ctrl = true
			for tick := 0; tick < 40; tick++ {
				c.Update()
			}
			if c.RuntimeError != nil {
				t.Fatal(c.RuntimeError)
			}
			for _, capture := range trace.Captures {
				for key, want := range capture.Variables {
					index, err := strconv.Atoi(key)
					if err != nil {
						t.Fatal(err)
					}
					if got := c.IntVars[index]; got != want {
						t.Errorf("var(%d)=%d native=%d", index, got, want)
					}
				}
			}
		})
	}
}

func TestBuiltinMovementRequiresControl(t *testing.T) {
	for _, command := range []string{"holdup", "holddown", "holdfwd", "holdback"} {
		t.Run(command, func(t *testing.T) {
			c := fixtureCharacter(t, "[State 7, idle]\ntype=Null\ntrigger1=1\n")
			c.Physics, c.Ctrl = "S", false
			c.ActiveCommands = []string{command}
			c.HandleBuiltinTransitions()
			if c.RuntimeError != nil || c.StateNo != 7 {
				t.Fatal("movement escaped control gate", c.StateNo, c.RuntimeError)
			}
		})
	}
}
