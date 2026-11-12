package engine

import "math"

// Amplitude and output offset are in the beta's 240p camera units, independent
// of the render surface or the requesting character's script coordinates.
type ShakeParameters struct {
	Time                        int
	Amplitude, Frequency, Phase float32
}

func (t *Timeline) RequestShake(parameters ShakeParameters) {
	if parameters.Time <= 0 {
		return
	}
	// The issuing frame consumes a tick without advancing the initial phase.
	parameters.Time--
	t.pendingShake = &parameters
}

func (t *Timeline) advanceShake() {
	if t.shake.Time > 0 {
		t.shake.Time--
		t.shake.Phase += t.shake.Frequency
	}
	if t.pendingShake != nil {
		t.shake, t.pendingShake = *t.pendingShake, nil
	}
}

func (t *Timeline) ShakeOffset() float64 {
	if t.shake.Time <= 0 {
		return 0
	}
	wave := t.shake.Amplitude * float32(math.Sin(float64(t.shake.Phase)*math.Pi/180))
	// Native rounds the wave in 240p units, then negates it. Half values
	// therefore have the measured asymmetric signs (-2.5 -> +2 offset).
	return -math.Floor(float64(wave) + .5)
}

func (t *Timeline) ClearShake() {
	t.shake, t.pendingShake = ShakeParameters{}, nil
}
