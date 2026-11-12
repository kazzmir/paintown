package engine

import (
	"strings"
	"testing"
)

func TestHitSnapReplacesBoundsAndEvaluatesSourceTwice(t *testing.T) {
	source := "[State 7, snap]\ntype=HitDef\ntrigger1=1\nmindist=(Var(2):=99),0\nmaxdist=(Var(2):=99),0\nsnap=(Var(0):=Var(0)+10),20,(Var(1):=Var(1)+1)\n"
	state := stateSource(t, source)
	machine := NewStateMachine()
	for i := 0; i < 2; i++ {
		h := newParameterHost()
		if _, err := machine.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
		if h.vars[0] != 20 || h.vars[1] != 1 || h.vars[2] != 0 || h.hitParams["mindist"] != "10,20" || h.hitParams["maxdist"] != "20,20" || h.hitParams["snap.time"] != "1" {
			t.Fatal(h.vars, h.hitParams)
		}
		if _, raw := h.hitParams["snap"]; raw {
			t.Fatal("unevaluated snap reached runtime")
		}
	}
	if !strings.Contains(state.Controllers[0].Params["snap"], "Var(0)") {
		t.Fatal("shared source mutated")
	}
}

func TestHitConstraintComponentsAndAtomicErrors(t *testing.T) {
	for _, key := range []string{"mindist", "maxdist", "snap"} {
		h := newParameterHost()
		h.floats[0] = 30.75
		runController(t, h, "HitDef", key+"=FVar(0)")
		output := key
		if key == "snap" {
			output = "mindist"
		}
		if h.hitParams[output] != "30" {
			t.Fatal(key, h.hitParams)
		}
		for _, raw := range []string{"1.25,2", "unknown_constraint_trigger,0", "1,2,3,4"} {
			h := newParameterHost()
			state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, constraint]\ntype=HitDef\ntrigger1=1\n"+key+"="+raw+"\n")
			_, err := NewStateMachine().ExecuteState(state, h)
			if err == nil || !strings.Contains(err.Error(), key) || h.vars[0] != 0 || h.hitParams != nil {
				t.Fatal(key, raw, err, h.vars[0])
			}
		}
	}
}
