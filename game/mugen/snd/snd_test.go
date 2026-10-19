package snd

import (
	"bytes"
	"encoding/binary"
	"math"
	"strings"
	"testing"
)

func waveFixture(format, channels, bits uint16, samples []byte) []byte {
	var b bytes.Buffer
	b.WriteString("RIFF")
	binary.Write(&b, binary.LittleEndian, uint32(0))
	b.WriteString("WAVE")
	b.WriteString("JUNK")
	binary.Write(&b, binary.LittleEndian, uint32(1))
	b.Write([]byte{7, 0})
	b.WriteString("fmt ")
	binary.Write(&b, binary.LittleEndian, uint32(16))
	align := channels * bits / 8
	for _, n := range []any{format, channels, uint32(11025), uint32(11025) * uint32(align), align, bits} {
		binary.Write(&b, binary.LittleEndian, n)
	}
	b.WriteString("data")
	binary.Write(&b, binary.LittleEndian, uint32(len(samples)))
	b.Write(samples)
	data := b.Bytes()
	binary.LittleEndian.PutUint32(data[4:], uint32(len(data)-8))
	return data
}

func TestPCMSampleDecoding(t *testing.T) {
	for _, tc := range []struct {
		name                   string
		format, channels, bits uint16
		data                   []byte
		want                   []float32
	}{
		{"unsigned8", 1, 1, 8, []byte{0, 128, 255}, []float32{-1, 0, 127.0 / 128}},
		{"signed16-stereo", 1, 2, 16, []byte{0, 128, 255, 127}, []float32{-1, 32767.0 / 32768}},
		{"signed24", 1, 1, 24, []byte{0, 0, 128, 255, 255, 127}, []float32{-1, 8388607.0 / 8388608}},
		{"signed32", 1, 1, 32, []byte{0, 0, 0, 128, 255, 255, 255, 127}, []float32{-1, 1}},
		{"float32", 3, 1, 32, []byte{0, 0, 0, 63}, []float32{.5}},
		{"float64", 3, 1, 64, []byte{0, 0, 0, 0, 0, 0, 224, 63}, []float32{.5}},
	} {
		t.Run(tc.name, func(t *testing.T) {
			wave, err := ParseWAV(waveFixture(tc.format, tc.channels, tc.bits, tc.data))
			if err != nil {
				t.Fatal(err)
			}
			frames, ok := wave.Frames()
			if !ok || frames != len(tc.want)/int(tc.channels) {
				t.Fatal("frame metadata")
			}
			got, err := wave.DecodePCM()
			if err != nil {
				t.Fatal(err)
			}
			if len(got) != len(tc.want) {
				t.Fatal(got)
			}
			for i, v := range got {
				if v != tc.want[i] {
					t.Fatalf("sample %d=%g want %g", i, v, tc.want[i])
				}
			}
		})
	}
	data := make([]byte, 4)
	binary.LittleEndian.PutUint32(data, math.Float32bits(float32(math.NaN())))
	w, err := ParseWAV(waveFixture(3, 1, 32, data))
	if err != nil {
		t.Fatal(err)
	}
	if _, err := w.DecodePCM(); err == nil {
		t.Fatal("accepted nonfinite PCM")
	}
}

func tinySND() []byte {
	wave := waveFixture(1, 1, 8, []byte{0, 128, 255})
	b := make([]byte, 512+16)
	copy(b, []byte("ElecbyteSnd"))
	binary.LittleEndian.PutUint32(b[12:], 4)
	binary.LittleEndian.PutUint32(b[16:], 1)
	binary.LittleEndian.PutUint32(b[20:], 512)
	binary.LittleEndian.PutUint32(b[512:], uint32(len(b)+len(wave)))
	binary.LittleEndian.PutUint32(b[516:], uint32(len(wave)))
	binary.LittleEndian.PutUint32(b[520:], 1)
	binary.LittleEndian.PutUint32(b[524:], 2)
	return append(b, wave...)
}

func TestSNDBoundsAndTail(t *testing.T) {
	for _, zeroTail := range []bool{false, true} {
		b := tinySND()
		if zeroTail {
			binary.LittleEndian.PutUint32(b[512:], 0)
		}
		bank, err := Parse(bytes.NewReader(b))
		if err != nil {
			t.Fatal(err)
		}
		if len(bank.Sounds) != 1 || bank.Version != 4 || bank.Map["1-2"].Wave == nil {
			t.Fatal("missing metadata")
		}
	}
	for _, mutate := range []func([]byte) []byte{
		func(b []byte) []byte { return b[:23] },
		func(b []byte) []byte { binary.LittleEndian.PutUint32(b[16:], 0xffffffff); return b },
		func(b []byte) []byte { binary.LittleEndian.PutUint32(b[20:], 0); return b },
		func(b []byte) []byte { binary.LittleEndian.PutUint32(b[516:], 0xffffffff); return b },
		func(b []byte) []byte {
			binary.LittleEndian.PutUint32(b[16:], 2)
			binary.LittleEndian.PutUint32(b[512:], 512)
			return b
		},
		func(b []byte) []byte {
			binary.LittleEndian.PutUint32(b[16:], 2)
			binary.LittleEndian.PutUint32(b[512:], 0)
			return b
		},
		func(b []byte) []byte { b[528] = 0; return b },
	} {
		if _, err := Parse(bytes.NewReader(mutate(tinySND()))); err == nil {
			t.Fatal("accepted malformed SND")
		}
	}
	b := tinySND()
	b[528] = 0
	if _, err := Parse(bytes.NewReader(b)); err == nil || !strings.Contains(err.Error(), "(1,2)") {
		t.Fatal("missing sound context", err)
	}
}

func TestWaveRejectsMalformedChunks(t *testing.T) {
	for _, mutate := range []func([]byte) []byte{
		func(b []byte) []byte { return b[:11] },
		func(b []byte) []byte { binary.LittleEndian.PutUint32(b[4:], 0xffffffff); return b },
		func(b []byte) []byte { binary.LittleEndian.PutUint32(b[16:], 0xffffffff); return b },
		func(b []byte) []byte { b[34] = 0; return b }, // sample-rate/byte-rate mismatch
	} {
		if _, err := ParseWAV(mutate(waveFixture(1, 1, 8, []byte{1, 2, 3}))); err == nil {
			t.Fatal("accepted malformed WAV")
		}
	}
}

func FuzzSoundArchives(f *testing.F) {
	f.Add(tinySND())
	f.Fuzz(func(t *testing.T, data []byte) {
		if len(data) > 1<<20 {
			return
		}
		_, _ = Parse(bytes.NewReader(data))
	})
}
