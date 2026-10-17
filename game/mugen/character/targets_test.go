package character

import (
	"bytes"
	"encoding/json"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
)

func TestNativeTargetsAndRedirectedCoordinates(t *testing.T) {
	data, err := os.ReadFile("../temp/mugen-1.1b1/chars/kfm/kfm.cns")
	if err != nil {
		t.Fatal(err)
	}
	constants, err := cns.Parse(bytes.NewReader(data))
	if err != nil {
		t.Fatal(err)
	}
	names, err := filepath.Glob("../tests/testdata/reference/target*.json")
	if err != nil {
		t.Fatal(err)
	}
	names = append(names, "../tests/testdata/reference/redirect-coordinates.json", "../tests/testdata/reference/redirect-arguments.json", "../tests/testdata/reference/redirect-cond-coordinates.json")
	if len(names) != 49 {
		t.Fatal("native target corpus is incomplete")
	}
	for _, path := range names {
		t.Run(filepath.Base(path), func(t *testing.T) {
			data, err := os.ReadFile(path)
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				CNSHash  string `json:"cns_sha256"`
				AIRHash  string `json:"air_sha256"`
				CMDHash  string `json:"cmd_sha256"`
				P2Local  string `json:"p2_localcoord"`
				Error    string `json:"expected_load_error"`
				Captures []struct{ Variables map[string]float64 }
			}
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			stem := strings.TrimSuffix(path, ".json")
			source := checkedAudioInput(t, stem+".cns", reference.CNSHash)
			frames := checkedAudioInput(t, stem+".air", reference.AIRHash)
			file, err := cns.ParseNamed(bytes.NewReader(source), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			file.SetLanguageVersion("1.1")
			file.Data, file.Size, file.Velocity, file.Movement, file.ConstantFields = constants.Data, constants.Size, constants.Velocity, constants.Movement, constants.ConstantFields
			if reference.CMDHash != "" {
				commandSource := checkedAudioInput(t, stem+".cmd", reference.CMDHash)
				commands, err := cns.ParseNamed(bytes.NewReader(commandSource), stem+".cmd")
				if err != nil {
					t.Fatal(err)
				}
				commands.SetLanguageVersion("1.1")
				for id, state := range commands.States {
					if file.States[id] == nil {
						file.States[id] = state
					}
				}
			}
			if reference.Error != "" {
				err := NewStateMachine().CompileState(file.States[700])
				if err == nil || !strings.Contains(err.Error(), "missing required parameter value") || !strings.Contains(err.Error(), "state 700 controller") {
					t.Fatal("accepted native required-parameter rejection", err)
				}
				return
			}
			actions, err := air.Parse(bytes.NewReader(frames))
			if err != nil {
				t.Fatal(err)
			}
			players := []*Character{NewCharacter(file, actions, nil), NewCharacter(file, actions, nil)}
			world := NewWorld(56)
			if err := world.AddRoots(players...); err != nil {
				t.Fatal(err)
			}
			view := &coord.Viewport{Space: coord.Space{1280, 960}, GroundY: 800, Zoom: 1}
			for slot, c := range players {
				if slot == 1 && reference.P2Local != "" {
					c.LocalCoord, err = coord.Parse(reference.P2Local)
					if err != nil {
						t.Fatal(err)
					}
				}
				c.GameCoord, c.Viewport = view.Space, view
				c.InputSlot, c.Facing = slot+1, 1-2*slot
				c.X = float64(-70+140*slot) * float64(c.LocalCoordinateWidth()) / 320
				c.ChangeState(0, -1, -1)
				c.Ctrl = true
			}
			for tick := 0; tick < 60; tick++ {
				if err := TickCharacters(players); err != nil {
					t.Fatal(err)
				}
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
		})
	}
}
