package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"image"
	"image/color"
	"math"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/sff"
	"github.com/kazzmir/paintown/game/mugen/stage"
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

// One zoom lifecycle spans authored stage/BG settings, camera participation,
// mixed character coordinates, world/screen drawing, boundaries and reset.
// It checks functional projection; native zoom easing/raster parity is open.
func TestStageZoomCameraRenderingAndResetFlow(t *testing.T) {
	e := soundTestEngine(t)
	e.cfg.Config.GameWidth, e.cfg.Config.GameHeight = 320, 240
	root, err := filepath.Abs(e.dataDir)
	if err != nil {
		t.Fatal(err)
	}
	path := filepath.Join(t.TempDir(), "zoom.def")
	if err := os.WriteFile(path, []byte(`[Info]
name=Connected zoom
[Camera]
startzoom=.75
zoomout=.5
zoomin=1
boundleft=-1000
boundright=1000
tension=30
[PlayerInfo]
p1startx=-40
p2startx=40
p1facing=1
p2facing=-1
leftbound=-10000
rightbound=10000
[Bound]
screenleft=15
screenright=15
[StageInfo]
localcoord=320,240
zoffset=200
zoffsetlink=90
autoturn=0
xscale=1
yscale=1
hires=1
[BG Follow]
type=normal
spriteno=0,0
start=-128,20
delta=0,0
zoomdelta=1
scalestart=1,1
scaledelta=.125,0
windowdelta=1,0
[BG Fixed]
type=normal
spriteno=0,1
start=112,50
delta=0,0
zoomdelta=0
maskwindow=112,50,120,58
[BG Floor Anchor]
type=dummy
id=91
start=0,180
[BG Moving Floor]
type=dummy
id=90
start=0,20
positionlink=1
`), 0600); err != nil {
		t.Fatal(err)
	}
	request := match.Request{Mode: "Versus Mode", StagePath: path, Players: [2]match.Selection{
		{DefPath: filepath.Join(root, "chars/kfm/kfm.def"), Palette: 1},
		{DefPath: filepath.Join(root, "chars/kfm720/kfm720.def"), Palette: 1},
	}}
	f, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	m := f.match
	if !m.Stage.StageInfo.HiRes || m.Stage.StageInfo.XScale != 1 || m.Stage.StageInfo.YScale != 1 {
		t.Fatal("explicit stage scales did not override hires defaults")
	}
	program := `[Statedef 5900]
type=S
[State 5900, enter]
type=ChangeState
trigger1=1
value=0
[Statedef 0]
type=S
physics=N
movetype=I
ctrl=1
anim=0
[State 0, camera]
type=ScreenBound
trigger1=1
value=var(50)
movecamera=var(51),var(51)
[State 0, zoom]
type=VarSet
trigger1=1
fv=0
value=CameraZoom
[State 0, visible width]
type=VarSet
trigger1=1
fv=1
value=GameWidth
[State 0, left edge]
type=VarSet
trigger1=1
fv=2
value=LeftEdge
[State 0, right edge]
type=VarSet
trigger1=1
fv=3
value=RightEdge
[Statedef 180]
type=S
movetype=I
physics=N
anim=0
[State 180, idle]
type=Null
trigger1=1
[Statedef 5150]
type=L
movetype=H
physics=N
anim=0
[State 5150, idle]
type=Null
trigger1=1
`
	for _, p := range m.Players {
		file, err := cns.ParseNamed(strings.NewReader(program), "connected-zoom.cns")
		if err != nil {
			t.Fatal(err)
		}
		file.SetLanguageVersion("1.1")
		old := p.Character.StateFile
		file.Data, file.Size, file.Movement = old.Data, old.Size, old.Movement
		p.Character.StateFile, p.CMD = file, nil
		p.Character.ChangeState(5900, -1, -1)
		p.Character.InputSource = func(int, int) []string { return nil }
		if err := p.Character.SetScreenBound(engine.ScreenBoundParameters{}); err != nil {
			t.Fatal(err)
		}
	}
	m.Options.Input = func(int, int) []string { return nil }
	step := func(ticks int) {
		t.Helper()
		for n := 0; n < ticks; n++ {
			if err := m.Update(); err != nil {
				t.Fatal(err)
			}
		}
	}
	step(10)
	if m.Viewport.Zoom != .75 {
		t.Fatal("startzoom did not survive disabled camera participation", m.Viewport.Zoom)
	}
	// Fight-call entry restores authored root positions; spread the actors
	// after that transition, as actual player movement does.
	for n := 0; n < 650 && m.Phase != match.Active; n++ {
		step(1)
	}
	if m.Phase != match.Active {
		t.Fatal("zoom scene did not release fight control")
	}
	for side, p := range m.Players {
		c := p.Character
		c.IntVars[51] = 1
		c.X = float64((side*2-1)*400) * float64(c.LocalCoordinateWidth()) / 320
	}
	step(120)
	if math.Abs(m.Viewport.Zoom-.5) > .001 {
		t.Fatal("separated roots did not zoom out to authored limit", m.Viewport.Zoom)
	}
	for side, p := range m.Players {
		c := p.Character
		if math.Abs(c.FloatVars[1]-float64(c.LocalCoordinateWidth())/.5) > .1 {
			t.Fatal("zoomed viewport did not reach mixed-coordinate CNS", c.FloatVars[:3])
		}
		c.IntVars[50], c.IntVars[51], c.X = 1, 0, float64(side*2-1)*10000
	}
	step(2)
	for side, p := range m.Players {
		p.Character.X = float64(side*2-1) * 10000 // Retry after camera publication settles.
	}
	step(1)
	for side, p := range m.Players {
		c := p.Character
		want := c.FloatVars[2] + c.ScreenLeftMargin
		if side == 1 {
			want = c.FloatVars[3] - c.ScreenRightMargin
		}
		if math.Abs(c.X-want) > .01 {
			t.Fatal("screen constraint retained the unzoomed edge", c.X, c.FloatVars[2])
		}
		c.IntVars[50], c.IntVars[51], c.X = 0, 1, 0
	}
	step(120)
	if math.Abs(m.Viewport.Zoom-1) > .001 {
		t.Fatal("close roots did not return to zoomin", m.Viewport.Zoom)
	}

	// Small in-memory markers exercise the real BG/effect consumers. Existing
	// KFM/KFM720 assets remain shared in place; no image bank is written.
	colors := []color.NRGBA{{R: 17, G: 31, B: 47, A: 255}, {R: 53, G: 71, B: 89, A: 255}, {R: 97, G: 113, B: 131, A: 255}}
	archive := &sff.SFF{Version: [4]byte{2, 1}}
	for i, col := range colors {
		pixels := image.NewNRGBA(image.Rect(0, 0, 16, 16))
		for y := 0; y < 16; y++ {
			for x := 0; x < 16; x++ {
				pixels.SetNRGBA(x, y, col)
			}
		}
		archive.Sprites = append(archive.Sprites, sff.Sprite{ImageNumber: uint16(i), Image: pixels, ImageOpaque: pixels})
	}
	m.Stage.Background.SpriteData = archive
	for i, element := range m.Stage.Background.Elements {
		if normal, ok := element.(*background.NormalElement); ok {
			normal.Sprite = &archive.Sprites[i]
		}
	}
	animations := &air.Data{Actions: map[int]*air.Action{0: {ID: 0, LoopStart: -1, Elements: []air.Element{{Group: 0, Image: 2, Time: -1}}}}}
	lifetime := -1
	for _, screen := range []bool{false, true} {
		x, y := -64.0, -60.0
		if screen {
			x, y = 280, 100
		}
		if err := m.Effects.Queue.Push(effect.Request{Source: effect.Source{Sprites: archive, Animations: animations, Scale: 1}, Action: 0, Facing: 1, X: x, Y: y, ScreenSpace: screen, OnTop: true, RemoveTime: &lifetime}); err != nil {
			t.Fatal(err)
		}
	}
	if err := m.Effects.Flush(); err != nil {
		t.Fatal(err)
	}
	m.CameraX, m.CameraY = 0, 0
	canvas := ebiten.NewImage(320, 240)
	defer canvas.Deallocate()
	read := func(surface *ebiten.Image) []byte {
		pixels := make([]byte, surface.Bounds().Dx()*surface.Bounds().Dy()*4)
		surface.ReadPixels(pixels)
		return pixels
	}
	counts := func(pixels []byte) [3]int {
		var result [3]int
		for offset := 0; offset < len(pixels); offset += 4 {
			for i, col := range colors {
				if pixels[offset] == col.R && pixels[offset+1] == col.G && pixels[offset+2] == col.B && pixels[offset+3] == 255 {
					result[i]++
				}
			}
		}
		return result
	}
	m.Viewport.Zoom = 1
	f.Draw(canvas)
	fullScene, hud := read(f.gameCanvas), read(f.hudCanvas)
	full := counts(fullScene)
	m.Viewport.Zoom = .5
	f.Draw(canvas)
	half := counts(read(f.gameCanvas))
	if f.renderError != nil {
		t.Fatal(f.renderError)
	}
	if full != [3]int{256, 64, 512} || half != [3]int{64, 64, 320} || !bytes.Equal(hud, read(f.hudCanvas)) {
		t.Fatal("world/BG zoomdelta/screen-effect/HUD projection", full, half)
	}
	// Existing mixed-resolution fighter images also shrink before clipping.
	actor := ebiten.NewImage(320, 240)
	defer actor.Deallocate()
	area := func(zoom float64) int {
		m.Viewport.Zoom = zoom
		actor.Clear()
		character.DrawPlayers(actor, m.Actors()[:1], m.Options.GameSpace, 0, 200)
		count := 0
		for i, v := range read(actor) {
			if i%4 == 3 && v != 0 {
				count++
			}
		}
		return count
	}
	large, small := area(1), area(.5)
	if large < 100 || small < large/5 || small > large/3 {
		t.Fatal("fighter pixels did not follow camera zoom", large, small)
	}
	m.Viewport.Zoom = 1
	m.Stage.Reflection.Intensity = 128
	f.Draw(canvas)
	reflected := read(f.gameCanvas)
	different := 0
	for i := 200 * 320 * 4; i < len(reflected); i++ {
		if reflected[i] != fullScene[i] {
			different++
		}
	}
	if different < 100 {
		t.Fatal("stage reflection intensity produced no colored floor reflection", different)
	}
	m.Timeline.GlobalNoShadow = true
	f.Draw(canvas)
	if !bytes.Equal(read(f.gameCanvas)[200*320*4:], fullScene[200*320*4:]) {
		t.Fatal("global shadow suppression retained stage reflections")
	}
	m.Timeline.GlobalNoShadow = false
	// One original-beta scene establishes global BG scaling while retaining
	// the fighter's floor: xscale2/yscale.5 draws this16px sprite at
	// [220,60,252,68], with ScreenPosY200 and GameWidth320.
	bg := m.Stage.Background.Elements[0].(*background.NormalElement)
	savedStart, savedDelta, savedWindowDelta := bg.Start, bg.ScaleDelta, bg.WindowDelta
	savedLink := m.Stage.StageInfo.ZOffsetLink
	m.Stage.StageInfo.ZOffsetLink = nil
	m.Stage.StageInfo.XScale, m.Stage.StageInfo.YScale = 2, .5
	bg.Start, bg.ScaleDelta, bg.WindowDelta = [2]float64{30, 120}, [2]float64{}, [2]float64{}
	f.Draw(canvas)
	pixels := read(f.gameCanvas)
	bound := image.Rectangle{}
	for offset := 0; offset < len(pixels); offset += 4 {
		col := colors[0]
		if pixels[offset] == col.R && pixels[offset+1] == col.G && pixels[offset+2] == col.B && pixels[offset+3] == 255 {
			x, y := (offset/4)%320, (offset/4)/320
			bound = bound.Union(image.Rect(x, y, x+1, y+1))
		}
	}
	position, _ := m.Players[0].Character.LookupIdentifier("screenpos y")
	if bound != image.Rect(220, 60, 252, 68) || position.Number() != 200 || m.Stage.GroundLevel() != 200 {
		t.Fatal("native global-stage-scale scene", bound, position)
	}
	m.Stage.StageInfo.XScale, m.Stage.StageInfo.YScale = 1, 1
	m.Stage.StageInfo.ZOffsetLink = savedLink
	bg.Start, bg.ScaleDelta, bg.WindowDelta = savedStart, savedDelta, savedWindowDelta
	// Camera-driven background scaling and a moving clip window consume the
	// same stage element, independently of its zero movement delta.
	m.CameraX = 8
	f.Draw(canvas)
	if got := counts(read(f.gameCanvas))[0]; got != 512 {
		t.Fatal("normal BG camera scaling did not reach drawing", got)
	}
	bg.Window = [4]int{32, 20, 47, 35}
	m.CameraX = 0
	f.Draw(canvas)
	stationary := counts(read(f.gameCanvas))[0]
	m.CameraX = 8
	f.Draw(canvas)
	if got := counts(read(f.gameCanvas))[0]; stationary != 256 || got != 128 {
		t.Fatal("moving BG window displaced the sprite instead of clipping it", stationary, got)
	}
	m.CameraX, m.Viewport.CameraX = 0, 0
	for _, p := range m.Players {
		p.Character.IntVars[50], p.Character.IntVars[51] = 0, 0
	}
	floor := m.Stage.Background.Elements[2]
	floor.SetVelocityY(2)
	floor.SetSinY([3]float64{4, 16, 0})
	floor.(*background.CommonElement).Time = 0
	m.Players[0].Character.X, m.Players[0].Character.Facing = -40, -1
	m.Players[1].Character.X, m.Players[1].Character.Facing = 40*4, 1
	step(4)
	if m.Stage.GroundLevel() != 212 || m.Viewport.GroundY != 212 || m.Players[0].Character.Y != 0 || m.Players[0].Character.Facing != -1 || m.Players[1].Character.Facing != 1 {
		t.Fatal("linked dummy floor or stage autoturn did not reach characters", m.Stage.GroundLevel(), m.Players[0].Character.Facing)
	}
	for _, p := range m.Players {
		position, _ := p.Character.LookupIdentifier("screenpos y")
		if math.Abs(position.Number()-212*float64(p.Character.LocalCoordinateWidth())/320) > .01 {
			t.Fatal("moving floor was absent from viewport query", position)
		}
	}
	m.Players[1].Character.SetLife(0)
	m.Players[1].Character.ChangeState(5150, 0, -1)
	for n := 0; n < 900 && m.Round == 1; n++ {
		step(1)
	}
	if m.Round != 2 || m.Viewport.Zoom != .75 || len(m.Effects.Active) != 0 || m.Stage.GroundLevel() != 200 || m.Viewport.GroundY != 200 {
		t.Fatal("round reset lost startzoom or retained effects", m.Round, m.Viewport.Zoom)
	}
	step(1) // Consume the next round's initialization frame before the new scene.
	// Continue through a three-element moving-floor chain, animated parallax,
	// controller disable/hide/action changes, camera travel and round reset.
	path = filepath.Join(t.TempDir(), "linked-parallax.def")
	if err := os.WriteFile(path, []byte(`[Info]
name=Connected linked parallax
mugenversion=1.1
[Bound]
screenleft=15
screenright=15
[StageInfo]
localcoord=320,240
zoffsetlink=502
[BG Root]
type=dummy
id=501
start=-80,160
delta=.5,.25
velocity=2,1
[BG Middle]
type=dummy
id=502
positionlink=1
start=10,5
delta=9,9
velocity=1,0
[BG Animated Floor]
type=parallax
id=503
positionlink=1
start=6,15
delta=9,9
actionno=900
width=16,32
tile=1,1
tilespacing=1000,1000
maskwindow=-40,0,40,240
trans=none
mask=0
[BG Legacy Floor]
type=parallax
spriteno=0,1
start=-20,30
delta=1,0
xscale=1,2
[BG Tiled Animation]
type=anim
actionno=901
start=-160,80
delta=0,0
tile=1,0
tilespacing=16,0
trans=none
mask=0
[BGCtrlDef Scene clocks]
[BGCtrl Accelerate root]
type=PosAdd
ctrlid=501
time=2
x=4
[BGCtrl Disable floor]
type=Enabled
ctrlid=503
time=1
value=0
[BGCtrl Enable floor]
type=Enabled
ctrlid=503
time=3
value=1
[BGCtrl Change floor action]
type=Anim
ctrlid=503
time=4
value=902
[BGCtrl Hide floor]
type=Visible
ctrlid=503
time=5
value=0
[BGCtrl Show floor]
type=Visible
ctrlid=503
time=6
value=1
[BGCtrl Settle floor]
type=VelSet
ctrlid=501
time=7
x=0
y=0
[Begin Action 900]
0,0,0,0,2
0,1,0,0,2
[Begin Action 901]
0,2,0,0,-1,,AS128D0
[Begin Action 902]
0,0,0,0,-1,,AS128D0
`), 0600); err != nil {
		t.Fatal(err)
	}
	linked, err := stage.LoadStage(path, root)
	if err != nil {
		t.Fatal(err)
	}
	// Change only in-memory markers: a noncentral SFF axis and a masked tile
	// column distinguish width anchoring/masking from a solid opaque rectangle.
	archive.Sprites[0].XAxis = 7
	masked := image.NewNRGBA(image.Rect(0, 0, 16, 16))
	for y := 0; y < 16; y++ {
		for x := 1; x < 16; x++ {
			masked.SetNRGBA(x, y, colors[2])
		}
	}
	archive.Sprites[2].Image = masked
	linked.SpriteData, linked.Background.SpriteData = archive, archive
	linked.ResetBackground() // Rebind the loaded definitions to shared markers.
	m.Stage, m.CameraX, m.CameraY, m.Viewport.Zoom = linked, 0, 0, 1
	rootBG := linked.Background.Elements[0]
	middle := linked.Background.Elements[1]
	animated := linked.Background.Elements[2].(*background.ParallaxElement)
	tiled := linked.Background.Elements[4].(*background.AnimationElement)
	sx, sy := animated.GetStart()
	dx, dy := animated.GetDelta()
	if len(linked.Background.Controllers) != 1 || animated.Player == nil || !animated.Mask || !tiled.Mask || animated.GetLink() != middle || middle.GetLink() != rootBG || sx != -64 || sy != 180 || dx != .5 || dy != .25 {
		t.Fatal("linked animated parallax/controller loading", sx, sy, dx, dy)
	}
	drawBG := func(camera, zoom float64) []byte {
		t.Helper()
		canvas.Clear()
		linked.Background.CameraZoom = zoom
		if err := linked.DrawWithPalette(canvas, camera, 0, palette.Transform{}, palette.Transform{}); err != nil {
			t.Fatal(err)
		}
		return read(canvas)
	}
	rowCount := func(pixels []byte, y, index int) int {
		count := 0
		for x := 0; x < 320; x++ {
			offset, col := (y*320+x)*4, colors[index]
			if pixels[offset] == col.R && pixels[offset+1] == col.G && pixels[offset+2] == col.B && pixels[offset+3] == 255 {
				count++
			}
		}
		return count
	}
	pixels = drawBG(0, 1)
	if rowCount(pixels, 188, 0) != 80 || rowCount(pixels, 214, 0) != 0 || rowCount(pixels, 35, 1) != 16 || rowCount(pixels, 88, 2) != 300 {
		t.Fatal("width/legacy xscale, parallax tiling or masked animated spacing")
	}
	if rowCount(drawBG(10000, 1), 188, 0) != 80 {
		t.Fatal("infinite parallax tiles stopped following the visible camera")
	}
	pixels = drawBG(0, .5)
	red := 0
	for offset := 0; offset < len(pixels); offset += 4 {
		if pixels[offset] == colors[0].R && pixels[offset+1] == colors[0].G && pixels[offset+2] == colors[0].B {
			red++
			x := offset / 4 % 320
			if x < 140 || x >= 180 {
				t.Fatal("maskwindow did not follow its zoomed stage geometry", x)
			}
		}
	}
	if red == 0 {
		t.Fatal("zoomed parallax disappeared")
	}
	step(1)
	if linked.GroundLevel() != 166 || m.Viewport.GroundY != 166 || animated.Time != 1 {
		t.Fatal("parent motion was duplicated in the linked floor")
	}
	step(1)
	if animated.Enabled || animated.Time != 1 || counts(drawBG(0, 1))[0] != 0 {
		t.Fatal("disabled parallax advanced or remained visible")
	}
	step(1)
	x, y := rootBG.GetPosition()
	if x != 10 || y != 3 || linked.GroundLevel() != 168 || animated.Time != 1 {
		t.Fatal("named BG controller group applied twice or linked motion diverged", x, y, linked.GroundLevel())
	}
	step(1)
	if !animated.Enabled || animated.Time != 2 || animated.Player.CurrentElement().Image != 1 {
		t.Fatal("enabled parallax did not resume its AIR clock")
	}
	step(1)
	if animated.Player.Action.ID != 902 || counts(drawBG(0, 1))[0] == 0 {
		t.Fatal("BG Anim override did not reach animated parallax drawing")
	}
	step(1)
	if animated.Visible || animated.Time != 4 || counts(drawBG(0, 1))[0] != 0 {
		t.Fatal("invisible parallax failed to hide or keep its clock running")
	}
	step(1)
	if !animated.Visible || animated.Time != 5 || counts(drawBG(0, 1))[0] == 0 {
		t.Fatal("visible parallax did not resume drawing its current action")
	}
	// Return to the AIR blend after the explicit modern trans=none override.
	tiled.Trans = "default"
	pixels = drawBG(0, 1)
	offset := (88*320 + 1) * 4
	if pixels[offset] < 47 || pixels[offset] > 49 || pixels[offset+1] < 55 || pixels[offset+1] > 57 || pixels[offset+2] < 64 || pixels[offset+2] > 66 {
		t.Fatal("default animated blend ignored the AIR independent-alpha pair", pixels[offset:offset+4])
	}
	for n := 0; n < 650 && m.Phase != match.Active; n++ {
		step(1)
	}
	if m.Phase != match.Active {
		t.Fatal("linked stage did not reach the next fight")
	}
	m.Players[0].Character.SetLife(0)
	m.Players[0].Character.ChangeState(5150, 0, -1)
	for n := 0; n < 900 && m.Round == 2; n++ {
		step(1)
	}
	newMiddle := linked.Background.Elements[1]
	newAnimation := linked.Background.Elements[2].(*background.ParallaxElement)
	if m.Round != 3 || linked.GroundLevel() != 165 || m.Viewport.GroundY != 165 || newMiddle == middle || newAnimation.GetLink() != newMiddle || newAnimation.Time != 0 || newAnimation.Player.Action.ID != 900 || linked.Background.Controllers[0].Time != 0 {
		t.Fatal("round reset retained linked/controller/animation state")
	}
	cameraProjectionFlow(t, e, root)
}

// Continue the connected stage scenario at HD game resolution with a cropped,
// fixed zoom. One beta scene distinguishes legacy BG rounding from fractional
// actor/effect/query coordinates, then continues through clipping and reset.
func cameraProjectionFlow(t *testing.T, e *Engine, root string) {
	t.Helper()
	e.cfg.Config.GameWidth, e.cfg.Config.GameHeight = 1280, 720
	e.cfg.Video.StageFit = false
	colors := []color.NRGBA{{220, 30, 40, 255}, {30, 210, 50, 255}, {40, 60, 230, 255}, {230, 200, 30, 255}}
	markers := &sff.SFF{Version: [4]byte{2, 1}}
	animations := &air.Data{Actions: make(map[int]*air.Action)}
	for index, col := range colors {
		pixels := image.NewNRGBA(image.Rect(0, 0, 32, 32))
		for y := 0; y < 32; y++ {
			for x := 0; x < 32; x++ {
				pixels.SetNRGBA(x, y, col)
			}
		}
		markers.Sprites = append(markers.Sprites, sff.Sprite{ImageNumber: uint16(index), XAxis: 16, YAxis: 32, Image: pixels, ImageOpaque: pixels})
		animations.Actions[index] = &air.Action{ID: index, LoopStart: -1, Elements: []air.Element{{Group: 0, Image: index, Time: -1}}}
	}
	canvas := ebiten.NewImage(1280, 720)
	defer canvas.Deallocate()
	for _, legacy := range []bool{false, true} {
		version := "mugenversion=1.1\n"
		if legacy {
			version = "" // The beta defaults to date-style stage compatibility.
		}
		path := filepath.Join(t.TempDir(), "projection.def")
		source := `[Info]
name=Connected camera projection
` + version + `[Camera]
startx=10.75
starty=-20.75
startzoom=.75
zoomout=.5
zoomin=1
boundleft=-400
boundright=400
boundhigh=-180
boundlow=0
cuthigh=35
cutlow=25
[PlayerInfo]
p1startx=-40
p1starty=-20
p2startx=40
p2starty=-20
p1facing=1
p2facing=-1
leftbound=-1000
rightbound=1000
[Bound]
screenleft=15
screenright=15
[StageInfo]
localcoord=320,240
zoffset=200
[Shadow]
intensity=0
[BG Marker]
type=normal
spriteno=0,1
start=0,100
delta=1,1
maskwindow=-160,0,160,240
`
		if err := os.WriteFile(path, []byte(source), 0600); err != nil {
			t.Fatal(err)
		}
		fight, err := NewFightState(e, match.Request{Mode: "Versus Mode", StagePath: path, Players: [2]match.Selection{
			{DefPath: filepath.Join(root, "chars/kfm/kfm.def"), Palette: 1},
			{DefPath: filepath.Join(root, "chars/kfm720/kfm720.def"), Palette: 1},
		}})
		if err != nil {
			t.Fatal(err)
		}
		m := fight.match
		m.Stage.SpriteData, m.Stage.Background.SpriteData = markers, markers
		m.Stage.ResetBackground()
		program := `[Statedef 5900]
type=S
physics=N
[State 5900, fixed]
type=ScreenBound
trigger1=1
value=0
movecamera=0,0
[State 5900, enter]
type=ChangeState
trigger1=1
value=0
[Statedef 0]
type=S
movetype=I
physics=N
anim=0
ctrl=0
velset=0,0
[State 0, fixed]
type=ScreenBound
trigger1=1
value=0
movecamera=0,0
[State 0, second marker]
type=ChangeAnim
trigger1=TeamSide=2
value=2
[Statedef 180]
type=S
physics=N
anim=0
[State 180, idle]
type=Null
trigger1=1
[Statedef 5150]
type=L
movetype=H
physics=N
anim=0
[State 5150, idle]
type=Null
trigger1=1
`
		for _, p := range m.Players {
			file, err := cns.ParseNamed(strings.NewReader(program), "connected-projection.cns")
			if err != nil {
				t.Fatal(err)
			}
			file.SetLanguageVersion("1.1")
			old := p.Character.StateFile
			file.Data, file.Size, file.Movement = old.Data, old.Size, old.Movement
			p.SFF, p.AIR, p.CMD = markers, animations, nil
			p.Character.StateFile, p.Character.AirFile = file, animations
			p.Character.Palettes = palette.New(markers)
			p.Character.ChangeState(5900, -1, -1)
			if err := p.Character.ChangeAnimation(0, 1); err != nil {
				t.Fatal(err)
			}
			// These programs replace already-booted stock characters. Install
			// both roots' camera choice before the first per-entity update.
			if err := p.Character.SetScreenBound(engine.ScreenBoundParameters{}); err != nil {
				t.Fatal(err)
			}
		}
		step := func() {
			t.Helper()
			if err := m.Update(); err != nil {
				t.Fatal(err)
			}
		}
		for tick := 0; tick < 650 && m.Phase != match.Active; tick++ {
			step()
		}
		if m.Phase != match.Active || m.CameraX != 10 || m.CameraY != 120 || m.Viewport.CropY != 140 || m.Viewport.Zoom != .75 || m.Stage.Camera.RoundBackground != legacy {
			t.Fatal("native fixed-camera startup/crop/version", legacy, m.Phase, m.CameraX, m.CameraY, m.Viewport)
		}
		for _, p := range m.Players {
			factor := 320 / float64(p.Character.LocalCoordinateWidth())
			for name, want := range map[string]float64{"camerapos x": 2.5, "camerapos y": 31.25, "screenpos y": 205, "topedge": -225, "bottomedge": 15, "gameheight": 240, "screenheight": 180} {
				value, _ := p.Character.LookupIdentifier(name)
				if math.Abs(value.Number()*factor-want) > .001 {
					t.Fatal("native cropped/zoomed camera query", legacy, name, value, want)
				}
			}
		}
		lifetime := -1
		if err := m.Effects.Queue.Push(effect.Request{Source: effect.Source{Sprites: markers, Animations: animations, Scale: 4}, Action: 3, Facing: 1, X: 240, Y: -280, OnTop: true, RemoveTime: &lifetime}); err != nil {
			t.Fatal(err)
		}
		if err := m.Effects.Flush(); err != nil {
			t.Fatal(err)
		}
		draw := func() {
			t.Helper()
			fight.Draw(canvas)
			if fight.renderError != nil {
				t.Fatal(fight.renderError)
			}
		}
		bounds := func(col color.NRGBA) image.Rectangle {
			t.Helper()
			pixels := make([]byte, 1280*720*4)
			fight.gameCanvas.ReadPixels(pixels)
			rect := image.Rectangle{}
			for offset := 0; offset < len(pixels); offset += 4 {
				if pixels[offset] == col.R && pixels[offset+1] == col.G && pixels[offset+2] == col.B && pixels[offset+3] == 255 {
					x, y := offset/4%1280, offset/4/1280
					rect = rect.Union(image.Rect(x, y, x+1, y+1))
				}
			}
			return rect
		}
		wantBG := image.Rect(584, 279, 680, 375)
		if legacy {
			wantBG = image.Rect(586, 279, 682, 375)
		}
		draw()
		if bounds(colors[1]) != wantBG || bounds(colors[0]) != image.Rect(464, 519, 560, 615) || bounds(colors[3]) != image.Rect(764, 369, 860, 465) {
			t.Fatal("native BG/fighter/effect projection", legacy, bounds(colors[1]), bounds(colors[0]), bounds(colors[3]))
		}
		bg := m.Stage.Background.Elements[0].(*background.NormalElement)
		bg.LayerNo = 1
		draw()
		if bounds(colors[1]) != wantBG {
			t.Fatal("foreground lost stage projection", legacy, bounds(colors[1]))
		}
		bg.Window = [4]int{144, 75, 159, 90}
		draw()
		if bounds(colors[1]) != image.Rect(592, 285, 640, 333) {
			t.Fatal("maskwindow lost game projection/crop", legacy, bounds(colors[1]))
		}
		m.Players[1].Character.SetLife(0)
		m.Players[1].Character.ChangeState(5150, 0, -1)
		for tick := 0; tick < 1000 && m.Round == 1; tick++ {
			step()
		}
		if m.Round != 2 || m.CameraX != 10 || m.CameraY != 120 || m.Viewport.Zoom != .75 || len(m.Effects.Active) != 0 {
			t.Fatal("round reset retained projection/effects/camera state", legacy, m.Round, m.CameraX, m.CameraY)
		}
		draw()
		if bounds(colors[1]) != wantBG {
			t.Fatal("round reset lost BG compatibility/projection", legacy, bounds(colors[1]))
		}
	}
}
