package sound

import (
	"encoding/binary"
	"fmt"
	"io"
	"math"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"sync"

	"github.com/hajimehoshi/ebiten/v2/audio"
	"github.com/hajimehoshi/ebiten/v2/audio/mp3"
	"github.com/hajimehoshi/ebiten/v2/audio/vorbis"
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
	playMu   sync.Mutex // decoder setup must not hold up the audio callback
	mu       sync.Mutex
	rate     int
	reader   io.ReadSeeker
	file     *os.File
	decoder  io.Closer
	spec     MusicSpec
	position int64
	end      int64
	start    int64
	gain     float32
	paused   bool
	err      error
	// Decode in bounded stereo blocks. position counts consumed frames, not
	// read-ahead, so NoMusic and screen continuity retain the exact cursor.
	pcm      [4096]byte
	cursor   int
	buffered int
	readErr  error
}

func (m *Music) stopLocked() {
	if m.decoder != nil {
		m.decoder.Close()
	}
	if m.file != nil {
		m.file.Close()
	}
	m.reader, m.file = nil, nil
	m.decoder = nil
	m.position, m.end, m.start = 0, 0, 0
	m.cursor, m.buffered, m.readErr = 0, 0, nil
	m.spec = MusicSpec{}
}

func (m *Music) Stop() {
	m.playMu.Lock()
	defer m.playMu.Unlock()
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
	m.playMu.Lock()
	defer m.playMu.Unlock()
	m.mu.Lock()
	if !restart && m.reader != nil && spec.File == m.spec.File && spec.Loop && m.spec.Loop && spec.LoopStart == m.spec.LoopStart && spec.LoopEnd == m.spec.LoopEnd {
		m.spec.Volume = spec.Volume
		m.mu.Unlock()
		return nil
	}
	m.stopLocked()
	m.err = nil
	m.mu.Unlock()
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
		decoded, err = decodeWaveMusic(f)
	case ".mp3":
		decoded, err = mp3.DecodeWithoutResampling(f)
	case ".ogg", ".oga":
		decoded, err = vorbis.DecodeWithoutResampling(f)
	case ".mid", ".midi", ".mod", ".s3m", ".xm", ".it":
		decoded, err = decodeLegacyMusic(f, strings.ToLower(filepath.Ext(spec.File)), m.rate)
	default:
		err = fmt.Errorf("unsupported BGM format %q (WAV, MP3, Ogg, MIDI, MOD/S3M/XM/IT supported; physical CD playback unavailable)", filepath.Ext(spec.File))
	}
	if err != nil {
		f.Close()
		return fmt.Errorf("%s: %w", spec.File, err)
	}
	length, sourceRate := decoded.Length(), decoded.SampleRate()
	closer, _ := decoded.(io.Closer)
	cleanup := func() {
		if closer != nil {
			closer.Close()
		}
		f.Close()
	}
	if sourceRate <= 0 || length == 0 || spec.Volume < 0 {
		cleanup()
		return fmt.Errorf("%s: invalid/empty music stream", spec.File)
	}
	// Clamp loop positions before multiplying, avoiding overflow on authored
	// offsets. Reject a reversed/empty loop instead of spinning at EOF.
	sourceFrames := length / 4
	if length < 0 { // a sample-clock sequencer discovers its length at EOF
		sourceFrames = math.MaxInt64 / 8
	}
	start, end := min(spec.LoopStart, sourceFrames), sourceFrames
	if spec.Loop && spec.LoopEnd > 0 {
		end = min(spec.LoopEnd, sourceFrames)
	}
	if start < 0 || end <= 0 || (spec.Loop && start >= end) {
		cleanup()
		return fmt.Errorf("%s: invalid music loop %d..%d", spec.File, start, end)
	}
	var reader io.ReadSeeker = decoded
	if sourceRate != m.rate {
		if length < 0 {
			cleanup()
			return fmt.Errorf("cannot resample an unknown-length music stream")
		}
		reader = audio.ResampleReader(decoded, length, sourceRate, m.rate).(io.ReadSeeker)
	}
	m.mu.Lock()
	defer m.mu.Unlock()
	m.reader, m.decoder = reader, closer
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
		if m.cursor == m.buffered && m.readErr != nil {
			m.err = m.readErr
			m.stopLocked()
			return 0, 0
		}
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
			m.cursor, m.buffered = 0, 0
		}
		if m.cursor == m.buffered {
			count := min(int64(len(m.pcm)), m.end-m.position)
			n, err := io.ReadFull(m.reader, m.pcm[:count])
			m.cursor, m.buffered = 0, n/4*4
			if err == io.EOF || err == io.ErrUnexpectedEOF {
				m.end = m.position + int64(m.buffered)
			} else {
				m.readErr = err
			}
		}
		if m.cursor < m.buffered {
			sample := m.pcm[m.cursor : m.cursor+4]
			m.cursor += 4
			m.position += 4
			gain := m.gain * float32(m.spec.Volume) / 100
			return float32(int16(binary.LittleEndian.Uint16(sample[:2]))) / 32768 * gain,
				float32(int16(binary.LittleEndian.Uint16(sample[2:]))) / 32768 * gain
		}
	}
	m.err = fmt.Errorf("%s: music loop yielded no complete sample", m.spec.File)
	m.stopLocked()
	return 0, 0
}
