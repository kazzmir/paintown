package character

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"math"
	"os"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
)

func TestReferenceHitCoordinates(t *testing.T) {
	for _, name := range []string{"hit-coordinates", "hit-defaults"} {
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
				Cases map[string]struct {
					Localcoord   string
					P2Localcoord string `json:"p2_localcoord"`
					GameWidth    int    `json:"game_width"`
					CNSHash      string `json:"cns_sha256"`
					AIRHash      string `json:"air_sha256"`
					Captures     []struct{ Variables map[string]json.RawMessage }
				}
			}
			if err := json.Unmarshal(data, &trace); err != nil {
				t.Fatal(err)
			}
			for caseName, test := range trace.Cases {
				t.Run(caseName, func(t *testing.T) {
					for _, input := range []struct {
						bytes []byte
						hash  string
					}{{source, test.CNSHash}, {frames, test.AIRHash}} {
						sum := sha256.Sum256(input.bytes)
						if hex.EncodeToString(sum[:]) != input.hash {
							t.Fatal("fixture changed since native measurement")
						}
					}
					file, err := cns.ParseNamed(strings.NewReader(string(source)), stem+".cns")
					if err != nil {
						t.Fatal(err)
					}
					file.Data.Life = 1000
					actions, err := air.Parse(strings.NewReader(string(frames)))
					if err != nil {
						t.Fatal(err)
					}
					players := [2]*Character{NewCharacter(file, actions, nil), NewCharacter(file, actions, nil)}
					for slot, profile := range []string{test.Localcoord, test.P2Localcoord} {
						c := players[slot]
						c.LocalCoord, err = coord.Parse(profile)
						if err != nil {
							t.Fatal(err)
						}
						c.GameCoord = coord.Space{Width: 1280, Height: 720}
						if test.GameWidth > 0 {
							c.GameCoord.Width = test.GameWidth
						}
						c.X = float64(c.LocalCoordinateWidth()) * 70 / 320
						c.Facing = -1
						if slot == 0 {
							c.X = -c.X
							c.Facing = 1
						}
						c.PlayerID = slot + 1
						c.Target = []*Character{players[1-slot]}
						c.ChangeState(0, -1, -1)
						c.Ctrl = true
					}
					for tick := 0; tick < 40; tick++ {
						for _, c := range players {
							c.Update()
							if c.RuntimeError != nil {
								t.Fatal(c.RuntimeError)
							}
						}
					}
					if players[0].StateNo != 5000 {
						t.Fatal("authored contact did not reach victim state", players[0].StateNo)
					}
					for _, capture := range test.Captures {
						for key, raw := range capture.Variables {
							want := referenceFloat(t, raw)
							index, err := strconv.Atoi(key)
							if err != nil {
								t.Fatal(err)
							}
							if got := players[0].FloatVars[index]; float32(got) != float32(want) {
								t.Errorf("fvar(%d)=%v native=%v", index, got, want)
							}
						}
					}
					// Converting a received hit must not rewrite the attacker's definition.
					if players[1].CurrentHitDef.GroundVel[0] != -16 {
						t.Fatal("shared attacker vectors changed")
					}
				})
			}
		})
	}
}

func referenceFloat(t *testing.T, raw json.RawMessage) float64 {
	t.Helper()
	var text string
	if json.Unmarshal(raw, &text) == nil {
		if text == "positive_infinity" {
			return math.Inf(1)
		}
		if text == "negative_infinity" {
			return math.Inf(-1)
		}
		t.Fatalf("unknown native numeric label %q", text)
	}
	var value float64
	if err := json.Unmarshal(raw, &value); err != nil {
		t.Fatal(err)
	}
	return value
}

func TestReferenceFloatOverflow(t *testing.T) {
	for _, name := range []string{"float-overflow", "float-math-overflow", "scientific-accepted"} {
		t.Run(name, func(t *testing.T) {
			stem := "../tests/testdata/reference/" + name
			source, err := os.ReadFile(stem + ".cns")
			if err != nil {
				t.Fatal(err)
			}
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var trace struct {
				CNSHash  string `json:"cns_sha256"`
				Captures []struct{ Variables map[string]json.RawMessage }
			}
			if err := json.Unmarshal(data, &trace); err != nil {
				t.Fatal(err)
			}
			sum := sha256.Sum256(source)
			if hex.EncodeToString(sum[:]) != trace.CNSHash {
				t.Fatal("fixture changed since native measurement")
			}
			file, err := cns.ParseNamed(strings.NewReader(string(source)), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			c := NewCharacter(file, nil, nil)
			c.ChangeState(0, -1, -1)
			c.Update()
			if c.RuntimeError != nil {
				t.Fatal(c.RuntimeError)
			}
			for _, capture := range trace.Captures {
				for key, raw := range capture.Variables {
					index, err := strconv.Atoi(key)
					if err != nil {
						t.Fatal(err)
					}
					want := referenceFloat(t, raw)
					if got := c.FloatVars[index]; float32(got) != float32(want) {
						t.Errorf("fvar(%d)=%v native=%v", index, got, want)
					}
				}
			}

		})
	}
}

func TestHitAccelerationOmissionDiffersFromZero(t *testing.T) {
	c := NewCharacter(&cns.CNS{}, nil, nil)
	c.LocalCoord = coord.Space{Width: 1280, Height: 720}
	c.HitDef(map[string]string{})
	if float32(c.CurrentHitDef.YAccel) != float32(1.4) {
		t.Fatal("omitted acceleration did not use local default", c.CurrentHitDef.YAccel)
	}
	c.HitDef(map[string]string{"yaccel": "0"})
	if c.CurrentHitDef.YAccel != 0 {
		t.Fatal("explicit zero acceleration used the omitted default")
	}
}
