package sound

import (
	"encoding/binary"
	"fmt"
	"io"

	"github.com/kazzmir/paintown/game/mugen/snd"
)

// waveStream converts bounded source blocks to the mixer's signed16 stereo
// format. WAV music and embedded SND samples share metadata/sample decoding.
// The owning Music closes the source file; this adapter only borrows it.
type waveStream struct {
	source           io.ReadSeeker
	wave             snd.Wave
	length, position int64
	raw              [8192]byte // 512 stereo float64 frames, the largest input
	samples          [1024]float32
	pcm              [2048]byte
	cursor, buffered int
	readErr          error
}

func decodeWaveMusic(source io.ReadSeeker) (*waveStream, error) {
	header, err := snd.ReadWAVHeader(source)
	if err != nil {
		return nil, err
	}
	frames, ok := header.Frames()
	if !ok || header.Channels < 1 || header.Channels > 2 {
		return nil, fmt.Errorf("WAV music requires mono/stereo integer or floating PCM")
	}
	stream := &waveStream{source: source, wave: *header, length: int64(frames) * 4}
	if _, err := stream.Seek(0, io.SeekStart); err != nil {
		return nil, err
	}
	return stream, nil
}

func (s *waveStream) SampleRate() int { return int(s.wave.SampleRate) }
func (s *waveStream) Length() int64   { return s.length }

func (s *waveStream) Read(output []byte) (int, error) {
	if len(output) == 0 {
		return 0, nil
	}
	if s.cursor == s.buffered {
		if s.readErr != nil {
			return 0, s.readErr
		}
		if s.position >= s.length {
			return 0, io.EOF
		}
		frames := min(int64(512), (s.length-s.position)/4)
		n, err := io.ReadFull(s.source, s.raw[:frames*int64(s.wave.BlockAlign)])
		frames = int64(n) / int64(s.wave.BlockAlign)
		block := s.wave
		block.Data = s.raw[:frames*int64(block.BlockAlign)]
		if _, decodeErr := block.DecodePCMInto(s.samples[:]); decodeErr != nil {
			s.readErr = decodeErr
			return 0, decodeErr
		}
		for frame := 0; frame < int(frames); frame++ {
			left, right := frame*int(block.Channels), frame*int(block.Channels)
			if block.Channels == 2 {
				right++
			}
			binary.LittleEndian.PutUint16(s.pcm[frame*4:], quantize(s.samples[left]))
			binary.LittleEndian.PutUint16(s.pcm[frame*4+2:], quantize(s.samples[right]))
		}
		s.cursor, s.buffered, s.readErr = 0, int(frames)*4, err
		if err != nil && err != io.EOF && err != io.ErrUnexpectedEOF {
			s.readErr = fmt.Errorf("WAV sample data: %w", err)
		} else if n%int(s.wave.BlockAlign) != 0 || s.position+int64(s.buffered) < s.length && err != nil {
			s.readErr = fmt.Errorf("WAV sample data: %w", io.ErrUnexpectedEOF)
		}
		if s.buffered == 0 {
			return 0, s.readErr
		}
	}
	n := copy(output, s.pcm[s.cursor:s.buffered])
	s.cursor += n
	s.position += int64(n)
	if s.cursor == s.buffered {
		return n, s.readErr
	}
	return n, nil
}

func (s *waveStream) Seek(offset int64, whence int) (int64, error) {
	switch whence {
	case io.SeekStart:
	case io.SeekCurrent:
		// Check before addition, including negative offsets, avoiding overflow.
		if offset < -s.position || offset > s.length-s.position {
			return s.position, fmt.Errorf("WAV seek outside sample data")
		}
		offset += s.position
	case io.SeekEnd:
		if offset < -s.length || offset > 0 {
			return s.position, fmt.Errorf("WAV seek outside sample data")
		}
		offset += s.length
	default:
		return s.position, fmt.Errorf("invalid WAV seek origin")
	}
	if offset < 0 || offset > s.length || offset%4 != 0 {
		return s.position, fmt.Errorf("WAV seek must address a stereo sample frame")
	}
	if _, err := s.source.Seek(s.wave.DataOffset+offset/4*int64(s.wave.BlockAlign), io.SeekStart); err != nil {
		return s.position, err
	}
	s.position = offset
	s.cursor, s.buffered, s.readErr = 0, 0, nil
	return offset, nil
}
