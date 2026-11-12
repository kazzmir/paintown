package engine

import (
	"strings"
	"testing"
)

func TestHitPauseDefaultsRepeatExpressionsWithoutSharingHostState(t *testing.T) {
	state := stateSource(t, "[State 7, hit]\ntype=HitDef\ntrigger1=1\npausetime=(var(0):=var(0)+1),(var(1):=var(1)+2)\n")
	machine := NewStateMachine()
	for _, test := range []struct {
		seed         int
		pause, guard string
	}{{0, "1,2", "2,4"}, {10, "11,12", "12,14"}} {
		h := newParameterHost()
		h.vars[0], h.vars[1] = test.seed, test.seed
		if _, err := machine.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
		// Native inherited-guard records both source components executing
		// again for omitted guard.pausetime, rather than copying integers.
		if h.hitParams["pausetime"] != test.pause || h.hitParams["guard.pausetime"] != test.guard || h.vars[0] != test.seed+2 || h.vars[1] != test.seed+4 {
			t.Fatal("pause defaults/host isolation", test.seed, h.hitParams, h.vars[:2])
		}
	}
	if state.Controllers[0].Params["pausetime"] != "(var(0):=var(0)+1),(var(1):=var(1)+2)" {
		t.Fatal("compilation changed shared source")
	}
	if _, present := state.Controllers[0].Params["guard.pausetime"]; present {
		t.Fatal("default leaked into shared source")
	}
}

func TestHitPauseValidationPrecedesAllStateEffects(t *testing.T) {
	for _, params := range []string{
		"pausetime=unknown_pause_trigger,2",
		"guard.pausetime=unknown_pause_trigger,2",
		"pausetime=2.5,4",
		"guard.pausetime=2,3.5",
	} {
		t.Run(params, func(t *testing.T) {
			h := newParameterHost()
			state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, hit]\ntype=HitDef\ntrigger1=1\n"+params+"\n")
			_, err := NewStateMachine().ExecuteState(state, h)
			if err == nil || !strings.Contains(err.Error(), "pausetime") || h.vars[0] != 0 || h.hitParams != nil {
				t.Fatal("invalid pause compiled or mutated the host", err, h.vars[0], h.hitParams)
			}
		})
	}
}
