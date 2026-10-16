package main

import (
	"bytes"
	"crypto/sha256"
	"encoding/binary"
	"encoding/hex"
	"encoding/json"
	"math"
	"math/cmplx"
	"os"
	"path/filepath"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/motif"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/snd"
	"github.com/kazzmir/paintown/game/mugen/sound"
)

func soundTestEngine(t *testing.T) *Engine {
	t.Helper()
	root, err := filepath.Abs("../temp/mugen-1.1b1")
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
	e := &Engine{cfg: cfg, motif: m, dataDir: root}
	if err := e.ensureSound(); err != nil {
		t.Fatal(err)
	}
	t.Cleanup(func() {
		if err := e.closeSound(); err != nil {
			t.Error(err)
		}
	})
	return e
}

func TestProductionHUDCuesDelayOnceAndUseFightBank(t *testing.T) {
	e := soundTestEngine(t)
	request := match.Request{Players: [2]match.Selection{{DefPath: filepath.Join(e.dataDir, "chars/kfm/kfm.def"), Palette: 1}, {DefPath: filepath.Join(e.dataDir, "chars/kfm/kfm.def"), Palette: 1}}, StagePath: "stages/kfm.def"}
	f, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.sound.queue.Drain()
	f.match.Fight.Attribute("Round", "round.sndtime").Value = parsers.NumberValue{Val: 2}
	f.match.Phase = match.RoundCall
	for tick := 0; tick < 5; tick++ {
		if err := f.hud.Update(match.RoundCall, 1); err != nil {
			t.Fatal(err)
		}
		commands := e.sound.queue.Drain()
		if tick != 2 {
			if len(commands) != 0 {
				t.Fatal("early/repeated round cue", tick, commands)
			}
			continue
		}
		if len(commands) != 1 || commands[0].Bank != f.match.Fight.Sounds || commands[0].Group != 0 || commands[0].Item != 1 {
			t.Fatal("round cue bank/identifier", commands)
		}
	}
	f.match.Players[0].Character.Power = 1000
	if err := f.hud.Update(match.RoundCall, 1); err != nil {
		t.Fatal(err)
	}
	commands := e.sound.queue.Drain()
	if len(commands) != 1 || commands[0].Group != 21 || commands[0].Item != 0 {
		t.Fatal("power level cue lost", commands)
	}
	if err := e.sound.mixer.Apply(commands[0]); err != nil {
		t.Fatal(err)
	}
	pcm := make([]byte, 4*4410)
	if _, err := e.sound.mixer.Read(pcm); err != nil || bytes.Equal(pcm, make([]byte, len(pcm))) {
		t.Fatal("loaded HUD cue produced no PCM", err)
	}
}

func TestProductionSceneCleanupAndLiveMute(t *testing.T) {
	e := soundTestEngine(t)
	data, err := os.ReadFile("../tests/testdata/reference/sound-pan.extra/probe.snd")
	if err != nil {
		t.Fatal(err)
	}
	bank, err := snd.Parse(bytes.NewReader(data))
	if err != nil {
		t.Fatal(err)
	}
	if err := e.sound.queue.Push(sound.Command{Operation: sound.Play, Owner: 3, Bank: bank, Group: 1, Channel: 0, VolumeScale: 100, Frequency: 1, Loop: true}); err != nil {
		t.Fatal(err)
	}
	read := func() bool {
		t.Helper()
		if err := e.flushSound(); err != nil {
			t.Fatal(err)
		}
		pcm := make([]byte, 4*735)
		if _, err := e.sound.mixer.Read(pcm); err != nil {
			t.Fatal(err)
		}
		return !bytes.Equal(pcm, make([]byte, len(pcm)))
	}
	if !read() {
		t.Fatal("loop did not reach the shared stream")
	}
	e.cfg.Sound.WavVolume = 0
	if read() {
		t.Fatal("options volume did not mute active loop")
	}
	e.cfg.Sound.WavVolume = 80
	if !read() {
		t.Fatal("unmute restarted/lost loop")
	}
	m := &match.Match{SoundQueue: e.sound.queue}
	if err := m.ClearSounds(); err != nil {
		t.Fatal(err)
	}
	if read() {
		t.Fatal("retired fight loop escaped cleanup")
	}
	e.cfg.Sound.Sound = false
	if err := e.startSoundDevice(); err != nil || e.sound.player != nil {
		t.Fatal("disabled sound opened a device", err)
	}
}

func TestNativeRoundAnnouncementPCM(t *testing.T) {
	e := soundTestEngine(t)
	stem := "../tests/testdata/reference/hud-sound"
	data, err := os.ReadFile(stem + ".json")
	if err != nil {
		t.Fatal(err)
	}
	var reference struct {
		Cases map[string]struct {
			CNSHash string            `json:"cns_sha256"`
			AIRHash string            `json:"air_sha256"`
			Extra   map[string]string `json:"extra_sha256"`
			Audio   struct {
				Tolerance float64                                  `json:"amplitude_tolerance"`
				Segments  []struct{ Levels map[string][2]float64 } `json:"segments"`
			} `json:"audio"`
		}
	}
	if err := json.Unmarshal(data, &reference); err != nil {
		t.Fatal(err)
	}
	want := reference.Cases["default"]
	inputs := map[string]string{stem + ".cns": want.CNSHash, stem + ".air": want.AIRHash, stem + ".extra/probe.snd": want.Extra["probe.snd"]}
	for path, expected := range inputs {
		data, err := os.ReadFile(path)
		if err != nil {
			t.Fatal(err)
		}
		hash := sha256.Sum256(data)
		if hex.EncodeToString(hash[:]) != expected {
			t.Fatal("native HUD fixture changed", path)
		}
	}
	data, err = os.ReadFile(stem + ".extra/probe.snd")
	if err != nil {
		t.Fatal(err)
	}
	bank, err := snd.Parse(bytes.NewReader(data))
	if err != nil {
		t.Fatal(err)
	}
	request := match.Request{Players: [2]match.Selection{{DefPath: filepath.Join(e.dataDir, "chars/kfm/kfm.def"), Palette: 1}, {DefPath: filepath.Join(e.dataDir, "chars/kfm/kfm.def"), Palette: 1}}, StagePath: "stages/kfm.def"}
	f, err := NewFightState(e, request)
	if err != nil {
		t.Fatal(err)
	}
	e.sound.queue.Drain()
	f.match.Fight.Sounds = bank
	f.match.Fight.Attribute("Round", "round1.snd").RawValue = "4,0"
	f.match.Phase = match.RoundCall
	if err := f.hud.Update(match.Intro, 1); err != nil {
		t.Fatal(err)
	}
	if err := e.flushSound(); err != nil {
		t.Fatal(err)
	}
	pcm := make([]byte, 8820*4)
	if _, err := e.sound.mixer.Read(pcm); err != nil {
		t.Fatal(err)
	}
	var sums [2]complex128
	var weight float64
	for frame := 0; frame < 8820; frame++ {
		window := .5 - .5*math.Cos(2*math.Pi*float64(frame)/8819)
		weight += window
		oscillator := cmplx.Rect(window, -2*math.Pi*441*float64(frame)/44100)
		for channel := 0; channel < 2; channel++ {
			value := int16(binary.LittleEndian.Uint16(pcm[frame*4+channel*2:]))
			sums[channel] += complex(float64(value), 0) * oscillator
		}
	}
	for channel, wanted := range want.Audio.Segments[0].Levels["441"] {
		actual := 2 * cmplx.Abs(sums[channel]) / weight
		if math.Abs(actual-wanted) > want.Audio.Tolerance {
			t.Fatalf("HUD channel %d amplitude %.1f want %.1f", channel, actual, wanted)
		}
	}
	// The one-second native cue expires rather than looping after presentation.
	if _, err := e.sound.mixer.Read(make([]byte, 4*44100)); err != nil {
		t.Fatal(err)
	}
	if _, err := e.sound.mixer.Read(pcm); err != nil || !bytes.Equal(pcm, make([]byte, len(pcm))) {
		t.Fatal("finite announcement kept playing", err)
	}
}
