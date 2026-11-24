//go:build !linux || (!amd64 && !arm64)

package font

import (
	"fmt"
	"runtime"

	"golang.org/x/image/font"
)

func newHintedFace([]byte, int, bool) (font.Face, error) {
	return nil, fmt.Errorf("compatible automatic hinting is unavailable on %s/%s", runtime.GOOS, runtime.GOARCH)
}
