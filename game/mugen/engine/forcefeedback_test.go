package engine

import "testing"

// The supplied beta lowers ForceFeedback to the same kind as Null. An embedding
// host's optional device hook must therefore never receive this controller.
type nativeFeedbackHost struct {
	*parameterHost
	deviceCalls int
}

func (h *nativeFeedbackHost) ForceFeedback(map[string]string) { h.deviceCalls++ }

func TestNativeForceFeedbackHasNoDeviceDispatch(t *testing.T) {
	for _, version := range []string{"1.0", "1.1"} {
		t.Run(version, func(t *testing.T) {
			state := stateSource(t, "[State 7, ForceFeedback]\ntype=ForceFeedback\ntrigger1=(Var(0):=Var(0)+1)\ntime=1+\nwaveform=not_a_wave\nfreq=M4dbMissingTrigger\n")
			state.LanguageVersion = version
			for _, ctrl := range state.Controllers {
				ctrl.LanguageVersion = version
			}
			h := &nativeFeedbackHost{parameterHost: newParameterHost()}
			if _, err := NewStateMachine().ExecuteState(state, h); err != nil {
				t.Fatal(err)
			}
			if h.deviceCalls != 0 {
				t.Fatalf("native Null-kind controller dispatched %d device calls", h.deviceCalls)
			}
			if h.vars[0] != 1 {
				t.Fatalf("ordinary trigger assignment = %d; native evaluates it", h.vars[0])
			}
		})
	}
}
