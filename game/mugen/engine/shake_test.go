package engine

import "testing"

func TestNativeShakeFrameClockAndRounding(t *testing.T) {
	clock := &Timeline{}
	clock.BeginFrame()
	clock.RequestShake(ShakeParameters{Time: 8, Amplitude: 4, Frequency: 90})
	if clock.ShakeOffset() != 0 {
		t.Fatal("new request visible during issuing CNS")
	}
	clock.EndFrame()
	for _, want := range []float64{0, -4, 0, 4, 0, -4, 0, 0} {
		if actual := clock.ShakeOffset(); actual != want {
			t.Fatal(actual, want, clock.shake)
		}
		clock.BeginFrame()
		clock.EndFrame()
	}
	for _, test := range []struct {
		amplitude, phase float32
		want             float64
	}{
		{2.5, 90, -3}, {-2.5, 90, 2}, {4.75, 30, -2}, {4.75, 90.5, -5},
	} {
		clock.BeginFrame()
		clock.RequestShake(ShakeParameters{Time: 60, Amplitude: test.amplitude, Phase: test.phase})
		clock.EndFrame()
		if actual := clock.ShakeOffset(); actual != test.want {
			t.Fatal(test, actual)
		}
	}
}

func TestShakeZeroPreservesAndRoundResetClears(t *testing.T) {
	clock := &Timeline{}
	clock.BeginFrame()
	clock.RequestShake(ShakeParameters{Time: 60, Amplitude: 4, Phase: 90})
	clock.EndFrame()
	for _, time := range []int{0, -1} {
		clock.BeginFrame()
		clock.RequestShake(ShakeParameters{Time: time})
		clock.EndFrame()
		if clock.ShakeOffset() != -4 {
			t.Fatal("nonpositive request reset the active effect")
		}
	}
	clock.ClearShake()
	if clock.ShakeOffset() != 0 || clock.pendingShake != nil {
		t.Fatal("round reset retained shake")
	}
}
