//go:build !linux || (!amd64 && !arm64)

package sound

import (
	"fmt"
	"os"
)

func decodeLegacyMusic(_ *os.File, _ string, _ int) (musicStream, error) {
	return nil, fmt.Errorf("MIDI/tracker playback currently requires Linux amd64/arm64 with FluidSynth/libopenmpt")
}
