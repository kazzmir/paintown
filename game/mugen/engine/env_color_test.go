package engine

import (
	"image/color"
	"testing"
)

type environmentColorHost struct {
	*parameterHost
	color EnvironmentColorParameters
}

func (h *environmentColorHost) SetEnvironmentColor(p EnvironmentColorParameters) error {
	h.color = p
	return nil
}

func TestEnvironmentColorNativePackingAndDefaults(t *testing.T) {
	for _, test := range []struct {
		source string
		want   color.NRGBA
	}{
		{"", color.NRGBA{255, 255, 255, 255}},
		{"value=12,34,56", color.NRGBA{12, 34, 56, 255}},
		{"value=-1,34,56", color.NRGBA{255, 255, 255, 255}},
		{"value=12,-1,56", color.NRGBA{12, 255, 255, 255}},
		{"value=12,34,-1", color.NRGBA{12, 34, 255, 255}},
		{"value=-256,34,56", color.NRGBA{0, 255, 255, 255}},
		{"value=256,34,56", color.NRGBA{0, 35, 56, 255}},
		{"value=-0.9+0.0,34,56", color.NRGBA{0, 34, 56, 255}},
		{"value=2147483647,34,56", color.NRGBA{255, 255, 255, 255}},
	} {
		h := &environmentColorHost{parameterHost: newParameterHost()}
		state := stateSource(t, "[State 7, color]\ntype=EnvColor\ntrigger1=1\n"+test.source+"\n")
		if _, err := NewStateMachine().ExecuteState(state, h); err != nil {
			t.Fatal(test.source, err)
		}
		if h.color.Color != test.want || h.color.Time != 1 || h.color.Under {
			t.Fatal(test.source, h.color, test.want)
		}
	}
}

func TestEnvironmentColorClockThroughNestedAndPausedFrames(t *testing.T) {
	timeline := &Timeline{}
	timeline.BeginFrame()
	timeline.SetEnvironmentColor(EnvironmentColorParameters{Time: 2})
	timeline.RequestPause(NewPauseActor(), PauseParameters{Time: 30}, false)
	timeline.EndFrame()
	if timeline.EnvironmentColor.Time != 2 {
		t.Fatal("issuing frame consumed color duration")
	}
	timeline.BeginFrame()
	timeline.BeginFrame()
	if !timeline.Paused() || timeline.EnvironmentColor.Time != 1 {
		t.Fatal("global clock froze or nested twice", timeline.EnvironmentColor)
	}
	timeline.EndFrame()
	timeline.EndFrame()
	timeline.SkipSimulationFrame()
	if timeline.EnvironmentColor.Time != 0 {
		t.Fatal("global color failed to expire", timeline.EnvironmentColor)
	}
	for _, duration := range []int{-1, -2} {
		timeline.SetEnvironmentColor(EnvironmentColorParameters{Time: duration})
		timeline.BeginFrame()
		timeline.EndFrame()
		timeline.SkipSimulationFrame()
		if timeline.EnvironmentColor.Time != duration {
			t.Fatal("negative stage suppression expired")
		}
	}
	timeline.SetEnvironmentColor(EnvironmentColorParameters{})
	if timeline.EnvironmentColor.Time != 0 {
		t.Fatal("zero did not cancel suppression")
	}
}
