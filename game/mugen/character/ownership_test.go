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

func TestNativeCustomStateOwnership(t *testing.T) {
	replayNativeWorldPrograms(t, "ownership-*.json", 11)
}

func TestNativeRootUpdatePhases(t *testing.T) {
	replayNativeWorldPrograms(t, "phase-*.json", 3)
}

func replayNativeWorldPrograms(t *testing.T, pattern string, count int) {
	t.Helper()
	stock, err := os.ReadFile("../temp/mugen-1.1b1/chars/kfm/kfm.cns")
	if err != nil {
		t.Fatal(err)
	}
	constants, err := cns.Parse(bytes.NewReader(stock))
	if err != nil {
		t.Fatal(err)
	}
	paths, err := filepath.Glob(filepath.Join("../tests/testdata/reference", pattern))
	if err != nil || len(paths) != count {
		t.Fatal("incomplete native world corpus", pattern, len(paths), count, err)
	}
	for _, path := range paths {
		t.Run(filepath.Base(path), func(t *testing.T) {
			data, err := os.ReadFile(path)
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				CNSHash   string            `json:"cns_sha256"`
				AIRHash   string            `json:"air_sha256"`
				CMDHash   string            `json:"cmd_sha256"`
				P2Fixture string            `json:"p2_fixture"`
				P2Hash    map[string]string `json:"p2_sha256"`
				P2Local   string            `json:"p2_localcoord"`
				Captures  []struct{ Variables map[string]float64 }
			}
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			load := func(stem string, hashes map[string]string) *Character {
				file, err := cns.ParseNamed(bytes.NewReader(checkedAudioInput(t, stem+".cns", hashes["cns"])), stem+".cns")
				if err != nil {
					t.Fatal(err)
				}
				file.SetLanguageVersion("1.1")
				file.Data, file.Size, file.Velocity, file.Movement, file.ConstantFields = constants.Data, constants.Size, constants.Velocity, constants.Movement, constants.ConstantFields
				if hash := hashes["cmd"]; hash != "" {
					commands, err := cns.ParseNamed(bytes.NewReader(checkedAudioInput(t, stem+".cmd", hash)), stem+".cmd")
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
				frames, err := air.Parse(bytes.NewReader(checkedAudioInput(t, stem+".air", hashes["air"])))
				if err != nil {
					t.Fatal(err)
				}
				return NewCharacter(file, frames, nil)
			}
			players := []*Character{
				load(strings.TrimSuffix(path, ".json"), map[string]string{"cns": reference.CNSHash, "air": reference.AIRHash, "cmd": reference.CMDHash}),
				load(filepath.Join(filepath.Dir(path), reference.P2Fixture), reference.P2Hash),
			}
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
				c.GameCoord, c.Viewport, c.InputSlot, c.Facing = view.Space, view, slot+1, 1-2*slot
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
