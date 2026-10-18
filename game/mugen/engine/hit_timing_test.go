package engine

import (
	"strconv"
	"strings"
	"testing"
)

func TestHitTimingDefaultsRepeatSourcesForEachHost(t *testing.T) {
	state := stateSource(t, "[State 7, hit]\ntype=HitDef\ntrigger1=1\nground.hittime=(var(0):=var(0)+1)\nground.slidetime=(var(1):=var(1)+2)\n")
	machine := NewStateMachine()
	for _, seed := range []int{0, 10} {
		host := newParameterHost()
		host.vars[0], host.vars[1] = seed, seed
		if _, err := machine.ExecuteState(state, host); err != nil {
			t.Fatal(err)
		}
		for key, want := range map[string]int{
			"ground.hittime": seed + 1, "guard.hittime": seed + 2,
			"ground.slidetime": seed + 2, "guard.slidetime": seed + 4,
			"guard.ctrltime": seed + 6, "airguard.ctrltime": seed + 8,
		} {
			if host.hitParams[key] != strconv.Itoa(want) {
				t.Fatalf("seed=%d %s=%s want=%d", seed, key, host.hitParams[key], want)
			}
		}
		if host.vars[0] != seed+2 || host.vars[1] != seed+8 {
			t.Fatal("inherited sources did not re-execute", host.vars[:2])
		}
	}
	if state.Controllers[0].Params["ground.hittime"] != "(var(0):=var(0)+1)" || state.Controllers[0].Params["guard.hittime"] != "" {
		t.Fatal("compilation mutated shared source")
	}
}

func TestHitTimingValidationPrecedesStateEffects(t *testing.T) {
	for _, key := range []string{"ground.hittime", "ground.slidetime", "guard.hittime", "guard.slidetime", "guard.ctrltime", "airguard.ctrltime", "air.hittime", "down.hittime"} {
		for _, raw := range []string{"unknown_timing_trigger", "2.5", "1,2"} {
			t.Run(key+"="+raw, func(t *testing.T) {
				host := newParameterHost()
				state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, hit]\ntype=HitDef\ntrigger1=1\n"+key+"="+raw+"\n")
				_, err := NewStateMachine().ExecuteState(state, host)
				if err == nil || !strings.Contains(err.Error(), key) || host.vars[0] != 0 || host.hitParams != nil {
					t.Fatal("invalid timing compiled or mutated host", err, host.vars[0], host.hitParams)
				}
			})
		}
	}
}

func TestAirDownTimeDefaultsAndHostIsolation(t *testing.T) {
	for _, test := range []struct{ source, air, down string }{
		{"", "20", "20"},
		{"air.hittime=0\ndown.hittime=0", "0", "0"},
		{"air.hittime=-3\ndown.hittime=-4", "0", "0"},
	} {
		host := newParameterHost()
		runController(t, host, "HitDef", test.source)
		if host.hitParams["air.hittime"] != test.air || host.hitParams["down.hittime"] != test.down {
			t.Fatal("omission/zero/negative timing", test.source, host.hitParams)
		}
	}
	state := stateSource(t, "[State 7, hit]\ntype=HitDef\ntrigger1=1\nair.hittime=FVar(0)\ndown.hittime=FVar(1)\n")
	machine := NewStateMachine()
	for _, test := range []struct {
		air, down         float64
		wantAir, wantDown string
	}{{3.9, 4.9, "3", "4"}, {0.9, -3.9, "0", "0"}} {
		host := newParameterHost()
		host.floats[0], host.floats[1] = test.air, test.down
		if _, err := machine.ExecuteState(state, host); err != nil {
			t.Fatal(err)
		}
		if host.hitParams["air.hittime"] != test.wantAir || host.hitParams["down.hittime"] != test.wantDown {
			t.Fatal("shared timing retained another host's values", host.hitParams)
		}
	}
	if state.Controllers[0].Params["air.hittime"] != "FVar(0)" || state.Controllers[0].Params["down.hittime"] != "FVar(1)" {
		t.Fatal("timing compilation changed shared source")
	}
}
