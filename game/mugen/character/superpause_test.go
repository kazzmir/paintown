package character

import (
	"bytes"
	"encoding/json"
	"math"
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

func TestNativeSuperPauseContactAndTargetDefence(t *testing.T) {
	data, err := os.ReadFile("../temp/mugen-1.1b1/chars/kfm/kfm.cns")
	if err != nil {
		t.Fatal(err)
	}
	constants, err := cns.Parse(bytes.NewReader(data))
	if err != nil {
		t.Fatal(err)
	}
	for _, name := range []string{"superpause-unhittable-0", "superpause-unhittable-1", "superpause-defence-one", "superpause-defence-two", "superpause-defence-default", "superpause-defence-before", "superpause-defence-repeat", "superpause-defence-neutral", "superpause-defence-recover",
		"superpause-contact-freeze", "pause-contact-freeze", "superpause-contact-two-hits", "superpause-frozen-receiver", "superpause-frozen-two-hits", "superpause-defence-negative"} {
		t.Run(name, func(t *testing.T) {
			stem := "../tests/testdata/reference/" + name
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				CNSHash  string `json:"cns_sha256"`
				AIRHash  string `json:"air_sha256"`
				Captures []struct{ Variables map[string]float64 }
			}
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			source := checkedAudioInput(t, stem+".cns", reference.CNSHash)
			frames := checkedAudioInput(t, stem+".air", reference.AIRHash)
			file, err := cns.ParseNamed(bytes.NewReader(source), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			file.SetLanguageVersion("1.1")
			file.Data, file.Size, file.Velocity, file.Movement, file.ConstantFields = constants.Data, constants.Size, constants.Velocity, constants.Movement, constants.ConstantFields
			actions, err := air.Parse(bytes.NewReader(frames))
			if err != nil {
				t.Fatal(err)
			}
			players := []*Character{NewCharacter(file, actions, nil), NewCharacter(file, actions, nil)}
			for slot, c := range players {
				c.PlayerID, c.Facing, c.X = slot+1, 1-2*slot, float64(-70+140*slot)
				c.Target = []*Character{players[1-slot]}
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

func TestNativeSuperPauseEffectResourcesProjectionAndTint(t *testing.T) {
	var packets [2]map[string]struct {
		Point []int `json:"point"`
		RGB   []int `json:"rgb"`
	}
	for _, name := range []string{"superpause-dark-0", "superpause-dark-1", "superpause-effect-clock", "superpause-effect-position", "superpause-effect-expire", "superpause-effect-replace", "superpause-effect-finite", "superpause-effect-ordinary-pause", "superpause-effect-other-pause", "superpause-effect-super-replaced"} {
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
				Render   struct {
					Visible bool  `json:"visible"`
					Core    []int `json:"effect_core"`
					Samples map[string]struct {
						Point []int `json:"point"`
						RGB   []int `json:"rgb"`
					} `json:"samples"`
				} `json:"super_render"`
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
			file.Data.Life, file.Data.Power = 1000, 3000
			actions, err := air.Parse(bytes.NewReader(frames))
			if err != nil {
				t.Fatal(err)
			}
			archive, err := sff.Parse(bytes.NewReader(pixels))
			if err != nil {
				t.Fatal(err)
			}
			scene := effect.NewScene(8)
			players := []*Character{NewCharacter(file, actions, nil), NewCharacter(file, actions, nil)}
			for slot, c := range players {
				c.PlayerID, c.Facing, c.X = slot+1, 1-2*slot, float64(-70+140*slot)
				c.GameCoord = coord.Space{Width: 1280, Height: 960}
				c.Palettes = palette.New(archive)
				c.BindEffects(&scene.Queue, effect.Source{})
				c.ChangeState(0, -1, -1)
				c.Ctrl = true
			}
			for tick := 0; tick < 60; tick++ {
				world := players[0].Timeline
				if err := scene.AdvanceWithPause(world.Paused(), world.SuperPaused()); err != nil {
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
						t.Errorf("render fixture fvar(%d)=%v native=%v", index, got, want)
					}
				}
			}
			if !reference.Render.Visible {
				if len(scene.Active) != 0 {
					t.Fatal("finite super effect remained active")
				}
				return
			}
			if len(scene.Active) != 1 {
				t.Fatal("SuperPause did not emit one own effect", len(scene.Active))
			}
			instance := scene.Active[0]
			if instance.Source.Sprites != archive || instance.Source.Animations != actions || instance.Source.Palette != players[0].Palettes {
				t.Fatal("SuperPause effect lost own resource ownership")
			}
			el := instance.Clock.CurrentElement()
			x := 160 + (640+instance.X+float64(el.XOffset)*instance.Source.Scale)*.75
			y := (800 + instance.Y + float64(el.YOffset)*instance.Source.Scale) * .75
			if len(reference.Render.Core) != 4 || x != float64(reference.Render.Core[0]) || y != float64(reference.Render.Core[1]) {
				t.Fatal("native normal-facing effect anchor differs", x, y, reference.Render.Core)
			}
			if name == "superpause-dark-0" {
				packets[0] = reference.Render.Samples
			}
			if name == "superpause-dark-1" {
				packets[1] = reference.Render.Samples
				if players[0].Timeline.ActorTint(players[0].pauseActor) != [3]float32{1, 1, 1} || players[0].Timeline.ActorTint(players[1].pauseActor) != [3]float32{.5, .5, .75} {
					t.Fatal("super caller/opponent tints were conflated")
				}
			}
		})
	}
	for _, key := range []string{"stage", "stage_floor", "hud", "owner", "opponent", "super_effect"} {
		tint := [3]float64{.3125, .3125, .5}
		if key == "owner" || key == "super_effect" {
			tint = [3]float64{1, 1, 1}
		}
		if key == "opponent" {
			tint = [3]float64{.5, .5, .75}
		}
		if len(packets[0][key].RGB) != 3 || len(packets[1][key].RGB) != 3 {
			t.Fatal("missing native static color samples")
		}
		for channel := range tint {
			if math.Abs(math.Round(float64(packets[0][key].RGB[channel])*tint[channel])-float64(packets[1][key].RGB[channel])) > 1 {
				t.Errorf("native %s channel %d does not support the scene/actor tint", key, channel)
			}
		}
	}
}
