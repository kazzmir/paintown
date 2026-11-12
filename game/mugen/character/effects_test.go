package character

import (
	"bytes"
	"encoding/json"
	"math"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

func TestNativeExplodMarkerGeometry(t *testing.T) {
	replayNativeExplodMarkers(t, "explodmark-*.json", 9)
}

func TestNativeCameraExplodGeometry(t *testing.T) {
	replayNativeExplodMarkers(t, "cameramark-*.json", 2)
}

func replayNativeExplodMarkers(t *testing.T, pattern string, count int) {
	t.Helper()
	paths, err := filepath.Glob(filepath.Join("../tests/testdata/reference", pattern))
	if err != nil || len(paths) != count {
		t.Fatal("incomplete native Explod marker corpus", len(paths), err)
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
				Extra     map[string]string `json:"extra_sha256"`
				StageHash string            `json:"stage_fixture_sha256"`
				Captures  []struct{ Variables map[string]float64 }
				Marker    struct {
					Visible, Common bool
					Action, Facing  int
					Anchor          [2]float64 `json:"frame_anchor_240p"`
				} `json:"spark"` // shared RGB calibration packet
			}
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			stem := strings.TrimSuffix(path, ".json")
			file, err := cns.ParseNamed(bytes.NewReader(checkedAudioInput(t, stem+".cns", reference.CNSHash)), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			file.SetLanguageVersion("1.1")
			framesData := checkedAudioInput(t, stem+".air", reference.AIRHash)
			frames, err := air.Parse(bytes.NewReader(framesData))
			if err != nil {
				t.Fatal(err)
			}
			commonFrames, err := air.Parse(bytes.NewReader(framesData))
			if err != nil {
				t.Fatal(err)
			}
			pixels := checkedAudioInput(t, stem+".extra/probe.sff", reference.Extra["probe.sff"])
			own, err := sff.Parse(bytes.NewReader(pixels))
			if err != nil {
				t.Fatal(err)
			}
			common, err := sff.Parse(bytes.NewReader(pixels))
			if err != nil {
				t.Fatal(err)
			}
			scene := effect.NewScene(16)
			view := &coord.Viewport{Space: coord.Space{Width: 1280, Height: 960}, GroundY: 800, Zoom: 1}
			players := []*Character{NewCharacter(file, frames, nil), NewCharacter(file, frames, nil)}
			if reference.StageHash != "" {
				fixedStage := fixedCameraReferenceStage(t, stem, reference.StageHash, view)
				factor, _ := fixedStage.StageInfo.LocalCoord.ScaleTo(coord.Default())
				players[0].X, players[1].X = fixedStage.PlayerInfo.P1StartX*factor, fixedStage.PlayerInfo.P2StartX*factor
			}
			for slot, c := range players {
				c.InputSlot, c.Facing = slot+1, 1-2*slot
				if reference.StageHash == "" {
					c.X = float64(-70 + 140*slot)
				}
				c.GameCoord, c.Viewport = view.Space, view
				c.Palettes = palette.New(own)
				c.BindEffects(&scene.Queue, effect.Source{Sprites: common, Animations: commonFrames, Scale: 4})
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
					if got := players[0].FloatVars[index]; float32(got) != float32(want) {
						t.Errorf("fvar(%d)=%v native=%v", index, got, want)
					}
				}
			}
			if !reference.Marker.Visible || len(scene.Active) != 1 {
				t.Fatal("missing/extra marker", len(scene.Active))
			}
			instance := scene.Active[0]
			x, y := instance.X, instance.Y
			if instance.ScreenSpace {
				x -= 640
				y -= 800
			} else {
				x -= view.CameraX
				y -= view.CameraY
			}
			if math.Abs(x/4-reference.Marker.Anchor[0]) > .001 || math.Abs(y/4-reference.Marker.Anchor[1]) > .001 || instance.Facing != reference.Marker.Facing || instance.Action != reference.Marker.Action {
				t.Fatal("native marker anchor/action/facing mismatch", x/4, y/4, instance.Facing, reference.Marker)
			}
			archive := own
			if reference.Marker.Common {
				archive = common
			}
			if instance.Source.Sprites != archive {
				t.Fatal("Explod selected the wrong sprite bank")
			}
		})
	}
}

func TestNativeSparkGeometryAndResourceSelection(t *testing.T) {
	for _, name := range []string{"spark-own", "guard-spark-own", "spark-own-offset", "spark-common", "spark-common-default", "spark-disabled", "spark-common-missing",
		"guardcue-spark-fatal", "guardcue-spark-hit", "guardcue-spark-safe", "guardcue-spark-protected"} {
		t.Run(name, func(t *testing.T) {
			stem := "../tests/testdata/reference/" + name
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				CNSHash  string            `json:"cns_sha256"`
				AIRHash  string            `json:"air_sha256"`
				Version  string            `json:"mugenversion"`
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
			file.SetLanguageVersion(reference.Version)
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
