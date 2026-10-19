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

type identityReference struct {
	CNSHash       string `json:"cns_sha256"`
	AIRHash       string `json:"air_sha256"`
	HelperMax     *int   `json:"helper_max"`
	ExpectedError string `json:"expected_load_error"`
	Captures      []struct{ Variables map[string]float64 }
}

func TestNativeEntityIdentityAndBottomBoundaries(t *testing.T) {
	constantsSource, err := os.ReadFile("../temp/mugen-1.1b1/chars/kfm/kfm.cns")
	if err != nil {
		t.Fatal(err)
	}
	constants, err := cns.Parse(bytes.NewReader(constantsSource))
	if err != nil {
		t.Fatal(err)
	}
	for _, name := range []string{"identity-roots", "identity-helper", "identity-bottom", "identity-missing", "identity-bottom-operators", "identity-bottom-bitnot", "identity-bottom-bitor", "identity-helper-retire", "identity-helper-nested"} {
		stem := "../tests/testdata/reference/" + name
		data, err := os.ReadFile(stem + ".json")
		if err != nil {
			t.Fatal(err)
		}
		var catalogue struct{ Cases map[string]identityReference }
		if err := json.Unmarshal(data, &catalogue); err != nil {
			t.Fatal(err)
		}
		if catalogue.Cases == nil {
			var reference identityReference
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			catalogue.Cases = map[string]identityReference{"default": reference}
		}
		for caseName, reference := range catalogue.Cases {
			t.Run(name+"/"+caseName, func(t *testing.T) {
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
				players := []*Character{NewCharacter(file, actions, nil), NewCharacter(file, actions, nil)}
				limit := 56
				if reference.HelperMax != nil {
					limit = *reference.HelperMax
				}
				world := NewWorld(limit)
				if err := world.AddRoots(players...); err != nil {
					t.Fatal(err)
				}
				for slot, c := range players {
					c.InputSlot, c.Facing, c.X = slot+1, 1-2*slot, float64(-70+140*slot)
					c.ChangeState(0, -1, -1)
					c.Ctrl = true
				}
				for tick := 0; tick < 60; tick++ {
					err = TickCharacters(players)
					if err != nil {
						break
					}
				}
				if reference.ExpectedError != "" {
					if err == nil || !strings.Contains(err.Error(), reference.ExpectedError) || !strings.Contains(err.Error(), "state 700 controller") {
						t.Fatal("native malformed expression was not diagnosed with source context", err)
					}
					return
				}
				if err != nil {
					t.Fatal(err)
				}
				for _, capture := range reference.Captures {
					for key, want := range capture.Variables {
						index, err := strconv.Atoi(key)
						if err != nil {
							t.Fatal(err)
						}
						if got := players[0].FloatVars[index]; float32(got) != float32(want) {
							t.Errorf("fvar(%d)=%v native=%v", index, got, want)
						}
					}
				}
				if players[0].EntityID == int32(players[0].InputSlot) || players[0].EntityID == players[1].EntityID {
					t.Fatal("input slots and entity identity were conflated")
				}
			})
		}
	}
}
