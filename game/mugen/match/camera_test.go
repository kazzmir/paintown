package match

import (
	"bytes"
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/stage"
)

func TestNativeCameraFollowing(t *testing.T) {
	replayNativeGeometryPrograms(t, "camerafollow-*.json", 20)
}

func TestNativeScreenBoundaries(t *testing.T) {
	replayNativeGeometryPrograms(t, "screenbound-*.json", 47)
}

func TestNativeWidth(t *testing.T) {
	replayNativeGeometryPrograms(t, "width-*.json", 31)
}

func TestNativeHelperConfiguration(t *testing.T) {
	replayNativeGeometryPrograms(t, "helpercfg-*.json", 72)
}

func TestNativeHelperRetirement(t *testing.T) {
	replayNativeGeometryPrograms(t, "helperretire-*.json", 14)
}

func TestNativeHelperBindingRetirement(t *testing.T) {
	replayNativeGeometryPrograms(t, "helperbind-*.json", 27)
}

func TestNativeHelperDefenderContacts(t *testing.T) {
	replayNativeGeometryPrograms(t, "helpercontact-*.json", 10)
}

func TestNativeHitSelection(t *testing.T) {
	replayNativeGeometryPrograms(t, "hitselect-*.json", 28)
}

func replayNativeGeometryPrograms(t *testing.T, pattern string, count int) {
	t.Helper()
	paths, err := filepath.Glob(filepath.Join("../tests/testdata/reference", pattern))
	if err != nil || len(paths) != count {
		t.Fatal("incomplete native geometry corpus", len(paths), err)
	}
	for _, path := range paths {
		t.Run(filepath.Base(path), func(t *testing.T) {
			data, err := os.ReadFile(path)
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				CNSHash    string `json:"cns_sha256"`
				AIRHash    string `json:"air_sha256"`
				StageHash  string `json:"stage_fixture_sha256"`
				CMDHash    string `json:"cmd_sha256"`
				Local      string `json:"localcoord"`
				P2Local    string `json:"p2_localcoord"`
				StageError string `json:"expected_stage_error"`
				LoadError  string `json:"expected_load_error"`
				ErrorState int    `json:"expected_compile_state"`
				Captures   []struct{ Variables map[string]float64 }
			}
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			checked := func(path, want string) []byte {
				t.Helper()
				data, err := os.ReadFile(path)
				if err != nil {
					t.Fatal(err)
				}
				hash := sha256.Sum256(data)
				if hex.EncodeToString(hash[:]) != want {
					t.Fatal("native geometry input changed", path)
				}
				return data
			}
			stem := strings.TrimSuffix(path, ".json")
			// Check every authored input, including programs the reference rejects.
			airBytes := checked(stem+".air", reference.AIRHash)
			checked(stem+".stage.def", reference.StageHash)
			file, err := cns.ParseNamed(bytes.NewReader(checked(stem+".cns", reference.CNSHash)), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			file.SetLanguageVersion("1.1")
			var commands *cmd.Data
			if reference.CMDHash != "" {
				data := checked(stem+".cmd", reference.CMDHash)
				commands, err = cmd.Parse(bytes.NewReader(data))
				if err != nil {
					t.Fatal(err)
				}
				states, err := cns.ParseNamed(bytes.NewReader(data), stem+".cmd")
				if err != nil {
					t.Fatal(err)
				}
				states.SetLanguageVersion("1.1")
				for number, state := range states.States {
					if file.States[number] == nil {
						file.States[number] = state
					}
				}
			}
			if reference.LoadError != "" && reference.StageError == "" {
				number := reference.ErrorState
				if number == 0 {
					number = 700
				}
				if err := character.NewStateMachine().CompileState(file.States[number]); err == nil || !strings.Contains(err.Error(), "state "+strconv.Itoa(number)+" controller") || !strings.Contains(err.Error(), filepath.Base(stem)+".cns") {
					t.Fatal("native-rejected controller was not rejected with source context", err)
				}
				return
			}
			frames, err := air.Parse(bytes.NewReader(airBytes))
			if err != nil {
				t.Fatal(err)
			}
			background, err := stage.LoadStage(stem+".stage.def", "../temp/mugen-1.1b1/stages")
			if reference.StageError != "" {
				if err == nil || !strings.Contains(strings.ToLower(err.Error()), reference.StageError) || !strings.Contains(err.Error(), filepath.Base(stem)+".stage.def") {
					t.Fatal("native-rejected stage was not rejected with context", err)
				}
				return
			}
			if err != nil {
				t.Fatal(err)
			}
			m := stockMatch(t, 99)
			constants := m.Players[0].Character.StateFile
			file.Data, file.Size, file.Velocity, file.Movement, file.ConstantFields = constants.Data, constants.Size, constants.Velocity, constants.Movement, constants.ConstantFields
			m.Stage = background
			for slot, player := range m.Players {
				// The reference launches KFM against KFM; stockMatch's second
				// loading template is KFM720, whose name is different.
				player.Definition.Info["name"] = m.Players[0].Definition.Info["name"]
				player.Definition.Info["author"] = m.Players[0].Definition.Info["author"]
				player.Character.StateFile, player.AIR, player.CMD = file, frames, commands
				player.LocalCoord = coord.Default()
				local := reference.Local
				if slot == 1 && reference.P2Local != "" {
					local = reference.P2Local
				}
				if local != "" {
					player.LocalCoord, err = coord.Parse(local)
					if err != nil {
						t.Fatal(err)
					}
				}
			}
			if err := m.resetRound(); err != nil {
				t.Fatal(err)
			}
			advanceUntil(t, m, Active, 600)
			for tick := 0; tick < 90; tick++ {
				if err := m.Update(); err != nil {
					t.Fatal(err)
				}
			}
			for _, capture := range reference.Captures {
				for key, want := range capture.Variables {
					index, err := strconv.Atoi(key)
					if err != nil {
						t.Fatal(err)
					}
					if got := m.Players[0].Character.FloatVars[index]; float32(got) != float32(want) {
						t.Errorf("fvar(%d)=%v native=%v", index, got, want)
					}
				}
			}
		})
	}
}
