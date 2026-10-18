package engine

import (
	"strings"
	"testing"
)

func TestHitShakeDefaultAndRuntimeTypes(t *testing.T) {
	h := newParameterHost()
	runController(t, h, "HitDef", "")
	if h.hitParams["envshake.time"] != "0" || h.hitParams["envshake.freq"] != "60" || h.hitParams["envshake.ampl"] != "-4" {
		t.Fatal(h.hitParams)
	}
	if _, present := h.hitParams["envshake.phase"]; present {
		t.Fatal("default phase must use the evaluated frequency")
	}
	state := stateSource(t, "[State 7, shake]\ntype=HitDef\ntrigger1=1\nenvshake.time=FVar(0)\nenvshake.ampl=FVar(1)\nenvshake.freq=(Var(0):=Var(0)+1)*0+90\n")
	for i := 0; i < 2; i++ {
		h := newParameterHost()
		h.floats[0], h.floats[1] = 2.75, 4.75
		if _, err := NewStateMachine().ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
		if h.hitParams["envshake.time"] != "2" || h.hitParams["envshake.ampl"] != "4.75" || h.vars[0] != 1 || state.Controllers[0].Params["envshake.ampl"] != "FVar(1)" {
			t.Fatal(h.hitParams, h.vars[0])
		}
	}
}

func TestHitShakeErrorsAreAtomic(t *testing.T) {
	for key, raw := range map[string]string{"envshake.time": "1.5", "envshake.ampl": "4.5", "envshake.phase": "unknown_shake_trigger", "envshake.freq": "1,2"} {
		h := newParameterHost()
		state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, shake]\ntype=HitDef\ntrigger1=1\n"+key+"="+raw+"\n")
		_, err := NewStateMachine().ExecuteState(state, h)
		if err == nil || !strings.Contains(err.Error(), key) || h.vars[0] != 0 || h.hitParams != nil {
			t.Fatal(key, err, h.vars[0])
		}
	}
}
