package main

import (
	"bytes"
	"encoding/binary"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/sound"
	sb "github.com/kazzmir/paintown/game/mugen/storyboard"
)

// One media/state scenario follows menu continuity, one-shot replacement,
// stage override, helper-issued NoMusic, independent SFX/pause/scene clearing,
// rematch and storyboard scene inheritance/stop. PCM is generated in scratch;
// no asset copies, capture bank or new binary fixture is retained.
func TestMusicControllerAndSceneFlow(t *testing.T) {
	e := soundTestEngine(t)
	oldInput := input.GlobalManager
	t.Cleanup(func() { input.GlobalManager = oldInput })
	input.GlobalManager = input.NewInputManager(e.cfg)
	input.GlobalManager.Keyboard = &flowKeyboard{}
	e.cfg.Sound.MasterVolume, e.cfg.Sound.BGMVolume, e.cfg.Sound.WavVolume = 50, 40, 0
	var wav bytes.Buffer
	wav.WriteString("RIFF")
	binary.Write(&wav, binary.LittleEndian, uint32(36+64*4))
	wav.WriteString("WAVEfmt ")
	for _, value := range []any{uint32(16), uint16(1), uint16(2), uint32(44100), uint32(44100 * 4), uint16(4), uint16(16)} {
		binary.Write(&wav, binary.LittleEndian, value)
	}
	wav.WriteString("data")
	binary.Write(&wav, binary.LittleEndian, uint32(64*4))
	for frame := 0; frame < 64; frame++ {
		binary.Write(&wav, binary.LittleEndian, int16(10000))
		binary.Write(&wav, binary.LittleEndian, int16(-5000))
	}
	path := filepath.Join(t.TempDir(), "Music.WAV")
	if err := os.WriteFile(path, wav.Bytes(), 0600); err != nil {
		t.Fatal(err)
	}
	track := sound.MusicSpec{File: filepath.Join(filepath.Dir(path), "music.wav"), Loop: true, Volume: 100, LoopStart: 8, LoopEnd: 16}
	e.motif.Music["title"], e.motif.Music["select"] = track, track
	selectTrack := track
	selectTrack.Volume = 50
	e.motif.Music["select"] = selectTrack
	vsTrack := track
	vsTrack.Loop = false
	e.motif.Music["vs"] = vsTrack
	bus := e.sound.mixer.Music()
	step := func(frames int) []byte {
		t.Helper()
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
		pcm := make([]byte, frames*4)
		if _, err := e.sound.mixer.Read(pcm); err != nil {
			t.Fatal(err)
		}
		return pcm
	}
	position := func() int64 {
		t.Helper()
		_, position, _, err := bus.Status()
		if err != nil {
			t.Fatal(err)
		}
		return position
	}
	e.currentState = NewTitleState(e)
	pcm := step(10)
	if int16(binary.LittleEndian.Uint16(pcm)) != 2000 || int16(binary.LittleEndian.Uint16(pcm[2:])) != -1000 || position() != 10 {
		t.Fatal("independent BGM/master gain or initial cursor", pcm[:4], position())
	}
	e.currentState = NewSelectState(e, "Versus Mode")
	pcm = step(2)
	if position() != 12 || int16(binary.LittleEndian.Uint16(pcm)) != 1000 {
		t.Fatal("same looping menu track restarted or ignored screen gain", position(), pcm[:4])
	}
	step(12)
	if position() != 16 {
		t.Fatal("loop did not wrap to authored sample range", position())
	}
	e.currentState = NewVersusState(e, match.Request{})
	step(2)
	if position() != 2 {
		t.Fatal("one-shot screen failed to replace looping track", position())
	}
	step(70)
	if _, _, playing, _ := bus.Status(); playing {
		t.Fatal("one-shot music did not expire")
	}
	request := match.Request{Mode: "Versus Mode", StagePath: "stages/kfm.def", Music: path}
	for slot := range request.Players {
		request.Players[slot] = match.Selection{DefPath: filepath.Join(e.dataDir, "chars/kfm/kfm.def"), Palette: 1}
	}
	fight, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	if fight.match.Stage.Music.File != "sound/kfm.mid" || fight.match.Stage.Music.Volume != 100 {
		t.Fatal("stage music parameters were not loaded", fight.match.Stage.Music)
	}
	fight.match.Stage.Music.File = "missing-stage-track.ogg"
	e.currentState = fight
	step(2)
	if current, _, playing, _ := bus.Status(); !playing || current.File != path {
		t.Fatal("selection override did not replace stage track", current)
	}
	root := fight.match.Players[0].Character
	file, err := cns.ParseNamed(strings.NewReader(`[Statedef 8000]
type=S
movetype=I
physics=N
[State 8000, stage context]
type=VarSet
trigger1=1
v=56
value=StageVar(info.name)!="" && StageVar(info.author)!=""
[State 8000, suspend music]
type=AssertSpecial
trigger1=Parent, Var(55)
flag=nomusic
`), "connected-music.cns")
	if err != nil {
		t.Fatal(err)
	}
	file.SetLanguageVersion("1.1")
	root.StateFile.States[8000] = file.States[8000]
	if err := root.CreateHelper(engine.HelperParameters{ID: 70, StateNo: 8000, Type: "normal"}); err != nil {
		t.Fatal(err)
	}
	root.IntVars[55] = 1
	before := position()
	pcm = step(4)
	if root.Helpers[0].IntVars[56] != 1 {
		t.Fatal("stage metadata did not reach helper CNS")
	}
	if !fight.match.Timeline.NoMusic || position() != before || !bytes.Equal(pcm, make([]byte, len(pcm))) {
		t.Fatal("helper NoMusic did not suspend the BGM cursor", position(), before)
	}
	e.cfg.Sound.WavVolume = 80
	if err := e.sound.queue.Push(sound.Command{Operation: sound.Play, Owner: 99, Bank: root.OwnSounds, Group: 0, Item: 0, Channel: 1, VolumeScale: 100, Frequency: 1}); err != nil {
		t.Fatal(err)
	}
	pcm = step(735)
	if position() != before || bytes.Equal(pcm, make([]byte, len(pcm))) {
		t.Fatal("NoMusic also paused SFX")
	}
	e.cfg.Sound.WavVolume = 0
	root.IntVars[55] = 0
	step(3)
	if fight.match.Timeline.NoMusic || position() != before+3 {
		t.Fatal("NoMusic release failed to resume exact cursor", position())
	}
	if err := root.PauseGame(engine.PauseParameters{Time: 2}); err != nil {
		t.Fatal(err)
	}
	step(2)
	before = position()
	step(2)
	if position() != before+2 {
		t.Fatal("ordinary CNS Pause suspended music")
	}
	if err := fight.match.ClearSounds(); err != nil {
		t.Fatal(err)
	}
	before = position()
	step(2)
	if position() != before+2 {
		t.Fatal("round SFX clear restarted/stopped music")
	}
	fight, err = NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.currentState = fight
	step(2)
	if position() != 2 {
		t.Fatal("new match retained old music cursor", position())
	}
	story := &sb.Storyboard{Scenes: []*sb.Scene{
		{EndTime: 2, MusicSet: true, MusicFile: path, MusicLoop: true},
		{EndTime: 3},
		{EndTime: 3, MusicSet: true, MusicFile: ""},
	}}
	e.currentState = &StoryboardState{engine: e, loaded: true, player: sb.NewPlayer(story), nextState: func() State { return NewTitleState(e) }}
	step(2)
	step(2)
	if position() != 4 {
		t.Fatal("storyboard omitted BGM did not inherit previous scene")
	}
	for tick := 0; tick < 3; tick++ {
		step(2)
	}
	if _, _, playing, _ := bus.Status(); playing {
		t.Fatal("explicit blank storyboard BGM did not stop music")
	}
	e.currentState = NewTitleState(e)
	step(2)
	if current, _, playing, _ := bus.Status(); !playing || current.File != path {
		t.Fatal("return to title failed to restore menu music")
	}
	e.motif.Music["vs"] = sound.MusicSpec{File: "missing-track.ogg", Volume: 100}
	e.currentState = NewVersusState(e, match.Request{})
	step(2)
	if _, _, playing, _ := bus.Status(); playing {
		t.Fatal("missing replacement music left the previous track playing")
	}
	if err := e.closeSound(); err != nil {
		t.Fatal(err)
	}
	if _, _, playing, _ := bus.Status(); playing {
		t.Fatal("engine close retained the music file")
	}
}
