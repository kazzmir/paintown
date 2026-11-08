package snd

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"math"
)

// Wave holds CPU metadata and an immutable view of the embedded sample bytes.
// Unrecognized codecs can be retained; decoding them is an explicit error.
type Wave struct {
	Format, Channels, BitsPerSample, BlockAlign uint16
	SampleRate                                  uint32
	Data                                        []byte
	DataOffset, DataSize                        int64 // encoded payload bounds for streaming readers
	ValidBits                                   uint16
	ChannelMask                                 uint32
}

// ReadWAVHeader validates RIFF chunk boundaries without reading sample payloads.
// DataOffset/DataSize locate the immutable encoded PCM, including when fmt
// follows data or optional chunks are interleaved. The caller owns the reader.
func ReadWAVHeader(r io.ReadSeeker) (*Wave, error) {
	fileSize, err := r.Seek(0, io.SeekEnd)
	if err != nil {
		return nil, err
	}
	if _, err := r.Seek(0, io.SeekStart); err != nil {
		return nil, err
	}
	var header [12]byte
	if _, err := io.ReadFull(r, header[:]); err != nil || string(header[:4]) != "RIFF" || string(header[8:]) != "WAVE" {
		return nil, fmt.Errorf("invalid RIFF/WAVE header")
	}
	end := int64(binary.LittleEndian.Uint32(header[4:8])) + 8
	// Some legacy SND writers include a final odd-chunk alignment byte in
	// the RIFF length but omit that byte from the embedded payload. Allow
	// that one byte only; chunk data itself must still fit the physical file.
	if end < 12 || end > fileSize+1 {
		return nil, fmt.Errorf("RIFF length %d outside payload %d", end, fileSize)
	}
	wave := &Wave{}
	var format, samples bool
	var byteRate uint32
	for offset := int64(12); offset < end; {
		if end-offset < 8 {
			return nil, fmt.Errorf("truncated WAV chunk header at %d", offset)
		}
		if _, err := r.Seek(offset, io.SeekStart); err != nil {
			return nil, err
		}
		var entry [8]byte
		if _, err := io.ReadFull(r, entry[:]); err != nil {
			return nil, err
		}
		size := int64(binary.LittleEndian.Uint32(entry[4:]))
		if size > min(end, fileSize)-offset-8 {
			return nil, fmt.Errorf("WAV chunk %q exceeds RIFF length", entry[:4])
		}
		switch string(entry[:4]) {
		case "fmt ":
			if format || size < 16 {
				return nil, fmt.Errorf("missing/duplicate/short WAV format")
			}
			var chunk [40]byte
			if _, err := io.ReadFull(r, chunk[:min(size, int64(len(chunk)))]); err != nil {
				return nil, err
			}
			format = true
			wave.Format = binary.LittleEndian.Uint16(chunk[:])
			wave.Channels = binary.LittleEndian.Uint16(chunk[2:])
			wave.SampleRate = binary.LittleEndian.Uint32(chunk[4:])
			byteRate = binary.LittleEndian.Uint32(chunk[8:])
			wave.BlockAlign = binary.LittleEndian.Uint16(chunk[12:])
			wave.BitsPerSample = binary.LittleEndian.Uint16(chunk[14:])
			if wave.Format == 0xfffe {
				// WAVEFORMATEXTENSIBLE: complete extension, valid-bit precision and
				// the standard PCM/IEEE_FLOAT GUID family (mmreg.h / ksmedia.h).
				extension := int64(binary.LittleEndian.Uint16(chunk[16:]))
				if size < 40 || extension < 22 || extension > size-18 {
					return nil, fmt.Errorf("short/inconsistent extensible WAV format")
				}
				wave.ValidBits = binary.LittleEndian.Uint16(chunk[18:])
				wave.ChannelMask = binary.LittleEndian.Uint32(chunk[20:])
				codec := binary.LittleEndian.Uint32(chunk[24:])
				if (codec != 1 && codec != 3) || string(chunk[28:40]) != "\x00\x00\x10\x00\x80\x00\x00\xaa\x00\x38\x9b\x71" {
					return nil, fmt.Errorf("unsupported extensible WAV subformat")
				}
				wave.Format = uint16(codec)
				if wave.ValidBits == 0 || wave.ValidBits > wave.BitsPerSample || (codec == 3 && wave.ValidBits != wave.BitsPerSample) {
					return nil, fmt.Errorf("invalid extensible WAV valid bits %d/%d", wave.ValidBits, wave.BitsPerSample)
				}
			}
		case "data":
			if samples {
				return nil, fmt.Errorf("multiple WAV data chunks are unsupported")
			}
			samples = true
			wave.DataOffset, wave.DataSize = offset+8, size
		}
		offset += 8 + size
		// Old PCM writers may omit only the final alignment byte.
		if size&1 != 0 && offset < end {
			offset++
		}
	}
	if !format || !samples || wave.Channels == 0 || wave.SampleRate == 0 || wave.BlockAlign == 0 {
		return nil, fmt.Errorf("WAV is missing valid format/data metadata")
	}
	if wave.Format == 1 || wave.Format == 3 {
		if _, err := wave.sampleWidth(); err != nil {
			return nil, err
		}
		if uint64(wave.BlockAlign) != uint64(wave.Channels)*uint64(wave.BitsPerSample)/8 || uint64(byteRate) != uint64(wave.SampleRate)*uint64(wave.BlockAlign) {
			return nil, fmt.Errorf("inconsistent PCM format/rate/alignment")
		}
		if wave.DataSize%int64(wave.BlockAlign) != 0 {
			return nil, fmt.Errorf("PCM data ends within a frame")
		}
	}
	return wave, nil
}

func ParseWAV(data []byte) (*Wave, error) {
	wave, err := ReadWAVHeader(bytes.NewReader(data))
	if err != nil {
		return nil, err
	}
	wave.Data = data[wave.DataOffset : wave.DataOffset+wave.DataSize]
	return wave, nil
}

func (w *Wave) sampleWidth() (int, error) {
	if w == nil || (w.Format != 1 && w.Format != 3) {
		return 0, fmt.Errorf("unsupported WAV codec")
	}
	bits := w.BitsPerSample
	valid := w.Format == 1 && (bits == 8 || bits == 16 || bits == 24 || bits == 32) || w.Format == 3 && (bits == 32 || bits == 64)
	if !valid {
		return 0, fmt.Errorf("unsupported PCM sample width %d for codec %d", bits, w.Format)
	}
	return int(bits) / 8, nil
}

func (w *Wave) Frames() (int, bool) {
	if w == nil || w.Format != 1 && w.Format != 3 || w.BlockAlign == 0 {
		return 0, false
	}
	size := int64(len(w.Data))
	if w.Data == nil {
		size = w.DataSize
	}
	frames := size / int64(w.BlockAlign)
	if frames < 0 || frames > int64(math.MaxInt) {
		return 0, false
	}
	return int(frames), true
}

// DecodePCM returns interleaved normalized samples without resampling/mixing.
// Integer bytes are unsigned for PCM8 and signed little-endian otherwise.
func (w *Wave) DecodePCM() ([]float32, error) {
	width, err := w.sampleWidth()
	if err != nil {
		return nil, err
	}
	if len(w.Data)%width != 0 {
		return nil, fmt.Errorf("invalid sample width")
	}
	out := make([]float32, len(w.Data)/width)
	if _, err := w.DecodePCMInto(out); err != nil {
		return nil, err
	}
	return out, nil
}

// DecodePCMInto reuses the caller's output for bounded streaming blocks.
// Integer precision is left-aligned in its container for extensible PCM.
func (w *Wave) DecodePCMInto(out []float32) (int, error) {
	bytes, err := w.sampleWidth()
	if err != nil {
		return 0, err
	}
	if len(w.Data)%bytes != 0 || len(out) < len(w.Data)/bytes {
		return 0, fmt.Errorf("invalid sample/output length")
	}
	count := len(w.Data) / bytes
	out = out[:count]
	for i := range out {
		b := w.Data[i*bytes : (i+1)*bytes]
		if w.Format == 3 {
			switch bytes {
			case 4:
				out[i] = math.Float32frombits(binary.LittleEndian.Uint32(b))
			case 8:
				out[i] = float32(math.Float64frombits(binary.LittleEndian.Uint64(b)))
			default:
				return 0, fmt.Errorf("invalid floating PCM width")
			}
			if math.IsNaN(float64(out[i])) || math.IsInf(float64(out[i]), 0) {
				return 0, fmt.Errorf("nonfinite PCM sample")
			}
			continue
		}
		switch bytes {
		case 1:
			out[i] = float32(int(b[0])-128) / 128
		case 2:
			out[i] = float32(int16(binary.LittleEndian.Uint16(b))) / 32768
		case 3:
			n := int32(b[0]) | int32(b[1])<<8 | int32(b[2])<<16
			if n&0x800000 != 0 {
				n |= -1 << 24
			}
			out[i] = float32(n) / 8388608
		case 4:
			out[i] = float32(int32(binary.LittleEndian.Uint32(b))) / 2147483648
		default:
			return 0, fmt.Errorf("unsupported integer PCM width")
		}
	}
	return count, nil
}
