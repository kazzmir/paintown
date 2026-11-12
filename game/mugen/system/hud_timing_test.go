package main

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"image"
	"image/png"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/motif"
)

type hudNativeCapture struct {
	Label          string
	Path           string
	SHA256         string
	WorldRGBSHA256 string `json:"world_rgb_sha256"`
}

// Replay unchanged native-prepared assets through the production match, HUD,
// drawing and viewport path. Capture labels count real frames; clipboard values
// can stay stale during Pause/slowdown and must not be used as a frame counter.
func TestNativeTimedHUDFrames(t *testing.T) {
	bank := "../tests/testdata/reference"
	var paths []string
	for _, family := range []string{"hudduration", "hudtail", "hudstart"} {
		found, err := filepath.Glob(filepath.Join(bank, family+"-*.json"))
		if err != nil {
			t.Fatal(err)
		}
		paths = append(paths, found...)
	}
	if len(paths) != 23 {
		t.Fatal("incomplete native HUD programs", len(paths))
	}
	out := os.Getenv("MUGEN_HUD_TEST_ARTIFACT_DIR")
	if out == "" {
		var err error
		out, err = os.MkdirTemp("../temp/_tmp", "hud-timing-")
		if err != nil {
			t.Fatal(err)
		}
	} else if err := os.Mkdir(out, 0755); err != nil {
		t.Fatal(err)
	}
	oldInput := input.GlobalManager
	t.Cleanup(func() { input.GlobalManager = oldInput })
	checked := func(path, want string) []byte {
		t.Helper()
		data, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		hash := sha256.Sum256(data)
		if hex.EncodeToString(hash[:]) != want {
			t.Fatal("native input/capture changed", path)
		}
		return data
	}
	programs, runs, frames := 0, 0, 0
	for _, path := range paths {
		var spec struct {
			Schema        string
			FixtureSHA256 map[string]string `json:"fixture_sha256"`
			WorldRegion   [4]int            `json:"world_region"`
			Observations  []struct {
				NativeRun string `json:"native_run"`
				Seed      int64  `json:"random_seed"`
				Captures  []hudNativeCapture
			}
		}
		data, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		if err := json.Unmarshal(data, &spec); err != nil || spec.Schema != "timed-hud-native-v1" || spec.WorldRegion != [4]int{160, 140, 1120, 640} {
			t.Fatal("native-only HUD specification changed", path, err)
		}
		stem := strings.TrimSuffix(path, ".json")
		for suffix, want := range spec.FixtureSHA256 {
			checked(stem+suffix, want)
		}
		programs++
		for _, observation := range spec.Observations {
			runs++
			frames += len(observation.Captures)
			name := filepath.Base(observation.NativeRun)
			t.Run(name, func(t *testing.T) {
				runDir := filepath.Join("..", observation.NativeRun)
				root, err := filepath.Abs(filepath.Join(runDir, "stock"))
				if err != nil {
					t.Fatal(err)
				}
				cfg, err := config.LoadConfigWithOverrides(root)
				if err != nil {
					t.Fatal(err)
				}
				m, err := motif.LoadMotif("data/mugen1/system.def", root)
				if err != nil {
					t.Fatal(err)
				}
				e := &Engine{dataDir: root, cfg: cfg, motif: m, logicalWidth: 1280, logicalHeight: 720, pendingMatchSeed: observation.Seed}
				input.GlobalManager = input.NewInputManager(cfg)
				input.GlobalManager.Keyboard = &flowKeyboard{}
				request := match.Request{StagePath: "stages/probe-stage.def"}
				for slot := range request.Players {
					request.Players[slot] = match.Selection{DefPath: filepath.Join(root, "chars/kfm/kfm.def"), Palette: 1}
				}
				f, err := NewFightState(e, request)
				if err != nil {
					t.Fatal(err)
				}
				e.currentState = f
				t.Cleanup(func() {
					if err := e.closeSound(); err != nil {
						t.Error(err)
					}
				})
				directory := filepath.Join(out, name)
				if err := os.Mkdir(directory, 0755); err != nil {
					t.Fatal(err)
				}
				byFrame := map[int]hudNativeCapture{}
				last := 0
				for _, capture := range observation.Captures {
					if !strings.HasPrefix(capture.Label, "frame-") {
						continue // Wall-clock warmup and seek are recorder calibration.
					}
					n, err := strconv.Atoi(strings.TrimPrefix(capture.Label, "frame-"))
					if err != nil {
						t.Fatal(err)
					}
					byFrame[n] = capture
					last = max(last, n)
				}
				canvas := ebiten.NewImage(1280, 720)
				defer canvas.Deallocate()
				var measurements []map[string]any
				for frame := 0; frame <= last; frame++ {
					if e.gameTime != int32(frame) {
						t.Fatal("production process clock diverged", frame, e.gameTime)
					}
					if err := e.Update(); err != nil {
						t.Fatal(err)
					}
					canvas.Clear()
					e.Draw(canvas)
					if f.renderError != nil || f.hud.renderError != nil {
						t.Fatal("production drawing error", f.renderError, f.hud.renderError)
					}
					capture, ok := byFrame[frame]
					if !ok {
						continue
					}
					checked(filepath.Join(runDir, capture.Path), capture.SHA256)
					file, err := os.Open(filepath.Join(runDir, capture.Path))
					if err != nil {
						t.Fatal(err)
					}
					want, err := png.Decode(file)
					file.Close()
					if err != nil {
						t.Fatal(err)
					}
					pixels := make([]byte, 1280*720*4)
					canvas.ReadPixels(pixels)
					got := &image.RGBA{Pix: pixels, Stride: 1280 * 4, Rect: image.Rect(0, 0, 1280, 720)}
					nativeRGB := make([]byte, 0, 960*500*3)
					goRGB := make([]byte, 0, 960*500*3)
					differences, maxError := 0, 0
					for y := spec.WorldRegion[1]; y < spec.WorldRegion[3]; y++ {
						for x := spec.WorldRegion[0]; x < spec.WorldRegion[2]; x++ {
							r, g, b, _ := want.At(x, y).RGBA()
							expected := [3]byte{byte(r >> 8), byte(g >> 8), byte(b >> 8)}
							actual := got.RGBAAt(x, y)
							channels := [3]byte{actual.R, actual.G, actual.B}
							different := false
							for i := range channels {
								nativeRGB = append(nativeRGB, expected[i])
								goRGB = append(goRGB, channels[i])
								delta := int(channels[i]) - int(expected[i])
								if delta < 0 {
									delta = -delta
								}
								maxError = max(maxError, delta)
								different = different || delta != 0
							}
							if different {
								differences++
							}
						}
					}
					hash := sha256.Sum256(nativeRGB)
					if hex.EncodeToString(hash[:]) != capture.WorldRGBSHA256 {
						t.Fatal("original native world changed", capture.Label)
					}
					goHash := sha256.Sum256(goRGB)
					measurements = append(measurements, map[string]any{"frame": frame, "rgb_differences": differences, "max_rgb_error": maxError, "go_world_sha256": hex.EncodeToString(goHash[:]), "native_world_sha256": capture.WorldRGBSHA256, "phase": f.match.Phase, "phase_time": f.match.PhaseTime, "simulation_skipped": f.match.SimulationSkipped})
					output, err := os.OpenFile(filepath.Join(directory, fmt.Sprintf("frame-%d.png", frame)), os.O_WRONLY|os.O_CREATE|os.O_EXCL, 0644)
					if err != nil {
						t.Fatal(err)
					}
					err = png.Encode(output, got)
					output.Close()
					if err != nil {
						t.Fatal(err)
					}
					if differences != 0 {
						t.Errorf("frame%d: strict native world differs at%d pixels, max RGB error%d", frame, differences, maxError)
					}
				}
				data, err := json.MarshalIndent(measurements, "", "  ")
				if err != nil {
					t.Fatal(err)
				}
				if err := os.WriteFile(filepath.Join(directory, "measurements.json"), append(data, '\n'), 0644); err != nil {
					t.Fatal(err)
				}
			})
		}
	}
	if programs != 23 || runs != 34 || frames != 528 {
		t.Fatal("native corpus cardinality changed", programs, runs, frames)
	}
}
