//go:build linux && (amd64 || arm64)

package sound

// Optional Linux codecs use stable C ABIs. No audio driver or platform headers
// are needed: both render PCM into the same mixer as WAV/MP3/Ogg and SFX.
// API contracts: fluidsynth.org/api/group__midi__player.html and
// lib.openmpt.org/doc/libopenmpt_8h.html.
import (
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"reflect"
	"runtime"
	"sync"
	"unsafe"

	"github.com/ebitengine/purego"
)

func bindCodec(library string, functions any) error {
	handle, err := purego.Dlopen(library, purego.RTLD_NOW|purego.RTLD_LOCAL)
	if err != nil {
		return fmt.Errorf("install %s for this music format: %w", library, err)
	}
	value := reflect.ValueOf(functions).Elem()
	for i := 0; i < value.NumField(); i++ {
		name := value.Type().Field(i).Tag.Get("symbol")
		address, err := purego.Dlsym(handle, name)
		if err != nil {
			purego.Dlclose(handle)
			return fmt.Errorf("%s: %w", library, err)
		}
		purego.RegisterFunc(value.Field(i).Addr().Interface(), address)
	}
	// Keep the library loaded while process-global function pointers are live.
	return nil
}

var fluid struct {
	NewSettings    func() uintptr                                                                         `symbol:"new_fluid_settings"`
	DeleteSettings func(uintptr)                                                                          `symbol:"delete_fluid_settings"`
	SetString      func(uintptr, string, string) int32                                                    `symbol:"fluid_settings_setstr"`
	SetNumber      func(uintptr, string, float64) int32                                                   `symbol:"fluid_settings_setnum"`
	SetInt         func(uintptr, string, int32) int32                                                     `symbol:"fluid_settings_setint"`
	NewSynth       func(uintptr) uintptr                                                                  `symbol:"new_fluid_synth"`
	DeleteSynth    func(uintptr)                                                                          `symbol:"delete_fluid_synth"`
	Soundfont      func(uintptr, string, int32) int32                                                     `symbol:"fluid_synth_sfload"`
	Reset          func(uintptr) int32                                                                    `symbol:"fluid_synth_system_reset"`
	Render         func(uintptr, int32, unsafe.Pointer, int32, int32, unsafe.Pointer, int32, int32) int32 `symbol:"fluid_synth_write_s16"`
	NewPlayer      func(uintptr) uintptr                                                                  `symbol:"new_fluid_player"`
	DeletePlayer   func(uintptr)                                                                          `symbol:"delete_fluid_player"`
	Add            func(uintptr, unsafe.Pointer, uintptr) int32                                           `symbol:"fluid_player_add_mem"`
	Play           func(uintptr) int32                                                                    `symbol:"fluid_player_play"`
	Stop           func(uintptr) int32                                                                    `symbol:"fluid_player_stop"`
	Status         func(uintptr) int32                                                                    `symbol:"fluid_player_get_status"`
}
var fluidOnce sync.Once
var fluidError error

var openmpt struct {
	Create   func(unsafe.Pointer, uintptr, uintptr, uintptr, uintptr, uintptr, uintptr, uintptr, uintptr) uintptr `symbol:"openmpt_module_create_from_memory2"`
	Destroy  func(uintptr)                                                                                        `symbol:"openmpt_module_destroy"`
	Duration func(uintptr) float64                                                                                `symbol:"openmpt_module_get_duration_seconds"`
	Position func(uintptr, float64) float64                                                                       `symbol:"openmpt_module_set_position_seconds"`
	Render   func(uintptr, int32, uintptr, unsafe.Pointer) uintptr                                                `symbol:"openmpt_module_read_interleaved_stereo"`
}
var openmptOnce sync.Once
var openmptError error

// nativeMusic buffers C rendering in blocks; the mixer's single-frame reads
// do not cross the language boundary per sample. Position counts consumed PCM.
type nativeMusic struct {
	rate             int
	length, position int64
	pending          []byte
	block            [1024]int16
	encoded          [2048]byte
	render           func([]int16) (int, error)
	seek             func(int64) error
	close            func()
	closed           bool
}

func (s *nativeMusic) SampleRate() int { return s.rate }
func (s *nativeMusic) Length() int64   { return s.length }
func (s *nativeMusic) Close() error {
	if !s.closed {
		s.closed = true
		s.close()
		s.pending = nil
	}
	return nil
}
func (s *nativeMusic) Read(out []byte) (int, error) {
	if s.closed {
		return 0, io.ErrClosedPipe
	}
	if len(out) == 0 {
		return 0, nil
	}
	if len(s.pending) == 0 {
		n, err := s.render(s.block[:])
		if err != nil {
			return 0, err
		}
		if n == 0 {
			s.length = s.position
			return 0, io.EOF
		}
		if n < 0 || n > len(s.block)/2 {
			return 0, fmt.Errorf("legacy codec returned invalid frame count %d", n)
		}
		s.pending = s.encoded[:n*4]
		for i, v := range s.block[:n*2] {
			binary.LittleEndian.PutUint16(s.pending[i*2:], uint16(v))
		}
	}
	n := copy(out, s.pending)
	s.pending = s.pending[n:]
	s.position += int64(n)
	return n, nil
}
func (s *nativeMusic) Seek(offset int64, whence int) (int64, error) {
	if s.closed {
		return 0, io.ErrClosedPipe
	}
	switch whence {
	case io.SeekStart:
	case io.SeekCurrent:
		offset += s.position
	case io.SeekEnd:
		if s.length < 0 {
			return 0, fmt.Errorf("music duration not known until EOF")
		}
		offset += s.length
	default:
		return 0, fmt.Errorf("invalid music seek origin")
	}
	if offset < 0 || offset%4 != 0 {
		return 0, fmt.Errorf("music seek must be a nonnegative stereo frame")
	}
	if err := s.seek(offset / 4); err != nil {
		return s.position, err
	}
	s.pending = nil
	s.position = offset
	return offset, nil
}

func decodeLegacyMusic(file *os.File, ext string, rate int) (musicStream, error) {
	data, err := io.ReadAll(io.LimitReader(file, 64*1024*1024+1))
	if err != nil {
		return nil, err
	}
	if len(data) == 0 || len(data) > 64*1024*1024 {
		return nil, fmt.Errorf("legacy music must contain 1..64 MiB")
	}
	if ext == ".mid" || ext == ".midi" {
		return decodeMIDI(data, rate)
	}
	openmptOnce.Do(func() { openmptError = bindCodec("libopenmpt.so.0", &openmpt) })
	if openmptError != nil {
		return nil, openmptError
	}
	mod := openmpt.Create(unsafe.Pointer(&data[0]), uintptr(len(data)), 0, 0, 0, 0, 0, 0, 0)
	runtime.KeepAlive(data)
	if mod == 0 {
		return nil, fmt.Errorf("invalid tracker module")
	}
	s := &nativeMusic{rate: rate, length: int64(openmpt.Duration(mod)*float64(rate)) * 4}
	s.render = func(out []int16) (int, error) {
		n := openmpt.Render(mod, int32(rate), uintptr(len(out)/2), unsafe.Pointer(&out[0]))
		runtime.KeepAlive(out)
		return int(n), nil
	}
	s.seek = func(frame int64) error { openmpt.Position(mod, float64(frame)/float64(rate)); return nil }
	s.close = func() { openmpt.Destroy(mod) }
	return s, nil
}

func decodeMIDI(data []byte, rate int) (musicStream, error) {
	fluidOnce.Do(func() { fluidError = bindCodec("libfluidsynth.so.3", &fluid) })
	if fluidError != nil {
		return nil, fluidError
	}
	soundfont := os.Getenv("MUGEN_SOUNDFONT")
	if soundfont == "" {
		for _, candidate := range []string{"/usr/share/sounds/sf2/TimGM6mb.sf2", "/usr/share/sounds/sf2/FluidR3_GM.sf2", "/usr/share/soundfonts/default.sf2"} {
			if info, err := os.Stat(candidate); err == nil && !info.IsDir() {
				soundfont = candidate
				break
			}
		}
	}
	if soundfont == "" {
		return nil, fmt.Errorf("MIDI requires a GM SoundFont; set MUGEN_SOUNDFONT to an installed .sf2 file")
	}
	settings := fluid.NewSettings()
	if settings == 0 {
		return nil, fmt.Errorf("cannot create MIDI settings")
	}
	if fluid.SetString(settings, "player.timing-source", "sample") < 0 || fluid.SetNumber(settings, "synth.sample-rate", float64(rate)) < 0 {
		fluid.DeleteSettings(settings)
		return nil, fmt.Errorf("cannot configure sample-clock MIDI")
	}
	fluid.SetInt(settings, "synth.cpu-cores", 1)
	fluid.SetInt(settings, "synth.threadsafe-api", 0)
	synth := fluid.NewSynth(settings)
	if synth == 0 {
		fluid.DeleteSettings(settings)
		return nil, fmt.Errorf("cannot create MIDI synthesizer")
	}
	player := uintptr(0)
	s := &nativeMusic{rate: rate, length: -1}
	s.close = func() {
		if player != 0 {
			fluid.Stop(player)
			fluid.DeletePlayer(player)
			player = 0
		}
		fluid.DeleteSynth(synth)
		fluid.DeleteSettings(settings)
	}
	if fluid.Soundfont(synth, soundfont, 1) < 0 {
		s.Close()
		return nil, fmt.Errorf("cannot load MIDI SoundFont %s", soundfont)
	}
	restart := func() error {
		if player != 0 {
			fluid.Stop(player)
			fluid.DeletePlayer(player)
		}
		fluid.Reset(synth)
		player = fluid.NewPlayer(synth)
		if player == 0 {
			return fmt.Errorf("cannot create MIDI player")
		}
		result := fluid.Add(player, unsafe.Pointer(&data[0]), uintptr(len(data)))
		runtime.KeepAlive(data)
		if result < 0 || fluid.Play(player) < 0 {
			return fmt.Errorf("invalid MIDI stream")
		}
		return nil
	}
	if err := restart(); err != nil {
		s.Close()
		return nil, err
	}
	s.render = func(out []int16) (int, error) {
		if fluid.Status(player) == 3 {
			return 0, nil
		} // FLUID_PLAYER_DONE
		if fluid.Render(synth, int32(len(out)/2), unsafe.Pointer(&out[0]), 0, 2, unsafe.Pointer(&out[0]), 1, 2) < 0 {
			return 0, fmt.Errorf("MIDI synthesis failed")
		}
		runtime.KeepAlive(out)
		return len(out) / 2, nil
	}
	s.seek = func(frame int64) error {
		if err := restart(); err != nil {
			return err
		}
		var discard [1024]int16
		for frame > 0 {
			count := min(frame, int64(len(discard)/2))
			n, err := s.render(discard[:count*2])
			if err != nil {
				return err
			}
			if n == 0 {
				return io.EOF
			}
			frame -= int64(n)
		}
		return nil
	}
	return s, nil
}
