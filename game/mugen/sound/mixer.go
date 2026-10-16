package sound

import (
	"encoding/binary"
	"fmt"
	"io"
	"math"
	"sync"

	"github.com/kazzmir/paintown/game/mugen/snd"
)

type Options struct {
	SampleRate, MaxVoices int
	Volume                float32
	Stereo, Reverse       bool
	PanningWidth          float32
}

type samples struct {
	pcm              []float32
	channels, frames int
	rate             uint32
}

type voice struct {
	command  Command
	samples  *samples
	position float64
}

// Mixer is an endless signed-16-bit stereo PCM reader. All device interaction
// stays in presentation; tests advance the same playback by reading PCM.
type Mixer struct {
	mu                          sync.Mutex
	options                     Options
	voices                      []*voice
	cache                       map[*snd.Sound]*samples
	pending                     [4]byte
	pendingOffset, pendingBytes int
	closed                      bool
}

func NewMixer(options Options) (*Mixer, error) {
	if options.SampleRate <= 0 || options.MaxVoices <= 0 || !finite(options.Volume) || options.Volume < 0 || !finite(options.PanningWidth) {
		return nil, fmt.Errorf("invalid sound mixer options")
	}
	return &Mixer{options: options, cache: map[*snd.Sound]*samples{}}, nil
}

func finite(value float32) bool { return !math.IsNaN(float64(value)) && !math.IsInf(float64(value), 0) }

func (m *Mixer) Apply(command Command) error {
	m.mu.Lock()
	defer m.mu.Unlock()
	if m.closed {
		return fmt.Errorf("sound mixer is closed")
	}
	switch command.Operation {
	case StopAll:
		m.voices = nil
		return nil
	case StopOwner, Stop:
		var keep []*voice
		for _, voice := range m.voices {
			if voice.command.Owner == command.Owner && (command.Operation == StopOwner || voice.command.Channel == command.Channel) {
				continue
			}
			keep = append(keep, voice)
		}
		m.voices = keep
		return nil
	case Pan:
		if !finite(command.Pan) {
			return fmt.Errorf("nonfinite sound pan")
		}
		for _, voice := range m.voices {
			if voice.command.Owner == command.Owner && voice.command.Channel == command.Channel && command.Channel >= 0 {
				voice.command.Pan = command.Pan
			}
		}
		return nil
	case Play:
	default:
		return fmt.Errorf("unknown sound operation %d", command.Operation)
	}
	if command.Channel < -1 || command.Channel > 31 || !finite(command.VolumeScale) || !finite(command.Frequency) || command.Frequency <= 0 || !finite(command.Pan) {
		return fmt.Errorf("invalid PlaySnd channel/volume/frequency/pan")
	}
	if command.Bank == nil {
		return fmt.Errorf("sound bank is unavailable for %d,%d", command.Group, command.Item)
	}
	sound := command.Bank.Map[fmt.Sprintf("%d-%d", command.Group, command.Item)]
	if sound == nil {
		return fmt.Errorf("sound %d,%d does not exist", command.Group, command.Item)
	}
	var existing *voice
	for _, voice := range m.voices {
		if command.Channel >= 0 && voice.command.Owner == command.Owner && voice.command.Channel == command.Channel {
			existing = voice
			break
		}
	}
	if existing != nil && command.LowPriority {
		return nil
	}
	if existing == nil && len(m.voices) >= m.options.MaxVoices {
		return fmt.Errorf("sound voice limit %d reached", m.options.MaxVoices)
	}
	decoded := m.cache[sound]
	if decoded == nil {
		if sound.Wave == nil || sound.Wave.Channels < 1 || sound.Wave.Channels > 2 {
			return fmt.Errorf("sound %d,%d requires mono/stereo WAV", command.Group, command.Item)
		}
		pcm, err := sound.Wave.DecodePCM()
		if err != nil {
			return fmt.Errorf("sound %d,%d: %w", command.Group, command.Item, err)
		}
		frames := len(pcm) / int(sound.Wave.Channels)
		if frames == 0 {
			return fmt.Errorf("sound %d,%d has no sample frames", command.Group, command.Item)
		}
		decoded = &samples{pcm: pcm, channels: int(sound.Wave.Channels), frames: frames, rate: sound.Wave.SampleRate}
		m.cache[sound] = decoded
	}
	if existing != nil {
		*existing = voice{command: command, samples: decoded}
	} else {
		m.voices = append(m.voices, &voice{command: command, samples: decoded})
	}
	return nil
}

func (m *Mixer) panGains(pan float32) (float32, float32) {
	// Native 1.1 beta: an integer 240p offset has a +/-40 dead zone,
	// followed by 128/(PanningWidth-20), truncated and clamped to +/-127.
	// The byte split is asymmetric at center: left=128, right=127.
	offset := 0
	if m.options.Stereo && m.options.PanningWidth > 20 {
		pixel := math.Trunc(float64(pan))
		if math.Abs(pixel) > 40 {
			amount := min(127, math.Trunc((math.Abs(pixel)-40)*128/float64(m.options.PanningWidth-20)))
			if pixel < 0 {
				amount = -amount
			}
			offset = int(amount)
		}
	}
	if m.options.Reverse {
		offset = -offset
	}
	right := offset + 127
	if right >= 254 {
		right = 255
	}
	return float32(255-right) / 255, float32(right) / 255
}

func (m *Mixer) frame() (float32, float32) {
	var left, right float32
	active := m.voices[:0]
	for _, voice := range m.voices {
		data := voice.samples
		if voice.position >= float64(data.frames) {
			if !voice.command.Loop {
				continue
			}
			voice.position = math.Mod(voice.position, float64(data.frames))
		}
		index := int(voice.position)
		fraction := float32(voice.position - float64(index))
		next := index + 1
		if next >= data.frames {
			next = index
			if voice.command.Loop {
				next = 0
			}
		}
		sample := func(channel int) float32 {
			if data.channels == 1 {
				channel = 0
			}
			a, b := data.pcm[index*data.channels+channel], data.pcm[next*data.channels+channel]
			return a + (b-a)*fraction
		}
		lg, rg := m.panGains(voice.command.Pan)
		volume := max(0, min(128, math.Trunc(float64(voice.command.VolumeScale)*float64(m.options.Volume)*1.28)))
		// SDL's panning effect and channel volume each truncate signed PCM.
		scale := func(value, gain float32) float32 {
			panned := math.Trunc(float64(value * 32768 * gain))
			return float32(math.Trunc(panned*volume/128) / 32768)
		}
		left += scale(sample(0), lg)
		right += scale(sample(1), rg)
		voice.position += float64(data.rate) / float64(m.options.SampleRate) * float64(voice.command.Frequency)
		if voice.command.Loop || voice.position < float64(data.frames) {
			active = append(active, voice)
		}
	}
	clear(m.voices[len(active):])
	m.voices = active
	return left, right
}

func quantize(value float32) uint16 {
	return uint16(int16(max(-32768, min(32767, math.Round(float64(value)*32768)))))
}

func (m *Mixer) Read(output []byte) (int, error) {
	m.mu.Lock()
	defer m.mu.Unlock()
	if m.closed {
		return 0, io.EOF
	}
	for offset := 0; offset < len(output); {
		if m.pendingBytes == 0 {
			left, right := m.frame()
			binary.LittleEndian.PutUint16(m.pending[:2], quantize(left))
			binary.LittleEndian.PutUint16(m.pending[2:], quantize(right))
			m.pendingOffset, m.pendingBytes = 0, 4
		}
		n := copy(output[offset:], m.pending[m.pendingOffset:m.pendingOffset+m.pendingBytes])
		offset += n
		m.pendingOffset += n
		m.pendingBytes -= n
	}
	return len(output), nil
}

// Reset releases voices and decoded assets when a presentation scene retires.
func (m *Mixer) Reset() {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.voices = nil
	m.cache = map[*snd.Sound]*samples{}
}

func (m *Mixer) Close() error {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.voices = nil
	m.cache = nil
	m.closed = true
	return nil
}
