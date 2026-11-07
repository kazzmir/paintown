package character

import (
	"strconv"
	"strings"
)

// CommandInputs produces logical frames for authored CMD symbols. AI therefore
// uses the same matcher and CNS as humans, including chords/charge/held keys.
func CommandInputs(sequence string) [][]string {
	var frames [][]string
	var held []string
	for _, raw := range strings.Split(sequence, ",") {
		symbol := strings.TrimSpace(raw)
		keys := commandKeys(symbol)
		if len(keys) == 0 {
			return nil
		}
		isHold, release := strings.Contains(symbol, "/"), strings.Contains(symbol, "~")
		count := 1
		if release {
			s := strings.TrimLeft(symbol, ">/~$")
			n := 0
			for n < len(s) && s[n] >= '0' && s[n] <= '9' {
				n++
			}
			if n > 0 {
				value, err := strconv.Atoi(s[:n])
				if err != nil || value > 120 {
					return nil
				}
				count = max(1, value)
			}
		}
		if isHold {
			held = append(held, keys...)
		}
		for n := 0; n < count; n++ {
			frames = append(frames, append(append([]string(nil), held...), keys...))
		}
		if release || !isHold {
			frames = append(frames, append([]string(nil), held...))
		}
		if len(frames) > 120 {
			return nil
		}
	}
	return frames
}
