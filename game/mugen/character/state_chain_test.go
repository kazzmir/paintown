package character

import (
	"bytes"
	"encoding/json"
	"os"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestNativePositiveStateContinuationBoundaries(t *testing.T) {
	for _, name := range []string{"state-chain", "state-chain-2499", "state-chain-2500", "state-chain-2501", "state-chain-2502", "state-chain-2503", "state-chain-large",
		"state-chain-pairs-1251", "state-chain-pairs-1252", "state-chain-pairs-2499", "state-chain-pairs-2500", "state-chain-pairs-2501",
		"state-chain-self", "state-chain-negative", "state-chain-negative-overflow"} {
		t.Run(name, func(t *testing.T) {
			stem := "../tests/testdata/reference/" + name
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				CNSHash  string `json:"cns_sha256"`
				AIRHash  string `json:"air_sha256"`
				Failure  string `json:"expected_load_error"`
				Captures []struct{ Variables map[string]int }
			}
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			source := checkedAudioInput(t, stem+".cns", reference.CNSHash)
			frames := checkedAudioInput(t, stem+".air", reference.AIRHash)
			file, err := cns.ParseNamed(bytes.NewReader(source), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			file.SetLanguageVersion("1.1")
			file.Data.Life, file.Data.Power = 1000, 3000 // Native harness prepends stock constants.
			actions, err := air.Parse(bytes.NewReader(frames))
			if err != nil {
				t.Fatal(err)
			}
			c := NewCharacter(file, actions, nil)
			c.ChangeState(0, -1, -1)
			// Start at native's ordinary round control grant. The fixture resets
			// its own frame counter to remove unrelated intro scheduling.
			c.Ctrl = true
			c.Update()
			if reference.Failure != "" {
				if c.RuntimeError == nil || !strings.Contains(c.RuntimeError.Error(), reference.Failure) || !strings.Contains(c.RuntimeError.Error(), stem+".cns") {
					t.Fatal("native runaway boundary differs", c.RuntimeError)
				}
				return
			}
			if c.RuntimeError != nil {
				t.Fatal(c.RuntimeError)
			}
			for _, capture := range reference.Captures {
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
