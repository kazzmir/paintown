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
	for _, name := range []string{
		"hit-coordinates",
		"hit-defaults",
		"hit-trade",
		"hit-trade-phases",
		"hit-trade-pause",
		"override-basic",
		"override-slots",
		"override-forceair",
		"override-clear",
		"override-expire",
		"override-lasttick",
		"override-hitstate",
		"override-missing-state",
		"override-missing-attr",
		"override-default-time",
		"override-nonmatch",
		"override-projectile",
		"contact-activation",
		"hit-countdown",
		"guard-contact",
		"guard-crouch",
		"guard-air",
		"guard-wrong-height",
		"guard-exit-contact",
		"guard-override",
		"guard-defaults",
		"guard-air-defaults",
		"pause-release",
		"attacker-pause",
		"protect-all",
		"protect-attack-only",
		"protect-clear",
		"protect-expire",
		"protect-hitby-all",
		"protect-hitby-attack",
		"protect-hitby-pair",
		"protect-lasttick",
		"protect-negative",
		"protect-pair-attack",
		"protect-pair-hit",
		"protect-pair-other",
		"protect-pair-state",
		"protect-replace",
		"protect-slots",
		"protect-attacker-air",
		"protect-defender-air",
		"protect-attr-only",
		"protect-class-allow",
		"protect-empty",
		"protect-dynamic-pose",
		"protect-missing",
		"protect-both",
		"protect-both-first",
		"protect-both-second",
		"override-current-pose",
		"override-authored-pose",
		"guard-distance",
		"guard-distance-default",
		"guard-distance-reset",
		"guard-distance-clock",
		"guard-distance-no-def",
		"attack-distance",
		"state-order-both-attack",
		"state-order-victim-hit",
		"guard-distance-height",
		"guard-distance-defender-turn",
		"guard-distance-attacker-turn",
		"guard-distance-cross",
		"push-standing",
		"push-disabled",
		"push-overlap-eight",
		"push-overlap-five",
		"push-overlap-fraction",
		"push-near-axis",
		"push-negative-axis",
		"push-gap-four",
		"push-gap-eight",
		"push-gap-ten",
		"push-one-tick",
	} {
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
					ExpectedFail string `json:"expected_load_error"`
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
					if strings.HasPrefix(name, "guard-distance") || name == "attack-distance" || strings.HasPrefix(name, "state-order-") {
						data, err := os.ReadFile("../temp/mugen-1.1b1/chars/kfm/kfm.cns")
						if err != nil {
							t.Fatal(err)
						}
						stock, err := cns.Parse(strings.NewReader(string(data)))
						if err != nil {
							t.Fatal(err)
						}
						file.Size = stock.Size
					}
					if test.ExpectedFail != "" {
						if err := NewStateMachine().CompileState(file.States[700]); err == nil {
							t.Fatal("accepted native-rejected protection definition")
						}
						return
					}
					if strings.HasPrefix(name, "override-") || name == "attacker-pause" {
						data, err := os.ReadFile("../temp/mugen-1.1b1/data/common1.cns")
						if err != nil {
							t.Fatal(err)
						}
						common, err := cns.ParseNamed(strings.NewReader(string(data)), "common1.cns")
						if err != nil {
							t.Fatal(err)
						}
						for id, state := range common.States {
							if file.States[id] == nil {
								file.States[id] = state
							}
						}
					}
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
						if err := TickCharacters(players[:]); err != nil {
							t.Fatal(err)
						}
					}
					wantState := 5000
					if strings.HasPrefix(name, "override-") || (strings.HasPrefix(name, "guard-") && !strings.HasPrefix(name, "guard-distance")) || strings.HasPrefix(name, "protect-") {
						wantState = int(referenceFloat(t, test.Captures[0].Variables["0"]))
					}
					if strings.HasPrefix(name, "guard-distance") || name == "attack-distance" || strings.HasPrefix(name, "state-order-") || strings.HasPrefix(name, "push-") {
						wantState = 700
					}
					if name == "pause-release" {
						wantState = 150
					}
					if name == "attacker-pause" {
						wantState = 701
					}
					if players[0].StateNo != wantState {
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
					attacker := players[1]
					if name == "attacker-pause" {
						attacker = players[0]
					}
					if !strings.HasPrefix(name, "hit-trade") && !strings.HasPrefix(name, "guard-distance") && name != "attack-distance" && !strings.HasPrefix(name, "state-order-") && !strings.HasPrefix(name, "push-") && attacker.CurrentHitDef.GroundVel[0] != -16 {
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
