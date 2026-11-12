package cns

import (
	"math"
	"strconv"
)

func ParseHitShake(params map[string]string, prefix string) HitShake {
	result := HitShake{Amplitude: -4, Frequency: 60}
	result.Time, _ = strconv.Atoi(params[prefix+"time"])
	if raw, present := params[prefix+"ampl"]; present {
		result.Amplitude, _ = strconv.ParseFloat(raw, 64)
	}
	if raw, present := params[prefix+"freq"]; present {
		result.Frequency, _ = strconv.ParseFloat(raw, 64)
	}
	// The beta normalizes magnitude before selecting an omitted phase.
	// Only exact zero uses the fallback; tiny nonzero frequencies survive.
	result.Frequency = float64(float32(math.Abs(result.Frequency)))
	if result.Frequency == 0 {
		result.Frequency = float64(float32(.001))
	}
	if result.Frequency >= 90 {
		result.Phase = 90
	}
	if raw, present := params[prefix+"phase"]; present {
		result.Phase, _ = strconv.ParseFloat(raw, 64)
	}
	return result
}
