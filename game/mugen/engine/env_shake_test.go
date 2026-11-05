package engine

import (
	"testing"
)

type environmentShakeHost struct {
	*parameterHost
	shake ShakeParameters
}

func (h *environmentShakeHost) SetEnvironmentShake(p ShakeParameters) error { h.shake = p; return nil }

func TestEnvironmentShakeDefaultsAndFrequencyNormalization(t *testing.T) {
	for _, test := range []struct {
		source           string
		frequency, phase float32
	}{
		{"", 60, 0}, {"freq=-90", 90, 90}, {"freq=0", .001, 0},
		{"freq=-0.0001", .0001, 0}, {"freq=0.0001", .0001, 0},
		{"freq=360\nphase=-90", 360, -90},
	} {
		h := &environmentShakeHost{parameterHost: newParameterHost()}
		state := stateSource(t, "[State 7, shake]\ntype=EnvShake\ntrigger1=1\n"+test.source+"\n")
		if _, err := NewStateMachine().ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
		if h.shake.Time != 0 || h.shake.Amplitude != -4 || h.shake.Frequency != test.frequency || h.shake.Phase != test.phase {
			t.Fatal(test, h.shake)
		}
	}
}

func TestEnvironmentShakeEvaluatesInactiveParametersInNativeOrder(t *testing.T) {
	h := &environmentShakeHost{parameterHost: newParameterHost()}
	state := stateSource(t, `[State 7, shake]
type=EnvShake
trigger1=1
freq=((Var(0):=Var(0)*10+4)*0-90)
phase=((Var(0):=Var(0)*10+3)*0+0.5)
ampl=((Var(0):=Var(0)*10+2)*0+4.75)
time=((Var(0):=Var(0)*10+1)*0)
`)
	if _, err := NewStateMachine().ExecuteState(state, h); err != nil {
		t.Fatal(err)
	}
	if h.vars[0] != 1234 || h.shake.Time != 0 || h.shake.Amplitude != 4.75 || h.shake.Phase != .5 || h.shake.Frequency != 90 {
		t.Fatal(h.vars[0], h.shake)
	}
}

func TestEnvironmentShakeValidationPrecedesStateEffects(t *testing.T) {
	for _, bad := range []string{"time=1.5", "ampl=4.5", "freq=1,2", "time=0\nfreq=unknown_trigger"} {
		h := &environmentShakeHost{parameterHost: newParameterHost(), shake: ShakeParameters{Time: 57}}
		state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, shake]\ntype=EnvShake\ntrigger1=1\n"+bad+"\n")
		if _, err := NewStateMachine().ExecuteState(state, h); err == nil || h.vars[0] != 0 || h.shake.Time != 57 {
			t.Fatal(bad, err, h.vars[0], h.shake)
		}
	}
}

func TestEnvironmentShakePendingReplacementPreservesCurrentAndDefersExposure(t *testing.T) {
	clock := &Timeline{}
	clock.BeginFrame()
	clock.RequestEnvironmentShake(ShakeParameters{Time: 60, Amplitude: 4, Phase: 90})
	if clock.ShakeOffset() != 0 {
		t.Fatal("request exposed during issuing CNS")
	}
	clock.EndFrame()
	for _, duration := range []int{0, -1} {
		clock.BeginFrame()
		clock.RequestEnvironmentShake(ShakeParameters{Time: 60, Amplitude: 8, Phase: 90})
		clock.RequestEnvironmentShake(ShakeParameters{Time: duration})
		clock.EndFrame()
		if clock.ShakeOffset() != -4 {
			t.Fatal("nonpositive failed to discard pending or preserve current", duration, clock.shake)
		}
	}
	clock.BeginFrame()
	clock.RequestEnvironmentShake(ShakeParameters{Time: 1, Amplitude: 8, Phase: 90})
	if clock.ShakeOffset() != -4 {
		t.Fatal("time-one request changed same-frame getter")
	}
	clock.EndFrame()
	if clock.ShakeOffset() != 0 {
		t.Fatal("time-one failed to replace current at frame boundary")
	}
}
