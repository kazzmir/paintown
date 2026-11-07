package main

import (
	"bytes"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/input"
	sb "github.com/kazzmir/paintown/game/mugen/storyboard"
)

// One asset lifecycle loads existing sprites/SND/fonts without copies, runs
// animated/clipped layers and timed text/sounds, skips/resets a scene and exits.
func TestStoryboardAssetsTextSoundAndAnimationFlow(t *testing.T) {
	e := soundTestEngine(t)
	oldInput := input.GlobalManager
	t.Cleanup(func() { input.GlobalManager = oldInput })
	input.GlobalManager = input.NewInputManager(e.cfg)
	input.GlobalManager.Keyboard = &flowKeyboard{}
	dir := t.TempDir()
	root, err := filepath.Abs(e.dataDir)
	if err != nil {
		t.Fatal(err)
	}
	source := fmt.Sprintf(`[Info]
localcoord=320,240
[SceneDef]
spr=%s
snd=%s
font0=%s
[Scene 0]
end.time=12
clearcolor=0,0,0
window=20,20,160,120
layer0.anim=700
layer0.offset=70,60
layer1.text="AB\nCD"
layer1.font=0,0,1,255,180,100
layer1.offset=40,40
layer1.textdelay=2
layer1.starttime=1
layer1.endtime=8
sound0.value=0,0
sound0.starttime=2
sound0.volumescale=75
sound0.pan=-127
[Scene 1]
end.time=8
layer0.anim=700
layer0.offset=70,60
layer1.text="Done"
layer1.font=0,0,1
layer1.offset=40,40
[Begin Action 700]
Interpolate Offset
Interpolate Scale
Interpolate Angle
Interpolate Blend
0,0,0,0,4,,AS256D0,1,1,0
Interpolate Offset
Interpolate Scale
Interpolate Angle
Interpolate Blend
0,0,40,20,4,,AS128D128,2,0.5,90
`, filepath.Join(root, "data/kfm/intro.sff"), filepath.Join(root, "chars/kfm/kfm.snd"), filepath.Join(root, "font/f-4x6.def"))
	path := filepath.Join(dir, "scene.def")
	if err := os.WriteFile(path, []byte(source), 0600); err != nil {
		t.Fatal(err)
	}
	story, err := sb.Load(path, root)
	if err != nil {
		t.Fatal(err)
	}
	action := story.AIR.Actions[700]
	visual := action.Sample(0, 2)
	if visual.X != 20 || visual.Y != 10 || visual.ScaleX != 1.5 || visual.ScaleY != 0.75 || visual.Angle != 45 || visual.AlphaSrc != 192 || visual.AlphaDst != 64 {
		t.Fatal("AIR directives did not reach shared visual sampler", visual)
	}
	loop := air.NewPlayer(action)
	for i := 0; i < 10; i++ {
		loop.Update()
	}
	if v := loop.Visual(); v.X != 20 || v.Angle != 45 {
		t.Fatal("loop interpolation lost its boundary", v)
	}
	next := NewTitleState(e)
	state := &StoryboardState{engine: e, loaded: true, player: sb.NewPlayer(story), nextState: func() State { return next }}
	e.currentState = state
	canvas := ebiten.NewImage(640, 480)
	defer canvas.Deallocate()
	step := func() {
		t.Helper()
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
		e.Draw(canvas)
	}
	step()
	if story.Scenes[0].Layers[1].VisibleText() != "" {
		t.Fatal("text appeared before start")
	}
	step()
	if text := story.Scenes[0].Layers[1].VisibleText(); text != "A" {
		t.Fatal("text first letter", text)
	}
	step()
	pcm := make([]byte, 735*4)
	if _, err := e.sound.mixer.Read(pcm); err != nil {
		t.Fatal(err)
	}
	if bytes.Equal(pcm, make([]byte, len(pcm))) {
		t.Fatal("timed storyboard SND did not reach mixer")
	}
	for i := 0; i < 3; i++ {
		step()
	}
	if text := story.Scenes[0].Layers[1].VisibleText(); !strings.Contains(text, "B\n") {
		t.Fatal("typewriter did not advance through newline", text)
	}
	for e.currentState == state && state.player.CurrentScene() == story.Scenes[0] {
		step()
	}
	if story.Scenes[0].Layers[1].VisibleText() != "" {
		t.Fatal("endtime did not hide text")
	}
	if state.player.CurrentScene() != story.Scenes[1] || story.Scenes[1].Window != story.Scenes[0].Window {
		t.Fatal("scene window/layer lifecycle did not carry correctly")
	}
	for n := 0; n < 10 && e.currentState == state; n++ {
		step()
	}
	if e.currentState != next {
		t.Fatal("storyboard did not exit")
	}
	// A malformed referenced resource remains a contextual load error, rather
	// than silently dropping a visible layer.
	if err := os.WriteFile(path, []byte(strings.Replace(source, "layer1.font=0,0,1,255,180,100", "layer1.font=9,0,1", 1)), 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := sb.Load(path, root); err == nil || !strings.Contains(err.Error(), "layer1.font") {
		t.Fatal("missing font lost asset context", err)
	}
}
