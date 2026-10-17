package character

import (
	"bytes"
	"encoding/json"
	"os"
	"strconv"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

func TestNativeSparkGeometryAndResourceSelection(t *testing.T) {
	for _, name := range []string{"spark-own", "guard-spark-own", "spark-own-offset", "spark-common", "spark-common-default", "spark-disabled", "spark-common-missing"} {
		t.Run(name, func(t *testing.T) {
			stem := "../tests/testdata/reference/" + name
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				CNSHash  string            `json:"cns_sha256"`
				AIRHash  string            `json:"air_sha256"`
				Extra    map[string]string `json:"extra_sha256"`
				Captures []struct{ Variables map[string]float64 }
				Spark    struct {
					Visible, Common bool
					Action, Facing  int
					Anchor          [2]float64 `json:"frame_anchor_240p"`
				}
			}
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			source := checkedAudioInput(t, stem+".cns", reference.CNSHash)
			frames := checkedAudioInput(t, stem+".air", reference.AIRHash)
			pixels := checkedAudioInput(t, stem+".extra/probe.sff", reference.Extra["probe.sff"])
			file, err := cns.ParseNamed(bytes.NewReader(source), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			actions, err := air.Parse(bytes.NewReader(frames))
			if err != nil {
				t.Fatal(err)
			}
			ownArchive, err := sff.Parse(bytes.NewReader(pixels))
			if err != nil {
				t.Fatal(err)
			}
			commonArchive, err := sff.Parse(bytes.NewReader(pixels))
			if err != nil {
				t.Fatal(err)
			}
			commonActions := actions
			if !reference.Spark.Common {
				data, err := os.ReadFile("../temp/mugen-1.1b1/data/fightfx.sff")
				if err != nil {
					t.Fatal(err)
				}
				commonArchive, err = sff.Parse(bytes.NewReader(data))
				if err != nil {
					t.Fatal(err)
				}
				data, err = os.ReadFile("../temp/mugen-1.1b1/data/fightfx.air")
				if err != nil {
					t.Fatal(err)
				}
				commonActions, err = air.Parse(bytes.NewReader(data))
				if err != nil {
					t.Fatal(err)
				}
			}
			scene := effect.NewScene(16)
			common := effect.Source{Sprites: commonArchive, Animations: commonActions, Scale: 4}
			players := []*Character{NewCharacter(file, actions, nil), NewCharacter(file, actions, nil)}
			for slot, c := range players {
				c.GameCoord = coord.Space{Width: 1280, Height: 720}
				c.InputSlot, c.Facing, c.X = slot+1, 1, -70
				if slot == 1 {
					c.Facing, c.X = -1, 70
				}
				c.Palettes = palette.New(ownArchive)
				c.BindEffects(&scene.Queue, common)
				c.Opponents = []*Character{players[1-slot]}
				c.ChangeState(0, -1, -1)
				c.Ctrl = true
			}
			for tick := 0; tick < 40; tick++ {
				if err := scene.Advance(); err != nil {
					t.Fatal(err)
				}
				if err := TickCharacters(players); err != nil {
					t.Fatal(err)
				}
				if err := scene.Flush(); err != nil {
					t.Fatal(err)
				}
			}
			for _, capture := range reference.Captures {
				for key, want := range capture.Variables {
					index, err := strconv.Atoi(key)
					if err != nil {
						t.Fatal(err)
					}
					if got := players[0].FloatVars[index]; got != want {
						t.Errorf("fvar(%d)=%v native=%v", index, got, want)
					}
				}
			}
			if !reference.Spark.Visible {
				if len(scene.Active) != 0 {
					t.Fatal("native-absent spark displayed", len(scene.Active))
				}
				return
			}
			if len(scene.Active) != 1 {
				t.Fatal("spark count", len(scene.Active))
			}
			instance := scene.Active[0]
			if instance.Action != reference.Spark.Action || instance.Facing != reference.Spark.Facing {
				t.Fatal("spark action/facing", instance.Request)
			}
			wantArchive := ownArchive
			if reference.Spark.Common {
				wantArchive = commonArchive
			}
			if instance.Source.Sprites != wantArchive {
				t.Fatal("spark chose the wrong resource owner")
			}
			el := instance.Clock.CurrentElement()
			x := instance.X/4 + float64(instance.Facing*el.XOffset)*instance.Source.Scale/4
			y := instance.Y/4 + float64(el.YOffset)*instance.Source.Scale/4
			if x != reference.Spark.Anchor[0] || y != reference.Spark.Anchor[1] {
				t.Fatal("spark geometry", x, y, "native", reference.Spark.Anchor)
			}
		})
	}
}

func TestNativeDataSparkPrefixCoercion(t *testing.T) {
	stem := "../tests/testdata/reference/spark-own-default"
	data, err := os.ReadFile(stem + ".json")
	if err != nil {
		t.Fatal(err)
	}
	var reference struct {
		CNSHash  string `json:"cns_sha256"`
		Captures []struct{ Variables map[string]float64 }
	}
	if err := json.Unmarshal(data, &reference); err != nil {
		t.Fatal(err)
	}
	source := checkedAudioInput(t, stem+".cns", reference.CNSHash)
	file, err := cns.ParseNamed(bytes.NewReader(source), stem+".cns")
	if err != nil {
		t.Fatal(err)
	}
	for index, name := range []string{"data.sparkno", "data.guard.sparkno"} {
		value, integer, found := file.LookupConstant(name)
		want := reference.Captures[0].Variables[strconv.Itoa(index+6)]
		if !found || !integer || value != want {
			t.Fatal("native Data resource-prefix coercion", name, value, integer, found, "want", want)
		}
	}
	if file.Data.SparkNo != 0 || file.Data.GuardSparkNo != 0 {
		t.Fatal("Data prefix became an own-bank default")
	}
}
