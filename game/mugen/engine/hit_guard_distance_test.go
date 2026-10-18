package engine

import (
	"strconv"
	"strings"
	"testing"
)

func TestGuardDistanceOptionalAndHostLocal(t *testing.T) {
	h := newParameterHost()
	runController(t, h, "HitDef", "")
	if _, ok := h.hitParams["guard.dist"]; ok {
		t.Fatal("omission must preserve the actor's range")
	}
	state := stateSource(t, "[State 7, range]\ntype=HitDef\ntrigger1=1\nguard.dist=FVar(0)\n")
	machine := NewStateMachine()
	for _, n := range []float64{40.75, 0.75, -1.75} {
		h := newParameterHost()
		h.floats[0] = n
		if _, err := machine.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
		if h.hitParams["guard.dist"] != strconv.Itoa(int(n)) {
			t.Fatal(n, h.hitParams)
		}
	}
	if state.Controllers[0].Params["guard.dist"] != "FVar(0)" {
		t.Fatal("shared source mutated")
	}
}

func TestGuardDistanceRejectsBeforeEffects(t *testing.T) {
	for _, raw := range []string{"40.25", "-0.5", "1,2", "unknown_guard_distance_trigger"} {
		h := newParameterHost()
		state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, range]\ntype=HitDef\ntrigger1=1\nguard.dist="+raw+"\n")
		_, err := NewStateMachine().ExecuteState(state, h)
		if err == nil || !strings.Contains(err.Error(), "guard.dist") || h.vars[0] != 0 || h.hitParams != nil {
			t.Fatal(raw, err, h.vars[0])
		}
	}
}
