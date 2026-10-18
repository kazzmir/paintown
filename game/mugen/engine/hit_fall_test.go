package engine

import (
	"strconv"
	"strings"
	"testing"
)

func TestHitFallDefaultsAndSourceIsolation(t *testing.T) {
	host := newParameterHost()
	runController(t, host, "HitDef", "")
	for key, want := range map[string]string{"fall": "0", "air.fall": "0", "fall.recover": "1", "fall.recovertime": "4"} {
		if host.hitParams[key] != want {
			t.Fatal(key, host.hitParams[key], want)
		}
	}
	state := stateSource(t, "[State 7, hit]\ntype=HitDef\ntrigger1=1\nfall=(Var(0):=Var(0)+1)\nfall.recovertime=FVar(0)\n")
	machine := NewStateMachine()
	for _, test := range []struct {
		seed      int
		delay     float64
		wantDelay string
	}{{7, 3.9, "3"}, {20, -3.9, "4"}, {0, 0, "0"}} {
		host := newParameterHost()
		host.vars[0], host.floats[0] = test.seed, test.delay
		if _, err := machine.ExecuteState(state, host); err != nil {
			t.Fatal(err)
		}
		if host.vars[0] != test.seed+2 || host.hitParams["fall"] != strconv.Itoa(test.seed+1) || host.hitParams["air.fall"] != strconv.Itoa(test.seed+2) || host.hitParams["fall.recovertime"] != test.wantDelay {
			t.Fatal("fall source inheritance/coercion leaked across hosts", host.hitParams, host.vars[0])
		}
	}
	if state.Controllers[0].Params["fall"] != "(Var(0):=Var(0)+1)" || state.Controllers[0].Params["air.fall"] != "" || state.Controllers[0].Params["fall.recovertime"] != "FVar(0)" {
		t.Fatal("fall compilation mutated shared sources")
	}
}

func TestHitFallValidationPrecedesStateEffects(t *testing.T) {
	for _, key := range []string{"fall", "air.fall", "fall.recover", "fall.recovertime"} {
		for _, raw := range []string{"unknown_fall_trigger", "2.5", "(-2.5)", "1,2"} {
			t.Run(key+"="+raw, func(t *testing.T) {
				host := newParameterHost()
				state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, hit]\ntype=HitDef\ntrigger1=1\n"+key+"="+raw+"\n")
				_, err := NewStateMachine().ExecuteState(state, host)
				if err == nil || !strings.Contains(err.Error(), key) || host.vars[0] != 0 || host.hitParams != nil {
					t.Fatal("invalid fall parameter compiled or mutated host", err, host.vars[0], host.hitParams)
				}
			})
		}
	}
}
