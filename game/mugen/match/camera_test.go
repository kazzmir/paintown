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
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/stage"
)

func TestNativeCameraFollowing(t *testing.T) {
	paths, err := filepath.Glob("../tests/testdata/reference/camerafollow-*.json")
	if err != nil || len(paths) != 20 {
		t.Fatal("incomplete native camera corpus", len(paths), err)
	}
	for _, path := range paths {
		t.Run(filepath.Base(path), func(t *testing.T) {
			data, err := os.ReadFile(path)
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				CNSHash   string `json:"cns_sha256"`
				AIRHash   string `json:"air_sha256"`
				StageHash string `json:"stage_fixture_sha256"`
				Local     string `json:"localcoord"`
				P2Local   string `json:"p2_localcoord"`
				Captures  []struct{ Variables map[string]float64 }
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
					t.Fatal("native camera input changed", path)
				}
				return data
			}
			stem := strings.TrimSuffix(path, ".json")
			file, err := cns.ParseNamed(bytes.NewReader(checked(stem+".cns", reference.CNSHash)), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			file.SetLanguageVersion("1.1")
			frames, err := air.Parse(bytes.NewReader(checked(stem+".air", reference.AIRHash)))
			if err != nil {
				t.Fatal(err)
			}
			checked(stem+".stage.def", reference.StageHash)
			background, err := stage.LoadStage(stem+".stage.def", "../temp/mugen-1.1b1/stages")
			if err != nil {
				t.Fatal(err)
			}
			m := stockMatch(t, 99)
			constants := m.Players[0].Character.StateFile
			file.Data, file.Size, file.Velocity, file.Movement, file.ConstantFields = constants.Data, constants.Size, constants.Velocity, constants.Movement, constants.ConstantFields
			m.Stage = background
			for slot, player := range m.Players {
				player.Character.StateFile, player.AIR, player.CMD = file, frames, nil
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
