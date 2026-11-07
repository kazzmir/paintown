package sound

import (
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"sync"

	"github.com/hajimehoshi/ebiten/v2/audio"
	"github.com/hajimehoshi/ebiten/v2/audio/mp3"
	"github.com/hajimehoshi/ebiten/v2/audio/vorbis"
	"github.com/hajimehoshi/ebiten/v2/audio/wav"
)

// MusicSpec loop points count source sample frames, before device resampling.
// An end of zero selects the file's end. Volume is a percentage, independent
// of the master/BGM settings and of SFX channel volume/panning.
type MusicSpec struct {
	File               string
	Loop               bool
	Volume             int
	LoopStart, LoopEnd int64
}

func DefaultMusic(loop bool) MusicSpec { return MusicSpec{Loop: loop, Volume: 100} }

// SetParameter is shared by motif/stage loaders. Blank optional numeric fields
// retain defaults, as in the supplied screenpack.
func (s *MusicSpec) SetParameter(key, raw string) error {
	raw = strings.TrimSpace(raw)
	if key == "file" {
		s.File = strings.Trim(raw, "\"")
		return nil
	}
	if raw == "" {
		return nil
	}
	n, err := strconv.ParseInt(raw, 10, 64)
	if err != nil {
		return fmt.Errorf("music %s = %q: expected integer", key, raw)
	}
	switch key {
	case "loop":
		s.Loop = n != 0
	case "volume":
		if n < 0 || n > 1000 {
			return fmt.Errorf("music volume outside 0..1000")
		}
		s.Volume = int(n)
	case "loopstart", "loopend":
		if n < 0 {
			return fmt.Errorf("music %s must be nonnegative", key)
		}
		if key == "loopstart" {
			s.LoopStart = n
		} else {
			s.LoopEnd = n
		}
	default:
		return fmt.Errorf("unknown music parameter %q", key)
	}
	return nil
}

type musicStream interface {
	io.ReadSeeker
	Length() int64
	SampleRate() int
}

// Music is the persistent streaming BGM bus of a Mixer. It shares the output
// device with SFX but survives ClearScene and root/round changes. NoMusic
// suspends reading, preserving the exact playback cursor for resume.
type Music struct {
	mu       sync.Mutex
	rate     int
	reader   io.ReadSeeker
	file     *os.File
	spec     MusicSpec
	position int64
	end      int64
	start    int64
	gain     float32
	paused   bool
	err      error
}

func (m *Music) stopLocked() {
	if m.file != nil {
		m.file.Close()
	}
	m.reader, m.file = nil, nil
	m.position, m.end, m.start = 0, 0, 0
	m.spec = MusicSpec{}
}

func (m *Music) Stop() {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.stopLocked()
	m.err = nil
}

func (m *Music) SetPlayback(gain float32, paused bool) {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.gain, m.paused = gain, paused
}

func (m *Music) Status() (MusicSpec, int64, bool, error) {
	m.mu.Lock()
	defer m.mu.Unlock()
	return m.spec, m.position / 4, m.reader != nil, m.err
}

// Play continues a looping file across screen transitions only when the next
// screen also loops it. Non-looping entries and rematches can force a restart.
func (m *Music) Play(spec MusicSpec, restart bool) error {
	m.mu.Lock()
	defer m.mu.Unlock()
	if !restart && m.reader != nil && spec.File == m.spec.File && spec.Loop && m.spec.Loop && spec.LoopStart == m.spec.LoopStart && spec.LoopEnd == m.spec.LoopEnd {
		m.spec.Volume = spec.Volume
		return nil
	}
	m.stopLocked()
	m.err = nil
	if spec.File == "" {
		return nil
	}
	f, err := os.Open(spec.File)
	if err != nil {
		return err
	}
	var decoded musicStream
	switch strings.ToLower(filepath.Ext(spec.File)) {
	case ".wav":
		decoded, err = wav.DecodeWithoutResampling(f)
	case ".mp3":
		decoded, err = mp3.DecodeWithoutResampling(f)
	case ".ogg", ".oga":
		decoded, err = vorbis.DecodeWithoutResampling(f)
	default:
		err = fmt.Errorf("unsupported BGM format %q (supported: WAV, MP3, Ogg Vorbis)", filepath.Ext(spec.File))
	}
	if err != nil {
		f.Close()
		return fmt.Errorf("%s: %w", spec.File, err)
	}
	length, sourceRate := decoded.Length(), decoded.SampleRate()
	if sourceRate <= 0 || length <= 0 || spec.Volume < 0 {
		f.Close()
		return fmt.Errorf("%s: invalid/empty music stream", spec.File)
	}
	// Clamp loop positions before multiplying, avoiding overflow on authored
	// offsets. Reject a reversed/empty loop instead of spinning at EOF.
	sourceFrames := length / 4
	start, end := min(spec.LoopStart, sourceFrames), sourceFrames
	if spec.Loop && spec.LoopEnd > 0 {
		end = min(spec.LoopEnd, sourceFrames)
	}
	if start < 0 || end <= 0 || (spec.Loop && start >= end) {
		f.Close()
		return fmt.Errorf("%s: invalid music loop %d..%d", spec.File, start, end)
	}
	m.reader = audio.ResampleReader(decoded, length, sourceRate, m.rate).(io.ReadSeeker)
	m.file, m.spec = f, spec
	m.start = int64(float64(start)*float64(m.rate)/float64(sourceRate)) * 4
	m.end = int64(float64(end)*float64(m.rate)/float64(sourceRate)) * 4
	if m.end <= m.start && spec.Loop {
		m.stopLocked()
		return fmt.Errorf("%s: empty resampled loop", spec.File)
	}
	return nil
}

func (m *Music) frame() (float32, float32) {
	m.mu.Lock()
	defer m.mu.Unlock()
	if m.reader == nil || m.paused {
		return 0, 0
	}
	for attempt := 0; attempt < 2; attempt++ {
		if m.position >= m.end {
			if !m.spec.Loop {
				m.stopLocked()
				return 0, 0
			}
			if _, err := m.reader.Seek(m.start, io.SeekStart); err != nil {
				m.err = err
				m.stopLocked()
				return 0, 0
			}
			m.position = m.start
		}
		var sample [4]byte
		n, err := io.ReadFull(m.reader, sample[:])
		m.position += int64(n)
		if n == 4 {
			gain := m.gain * float32(m.spec.Volume) / 100
			return float32(int16(binary.LittleEndian.Uint16(sample[:2]))) / 32768 * gain,
				float32(int16(binary.LittleEndian.Uint16(sample[2:]))) / 32768 * gain
		}
		if err != io.EOF && err != io.ErrUnexpectedEOF {
			m.err = err
			m.stopLocked()
			return 0, 0
		}
		m.end = m.position - int64(n)
	}
	m.err = fmt.Errorf("%s: music loop yielded no complete sample", m.spec.File)
	m.stopLocked()
	return 0, 0
}
