package character

import (
	"bytes"
	"encoding/json"
	"fmt"
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

type viewportReference struct {
	CNSHash   string `json:"cns_sha256"`
	AIRHash   string `json:"air_sha256"`
	Local     string `json:"localcoord"`
	Width     int    `json:"game_width"`
	Height    int    `json:"game_height"`
	Stage     string `json:"stage"`
	Version   string `json:"mugenversion"`
	LoadError string `json:"load_error"`
	Captures  []struct{ Variables map[string]float64 }
}

func runViewportReference(t *testing.T, stem string, reference viewportReference) {
	t.Helper()
	source := checkedAudioInput(t, stem+".cns", reference.CNSHash)
	frames := checkedAudioInput(t, stem+".air", reference.AIRHash)
	file, err := cns.ParseNamed(bytes.NewReader(source), stem+".cns")
	if err != nil {
		t.Fatal(err)
	}
	file.SetLanguageVersion(reference.Version)
	err = NewStateMachine().CompileState(file.States[700])
	if reference.LoadError != "" {
		if err == nil || !strings.Contains(err.Error(), "mugenversion 1.1") {
			t.Fatal("accepted native-rejected viewport trigger version", err)
		}
		return
	}
	if err != nil {
		t.Fatal(err)
	}
	root, err := filepath.Abs("../temp/mugen-1.1b1")
	if err != nil {
		t.Fatal(err)
	}
	constants, err := os.ReadFile(filepath.Join(root, "chars/kfm/kfm.cns"))
	if err != nil {
		t.Fatal(err)
	}
	base, err := cns.Parse(bytes.NewReader(constants))
	if err != nil {
		t.Fatal(err)
	}
	file.Data, file.Size, file.Velocity, file.Movement, file.ConstantFields = base.Data, base.Size, base.Velocity, base.Movement, base.ConstantFields
	path := reference.Stage
	if path == "" {
		path = "kfm"
	}
	if !strings.HasSuffix(path, ".def") {
		path += ".def"
	}
	if !strings.HasPrefix(path, "stages/") {
		path = "stages/" + path
	}
	background, err := stage.LoadStage(path, root)
	if err != nil {
		t.Fatal(err)
	}
	width := reference.Width
	if width == 0 {
		width = 1280
	}
	space, err := coord.GameSpaceForStage(width, background.StageInfo.LocalCoord)
	if err != nil {
		t.Fatal(err)
	}
	stageScale, _ := background.StageInfo.LocalCoord.ScaleTo(space)
	view := &coord.Viewport{Space: space, CameraX: background.Camera.StartX * stageScale, CameraY: background.Camera.StartY * stageScale, GroundY: background.StageInfo.ZOffset * stageScale, Zoom: 1}
	local, err := coord.Parse(reference.Local)
	if err != nil {
		t.Fatal(err)
	}
	actions, err := air.Parse(bytes.NewReader(frames))
	if err != nil {
		t.Fatal(err)
	}
	players := []*Character{NewCharacter(file, actions, nil), NewCharacter(file, actions, nil)}
	localScale, _ := background.StageInfo.LocalCoord.ScaleTo(local)
	for slot, c := range players {
		c.LocalCoord, c.GameCoord, c.Viewport = local, space, view
		c.PlayerID = slot + 1
		c.X, c.Y, c.Facing = background.PlayerInfo.P1StartX*localScale, background.PlayerInfo.P1StartY*localScale, background.PlayerInfo.P1Facing
		if slot == 1 {
			c.X, c.Y, c.Facing = background.PlayerInfo.P2StartX*localScale, background.PlayerInfo.P2StartY*localScale, background.PlayerInfo.P2Facing
		}
		c.Target = []*Character{players[1-slot]}
		c.ChangeState(0, -1, -1)
		c.Ctrl = true
	}
	for tick := 0; tick < 40; tick++ {
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
}

func TestNativeViewportProfilesAndVersionGates(t *testing.T) {
	stem := "../tests/testdata/reference/viewport"
	data, err := os.ReadFile(stem + ".json")
	if err != nil {
		t.Fatal(err)
	}
	var catalogue struct{ Cases map[string]viewportReference }
	if err := json.Unmarshal(data, &catalogue); err != nil {
		t.Fatal(err)
	}
	for name, reference := range catalogue.Cases {
		t.Run(name, func(t *testing.T) { runViewportReference(t, stem, reference) })
	}
	for _, name := range []string{"viewport-motion", "viewport-legacy-screenheight", "viewport-legacy-zoom", "viewport-legacy-game"} {
		t.Run(name, func(t *testing.T) {
			stem := "../tests/testdata/reference/" + name
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var reference viewportReference
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			runViewportReference(t, stem, reference)
		})
	}
}

func TestLoaderBindsAndValidatesCharacterLanguageVersion(t *testing.T) {
	root, err := filepath.Abs("../temp/mugen-1.1b1")
	if err != nil {
		t.Fatal(err)
	}
	dir := t.TempDir()
	state := "[Statedef 700]\ntype=S\n[State 700, gate]\ntype=VarSet\ntrigger1=1\nfv=0\nvalue=ScreenWidth\n"
	if err := os.WriteFile(filepath.Join(dir, "gate.cns"), []byte(state), 0600); err != nil {
		t.Fatal(err)
	}
	for _, version := range []string{"1.0", "1.1"} {
		definition := "[Info]\nname=Viewport gate\nmugenversion=" + version + "\n[Files]\nst=gate.cns\n"
		for _, file := range []struct{ key, path string }{{"cns", "chars/kfm/kfm.cns"}, {"stcommon", "data/common1.cns"}, {"cmd", "chars/kfm/kfm.cmd"}, {"sprite", "chars/kfm/kfm.sff"}, {"anim", "chars/kfm/kfm.air"}, {"sound", "chars/kfm/kfm.snd"}} {
			definition += fmt.Sprintf("%s=%s\n", file.key, filepath.Join(root, file.path))
		}
		if err := os.WriteFile(filepath.Join(dir, "gate.def"), []byte(definition), 0600); err != nil {
			t.Fatal(err)
		}
		player, err := LoadPlayerWithOptions(dir, "gate.def", LoadOptions{RootDir: root})
		if version == "1.0" {
			if err == nil || !strings.Contains(err.Error(), "gate.cns") || !strings.Contains(err.Error(), "mugenversion 1.1") {
				t.Fatal("legacy source reached gameplay instead of failing at load", err)
			}
		} else if err != nil || player.Character.StateFile.States[700].LanguageVersion != "1.1" {
			t.Fatal("modern source version was not retained", err)
		}
	}
}
