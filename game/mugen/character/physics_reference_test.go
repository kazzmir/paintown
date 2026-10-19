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

func TestNativePhysicsAndLandingPasses(t *testing.T) {
	stock, err := os.ReadFile("../temp/mugen-1.1b1/chars/kfm/kfm.cns")
	if err != nil {
		t.Fatal(err)
	}
	constants, err := cns.Parse(bytes.NewReader(stock))
	if err != nil {
		t.Fatal(err)
	}
	for _, name := range []string{"landing-simple", "landing-transition", "landing-hit", "landing-frozen", "landing-physics-n", "landing-backhop",
		"landing-budget", "landing-budget-overflow", "landing-repeat-3", "landing-repeat-2510", "physics-standing", "physics-crouching",
		"custom-state-40", "custom-state-50", "custom-state-51", "custom-state-52", "custom-state-100", "custom-state-105"} {
		t.Run(name, func(t *testing.T) {
			stem := "../tests/testdata/reference/" + name
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				CNSHash  string `json:"cns_sha256"`
				AIRHash  string `json:"air_sha256"`
				Bank     string `json:"bank"`
				Failure  string `json:"expected_load_error"`
				Captures []struct{ Variables map[string]float64 }
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
			file.Data, file.Size, file.Velocity, file.Movement, file.ConstantFields = constants.Data, constants.Size, constants.Velocity, constants.Movement, constants.ConstantFields
			actions, err := air.Parse(bytes.NewReader(frames))
			if err != nil {
				t.Fatal(err)
			}
			c := NewCharacter(file, actions, nil)
			c.X = -70
			c.ChangeState(0, -1, -1)
			c.Ctrl = true // Begin at the native round's control grant.
			for tick := 0; tick < 40; tick++ {
				c.Update()
				if c.RuntimeError != nil {
					break
				}
			}
			if reference.Failure != "" {
				if c.RuntimeError == nil || !strings.Contains(c.RuntimeError.Error(), reference.Failure) {
					t.Fatal("native physics continuation failure differs", c.RuntimeError)
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
					got := float64(c.IntVars[index])
					if reference.Bank == "fvar" {
						got = c.FloatVars[index]
					}
					if float32(got) != float32(want) {
						t.Errorf("%s(%d)=%v native=%v", reference.Bank, index, got, want)
					}
				}
			}
		})
	}
}
