package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"image/color"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
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
	legacyPaletteAssetFlow(t, e, canvas)

	// A malformed referenced resource remains a contextual load error, rather
	// than silently dropping a visible layer.
	if err := os.WriteFile(path, []byte(strings.Replace(source, "layer1.font=0,0,1,255,180,100", "layer1.font=9,0,1", 1)), 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := sb.Load(path, root); err == nil || !strings.Contains(err.Error(), "layer1.font") {
		t.Fatal("missing font lost asset context", err)
	}
}

// Continue the asset scenario through a tiny authored V1 character, external
// ACT outfits, private/shared portraits, helper palettes and a real round reset.
// All binary input is generated in scratch; no copied asset or fixture bank.
func legacyPaletteAssetFlow(t *testing.T, e *Engine, canvas *ebiten.Image) {
	t.Helper()
	dir := t.TempDir()
	write := func(name string, data []byte) {
		t.Helper()
		path := filepath.Join(dir, name)
		if err := os.MkdirAll(filepath.Dir(path), 0700); err != nil {
			t.Fatal(err)
		}
		if err := os.WriteFile(path, data, 0600); err != nil {
			t.Fatal(err)
		}
	}
	green, gold, blue := color.NRGBA{0, 120, 0, 255}, color.NRGBA{150, 100, 0, 255}, color.NRGBA{0, 0, 130, 255}
	act := func(c color.NRGBA) []byte {
		data := make([]byte, 768)
		copy(data[254*3:], []byte{c.R, c.G, c.B})
		return data
	}
	write("Pal/Green.ACT", act(green))
	write("Pal/Gold.ACT", act(gold))
	// An unrelated ACT must never be guessed in place of authored pal1/pal2.
	write("aaa-unrelated.act", act(color.NRGBA{255, 0, 255, 255}))
	pcx := func(c color.NRGBA) []byte {
		data := make([]byte, 128)
		data[0], data[1], data[2], data[3], data[65] = 10, 5, 1, 8, 1
		binary.LittleEndian.PutUint16(data[8:], 1)
		binary.LittleEndian.PutUint16(data[66:], 2)
		data = append(data, 0, 1, 12)
		colors := make([]byte, 768)
		copy(colors[3:], []byte{c.R, c.G, c.B})
		return append(data, colors...)
	}
	archive := make([]byte, 512)
	copy(archive, "ElecbyteSpr\x00")
	archive[13], archive[15] = 1, 1
	binary.LittleEndian.PutUint32(archive[16:], 2)
	binary.LittleEndian.PutUint32(archive[20:], 5)
	binary.LittleEndian.PutUint32(archive[28:], 32)
	binary.LittleEndian.PutUint32(archive[24:], 512)
	for index, node := range []struct {
		group, item    int
		shared, linked bool
		c              color.NRGBA
	}{
		{9000, 1, false, false, blue}, {0, 0, false, false, green}, {0, 1, true, false, green}, {9000, 0, true, false, green}, {0, 2, true, true, green},
	} {
		data := pcx(node.c)
		if node.linked {
			data = nil
		}
		header := make([]byte, 32)
		if index < 4 {
			binary.LittleEndian.PutUint32(header, uint32(len(archive)+32+len(data)))
		}
		binary.LittleEndian.PutUint32(header[4:], uint32(len(data)))
		binary.LittleEndian.PutUint16(header[12:], uint16(node.group))
		binary.LittleEndian.PutUint16(header[14:], uint16(node.item))
		if node.shared {
			header[18] = 1
		}
		if node.linked {
			binary.LittleEndian.PutUint16(header[16:], 1)
		}
		archive = append(archive, header...)
		archive = append(archive, data...)
	}
	write("outfits.sff", archive)
	definition := `[Info]
name="Legacy outfits"
mugenversion=2002.04.14
pal.defaults=2,1
[Files]
sprite=outfits.sff
anim=outfits.air
cns=outfits.cns
st=outfits.cns
cmd=outfits.cmd
stcommon=
pal1=pal/green.act
pal2=Pal\Gold.ACT
`
	write("outfits.def", []byte(definition))
	write("outfits.cmd", []byte("[Defaults]\ncommand.time=15\ncommand.buffer.time=1\n"))
	write("outfits.air", []byte("[Begin Action 0]\n0,0,0,0,-1\n"))
	write("outfits.cns", []byte(`[Data]
life=1000
power=3000
attack=100
defence=100
[Size]
xscale=1
yscale=1
ground.back=8
ground.front=8
height=20
[Statedef 5900]
type=S
[State 5900, init]
type=ChangeState
trigger1=1
value=0
[Statedef 0]
type=S
movetype=I
physics=S
ctrl=1
anim=0
[State 0, idle]
type=Null
trigger1=1
[Statedef 180]
type=S
movetype=I
physics=N
anim=0
[State 180, win]
type=Null
trigger1=1
[Statedef 5150]
type=L
movetype=H
physics=N
anim=0
[State 5150, dead]
type=Null
trigger1=1
`))
	path := filepath.Join(dir, "outfits.def")
	request := match.Request{Mode: "Versus Mode", StagePath: "stages/kfm.def", Players: [2]match.Selection{{DefPath: path, Palette: 2}, {DefPath: path, Palette: 1}}}
	fight, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.currentState = fight
	step := func() {
		t.Helper()
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
	}
	reach := func(phase match.Phase) {
		t.Helper()
		for n := 0; n < 1000 && fight.match.Phase != phase; n++ {
			step()
		}
		if fight.match.Phase != phase {
			t.Fatal("legacy palette lifecycle stalled", fight.match.Phase, phase)
		}
	}
	pixel := func(c *character.Character, index int) color.NRGBA {
		t.Helper()
		img, err := c.Palettes.MappedImage(index, true)
		if err != nil {
			t.Fatal(err)
		}
		if _, _, _, alpha := img.At(0, 0).RGBA(); alpha != 0 {
			t.Fatal("ACT color zero lost masking")
		}
		return color.NRGBAModel.Convert(img.At(1, 0)).(color.NRGBA)
	}
	reach(match.Active)
	p1, p2 := fight.match.Players[0], fight.match.Players[1]
	if p1.Character.PaletteNo != 2 || pixel(p1.Character, 1) != gold || pixel(p2.Character, 1) != green || pixel(p1.Character, 0) != blue || pixel(p1.Character, 3) != green || pixel(p1.Character, 4) != gold {
		t.Fatal("selected ACT/shared/private/link color consumer")
	}
	// Exercise both production fighter and HUD sprite caches before remapping.
	e.Draw(canvas)
	p1.DrawSprite(canvas, p1.Character, 9000, 0, 10, 10, 1, 1)
	root := p1.Character
	if err := root.CreateHelper(engine.HelperParameters{ID: 90, StateNo: 0, Type: "normal", RemapPalette: [2]int{-1, 0}}); err != nil {
		t.Fatal(err)
	}
	if err := root.CreateHelper(engine.HelperParameters{ID: 91, StateNo: 0, Type: "normal", OwnPalette: true, RemapPalette: [2]int{-1, 0}}); err != nil {
		t.Fatal(err)
	}
	shared, own := root.Helpers[0], root.Helpers[1]
	if err := root.SetPaletteMapping([2]int{1, 1}, [2]int{1, 1}); err != nil {
		t.Fatal(err)
	}
	if pixel(root, 1) != green || pixel(shared, 1) != green || pixel(own, 1) != gold || pixel(root, 0) != blue {
		t.Fatal("legacy helper palette ownership")
	}
	e.Draw(canvas)
	p1.DrawSprite(canvas, root, 9000, 0, 10, 10, 1, 1)
	if root.RuntimeError != nil {
		t.Fatal(root.RuntimeError)
	}
	oldPalette := root.Palettes
	p2.Character.SetLife(0)
	p2.Character.ChangeState(5150, 0, -1)
	reach(match.PreOver)
	reach(match.PreIntro)
	if fight.match.Round != 2 || len(fight.match.World.Helpers) != 0 || p1.Character.Palettes == oldPalette || p1.Character.PaletteNo != 2 || pixel(p1.Character, 1) != gold || pixel(p1.Character, 0) != blue {
		t.Fatal("round reset did not restore authored outfit/retire helper palettes")
	}
	reach(match.Active)
	e.Draw(canvas)
	// The beta upgrading contract requires modern characters to author their
	// palette switch in state5900, even when their sprite file is still V1.
	write("outfits.def", []byte(strings.Replace(definition, "2002.04.14", "1.0", 1)))
	modern, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	fight = modern
	e.currentState = fight
	reach(match.Active)
	c := fight.match.Players[0].Character
	if c.PaletteNo != 2 || pixel(c, 1) != green {
		t.Fatal("modern V1 bypassed authored palette initialization")
	}
	if err := c.SetPaletteMapping([2]int{1, 1}, [2]int{1, 2}); err != nil {
		t.Fatal(err)
	}
	if pixel(c, 1) != gold || pixel(c, 3) != gold || pixel(c, 0) != blue {
		t.Fatal("modern authored remap/portrait consumer")
	}
	e.Draw(canvas)
	// Missing declared ACTs retain the DEF key/location; no directory fallback.
	write("outfits.def", []byte(strings.Replace(definition, `Pal\Gold.ACT`, `Pal\missing.act`, 1)))
	if _, err := character.LoadPlayer(dir, "outfits.def"); err == nil || !strings.Contains(err.Error(), "pal2") {
		t.Fatal("missing ACT lost resource context", err)
	}
}
