package coord

import (
	"encoding/json"
	"math"
	"os"
	"testing"
)

func TestNativeFightHUDAndWorldProjection(t *testing.T) {
	data, err := os.ReadFile("../tests/testdata/reference/video-fit.json")
	if err != nil {
		t.Fatal(err)
	}
	var catalogue struct {
		Cases map[string]struct {
			Width       int `json:"game_width"`
			Height      int `json:"game_height"`
			Video       map[string]int
			Measurement struct {
				Window    []int `json:"window"`
				HUDCore   []int `json:"hud_core"`
				WorldCore []int `json:"world_core"`
			} `json:"video_render"`
		}
	}
	if err := json.Unmarshal(data, &catalogue); err != nil {
		t.Fatal(err)
	}
	for name, reference := range catalogue.Cases {
		t.Run(name, func(t *testing.T) {
			game := Space{reference.Width, reference.Height}
			if game.Width == 0 {
				game.Width = 1280
			}
			if game.Height == 0 {
				game.Height = 720
			}
			fit, keep := true, true
			if v, ok := reference.Video["StageFit"]; ok {
				fit = v != 0
			}
			if v, ok := reference.Video["KeepAspect"]; ok {
				keep = v != 0
			}
			view, err := StageViewport(game, Space{320, 240}, fit, 35, 25)
			if err != nil {
				t.Fatal(err)
			}
			r := reference.Measurement
			if len(r.Window) != 2 || len(r.HUDCore) != 4 || len(r.WorldCore) != 4 {
				t.Fatal("missing native marker measurements")
			}
			target := Space{r.Window[0], r.Window[1]}
			hud, err := HUDProject(view.Space, Space{1280, 720}, target, keep)
			if err != nil {
				t.Fatal(err)
			}
			x, y := hud.Point(1152, 64)
			assertCore := func(kind string, x, y, w, h float64, core []int) {
				t.Helper()
				for i, got := range []float64{x, y, x + w, y + h} {
					// Native bilinear core boundaries round at fractional anchors.
					if math.Abs(got-float64(core[i])) > 1 {
						t.Errorf("%s boundary %d=%v native=%v", kind, i, got, core[i])
					}
				}
			}
			assertCore("HUD", x, y, 32*hud.ScaleX, 16*hud.ScaleY, r.HUDCore)
			world, err := Project(view.Space, target, keep)
			if err != nil {
				t.Fatal(err)
			}
			scale := float64(view.Space.Width) / 320
			x, y = world.Point(float64(view.Space.Width)/2-45*scale, 150*scale-view.CropY)
			assertCore("world", x, y, 8*scale*world.ScaleX, 4*scale*world.ScaleY, r.WorldCore)
		})
	}
}
