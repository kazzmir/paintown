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
	"github.com/kazzmir/paintown/game/mugen/effect"
)

func TestNativeCustomStateOwnership(t *testing.T) {
	replayNativeWorldPrograms(t, "ownership-*.json", 11)
}

func TestNativeRootUpdatePhases(t *testing.T) {
	replayNativeWorldPrograms(t, "phase-*.json", 3)
}

func TestNativeTargetBindings(t *testing.T) {
	replayNativeWorldPrograms(t, "binding-*.json", 53)
}

func TestNativeCustomHitStates(t *testing.T) {
	replayNativeWorldPrograms(t, "customhit-*.json", 38)
}

func TestNativeStockThrows(t *testing.T) {
	replayNativeWorldPrograms(t, "throw-*.json", 4, 240)
}

func TestNativeContactFrames(t *testing.T) {
	replayNativeWorldPrograms(t, "contactframe-*.json", 4)
}

func TestNativeExplodLifetimes(t *testing.T) {
	replayNativeWorldPrograms(t, "explod-*.json", 18)
}

func replayNativeWorldPrograms(t *testing.T, pattern string, count int, duration ...int) {
	t.Helper()
	stock, err := os.ReadFile("../temp/mugen-1.1b1/chars/kfm/kfm.cns")
	if err != nil {
		t.Fatal(err)
	}
	constants, err := cns.Parse(bytes.NewReader(stock))
	if err != nil {
		t.Fatal(err)
	}
	commonData, err := os.ReadFile("../temp/mugen-1.1b1/data/fightfx.air")
	if err != nil {
		t.Fatal(err)
	}
	commonFrames, err := air.Parse(bytes.NewReader(commonData))
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
				Error     string            `json:"expected_load_error"`
				Files     map[string]string `json:"files"`
				FileHash  map[string]struct {
					Path string `json:"path"`
					Hash string `json:"sha256"`
				} `json:"file_sha256"`
				Captures []struct{ Variables map[string]float64 }
			}
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			stockInput := func(key string) []byte {
				input, present := reference.FileHash[key]
				if !present || input.Hash == "" || filepath.IsAbs(input.Path) || strings.Contains(input.Path, "..") {
					t.Fatal("stock Files override lacks a safe pinned input", key, input)
				}
				return checkedAudioInput(t, filepath.Join("../temp/mugen-1.1b1", filepath.FromSlash(input.Path)), input.Hash)
			}
			load := func(stem string, hashes map[string]string) *Character {
				file, err := cns.ParseNamed(bytes.NewReader(checkedAudioInput(t, stem+".cns", hashes["cns"])), stem+".cns")
				if err != nil {
					t.Fatal(err)
				}
				file.SetLanguageVersion("1.1")
				file.Data, file.Size, file.Velocity, file.Movement, file.ConstantFields = constants.Data, constants.Size, constants.Velocity, constants.Movement, constants.ConstantFields
				merge := func(source []byte, name string) {
					states, err := cns.ParseNamed(bytes.NewReader(source), name)
					if err != nil {
						t.Fatal(err)
					}
					states.SetLanguageVersion("1.1")
					for id, state := range states.States {
						if file.States[id] == nil {
							file.States[id] = state
						}
					}
				}
				for index := 0; index <= 9; index++ {
					key := "st" + strconv.Itoa(index)
					if reference.Files[key] != "" {
						merge(stockInput(key), reference.FileHash[key].Path)
					}
				}
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
				if reference.Files["stcommon"] != "" {
					merge(stockInput("stcommon"), reference.FileHash["stcommon"].Path)
				}
				framesSource := checkedAudioInput(t, stem+".air", hashes["air"])
				if reference.Files["anim"] != "" {
					framesSource = stockInput("anim")
				}
				frames, err := air.Parse(bytes.NewReader(framesSource))
				if err != nil {
					t.Fatal(err)
				}
				return NewCharacter(file, frames, nil)
			}
			primary, primaryHashes := strings.TrimSuffix(path, ".json"), map[string]string{"cns": reference.CNSHash, "air": reference.AIRHash, "cmd": reference.CMDHash}
			opponent, opponentHashes := filepath.Join(filepath.Dir(path), reference.P2Fixture), reference.P2Hash
			if reference.P2Fixture == "" {
				opponent, opponentHashes = primary, primaryHashes
			}
			players := []*Character{load(primary, primaryHashes), load(opponent, opponentHashes)}
			if reference.Error != "" {
				err := players[0].SM.CompileState(players[0].StateFile.States[700])
				if err == nil || !strings.Contains(err.Error(), "state 700 controller") || !strings.Contains(err.Error(), filepath.Base(strings.TrimSuffix(path, ".json"))+".cns") {
					t.Fatal("native malformed controller was not rejected with source context", err)
				}
				return
			}
			world := NewWorld(56)
			if err := world.AddRoots(players...); err != nil {
				t.Fatal(err)
			}
			view := &coord.Viewport{Space: coord.Space{1280, 960}, GroundY: 800, Zoom: 1}
			for slot, c := range players {
				c.CommonEffects = effect.Source{Animations: commonFrames, Scale: 4}
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
			ticks := 60
			if len(duration) > 0 {
				ticks = duration[0]
			}
			for tick := 0; tick < ticks; tick++ {
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
