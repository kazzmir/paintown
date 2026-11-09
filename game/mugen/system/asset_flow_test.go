package main

import (
	"archive/zip"
	"bytes"
	"crypto/sha256"
	"encoding/binary"
	"fmt"
	"image"
	"image/color"
	"math"
	"os"
	"path/filepath"
	"slices"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/motif"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/render"
	"github.com/kazzmir/paintown/game/mugen/sff"
	"github.com/kazzmir/paintown/game/mugen/stage"
	sb "github.com/kazzmir/paintown/game/mugen/storyboard"
	"golang.org/x/image/font/gofont/goregular"
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
	// Exercise authored font paths through the storyboard consumer. Nested
	// Windows spellings and absolute paths must load the supplied face rather
	// than silently selecting the missing-font fallback.
	facePath := filepath.Join(dir, "Fonts", "Nested", "Face.TTF")
	if err := os.MkdirAll(filepath.Dir(facePath), 0700); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(facePath, goregular.TTF, 0600); err != nil {
		t.Fatal(err)
	}
	for name, file := range map[string]string{"nested.def": `fonts\nested\face.ttf`, "absolute.def": facePath} {
		definition := "[FNT v2]\nfntversion=2,00\nfntversion=0,00\n[Def]\nType=truetype\nSize=0,18\nSpacing=100,1\nOffset=0,0\nFile=" + file + "\nBlend=1\nType=invalid\nSize=invalid\nSpacing=invalid\nOffset=invalid\nFile=missing.ttf\nBlend=0\n"
		if err := os.WriteFile(filepath.Join(dir, name), []byte(definition), 0600); err != nil {
			t.Fatal(err)
		}
	}
	// A small legacy bitmap font spans padded PCX rows, color banks and
	// commented metadata. Generate it in scratch rather than copying a bank.
	pcx := make([]byte, 128)
	pcx[0], pcx[1], pcx[2], pcx[3], pcx[65] = 10, 5, 1, 8, 1
	binary.LittleEndian.PutUint16(pcx[8:], 4) // five visible pixels, eight encoded
	binary.LittleEndian.PutUint16(pcx[10:], 2)
	binary.LittleEndian.PutUint16(pcx[66:], 8)
	pcx = append(pcx, 0xcb, 254, 0xc2, 255, 0xc3, 0, 0xc5, 255, 0xc3, 254, 12)
	colors := make([]byte, 768)
	copy(colors[252*3:], []byte{47, 113, 23, 211, 137, 29, 17, 91, 143, 173, 53, 19})
	pcx = append(pcx, colors...)
	fontMetadata := `[Def] ; legacy metadata
Type=Variable ; retain explicit variable glyph widths
Size=3,3 ; cell size
Spacing=1,0 ; glyph gap
Colors=2 ; two colors per bank
Offset=0,0
Type=invalid
Size=invalid
Spacing=invalid
Colors=invalid
Offset=invalid
[Map] ; quote and hexadecimal semicolon are literal glyphs
A 0 2
" 2 1 ; a quote is not a string delimiter here
0X3B 3 2 ; semicolon
`
	legacyFont := func(metadata string) []byte {
		data := make([]byte, 32)
		copy(data, "ElecbyteFnt\x00\x00")
		data[13] = 1
		binary.LittleEndian.PutUint32(data[16:], 32)
		binary.LittleEndian.PutUint32(data[20:], uint32(len(pcx)))
		binary.LittleEndian.PutUint32(data[24:], uint32(32+len(pcx)))
		binary.LittleEndian.PutUint32(data[28:], uint32(len(metadata)))
		return append(append(data, pcx...), metadata...)
	}
	legacyPath := filepath.Join(dir, "legacy.fnt")
	if err := os.WriteFile(legacyPath, legacyFont(fontMetadata), 0600); err != nil {
		t.Fatal(err)
	}
	// Bitmap and TrueType font definitions share native first-value admission.
	// Reuse the stock sprite bank; ignored later fields cannot redirect it.
	bitmapPath := filepath.Join(dir, "bitmap.def")
	bitmapDefinition := "[FNT v2]\nfntversion=2,00\nfntversion=0,00\n[Def]\nType=bitmap\nBankType=palette\nSize=3,6\nSpacing=1,0\nOffset=0,1\nFile=" + filepath.Join(root, "font/f-4x6.sff") + "\nType=invalid\nBankType=invalid\nSize=invalid\nSpacing=invalid\nOffset=invalid\nFile=missing.sff\n"
	if err := os.WriteFile(bitmapPath, []byte(bitmapDefinition), 0600); err != nil {
		t.Fatal(err)
	}
	source := fmt.Sprintf(`[Info]
localcoord=320,240
localcoord=invalid
[SceneDef]
spr=%s
spr=missing-later.sff
snd=%s
snd=missing-later.snd
font0=%s
font0=missing-later.def
font1=nested.def
font2=absolute.def
font3=legacy.fnt
[Scene 0]
end.time=12
end.time=99
clearcolor=0,0,0
clearcolor=255,255,255
bg.name=Scene0bg
fadein.time=2
fadein.col=200,80,40
window=20,20,160,120
layer0.anim=700
layer0.anim=9999
layer0.offset=70,60
layer1.text="AB\nCD"
layer1.font=0,0,1,255,180,100
layer1.font=99,0,1
layer1.offset=40,40
layer1.textdelay=2
layer1.starttime=1
layer1.endtime=8
layer2.text="AVé"
layer2.font=1,314,1
layer2.offset=40,85
layer3.text="A;A"
layer3.font=3,1,1
layer3.offset=100,100
sound0.value=0,0
sound0.value=9999,9999
sound0.starttime=2
sound0.volumescale=75
sound0.pan=-127
[Scene 1]
end.time=8
clearcolor=-1,0,0
layer0.anim=700
layer0.offset=70,60
layer0.starttime=2
layer1.text="Done"
layer1.font=0,0,1
layer1.offset=40,40
layer1.starttime=2
layer2.text="AVé"
layer2.font=2,-99,1
layer2.offset=40,85
layer2.starttime=2
layer3.text="A;A"
layer3.font=3,0,1
layer3.offset=100,100
layer3.starttime=2
[Scene Ending]
end.time=3
clearcolor=12,24,36
[Scene0bgDef]
[Scene0bg origin]
type=dummy
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
`, filepath.Join(root, "data/kfm/intro.sff"), filepath.Join(root, "chars/kfm/kfm.snd"), bitmapPath)
	path := filepath.Join(dir, "scene.def")
	if err := os.WriteFile(path, []byte(source), 0600); err != nil {
		t.Fatal(err)
	}
	story, err := sb.Load(path, root)
	if err != nil {
		t.Fatal(err)
	}
	if len(story.Scenes) != 3 || story.Scenes[2].Name != "Scene Ending" || story.Scenes[0].Background == nil || len(story.Scenes[0].Background.Elements) != 1 {
		t.Fatal("legacy scene-prefixed background was mistaken for a scene")
	}
	for _, f := range story.Fonts[1:3] {
		if f == nil || f.Fallback != "" || f.TextWidth("AVé") <= 0 || f.TextWidth("AVé") > 80 || f.Glyph('é', 314) == nil {
			t.Fatal("authored TrueType path/UTF-8 metrics did not reach storyboard", f)
		}
	}
	legacy := story.Fonts[3]
	if legacy == nil || legacy.TextWidth("A\";") != 8 || legacy.Glyph('"', 0) == nil || legacy.Glyph(';', 0) == nil {
		t.Fatal("legacy commented metadata, variable widths or map characters were lost")
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
	gameSpace := coord.Space{Width: e.cfg.Config.GameWidth, Height: e.cfg.Config.GameHeight}
	storyToGame, err := coord.Project(story.LocalCoord, gameSpace, true)
	if err != nil {
		t.Fatal(err)
	}
	gameToWindow, err := coord.Project(gameSpace, coord.Space{Width: 640, Height: 480}, e.cfg.Video.KeepAspect)
	if err != nil {
		t.Fatal(err)
	}
	legacyPixel := func() color.NRGBA {
		x := (100.5*storyToGame.ScaleX+storyToGame.X)*gameToWindow.ScaleX + gameToWindow.X
		y := (97.5*storyToGame.ScaleY+storyToGame.Y)*gameToWindow.ScaleY + gameToWindow.Y
		return color.NRGBAModel.Convert(canvas.At(int(x), int(y))).(color.NRGBA)
	}
	step := func() {
		t.Helper()
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
		e.Draw(canvas)
	}
	step()
	fade := color.NRGBAModel.Convert(canvas.At(400, 400)).(color.NRGBA)
	if fade.R < 98 || fade.R > 101 || fade.G < 38 || fade.G > 41 || fade.B < 18 || fade.B > 21 || fade.A != 255 {
		t.Fatal("colored fade lost its partial opacity", fade)
	}
	if story.Scenes[0].Layers[1].VisibleText() != "" {
		t.Fatal("text appeared before start")
	}
	step()
	if text := story.Scenes[0].Layers[1].VisibleText(); text != "A" {
		t.Fatal("text first letter", text)
	}
	step()
	pcm := make([]byte, 735*4)
	if got := legacyPixel(); got != (color.NRGBA{47, 113, 23, 255}) {
		t.Fatal("legacy padded PCX/color bank did not reach storyboard", got)
	}
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
	previous := make([]byte, canvas.Bounds().Dx()*canvas.Bounds().Dy()*4)
	for e.currentState == state && state.player.CurrentScene() == story.Scenes[0] {
		canvas.ReadPixels(previous)
		step()
	}
	if story.Scenes[0].Layers[1].VisibleText() != "" {
		t.Fatal("endtime did not hide text")
	}
	if state.player.CurrentScene() != story.Scenes[1] || story.Scenes[1].Window != story.Scenes[0].Window {
		t.Fatal("scene window/layer lifecycle did not carry correctly")
	}
	retained := make([]byte, len(previous))
	canvas.ReadPixels(retained)
	if story.Scenes[1].ClearColorSet || !bytes.Equal(retained, previous) {
		t.Fatal("uncleared scene transition discarded previous pixels")
	}
	step() // No layers start yet: the retained image must survive another tick.
	canvas.ReadPixels(retained)
	if !bytes.Equal(retained, previous) {
		t.Fatal("clearcolor=-1 did not retain an otherwise blank frame")
	}
	step()
	step() // Text and alpha layers now draw over the retained scene.
	if got := legacyPixel(); got != (color.NRGBA{17, 91, 143, 255}) {
		t.Fatal("legacy font bank did not change across the retained scene", got)
	}
	canvas.ReadPixels(previous)
	e.Draw(canvas) // Another presentation of the same simulation tick.
	canvas.ReadPixels(retained)
	if !bytes.Equal(retained, previous) {
		t.Fatal("presentation refresh re-blended retained scene layers")
	}
	cleared := false
	for n := 0; n < 20 && e.currentState == state; n++ {
		step()
		if state.player.CurrentScene() == story.Scenes[2] {
			cleared = true
			if got := color.NRGBAModel.Convert(canvas.At(320, 240)).(color.NRGBA); got != (color.NRGBA{12, 24, 36, 255}) {
				t.Fatal("explicit scene color did not replace retained content", got)
			}
		}
	}
	if !cleared || e.currentState != next {
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
	if err := os.WriteFile(path, []byte(source), 0600); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(legacyPath, legacyFont(strings.Replace(fontMetadata, "Colors=2", "Colors=two", 1)), 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := sb.Load(path, root); err == nil || !strings.Contains(err.Error(), "font3") || !strings.Contains(err.Error(), "legacy.fnt") || !strings.Contains(err.Error(), "metadata line 5") {
		t.Fatal("malformed legacy font lost resource/line context", err)
	}
	if err := os.WriteFile(legacyPath, legacyFont(fontMetadata), 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := sb.Load(path, root); err != nil {
		t.Fatal("corrected legacy font could not reload", err)
	}
	// The beta honors explicit map coordinates even when a legacy font says
	// Fixed. Aliases reuse atlas pixels instead of advancing beyond its edge.
	fixed := strings.Replace(fontMetadata, "Type=Variable", "Type=Fixed", 1) + "B 0 2\n"
	if err := os.WriteFile(legacyPath, legacyFont(fixed), 0600); err != nil {
		t.Fatal(err)
	}
	reloaded, err := sb.Load(path, root)
	if err != nil || reloaded.Fonts[3].TextWidth("AB\";") != 11 {
		t.Fatal("fixed font ignored explicit map coordinates/aliases", err)
	}
	// A bad first resource stays an error even when a later one is valid.
	badFirst := strings.Replace(bitmapDefinition, "File="+filepath.Join(root, "font/f-4x6.sff"), "File=missing-primary.sff\nFile="+filepath.Join(root, "font/f-4x6.sff"), 1)
	if err := os.WriteFile(bitmapPath, []byte(badFirst), 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := sb.Load(path, root); err == nil || !strings.Contains(err.Error(), "font0") || !strings.Contains(err.Error(), "missing-primary.sff") {
		t.Fatal("bad first font resource was replaced or lost context", err)
	}
	if err := os.WriteFile(bitmapPath, []byte(bitmapDefinition), 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := sb.Load(path, root); err != nil {
		t.Fatal("corrected first font resource did not reload", err)
	}
	if err := os.WriteFile(facePath, []byte("invalid font"), 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := sb.Load(path, root); err == nil || !strings.Contains(err.Error(), "font1") || !strings.Contains(err.Error(), "TrueType") {
		t.Fatal("corrupt authored font was silently replaced or lost context", err)
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
		binary.LittleEndian.PutUint16(data[10:], 1)
		binary.LittleEndian.PutUint16(data[66:], 4)
		data = append(data, 0, 0xc4, 1, 0, 0xc2, 1, 12)
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
		{9000, 1, false, false, blue}, {0, 0, false, false, green}, {0, 1, true, false, green}, {9000, 0, true, true, green}, {0, 2, true, true, green},
	} {
		data := pcx(node.c)
		if node.group == 0 && node.item == 1 {
			// Shared-palette PCX without a trailer. Its literal pixel 12
			// happens to sit where a tagged palette would usually start.
			// The archive flag must prevent truncating 769 valid bytes.
			data = append([]byte(nil), data[:128]...)
			binary.LittleEndian.PutUint16(data[8:], 31)
			binary.LittleEndian.PutUint16(data[10:], 31)
			binary.LittleEndian.PutUint16(data[66:], 32)
			pixels := bytes.Repeat([]byte{1}, 32*32)
			pixels[0], pixels[255] = 0, 12
			data = append(data, pixels...)
		}
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
name="Ignored duplicate"
mugenversion=2002.04.14
mugenversion=1.1
localcoord=320,240
localcoord=1280,720
pal.defaults=2,1
[Files]
sprite=outfits.sff
sprite=missing.sff
anim=outfits.air
anim=missing.air
cns=outfits.cns
cns=missing.cns
st=outfits.cns
st=missing.cns
cmd=outfits.cmd
cmd=missing.cmd
stcommon=
stcommon=missing.cns
pal1=pal/green.act
pal2=Pal\Gold.ACT
`
	write("outfits.def", []byte(definition))
	commandSource := "[Defaults]\ncommand.time=15\ncommand.buffer.time=1\n[Command]\nname=\"x\"\ncommand=x\ntime=1\n"
	write("outfits.cmd", []byte(commandSource))
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
	if p1.Definition.Info["name"] != "Legacy outfits" || p1.LocalCoord.Width != 320 || p1.Definition.Files["sprite"] != "outfits.sff" || p1.Definition.Files["stcommon"] != "" {
		t.Fatal("first character DEF values were replaced", p1.Definition, p1.LocalCoord)
	}
	if line := p1.Definition.FileLines["sprite"]; strings.Split(definition, "\n")[line-1] != "sprite=outfits.sff" {
		t.Fatal("DEF source context followed an ignored duplicate", line)
	}
	sharedPixels := p1.SFF.Sprites[2].Image.(*image.Paletted)
	if sharedPixels.Bounds() != image.Rect(0, 0, 32, 32) || sharedPixels.ColorIndexAt(31, 7) != 12 || sharedPixels.ColorIndexAt(31, 31) != 1 || pixel(p1.Character, 2) != gold {
		t.Fatal("shared-palette PCX pixels were mistaken for a palette trailer")
	}
	if p1.Character.PaletteNo != 2 || pixel(p1.Character, 1) != gold || pixel(p2.Character, 1) != green || pixel(p1.Character, 0) != blue || pixel(p1.Character, 3) != green || pixel(p1.Character, 4) != gold {
		t.Fatal("selected ACT/shared/private/link color consumer")
	}
	// Exercise both production fighter and HUD sprite caches before remapping.
	e.Draw(canvas)
	p1.DrawSprite(canvas, p1.Character, 9000, 0, 10, 10, 1, 1)
	p1.DrawSprite(canvas, p1.Character, 0, 1, 40, 10, 1, 1)
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
	// Local PalFX changes the fighter's draw while the HUD retains its mapped
	// outfit. Both consumers can share a base texture through effect expiry.
	root = p1.Character
	mapping := root.Palettes.MappingRevision
	root.Palettes.SetEffect(palette.Effect{Time: 1, Color: 256, Mul: [3]int{256, 256, 256}, Invert: true})
	countColor := func(want color.NRGBA) int {
		pixels := make([]byte, canvas.Bounds().Dx()*canvas.Bounds().Dy()*4)
		canvas.ReadPixels(pixels)
		count := 0
		for i := 0; i < len(pixels); i += 4 {
			if pixels[i] == want.R && pixels[i+1] == want.G && pixels[i+2] == want.B && pixels[i+3] == want.A {
				count++
			}
		}
		return count
	}
	drawPalettePair := func() {
		canvas.Fill(color.Black)
		p1.DrawScaled(canvas, 40, 40, 8, 8)
		p1.DrawSprite(canvas, root, 0, 0, 80, 40, 8, 8)
	}
	drawPalettePair()
	if countColor(gold) == 0 || countColor(color.NRGBA{105, 155, 255, 255}) == 0 || root.Palettes.MappingRevision != mapping {
		t.Fatal("shared sprite pixels mixed fighter PalFX into HUD or changed mappings")
	}
	root.Palettes.AdvanceHitEffect()
	drawPalettePair()
	if countColor(gold) == 0 || countColor(color.NRGBA{105, 155, 255, 255}) != 0 || root.Palettes.MappingRevision != mapping || root.RuntimeError != nil {
		t.Fatal("PalFX expiry did not restore mapped fighter pixels", root.RuntimeError)
	}
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
	packedCharacterAssetFlow(t, e, canvas, dir, definition)
	localizedAssetFlow(t, e, canvas, dir, definition)
	// Full character loading, unlike a partial parser view, requires a usable
	// command set. Keep its source path and allow corrected content to reload.
	write("outfits.cmd", []byte("[Defaults]\ncommand.time=15\n"))
	if _, err := character.LoadPlayer(dir, "outfits.def"); err == nil || !strings.Contains(err.Error(), "outfits.cmd") || !strings.Contains(err.Error(), "at least one [Command]") {
		t.Fatal("empty character command set lost its diagnostic", err)
	}
	write("outfits.cmd", []byte(commandSource))
	// Missing declared ACTs retain the DEF key/location; no directory fallback.
	write("outfits.def", []byte(strings.Replace(definition, `Pal\Gold.ACT`, `Pal\missing.act`, 1)))
	if _, err := character.LoadPlayer(dir, "outfits.def"); err == nil || !strings.Contains(err.Error(), "pal2") {
		t.Fatal("missing ACT lost resource context", err)
	}
	write("outfits.def", []byte(strings.Replace(definition, "sprite=outfits.sff", "sprite=missing-primary.sff", 1)))
	if _, err := character.LoadPlayer(dir, "outfits.def"); err == nil || !strings.Contains(err.Error(), "missing-primary.sff") || !strings.Contains(err.Error(), fmt.Sprintf("line %d", p1.Definition.FileLines["sprite"])) {
		t.Fatal("primary DEF resource error was replaced by a duplicate", err)
	}
}

// Continue the shared asset lifecycle in a selected language, through scene
// exit, motif-relative selection, VS, commands, combat and the next round.
func localizedAssetFlow(t *testing.T, e *Engine, canvas *ebiten.Image, dir, definition string) {
	t.Helper()
	write := func(name, text string) string {
		t.Helper()
		path := filepath.Join(dir, name)
		if err := os.WriteFile(path, []byte(text), 0600); err != nil {
			t.Fatal(err)
		}
		return path
	}
	read := func(path string) string {
		t.Helper()
		data, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		return string(data)
	}
	previousMotif, previousLanguage := e.motif, e.cfg.Config.Language
	defer func() { e.motif, e.cfg.Config.Language = previousMotif, previousLanguage }()
	e.cfg.Config.Language = "ja"
	path := write("locale.def", definition+`[ja.Info]
name="Localized"
mugenversion=1.1
[ja.Files]
sprite=outfits.sff
anim=locale.air
cns=locale.cns
st=locale.cns
cmd=locale.cmd
`)
	write("locale.air", read(filepath.Join(dir, "outfits.air"))+"\n[ja.Begin Action 0]\n0,0,0,0,1\n")
	write("locale.cns", read(filepath.Join(dir, "outfits.cns"))+`
[ja.Data]
life=500
power=2000
attack=110
defence=100
[Quotes]
victory0="English"
[ja.Quotes]
victory0="Localized quote"
`)
	write("locale.cmd", `[Remap]
x=x
[ja.Remap]
x=a
[Command]
name="localized input"
command=x
time=1
[ja.Command]
name="ignored command"
command=y
[Statedef -1]
[State -1, input]
type=VarAdd
trigger1=command="localized input"
v=40
value=1
[ja.Statedef -1]
[ja.State -1, input]
type=VarSet
trigger1=1
v=40
value=-100
`)
	// Each definition font receives the same selected language as its owner.
	face := filepath.Join(dir, "locale.ttf")
	if err := os.WriteFile(face, goregular.TTF, 0600); err != nil {
		t.Fatal(err)
	}
	write("locale font.def", `[FNT v2]
fntversion=2,00
[Def]
type=truetype
size=0,18
file=locale.ttf
[ja.Def]
type=truetype
size=0,12
file=locale.ttf
`)
	storyPath := write("locale story.def", `[SceneDef]
spr=outfits.sff
font0=locale font.def
[Scene 0 hello]
end.time=3
layer0.text="English"
layer0.font=0,0,1
layer1.anim=700
[Scene 1 finish]
end.time=2
[ja.Scene 0 hello]
end.time=3
layer0.text="Localized"
layer0.font=0,0,1
[Begin Action 700]
0,0,0,0,-1
`)
	story := &StoryboardState{engine: e, storyboard: storyPath, nextState: func() State { return NewTitleState(e) }}
	story.load()
	if story.loadError != nil || story.player == nil {
		t.Fatal("localized storyboard load", story.loadError)
	}
	scene := story.player.CurrentScene()
	if scene.Name != "Scene 0 hello" || len(scene.Layers) != 1 || scene.Layers[0].Text != "Localized" || scene.Layers[0].Font.Height != 12 {
		t.Fatal("localized scene replaced order/font or inherited an English layer", scene)
	}
	for n := 0; n < 10; n++ {
		next, err := story.Update()
		if err != nil {
			t.Fatal(err)
		}
		story.Draw(canvas)
		if next != story {
			break
		}
		if n == 9 {
			t.Fatal("localized scene failed to exit")
		}
	}
	fallback, err := sb.Load(storyPath, e.dataDir, "fr")
	if err != nil || fallback.Scenes[0].Layers[0].Text != "English" || len(fallback.Scenes[0].Layers) != 2 || fallback.Fonts[0].Height != 18 {
		t.Fatal("absent locale did not fall back to English", err)
	}
	stagePath := write("locale stage.def", `[Info]
name="English stage"
author="English author"
[ja.Info]
name="Localized stage"
[Bound]
screenleft=15
screenright=15
[Camera]
boundleft=-100
boundright=100
boundhigh=0
boundlow=0
[PlayerInfo]
p1startx=-70
p2startx=70
p1facing=1
p2facing=-1
[StageInfo]
localcoord=320,240
zoffset=200
[BGDef]
spr=outfits.sff
[BG 0]
type=normal
spriteno=0,0
[ja.BG 0]
type=normal
spriteno=1234,5678
`)
	// Share immutable HUD banks and change only its tiny definition text.
	hudDir := filepath.Join(dir, "locale HUD")
	if err := os.Mkdir(hudDir, 0700); err != nil {
		t.Fatal(err)
	}
	entries, err := os.ReadDir(filepath.Join(e.dataDir, "data/mugen1"))
	if err != nil {
		t.Fatal(err)
	}
	for _, entry := range entries {
		if entry.Name() == "fight.def" {
			continue
		}
		if err := os.Symlink(filepath.Join(e.dataDir, "data/mugen1", entry.Name()), filepath.Join(hudDir, entry.Name())); err != nil {
			t.Fatal(err)
		}
	}
	fightPath := write("locale HUD/fight.def", read(filepath.Join(e.dataDir, "data/mugen1/fight.def"))+"\n[ja.Round]\nmatch.wins=1\n[ja.Time]\npos=15,20\n")
	write("locale select.def", fmt.Sprintf("[Characters]\n\"%s\", \"%s\"\n", path, stagePath))
	motifPath := write("locale motif.def", fmt.Sprintf(`[Info]
name="English motif"
localcoord=320,240
[ja.Info]
name="Localized motif"
[Files]
spr=missing-English.sff
select=missing-English.def
[ja.Files]
spr=%s
select=locale select.def
fight=%s
font1=locale font.def
[Select Info]
rows=1
columns=1
`, filepath.Join(e.dataDir, "data/mugen1/system.sff"), fightPath))
	e.motif, err = motif.LoadMotif(motifPath, e.dataDir, e.cfg.Config.Language)
	if err != nil {
		t.Fatal(err)
	}
	selection := NewSelectState(e, "Versus Mode")
	if selection.loadError != nil || len(selection.chars) != 1 || selection.chars[0].Name != "Localized" || e.motif.Info.Name != "Localized motif" || e.motif.Fonts[1].Height != 12 {
		t.Fatal("localized motif/relative roster/portraits", selection.loadError)
	}
	request := match.Request{Mode: "Versus Mode", StagePath: stagePath, Players: [2]match.Selection{{DefPath: path, Palette: 1}, {DefPath: path, Palette: 1}}}
	vs := NewVersusState(e, request)
	if vs.loadError != nil || vs.portraits[0].Name != "Localized" {
		t.Fatal("localized VS", vs.loadError)
	}
	vs.Draw(canvas)
	f, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.currentState = f
	reach := func(phase match.Phase) {
		t.Helper()
		for n := 0; n < 1000 && f.match.Phase != phase; n++ {
			if err := e.Update(); err != nil {
				t.Fatal(err)
			}
		}
		if f.match.Phase != phase {
			t.Fatal("localized fight stalled", f.match.Phase, phase)
		}
	}
	reach(match.Active)
	p := f.match.Players[0]
	if p.Character.Life != 500 || p.Character.StateFile.Data.Power != 2000 || p.Character.StateFile.Data.Attack != 110 || p.LocalCoord.Width != 320 || p.AIR.Actions[0].Elements[0].Time != -1 || p.Character.StateFile.Quotes["ja"][0] != "Localized quote" || p.Definition.Files["pal2"] != "" || f.match.Stage.Name != "Localized stage" || f.match.Stage.Author != "" || len(f.match.Stage.Background.Elements) != 1 || f.match.Fight.Attribute("Time", "pos").RawValue != "15,20" || f.match.Fight.Attribute("Round", "round.time") != nil {
		t.Fatal("localized combat metadata/constants/HUD or group replacement", p.Definition, f.match.Stage.Name)
	}
	if line := p.Definition.FileLines["sprite"]; strings.Split(read(path), "\n")[line-1] != "sprite=outfits.sff" || line != strings.Count(definition, "\n")+5 {
		t.Fatal("localized source line", line)
	}
	keyboard := input.GlobalManager.Keyboard.(*flowKeyboard)
	keyboard.keys = map[ebiten.Key]bool{e.cfg.Input.Player1Keyboard.A: true}
	if err := e.Update(); err != nil {
		t.Fatal(err)
	}
	keyboard.keys = nil
	if err := e.Update(); err != nil {
		t.Fatal(err)
	}
	if p.Character.GetVar(40) != 1 {
		t.Fatal("localized remap/exempt command/state", p.Character.GetVar(40))
	}
	e.Draw(canvas)
	f.match.Players[1].Character.SetLife(0)
	f.match.Players[1].Character.ChangeState(5150, 0, -1)
	reach(match.Finished)
	// A fresh load must select the same language without stale scene/input state.
	f, err = NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.currentState = f
	reach(match.Active)
	if f.match.Players[0].Character.Life != 500 || f.match.Players[0].Character.GetVar(40) != 0 {
		t.Fatal("localized rematch retained prior combat")
	}
	// The selected AST retains quote banks for runtime fallback and its source
	// positions; consumers do not mutate the original parsed definition.
	ast, err := parsers.Parse(strings.NewReader(read(filepath.Join(dir, "locale.cns"))))
	if err != nil {
		t.Fatal(err)
	}
	selected := parsers.Localize(ast, "ja")
	quotes := 0
	for _, group := range selected.Sections {
		if strings.HasSuffix(strings.ToLower(group.Name), "quotes") {
			quotes++
		}
	}
	if quotes != 2 || ast.Sections[0].Name != "Data" {
		t.Fatal("quote banks or original AST lost")
	}
}

// Continue the same authored asset lifecycle through the beta's ZIP selection
// syntax, cached alternate DEFs, intro, teams/fight/reset, ending and cleanup.
func packedCharacterAssetFlow(t *testing.T, e *Engine, canvas *ebiten.Image, dir, definition string) {
	t.Helper()
	files := make(map[string][]byte)
	for _, name := range []string{"outfits.sff", "outfits.air", "outfits.cns", "outfits.cmd", "Pal/Green.ACT", "Pal/Gold.ACT"} {
		data, err := os.ReadFile(filepath.Join(dir, name))
		if err != nil {
			t.Fatal(err)
		}
		files[name] = data
	}
	portraits, err := sff.ParsePortraits(bytes.NewReader(files["outfits.sff"]))
	if err != nil || portraits.Sprites[2].Image != nil || portraits.Sprites[3].Image == nil || !portraits.Sprites[3].Linked {
		t.Fatal("portrait loading decoded fight frames or lost a linked portrait", err)
	}
	definition += "[Arcade]\nintro.storyboard=Story\\Ending.def\nending.storyboard=Story\\Ending.def\n"
	files["Outfits pack.def"] = []byte(definition)
	files["Alternate.def"] = []byte(strings.Replace(definition, "Legacy outfits", "Packed alternate", 1))
	files["Story/Ending.def"] = []byte(`[SceneDef]
spr=..\outfits.sff
[Scene 0]
end.time=2
clearcolor=0,0,0
layer0.anim=700
layer0.offset=80,80
[Begin Action 700]
0,0,0,0,-1
`)
	writePack := func(name string, contents map[string][]byte) string {
		t.Helper()
		var buffer bytes.Buffer
		writer := zip.NewWriter(&buffer)
		for member, data := range contents {
			output, err := writer.Create(member)
			if err != nil {
				t.Fatal(err)
			}
			if _, err := output.Write(data); err != nil {
				t.Fatal(err)
			}
		}
		if err := writer.Close(); err != nil {
			t.Fatal(err)
		}
		path := filepath.Join(dir, name)
		if err := os.WriteFile(path, buffer.Bytes(), 0600); err != nil {
			t.Fatal(err)
		}
		return path
	}
	pack := writePack("Outfits pack.ZIP", files)
	stageDir := filepath.Join(dir, "Training Room")
	if err := os.Symlink(filepath.Join(e.dataDir, "stages"), stageDir); err != nil {
		t.Fatal(err)
	}
	stagePath := filepath.Join(stageDir, "kfm.def")
	selectPath := filepath.Join(dir, "packed select.def")
	selectText := fmt.Sprintf("[Characters]\n\"%s\", stages/kfm.def\n\"%s:alternate.def\", random\nempty\nEmPtY\nrandomselect\n[ExtraStages]\n\"%s\"\n[Options]\narcade.maxmatches=1\n", pack, pack, stagePath)
	if err := os.WriteFile(selectPath, []byte(selectText), 0600); err != nil {
		t.Fatal(err)
	}
	// Retain the first layout in a legacy motif with alternative settings
	// below it, and interpret startcell as row,column.
	motifPath := filepath.Join(dir, "packed motif.def")
	motifSource := fmt.Sprintf(`[Info]
name="First motif"
name="Ignored motif"
localcoord=320,240
localcoord=invalid
[Files]
spr=%s
spr=missing-duplicate.sff
font1=f-4x6.def
font1=missing-duplicate.def
select=%s
fight=%s
[Select Info]
rows=3
columns=2
pos=10,20
p1.cursor.startcell=0,1
p2.cursor.startcell=0,99
portrait.scale=.5,.5
showemptyboxes=0
moveoveremptyboxes=0
rows=1
columns=1
pos=0,0
[Title Info]
menu.pos=20,30
menu.pos=9999,9999
menu.itemname.arcade="First arcade"
menu.itemname.arcade="Ignored arcade"
[Music]
title.bgm=
title.bgm=missing-later.mp3
title.bgm.loop=1
title.bgm.loop=invalid
[TitleBGDef]
spr=%q
spr=missing-later.sff
bgclearcolor=12,24,36
bgclearcolor=255,255,255
`, filepath.Join(e.dataDir, "data/mugen1/system.sff"), filepath.Base(selectPath), filepath.Join(e.dataDir, "data/mugen1/fight.def"), filepath.Join(e.dataDir, "data/mugen1/system.sff"))
	if err := os.WriteFile(motifPath, []byte(motifSource), 0600); err != nil {
		t.Fatal(err)
	}
	badBackground := strings.Replace(motifSource, "[TitleBGDef]\nspr=", "[TitleBGDef]\nspr=\nspr=", 1)
	if err := os.WriteFile(motifPath, []byte(badBackground), 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := motif.LoadMotif(motifPath, e.dataDir); err == nil || !strings.Contains(err.Error(), "titlebgdef") || !strings.Contains(err.Error(), "empty resource") {
		t.Fatal("authored blank BG resource was rescued or lost context", err)
	}
	if err := os.WriteFile(motifPath, []byte(motifSource), 0600); err != nil {
		t.Fatal(err)
	}
	previousMotif := e.motif
	e.motif, err = motif.LoadMotif(motifPath, e.dataDir)
	if err != nil {
		t.Fatal(err)
	}
	defer func() { e.motif = previousMotif }()
	if e.motif.Info.Name != "First motif" || e.motif.Info.LocalCoord.Width != 320 || e.motif.Fonts[1] == nil || e.motif.TitleInfo.MenuPos != [2]float64{20, 30} || e.motif.TitleInfo.ItemNames["arcade"] != "First arcade" || e.motif.TitleBG.BGClearColor != (color.RGBA{12, 24, 36, 255}) || e.motif.Music["title"].File != "" {
		t.Fatal("motif loading replaced first metadata/resources", e.motif.Info, e.motif.Files)
	}
	selection := NewSelectState(e, "Arcade Mode")
	if selection.loadError != nil || len(selection.chars) != 5 || len(selection.stages) != 2 || selection.stages[1] != stagePath {
		t.Fatal("packed character/stage filenames did not reach selection", selection.loadError, selection.stages)
	}
	if !selection.chars[2].Empty || !selection.chars[3].Empty || selection.charAt(2%selection.cols(), 2/selection.cols()) != nil || selection.chosen(2%selection.cols(), 2/selection.cols()) != nil || selection.chosen(4%selection.cols(), 4/selection.cols()) == nil {
		t.Fatal("empty slots shifted the roster or became random selections")
	}
	if selection.cols() != 2 || selection.rows() != 3 || e.motif.SelectInfo.Pos != [2]int{10, 20} || selection.p1CursorX != 1 || selection.p1CursorY != 0 {
		t.Fatal("first motif layout or row,column startcell was lost", e.motif.SelectInfo, selection.p1CursorX, selection.p1CursorY)
	}
	x, y := 0, 0
	selection.moveCursor(&x, &y, 0, 1)
	if x != 0 || y != 2 {
		t.Fatal("cursor did not skip empty slots", x, y)
	}
	first, alternate := selection.chars[0], selection.chars[1]
	if first.SmallPortrait == nil || first.BigPortrait == nil || alternate.Name != "Packed alternate" || filepath.Dir(first.DefPath) != filepath.Dir(alternate.DefPath) || first.IntroStoryboard == "" || first.EndingStoryboard == "" {
		t.Fatal("ZIP portraits, alternate DEF, cache or storyboard paths were not loaded")
	}
	if small, big := color.NRGBAModel.Convert(first.SmallPortrait.At(1, 0)), color.NRGBAModel.Convert(first.BigPortrait.At(1, 0)); small != (color.NRGBA{0, 120, 0, 255}) || big != (color.NRGBA{0, 0, 130, 255}) {
		t.Fatal("selective portrait decode changed shared/private colors", small, big)
	}
	selection.p1CursorX, selection.p1CursorY, selection.p2CursorX, selection.p2CursorY = 0, 0, 1, 0
	selection.palettes = [2]int{2, 1}
	// Arcade has no manually selected opposing cursor. Invalid/empty hidden
	// coordinates must not prevent its scheduled opponent from being built.
	selection.p2CursorX, selection.p2CursorY = 0, 17
	selection.pickMember(0, first, 2)
	selection.p1CursorX, selection.p1CursorY = 0, 1 // selected roster survives cursor changes

	next, err := selection.startMatch()
	if err != nil {
		t.Fatal(err)
	}
	if _, ok := next.(*StoryboardState); !ok {
		t.Fatal("packed arcade intro was skipped")
	}
	e.currentState = next
	step := func() {
		t.Helper()
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
		e.Draw(canvas)
	}
	var fight *FightState
	for tick := 0; tick < 900; tick++ {
		step()
		if current, ok := e.currentState.(*FightState); ok && current.match.Phase == match.Active {
			fight = current
			break
		}
	}
	if fight == nil || fight.match.Players[0].Character.PaletteNo != 2 {
		t.Fatal("packed selection/intro/versus/fight handoff stalled")
	}
	fight.match.Players[1].Character.SetLife(0)
	fight.match.Players[1].Character.ChangeState(5150, 0, -1)
	for tick := 0; tick < 1000 && fight.match.Round == 1; tick++ {
		step()
	}
	if fight.match.Round != 2 || fight.match.Players[0].Character.PaletteNo != 2 {
		t.Fatal("packed round reset lost its character or palette")
	}
	// Direct ZIP references share the same extracted files. Resolving team
	// members must not rewrite the caller's rematch/campaign template slices.
	e.modeRun = nil
	request := match.Request{Mode: "Versus Mode", StagePath: stagePath}
	for side, name := range []string{pack, pack + ":ALTERNATE.DEF"} {
		request.Teams[side] = match.TeamSelection{Mode: match.Single, Members: []match.Selection{{DefPath: name, Palette: side + 1}}}
	}
	rematch, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	if request.Teams[0].Members[0].DefPath != pack || rematch.match.Players[0].DefPath != first.DefPath || rematch.match.Players[1].DefPath != alternate.DefPath {
		t.Fatal("ZIP reload repeated expansion or mutated a roster template")
	}
	e.currentState = &StoryboardState{engine: e, storyboard: first.EndingStoryboard, nextState: func() State { return NewTitleState(e) }}
	for tick := 0; tick < 3; tick++ {
		step()
	}
	if _, ok := e.currentState.(*TitleState); !ok {
		t.Fatal("packed ending did not finish")
	}
	if _, err := os.Stat(first.DefPath); !os.IsNotExist(err) {
		t.Fatal("title return retained inactive character pack storage", err)
	}
	reopened := NewSelectState(e, "Versus Mode")
	if reopened.loadError != nil || reopened.chars[0].DefPath == first.DefPath || reopened.chars[0].SmallPortrait == nil {
		t.Fatal("selection could not reload a retired character pack", reopened.loadError)
	}
	first = reopened.chars[0]
	if _, err := e.characterPacks.Resolve(pack + ":missing.def"); err == nil || !strings.Contains(err.Error(), "missing.def") {
		t.Fatal("missing packed DEF lost context", err)
	}
	invalid := writePack("Invalid.zip", map[string][]byte{"../escape.def": []byte("invalid")})
	if _, err := e.characterPacks.Resolve(invalid); err == nil || !strings.Contains(err.Error(), "invalid ZIP entry") {
		t.Fatal("archive entry escaped its expansion directory", err)
	}
	// A bad AIR in a selectable pack must fail at the fight handoff with the
	// asset/action/line intact. Restore the same pack and load it successfully;
	// no copied bank or per-parameter fixture is needed.
	animation := filepath.Join(filepath.Dir(first.DefPath), "outfits.air")
	validAnimation, err := os.ReadFile(animation)
	if err != nil {
		t.Fatal(err)
	}
	for _, count := range []string{"-1", "1e99"} {
		if err := os.WriteFile(animation, []byte("[Begin Action 700]\nClsn2Default: "+count+"\n0,0,0,0,-1\n"), 0600); err != nil {
			t.Fatal(err)
		}
		if _, err := NewFightState(e, request); err == nil || !strings.Contains(err.Error(), "outfits.air") || !strings.Contains(err.Error(), "AIR action 700 line 2") || !strings.Contains(err.Error(), "clsn2default") {
			t.Fatal("malformed packed animation lost its load context", err)
		}
	}
	if err := os.WriteFile(animation, validAnimation, 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := NewFightState(e, request); err != nil {
		t.Fatal("corrected packed animation could not reload", err)
	}
	spritePath := filepath.Join(filepath.Dir(first.DefPath), "outfits.sff")
	validSprites, err := os.ReadFile(spritePath)
	if err != nil {
		t.Fatal(err)
	}
	badSprites := append([]byte(nil), validSprites...)
	secondSprite := int(binary.LittleEndian.Uint32(badSprites[512:]))
	binary.LittleEndian.PutUint16(badSprites[secondSprite+32+66:], 1)
	if err := os.WriteFile(spritePath, badSprites, 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := NewFightState(e, request); err == nil || !strings.Contains(err.Error(), "outfits.sff") || !strings.Contains(err.Error(), "sprite 1 (0,0)") || !strings.Contains(err.Error(), "PCX row size") {
		t.Fatal("malformed packed sprite lost archive/frame context", err)
	}
	if err := os.WriteFile(spritePath, validSprites, 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := NewFightState(e, request); err != nil {
		t.Fatal("corrected packed sprites could not reload", err)
	}
	if err := e.Close(); err != nil {
		t.Fatal(err)
	}
	if _, err := os.Stat(first.DefPath); !os.IsNotExist(err) {
		t.Fatal("expanded ZIP files survived engine shutdown", err)
	}
	if _, err := os.Stat(pack); err != nil {
		t.Fatal("engine cleanup removed the source character ZIP", err)
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
	if err := os.WriteFile(path, []byte(fmt.Sprintf(`[Info]
name=Connected zoom
[Camera]
startzoom=.75
startzoom=2
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
localcoord=invalid
zoffset=200
zoffsetlink=90
autoturn=0
xscale=1
yscale=1
hires=1
[BGDef]
spr=%q
spr=missing-later.sff
[BG Follow]
type=normal
type=invalid
spriteno=0,0
spriteno=9999,9999
start=-128,20
start=9000,9000
delta=0,0
zoomdelta=1
scalestart=1,1
scaledelta=.125,0
windowdelta=1,0
[BG Follow]
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
[StageInfo]
localcoord=invalid
[Camera]
startzoom=9
`, filepath.Join(root, "stages/kfm.sff"))), 0600); err != nil {
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
	full[2] = counts(read(canvas))[2] // foreground is composed after the HUD
	m.Viewport.Zoom = .5
	f.Draw(canvas)
	half := counts(read(f.gameCanvas))
	half[2] = counts(read(canvas))[2]
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
	// Foreground Explods cover the HUD and retain that ordering through the
	// final palette pass. Their finite lifetime still belongs to the scene.
	foregroundLife := 2
	if err := m.Effects.Queue.Push(effect.Request{Source: effect.Source{Sprites: archive, Animations: animations, Scale: 1}, Action: 0, Facing: 1,
		X: 12, Y: 12, ScreenSpace: true, OnTop: true, RemoveTime: &foregroundLife}); err != nil {
		t.Fatal(err)
	}
	if err := m.Effects.Flush(); err != nil {
		t.Fatal(err)
	}
	f.Draw(canvas)
	if got := color.NRGBAModel.Convert(canvas.At(16, 16)).(color.NRGBA); got != colors[2] {
		t.Fatal("foreground effect did not cover the HUD", got)
	}
	// A projected foreground keeps parent coordinates inside a letterboxed
	// subimage. Global inversion occurs on the source and the final scene,
	// restoring this opaque marker while leaving outer letterboxes black.
	wide := ebiten.NewImage(480, 240)
	defer wide.Deallocate()
	savedAspect := e.cfg.Video.KeepAspect
	e.cfg.Video.KeepAspect = true
	m.Effects.Active[len(m.Effects.Active)-1].XAngle = 15
	savedPalette := m.Timeline.AllPalette
	m.Timeline.AllPalette = palette.New(nil)
	m.Timeline.AllPalette.SetEffect(palette.Effect{Time: -1, Color: 256, Mul: [3]int{256, 256, 256}, Invert: true})
	f.Draw(wide)
	if got := color.NRGBAModel.Convert(wide.At(96, 16)).(color.NRGBA); got != colors[2] {
		t.Fatal("projected foreground/global palette/window origin", got)
	}
	if got := color.NRGBAModel.Convert(wide.At(10, 16)).(color.NRGBA); got.R != 0 || got.G != 0 || got.B != 0 {
		t.Fatal("global palette colored an outer letterbox", got)
	}
	e.cfg.Video.KeepAspect = savedAspect
	m.Timeline.AllPalette = savedPalette
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
looptime=0
looptime=1
[BGCtrl Accelerate root]
type=PosAdd
type=Null
ctrlid=501
ctrlid=999
time=2
time=200
x=4
x=100
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
	rgbaBackgroundAssetFlow(t, root)
	afterImageVisualFlow(t, root)
}

// Continue the stage lifecycle through native-confirmed PNG32/raw RGBA masks,
// fractional scaling, visibility and reset, reusing the existing tiny archive.
func rgbaBackgroundAssetFlow(t *testing.T, root string) {
	t.Helper()
	archive, err := filepath.Abs("../sff/testdata/formats.sff")
	if err != nil {
		t.Fatal(err)
	}
	source := fmt.Sprintf("[Info]\nmugenversion=1.1\n[Bound]\nscreenleft=15\nscreenright=15\n[StageInfo]\nlocalcoord=320,240\nzoffset=200\n[BGDef]\nspr=%q\ndebugbg=1\n", archive)
	for _, sprite := range [][2]int{{3, 80}, {5, 180}} {
		for _, mode := range [][2]int{{0, 30}, {1, 50}} {
			source += fmt.Sprintf("[BG %d mask%d]\ntype=normal\nspriteno=0,%d\nstart=%d,%d\ndelta=0,0\nmask=%d\n", sprite[0], mode[0], sprite[0], sprite[1]-160, mode[1], mode[0])
		}
	}
	source += "[BG scaled]\ntype=normal\nspriteno=0,3\nstart=-79.75,70\ndelta=0,0\nmask=1\nscalestart=4,4\n"
	path := filepath.Join(t.TempDir(), "rgba.def")
	if err := os.WriteFile(path, []byte(source), 0600); err != nil {
		t.Fatal(err)
	}
	loaded, err := stage.LoadStage(path, root)
	if err != nil {
		t.Fatal(err)
	}
	canvas := ebiten.NewImage(320, 240)
	defer canvas.Deallocate()
	draw := func() {
		t.Helper()
		if err := loaded.DrawWithPalette(canvas, 0, 0, palette.Transform{}, palette.Transform{}); err != nil {
			t.Fatal(err)
		}
	}
	pixel := func(x, y int) [3]uint8 {
		c := color.NRGBAModel.Convert(canvas.At(x, y)).(color.NRGBA)
		return [3]uint8{c.R, c.G, c.B}
	}
	draw()
	unmasked := [][3]uint8{{0, 0, 0}, {3, 16, 9}, {15, 32, 18}, {33, 48, 27}, {60, 65, 36}, {94, 81, 45}, {135, 97, 54}, {184, 114, 63}}
	masked := [][3]uint8{{255, 0, 255}, {227, 16, 233}, {208, 32, 211}, {195, 48, 189}, {191, 65, 167}, {194, 81, 145}, {204, 97, 123}, {222, 114, 101}}
	for _, x := range []int{80, 180} {
		for offset := range unmasked {
			if got := pixel(x+offset, 32); got != unmasked[offset] {
				t.Fatal("native unmasked RGBA background", x, offset, got, unmasked[offset])
			}
			if got := pixel(x+offset, 52); got != masked[offset] {
				t.Fatal("native masked RGBA background", x, offset, got, masked[offset])
			}
		}
	}
	for offset, want := range map[int][3]uint8{2: {253, 1, 254}, 6: {226, 16, 232}, 10: {207, 31, 210}, 14: {195, 46, 188}, 18: {191, 62, 166}, 22: {195, 77, 144}, 26: {205, 92, 122}, 29: {219, 104, 105}} {
		if got := pixel(80+offset, 79); got != want {
			t.Fatal("native fractional RGBA background filtering", offset, got, want)
		}
	}
	first := loaded.Background.Elements[0].(*background.NormalElement)
	first.Mask = true
	loaded.Update()
	draw()
	if pixel(83, 32) != masked[3] {
		t.Fatal("mask change reused the opaque texture")
	}
	first.SetVisible(false)
	draw()
	if pixel(83, 32) != ([3]uint8{255, 0, 255}) {
		t.Fatal("hidden RGBA background retained its pixels")
	}
	loaded.ResetBackground()
	draw()
	if pixel(83, 32) != unmasked[3] {
		t.Fatal("background reset retained the changed RGBA mask/visibility")
	}
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
			if col == colors[3] {
				canvas.ReadPixels(pixels) // OnTop effects follow the HUD.
			} else {
				fight.gameCanvas.ReadPixels(pixels)
			}
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

// One retained-pose lifecycle records AIR1.1 interpolation, changes animation,
// freezes, drains/resumes the trail, then retires its state at a real round reset.
func afterImageVisualFlow(t *testing.T, root string) {
	t.Helper()
	source, err := os.Open("../sff/testdata/formats.sff")
	if err != nil {
		t.Fatal(err)
	}
	archive, err := sff.Parse(source)
	source.Close()
	if err != nil {
		t.Fatal(err)
	}
	animations, err := air.Parse(strings.NewReader(`[Begin Action 0]
-1,0,0,0,-1
[Begin Action 700]
0,4,0,0,20
Interpolate Offset
0,4,40,0,20
[Begin Action 701]
0,4,12,0,-1
[Begin Action 702]
0,1,0,0,-1
[Begin Action 710]
0,4,0,0,-1,,AS128D128
[Begin Action 711]
0,3,0,0,5,,AS128D128
0,3,0,0,5,,AS192D64
LoopStart
0,3,0,0,-1,,AS160D96
`))
	if err != nil {
		t.Fatal(err)
	}
	stageSource := fmt.Sprintf(`[Info]
mugenversion=1.1
[Camera]
boundleft=0
boundright=0
boundhigh=0
boundlow=0
verticalfollow=0
tension=60
[PlayerInfo]
p1startx=-100
p2startx=100
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
[BGDef]
spr=%q
[BG Anchor]
type=dummy
`, filepath.Join(root, "stages/kfm.sff"))
	path := filepath.Join(t.TempDir(), "trail.def")
	if err := os.WriteFile(path, []byte(stageSource), 0600); err != nil {
		t.Fatal(err)
	}
	selection := match.Selection{DefPath: filepath.Join(root, "chars/kfm/kfm.def"), Palette: 1}
	m, err := match.New(match.Request{Players: [2]match.Selection{selection, selection}, StagePath: path, Mode: "versus"}, match.Options{RootDir: root, FightPath: "data/fight.def", GameSpace: coord.Default(), RoundTime: 99, LifePercent: 100, RandomSeed: 1})
	if err != nil {
		t.Fatal(err)
	}
	const program = `[Statedef 5900]
type=S
movetype=I
physics=N
[State 5900, enter]
type=ChangeState
trigger1=1
value=0
[Statedef 0]
type=S
movetype=I
physics=N
anim=0
ctrl=1
[State 0, begin]
type=ChangeState
trigger1=RoundState=2
trigger1=TeamSide=1
value=700
[Statedef 700]
type=S
movetype=I
physics=N
anim=700
ctrl=0
velset=0,0
[State 700, setup]
type=AfterImage
trigger1=Time=0
time=-1
length=6
timegap=1
framegap=1
palcolor=256
palbright=0,0,0
palcontrast=256,256,256
palpostbright=0,0,0
paladd=0,0,0
palmul=1,1,1
trans=none
[State 700, move]
type=PosAdd
trigger1=Time<=6
x=20
[State 700, height]
type=PosSet
trigger1=1
y=-60
[State 700, hold animation]
type=ChangeAnim
trigger1=Time=6
value=701
[State 700, observe]
type=DisplayToClipboard
trigger1=Time=6
text="P0:%d,%d,%d,%d,%d,%d\n"
params=Time,AnimElemTime(1),Pos X,CameraPos X,GameTime,RoundState
[State 700, freeze]
type=Pause
trigger1=Time=6
time=10000
movetime=0
[Statedef 8000]
type=S
movetype=I
physics=N
anim=701
ctrl=0
velset=0,0
[State 8000, idle]
type=Null
trigger1=1
[Statedef 180]
type=S
movetype=I
physics=N
anim=0
ctrl=0
[State 180, idle]
type=Null
trigger1=1
[Statedef 5150]
type=L
movetype=H
physics=N
anim=0
ctrl=0
[State 5150, idle]
type=Null
trigger1=1
`
	for _, p := range m.Players {
		file, err := cns.ParseNamed(strings.NewReader(program), "trail-flow.cns")
		if err != nil {
			t.Fatal(err)
		}
		file.SetLanguageVersion("1.1")
		previous := p.Character.StateFile
		file.Data, file.Size, file.Movement = previous.Data, previous.Size, previous.Movement
		p.SFF, p.AIR, p.CMD = archive, animations, nil
		p.Character.StateFile = file
		p.Character.AirFile = animations
		p.Character.Palettes = palette.New(archive)
		p.Character.ChangeAnimation(0, 1)
		p.Character.ChangeState(5900, -1, -1)
	}
	for tick := 0; tick < 700 && !m.Timeline.Paused(); tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
	}
	if !m.Timeline.Paused() {
		t.Fatal("trail flow did not reach Pause")
	}
	canvas := ebiten.NewImage(320, 240)
	defer canvas.Deallocate()
	runs := func() [][2]int {
		t.Helper()
		canvas.Fill(color.NRGBA{255, 0, 255, 255})
		character.DrawPlayers(canvas, m.Players[:], coord.Default(), 0, 200)
		pixels := make([]byte, 320*240*4)
		canvas.ReadPixels(pixels)
		result := [][2]int{}
		start := -1
		for x := 0; x <= 320; x++ {
			visible := x < 320 && pixels[(142*320+x)*4+2] == 75
			if visible && start < 0 {
				start = x
			}
			if !visible && start >= 0 {
				result = append(result, [2]int{start, x})
				start = -1
			}
		}
		return result
	}
	// Observed in one beta interpolation/change-animation/Pause sequence.
	want := [][2]int{{124, 132}, {146, 154}, {168, 176}, {190, 198}, {212, 220}}
	if got := runs(); !slices.Equal(got, want) {
		t.Fatal("native saved AIR pose positions", got, want)
	}
	for tick := 0; tick < 4; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
	}
	if got := runs(); !slices.Equal(got, want) {
		t.Fatal("paused trails changed their saved AIR clocks", got)
	}
	c := m.Players[0].Character
	m.Timeline.ClearPauses()
	c.ChangeState(8000, -1, -1)
	c.SetAfterImageTime(0)
	for tick := 0; tick < 8; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
	}
	if got := runs(); !slices.Equal(got, [][2]int{{212, 220}}) {
		t.Fatal("expired recording did not drain", got)
	}
	c.SetAfterImageTime(-1)
	for tick := 0; tick < 3; tick++ {
		c.X -= 20
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
	}
	if got := runs(); len(got) < 2 {
		t.Fatal("recording failed to resume", got)
	}
	old := c
	m.Players[1].Character.SetLife(0)
	m.Players[1].Character.ChangeState(5150, 0, -1)
	for tick := 0; tick < 1100 && m.Round == 1; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
	}
	if m.Round != 2 || m.Players[0].Character == old || m.Players[0].Character.HasAfterImage() {
		t.Fatal("round reset retained trail history")
	}
	// Continue with the fresh actor's rotated indexed pose and floor shadow.
	// These RGB digests are from one bounded beta scene using this same tiny
	// archive; no copied images or per-parameter fixtures are required.
	p := m.Players[0]
	c = p.Character
	c.AirFile, c.Palettes = animations, palette.New(archive)
	if err := c.ChangeAnimation(702, 1); err != nil {
		t.Fatal(err)
	}
	c.X, c.Y, c.Facing = -90, -60, 1
	c.StartX, c.StartY = c.X, c.Y
	c.DrawAngle, c.AngleDrawingEnabled, c.DrawScale = 17, true, [2]float32{1.25, .875}
	c.DrawOffset, c.Timeline, c.DrawTransparency = nil, nil, engine.Transparency{}
	canvas.Fill(color.NRGBA{255, 0, 255, 255})
	character.DrawPlayerShadows(canvas, m.Players[:1], coord.Default(), 0, render.Shadow{Intensity: 128, YScale: .4, FloorY: 200})
	character.DrawPlayers(canvas, m.Players[:1], coord.Default(), 0, 200)
	checkRGB := func(region image.Rectangle, want string) {
		t.Helper()
		pixels := make([]byte, 320*240*4)
		canvas.ReadPixels(pixels)
		rgb := make([]byte, 0, region.Dx()*region.Dy()*3)
		for y := region.Min.Y; y < region.Max.Y; y++ {
			for x := region.Min.X; x < region.Max.X; x++ {
				offset := (y*320 + x) * 4
				rgb = append(rgb, pixels[offset:offset+3]...)
			}
		}
		if got := fmt.Sprintf("%x", sha256.Sum256(rgb)); got != want {
			t.Fatal("native rendering scene", region, got, want)
		}
	}
	checkRGB(image.Rect(60, 128, 155, 160), "00773db1b67b2e0f9a25c6aed9cf9f4b94cec4a095765e4686a3e167c2467d6a")
	checkRGB(image.Rect(60, 204, 155, 239), "3159f10da341df2081e209739c94b0f83abfd8b012cfe5ee406ba19aa65ec87c")
	// Finite AIR alpha entries advance into an indefinite overlapping pose,
	// survive a pause without accumulating draws, then expire together.
	m.Effects.Clear()
	effectLife := 20
	for i, action := range []int{710, 711} {
		if err := m.Effects.Queue.Push(effect.Request{Source: effect.Source{Sprites: archive, Animations: animations, Scale: 1}, Action: action, Facing: 1,
			X: 120 + float64(i)*10, Y: 100, ScreenSpace: true, OnTop: true, ScaleX: 5.5, ScaleY: 6.5, HasScale: true, RemoveTime: &effectLife}); err != nil {
			t.Fatal(err)
		}
	}
	if err := m.Effects.Flush(); err != nil {
		t.Fatal(err)
	}
	for tick := 0; tick < 12; tick++ {
		if err := m.Effects.Advance(); err != nil {
			t.Fatal(err)
		}
	}
	foreground := &FightState{match: m}
	defer func() { foreground.beginEffectDraw(); foreground.endEffectDraw() }()
	drawOverlap := func() {
		canvas.Fill(color.NRGBA{255, 0, 255, 255})
		character.DrawPlayerShadows(canvas, m.Players[:1], coord.Default(), 0, render.Shadow{Intensity: 128, YScale: .4, FloorY: 200})
		character.DrawPlayers(canvas, m.Players[:1], coord.Default(), 0, 200)
		foreground.drawEffects(canvas, 2)
	}
	drawOverlap()
	checkRGB(image.Rect(133, 99, 180, 127), "36c2d43dc953a62bc1105ef9e6cfa0e4ad3a73c86718e9277f9ca78da785abe8")
	if err := m.Effects.AdvanceWithPause(true, false); err != nil {
		t.Fatal(err)
	}
	drawOverlap()
	checkRGB(image.Rect(133, 99, 180, 127), "36c2d43dc953a62bc1105ef9e6cfa0e4ad3a73c86718e9277f9ca78da785abe8")
	for tick := 0; tick < 8; tick++ {
		if err := m.Effects.Advance(); err != nil {
			t.Fatal(err)
		}
	}
	if len(m.Effects.Active) != 0 {
		t.Fatal("finite overlap effects failed to retire")
	}
}
