package engine

import (
	"strings"
	"testing"
)

type fallSetHost struct {
	*parameterHost
	fall int32
}

func (h *fallSetHost) SetHitFallValue(value int32) {
	if value != -1 {
		h.fall = value
	}
}

func TestHitFallSetSourceIsolationAndNativeIgnoredParameters(t *testing.T) {
	state := stateSource(t, "[State 7, fall]\ntype=HitFallSet\ntrigger1=1\nvalue=FVar(0)\nxvel=unknown_ignored_trigger\nyvel=1,2\n")
	machine := NewStateMachine()
	for _, value := range []float64{-2.75, 0.75, 2.75} {
		h := &fallSetHost{parameterHost: newParameterHost(), fall: 1}
		h.floats[0] = value
		if _, err := machine.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
		if h.fall != int32(value) {
			t.Fatal(value, h.fall)
		}
	}
	if state.Controllers[0].Params["value"] != "FVar(0)" {
		t.Fatal("source mutated")
	}
	for _, params := range []string{"", "value=-1"} {
		h := &fallSetHost{parameterHost: newParameterHost(), fall: 2}
		state := stateSource(t, "[State 7, preserve]\ntype=HitFallSet\ntrigger1=1\n"+params+"\n")
		if _, err := machine.ExecuteState(state, h); err != nil || h.fall != 2 {
			t.Fatal(h.fall, err)
		}
	}
}

func TestHitFallSetValidationPrecedesStateEffects(t *testing.T) {
	for _, raw := range []string{"0.5", "1,2", "unknown_fall_set_trigger"} {
		h := &fallSetHost{parameterHost: newParameterHost(), fall: 1}
		state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, fall]\ntype=HitFallSet\ntrigger1=1\nvalue="+raw+"\n")
		_, err := NewStateMachine().ExecuteState(state, h)
		if err == nil || !strings.Contains(err.Error(), "value") || h.vars[0] != 0 || h.fall != 1 {
			t.Fatal(raw, err, h.vars[0], h.fall)
		}
	}
}
