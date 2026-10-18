package cns

import "strconv"

func ParseHitShake(params map[string]string, prefix string) HitShake {
	result := HitShake{Amplitude: -4, Frequency: 60}
	result.Time, _ = strconv.Atoi(params[prefix+"time"])
	if raw, present := params[prefix+"ampl"]; present {
		result.Amplitude, _ = strconv.ParseFloat(raw, 64)
	}
	if raw, present := params[prefix+"freq"]; present {
		result.Frequency, _ = strconv.ParseFloat(raw, 64)
	}
	result.Frequency = float64(max(float32(.001), float32(result.Frequency)))
	if result.Frequency >= 90 {
		result.Phase = 90
	}
	if raw, present := params[prefix+"phase"]; present {
		result.Phase, _ = strconv.ParseFloat(raw, 64)
	}
	return result
}
