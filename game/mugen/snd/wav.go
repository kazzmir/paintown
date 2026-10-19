package snd

import (
	"encoding/binary"
	"fmt"
	"math"
)

// Wave holds CPU metadata and an immutable view of the embedded sample bytes.
// Unrecognized codecs can be retained; decoding them is an explicit error.
type Wave struct {
	Format, Channels, BitsPerSample, BlockAlign uint16
	SampleRate                                  uint32
	Data                                        []byte
}

func ParseWAV(data []byte) (*Wave, error) {
	if len(data) < 12 || string(data[:4]) != "RIFF" || string(data[8:12]) != "WAVE" {
		return nil, fmt.Errorf("invalid RIFF/WAVE header")
	}
	end := int64(binary.LittleEndian.Uint32(data[4:8])) + 8
	if end < 12 || end > int64(len(data)) {
		return nil, fmt.Errorf("RIFF length %d outside payload %d", end, len(data))
	}
	wave := &Wave{}
	var format, samples bool
	var byteRate uint32
	for offset := int64(12); offset < end; {
		if end-offset < 8 {
			return nil, fmt.Errorf("truncated WAV chunk header at %d", offset)
		}
		size := int64(binary.LittleEndian.Uint32(data[offset+4 : offset+8]))
		if size > end-offset-8 {
			return nil, fmt.Errorf("WAV chunk %q exceeds RIFF length", data[offset:offset+4])
		}
		chunk := data[offset+8 : offset+8+size]
		switch string(data[offset : offset+4]) {
		case "fmt ":
			if format || len(chunk) < 16 {
				return nil, fmt.Errorf("missing/duplicate/short WAV format")
			}
			format = true
			wave.Format = binary.LittleEndian.Uint16(chunk)
			wave.Channels = binary.LittleEndian.Uint16(chunk[2:])
			wave.SampleRate = binary.LittleEndian.Uint32(chunk[4:])
			byteRate = binary.LittleEndian.Uint32(chunk[8:])
			wave.BlockAlign = binary.LittleEndian.Uint16(chunk[12:])
			wave.BitsPerSample = binary.LittleEndian.Uint16(chunk[14:])
		case "data":
			if samples {
				return nil, fmt.Errorf("multiple WAV data chunks are unsupported")
			}
			samples = true
			wave.Data = chunk
		}
		offset += 8 + size
		// Some old PCM writers omit the final alignment byte. Earlier chunks
		// still require padding so the following header starts at the right byte.
		if size&1 != 0 && offset < end {
			offset++
		}
	}
	if !format || !samples || wave.Channels == 0 || wave.SampleRate == 0 || wave.BlockAlign == 0 {
		return nil, fmt.Errorf("WAV is missing valid format/data metadata")
	}
	if wave.Format == 1 || wave.Format == 3 {
		bits := wave.BitsPerSample
		valid := wave.Format == 1 && (bits == 8 || bits == 16 || bits == 24 || bits == 32) || wave.Format == 3 && (bits == 32 || bits == 64)
		if !valid || uint64(wave.BlockAlign) != uint64(wave.Channels)*uint64(bits)/8 || uint64(byteRate) != uint64(wave.SampleRate)*uint64(wave.BlockAlign) {
			return nil, fmt.Errorf("inconsistent PCM format/rate/alignment")
		}
		if len(wave.Data)%int(wave.BlockAlign) != 0 {
			return nil, fmt.Errorf("PCM data ends within a frame")
		}
	}
	return wave, nil
}

func (w *Wave) Frames() (int, bool) {
	if w == nil || w.Format != 1 && w.Format != 3 || w.BlockAlign == 0 {
		return 0, false
	}
	return len(w.Data) / int(w.BlockAlign), true
}

// DecodePCM returns interleaved normalized samples without resampling/mixing.
// Integer bytes are unsigned for PCM8 and signed little-endian otherwise.
func (w *Wave) DecodePCM() ([]float32, error) {
	if w == nil || w.Format != 1 && w.Format != 3 {
		return nil, fmt.Errorf("unsupported WAV codec")
	}
	bytes := int(w.BitsPerSample) / 8
	if bytes == 0 || len(w.Data)%bytes != 0 {
		return nil, fmt.Errorf("invalid sample width")
	}
	out := make([]float32, len(w.Data)/bytes)
	for i := range out {
		b := w.Data[i*bytes : (i+1)*bytes]
		if w.Format == 3 {
			switch bytes {
			case 4:
				out[i] = math.Float32frombits(binary.LittleEndian.Uint32(b))
			case 8:
				out[i] = float32(math.Float64frombits(binary.LittleEndian.Uint64(b)))
			default:
				return nil, fmt.Errorf("invalid floating PCM width")
			}
			if math.IsNaN(float64(out[i])) || math.IsInf(float64(out[i]), 0) {
				return nil, fmt.Errorf("nonfinite PCM sample")
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
			return nil, fmt.Errorf("unsupported integer PCM width")
		}
	}
	return out, nil
}
