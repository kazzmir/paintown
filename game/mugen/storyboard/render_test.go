package storyboard

import (
	"encoding/json"
	"fmt"
	"image/color"
	"os"
	"path/filepath"
	"reflect"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

type canvasRender struct {
	Dimensions [2]int
	Markers    []struct {
		RGB    [3]uint8
		Bounds [4]int
		Pixels int
	}
	EdgeSamples [][3]uint8 `json:"edge_samples"`
}

type canvasRenderGame struct {
	drawn bool
	err   error
}

func (g *canvasRenderGame) Update() error {
	if g.drawn {
		return ebiten.Termination
	}
	return nil
}

func (g *canvasRenderGame) Layout(int, int) (int, int) { return 320, 240 }

func (g *canvasRenderGame) Draw(*ebiten.Image) {
	if g.drawn {
		return
	}
	g.drawn = true
	for _, name := range []string{"default-wide", "default-standard", "explicit-standard", "explicit-wide", "wide-standard-window", "default-stretch"} {
		if err := checkNativeCanvas(name); err != nil {
			g.err = fmt.Errorf("%s: %w", name, err)
			return
		}
	}
}

func checkNativeCanvas(name string) error {
	data, err := os.ReadFile(filepath.Join("../tests/testdata", "storyboard-"+name+".json"))
	if err != nil {
		return err
	}
	var fixture struct {
		Inputs struct {
			LocalCoord [2]int `json:"localcoord"`
			Window     [2]int
			KeepAspect int
		}
		Render canvasRender
	}
	if err := json.Unmarshal(data, &fixture); err != nil {
		return err
	}
	local := coord.Space{Width: fixture.Inputs.LocalCoord[0], Height: fixture.Inputs.LocalCoord[1]}
	story := &Storyboard{LocalCoord: local, prepared: true}
	scene := &Scene{EndTime: 36000, ClearColorSet: true, ClearColor: color.RGBA{255, 255, 255, 255}}
	for i, rgb := range [][3]uint8{{220, 30, 40}, {30, 210, 50}} {
		image := ebiten.NewImage(32, 32)
		defer image.Deallocate()
		image.Fill(color.RGBA{rgb[0], rgb[1], rgb[2], 255})
		position := [2]float64{80, 60}
		if i == 1 {
			position = [2]float64{float64(local.Width - 16), float64(local.Height - 16)}
		}
		key := fmt.Sprintf("0-%d", i)
		scene.Layers = append(scene.Layers, &Layer{Offset: position,
			Player:     air.NewPlayer(&air.Action{Elements: []air.Element{{Group: 0, Image: i, Time: -1}}}),
			Sprites:    map[string]*background.SpriteImages{key: {Masked: image}},
			RawSprites: map[string]*sff.Sprite{key: {XAxis: 16, YAxis: 32}},
		})
	}
	story.Scenes = []*Scene{scene}
	player := NewPlayer(story)
	player.Update(InputState{})
	width, height := fixture.Inputs.Window[0], fixture.Inputs.Window[1]
	output := ebiten.NewImage(width, height)
	defer output.Deallocate()
	player.DrawToWindow(output, coord.Space{Width: 1280, Height: 720}, fixture.Inputs.KeepAspect != 0)
	if player.Error != nil {
		return player.Error
	}
	defer player.canvas.Deallocate()
	defer player.gameCanvas.Deallocate()
	pixels := make([]byte, width*height*4)
	output.ReadPixels(pixels)
	actual := fixture.Render
	actual.Markers = append(actual.Markers[:0:0], fixture.Render.Markers...)
	actual.EdgeSamples = nil
	for i := range actual.Markers {
		marker := &actual.Markers[i]
		marker.Bounds, marker.Pixels = [4]int{width, height, 0, 0}, 0
		for y := 0; y < height; y++ {
			for x := 0; x < width; x++ {
				offset := 4 * (y*width + x)
				if [3]uint8{pixels[offset], pixels[offset+1], pixels[offset+2]} == marker.RGB {
					marker.Pixels++
					marker.Bounds = [4]int{min(marker.Bounds[0], x), min(marker.Bounds[1], y), max(marker.Bounds[2], x+1), max(marker.Bounds[3], y+1)}
				}
			}
		}
	}
	for _, point := range [][2]int{{0, 0}, {width / 2, 0}, {0, height / 2}, {width / 2, height / 2}} {
		offset := 4 * (point[1]*width + point[0])
		actual.EdgeSamples = append(actual.EdgeSamples, [3]uint8{pixels[offset], pixels[offset+1], pixels[offset+2]})
	}
	if !reflect.DeepEqual(actual, fixture.Render) {
		return fmt.Errorf("render=%+v; native=%+v", actual, fixture.Render)
	}
	return nil
}

func TestNativeStoryboardCanvasRendering(t *testing.T) {
	game := &canvasRenderGame{}
	if err := ebiten.RunGame(game); err != nil {
		t.Fatal(err)
	}
	if game.err != nil {
		t.Fatal(game.err)
	}
}
