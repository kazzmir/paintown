package character

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"os"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
)

func TestReferenceMixedPlayerDistances(t *testing.T) {
	stem := "../tests/testdata/reference/player-distance"
	source, err := os.ReadFile(stem + ".cns")
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
			CNSHash      string `json:"cns_sha256"`
			Captures     []struct{ Variables map[string]float64 }
		}
	}
	if err := json.Unmarshal(data, &trace); err != nil {
		t.Fatal(err)
	}
	constants, err := os.ReadFile("../temp/mugen-1.1b1/chars/kfm/kfm.cns")
	if err != nil {
		t.Fatal(err)
	}
	for name, test := range trace.Cases {
		t.Run(name, func(t *testing.T) {
			sum := sha256.Sum256(source)
			if hex.EncodeToString(sum[:]) != test.CNSHash {
				t.Fatal("fixture changed since native measurement")
			}
			file, err := cns.ParseNamed(strings.NewReader(string(source)), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			stock, err := cns.Parse(strings.NewReader(string(constants)))
			if err != nil {
				t.Fatal(err)
			}
			file.Size = stock.Size
			c, other := NewCharacter(file, nil, nil), NewCharacter(stock, nil, nil)
			c.LocalCoord, err = coord.Parse(test.Localcoord)
			if err != nil {
				t.Fatal(err)
			}
			other.LocalCoord, err = coord.Parse(test.P2Localcoord)
			if err != nil {
				t.Fatal(err)
			}
			c.X = -70 * float64(c.LocalCoordinateWidth()) / 320
			other.X = 70 * float64(other.LocalCoordinateWidth()) / 320
			other.Facing, other.PlayerID = -1, 2
			c.Target = []*Character{other}
			c.ChangeState(0, -1, -1)
			c.Update()
			if c.RuntimeError != nil {
				t.Fatal(c.RuntimeError)
			}
			for _, capture := range test.Captures {
				for index, expected := range capture.Variables {
					i, err := strconv.Atoi(index)
					if err != nil {
						t.Fatal(err)
					}
					if actual := c.GetFVar(i); float32(actual) != float32(expected) {
						t.Errorf("fvar(%d)=%v reference=%v", i, actual, expected)
					}
				}
			}
		})
	}
}

func TestMixedCoordinateCollisionAndPush(t *testing.T) {
	a := NewCharacter(&cns.CNS{}, nil, nil)
	b := NewCharacter(&cns.CNS{}, nil, nil)
	// Use separately authored boxes describing equal physical extents.
	for _, item := range []struct {
		c             *Character
		width, extent int
	}{{a, 320, 10}, {b, 1280, 40}} {
		item.c.LocalCoord = coord.Space{Width: item.width, Height: 240}
		box := air.Box{X1: -item.extent, X2: item.extent, Y1: -item.extent, Y2: item.extent}
		item.c.AirFile = &air.Data{Actions: map[int]*air.Action{0: {Elements: []air.Element{{Clsn1: []air.Box{box}, Clsn2: []air.Box{box}}}}}}
		item.c.Anim = 0
		item.c.MoveType, item.c.HitDefActive = "A", true
	}
	a.X, b.X = -10, 36
	if !CheckCollision(a, b) || !CheckCollision(b, a) {
		t.Fatal("equal physical extents failed across coordinate spaces")
	}
	b.X = 84
	if CheckCollision(a, b) || CheckCollision(b, a) {
		t.Fatal("separated boxes overlapped")
	}
	a.StateFile.Size.GroundFront, a.StateFile.Size.GroundBack = 10, 5
	b.StateFile.Size.GroundFront, b.StateFile.Size.GroundBack = 40, 20
	a.X, b.X, a.Facing, b.Facing = 0, 40, 1, -1
	a.Target = []*Character{b}
	a.ResolvePushCollision()
	if a.X != -5 || b.X != 60 {
		t.Fatal("push must move each body by five common units", a.X, b.X)
	}
	// Turning compares converted positions rather than the raw numbers.
	a.X, b.X, a.Facing = 20, 60, 1
	a.FaceOpponent()
	if a.Facing != -1 {
		t.Fatal("opponent at common x=15 is left of player at x=20")
	}
}
