package sound

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"sync"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/snd"
)

func testBank(patterns ...[]int16) *snd.SoundBank {
	bank := &snd.SoundBank{Map: map[string]*snd.Sound{}}
	for group, pattern := range patterns {
		data := make([]byte, len(pattern)*2)
		for i, value := range pattern {
			binary.LittleEndian.PutUint16(data[i*2:], uint16(value))
		}
		bank.Map[fmt.Sprintf("%d-0", group+1)] = &snd.Sound{Wave: &snd.Wave{Format: 1, Channels: 1, SampleRate: 4, BitsPerSample: 16, BlockAlign: 2, Data: data}}
	}
	return bank
}

func testMixer(t *testing.T) *Mixer {
	t.Helper()
	m, err := NewMixer(Options{SampleRate: 4, MaxVoices: 32, Volume: .8, Stereo: true, PanningWidth: 240})
	if err != nil {
		t.Fatal(err)
	}
	return m
}

func readFrames(t *testing.T, m *Mixer, count int) [][2]int16 {
	t.Helper()
	data := make([]byte, count*4)
	if _, err := io.ReadFull(m, data); err != nil {
		t.Fatal(err)
	}
	frames := make([][2]int16, count)
	for i := range frames {
		frames[i] = [2]int16{int16(binary.LittleEndian.Uint16(data[i*4:])), int16(binary.LittleEndian.Uint16(data[i*4+2:]))}
	}
	return frames
}

func TestChannelOwnershipReplacementAndPriority(t *testing.T) {
	bank := testBank([]int16{10000}, []int16{5000})
	m := testMixer(t)
	command := Command{Operation: Play, Owner: 1, Bank: bank, Group: 1, Channel: 1, Frequency: 1, VolumeScale: 100, Loop: true}
	apply := func(command Command) {
		t.Helper()
		if err := m.Apply(command); err != nil {
			t.Fatal(err)
		}
	}
	apply(command)
	baseline := readFrames(t, m, 1)[0]
	command.Group, command.LowPriority = 2, true
	apply(command)
	if got := readFrames(t, m, 1)[0]; got != baseline {
		t.Fatal("low-priority sound replaced an occupied channel", got, baseline)
	}
	command.Group, command.LowPriority, command.Owner = 1, false, 2
	apply(command)
	if got := readFrames(t, m, 1)[0]; got[0] != baseline[0]*2 || got[1] != baseline[1]*2 {
		t.Fatal("same channel numbers on different owners collided", got)
	}
	apply(Command{Operation: Stop, Owner: 1, Channel: 1})
	if got := readFrames(t, m, 1)[0]; got != baseline {
		t.Fatal("owner-specific stop affected another player", got)
	}
	command.Group = 2
	apply(command)
	if got := readFrames(t, m, 1)[0]; got[0] >= baseline[0] || got[0] <= 0 {
		t.Fatal("numbered replacement did not replace the old sample", got)
	}
	command.Channel, command.Owner = -1, 3
	apply(command)
	apply(Command{Operation: StopAll, Owner: 1})
	if got := readFrames(t, m, 1)[0]; got != [2]int16{} {
		t.Fatal("global stop retained another player's/automatic voice", got)
	}
}

func TestFailedPlaybackRetainsExistingVoice(t *testing.T) {
	m := testMixer(t)
	command := Command{Operation: Play, Owner: 1, Bank: testBank([]int16{10000}), Group: 1, Channel: 0, Frequency: 1, VolumeScale: 100, Loop: true}
	if err := m.Apply(command); err != nil {
		t.Fatal(err)
	}
	before := readFrames(t, m, 1)[0]
	command.Group = 404
	if err := m.Apply(command); err == nil {
		t.Fatal("missing sound accepted")
	}
	if got := readFrames(t, m, 1)[0]; got != before {
		t.Fatal("failed replacement interrupted the old voice", got)
	}
}

func TestSampleClockFrequencyLoopAndExpiry(t *testing.T) {
	m := testMixer(t)
	command := Command{Operation: Play, Owner: 1, Bank: testBank([]int16{10000, 20000, 30000, 10000}), Group: 1, Channel: 1, Frequency: 2, VolumeScale: 100}
	if err := m.Apply(command); err != nil {
		t.Fatal(err)
	}
	got := readFrames(t, m, 4)
	if got[0][0] <= 0 || got[1][0] < got[0][0]*2 || got[2] != [2]int16{} || got[3] != [2]int16{} {
		t.Fatal("double rate did not skip frames and expire", got)
	}
	command.Loop = true
	if err := m.Apply(command); err != nil {
		t.Fatal(err)
	}
	got = readFrames(t, m, 6)
	for i := 2; i < len(got); i++ {
		if got[i] != got[i%2] {
			t.Fatal("loop did not wrap on its sample clock", got)
		}
	}
	if err := m.Apply(Command{Operation: StopOwner, Owner: 1}); err != nil {
		t.Fatal(err)
	}
	if got := readFrames(t, m, 1)[0]; got != [2]int16{} {
		t.Fatal("retired owner retained loop", got)
	}
}

func TestPCMReaderFragmentationAndSceneReset(t *testing.T) {
	a, b := testMixer(t), testMixer(t)
	command := Command{Operation: Play, Owner: 1, Bank: testBank([]int16{10000, -10000, 5000}), Group: 1, Channel: 1, Frequency: .75, VolumeScale: 100, Loop: true}
	for _, m := range []*Mixer{a, b} {
		if err := m.Apply(command); err != nil {
			t.Fatal(err)
		}
	}
	whole := make([]byte, 4096)
	if _, err := io.ReadFull(a, whole); err != nil {
		t.Fatal(err)
	}
	var fragmented []byte
	for i := 0; len(fragmented) < len(whole); i++ {
		data := make([]byte, min(i%7+1, len(whole)-len(fragmented)))
		if _, err := io.ReadFull(b, data); err != nil {
			t.Fatal(err)
		}
		fragmented = append(fragmented, data...)
	}
	if !bytes.Equal(whole, fragmented) {
		t.Fatal("short reads changed PCM/frame alignment")
	}
	c := testMixer(t)
	if err := c.Apply(command); err != nil {
		t.Fatal(err)
	}
	first := make([]byte, 1)
	if _, err := c.Read(first); err != nil {
		t.Fatal(err)
	}
	c.Reset()
	rest := make([]byte, 3)
	if _, err := io.ReadFull(c, rest); err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(append(first, rest...), whole[:4]) {
		t.Fatal("scene reset split a PCM frame")
	}
	if got := readFrames(t, c, 1)[0]; got != [2]int16{} {
		t.Fatal("scene reset retained voices", got)
	}
}

func TestConcurrentControlAndAudioRead(t *testing.T) {
	m := testMixer(t)
	command := Command{Operation: Play, Owner: 1, Bank: testBank([]int16{10000, -10000}), Group: 1, Channel: 1, Frequency: 1, VolumeScale: 100, Loop: true}
	var wait sync.WaitGroup
	wait.Add(1)
	go func() {
		defer wait.Done()
		for i := 0; i < 1000; i++ {
			data := make([]byte, 65)
			if _, err := m.Read(data); err != nil {
				t.Error(err)
				return
			}
		}
	}()
	for i := 0; i < 1000; i++ {
		if err := m.Apply(command); err != nil {
			t.Fatal(err)
		}
		if err := m.Apply(Command{Operation: Pan, Owner: 1, Channel: 1, Pan: float32(i%500 - 250)}); err != nil {
			t.Fatal(err)
		}
		if err := m.Apply(Command{Operation: Stop, Owner: 1, Channel: 1}); err != nil {
			t.Fatal(err)
		}
	}
	wait.Wait()
	if err := m.Close(); err != nil {
		t.Fatal(err)
	}
	if _, err := m.Read(make([]byte, 4)); err != io.EOF {
		t.Fatal("closed audio reader did not finish", err)
	}
}
