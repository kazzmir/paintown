package engine

import (
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestDownBounceDefaultsAndSharedSources(t *testing.T) {
	h := newParameterHost()
	runController(t, h, "HitDef", "")
	if cns.ParseHitDef(h.hitParams).DownBounce {
		t.Fatal("default bounce enabled")
	}
	state := stateSource(t, "[State 7, hit]\ntype=HitDef\ntrigger1=1\ndown.bounce=FVar(0)\n")
	machine := NewStateMachine()
	for _, value := range []float64{0.75, -2.75, 2.75} {
		h := newParameterHost()
		h.floats[0] = value
		if _, err := machine.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
		if h.hitParams["down.bounce"] != strconv.Itoa(int(value)) || cns.ParseHitDef(h.hitParams).DownBounce != (int(value) != 0) {
			t.Fatal(value, h.hitParams)
		}
	}
	if state.Controllers[0].Params["down.bounce"] != "FVar(0)" {
		t.Fatal("source mutated")
	}
}

func TestDownBounceValidationPrecedesEffects(t *testing.T) {
	for _, raw := range []string{"0.5", "1,2", "unknown_down_bounce_trigger"} {
		h := newParameterHost()
		state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, hit]\ntype=HitDef\ntrigger1=1\ndown.bounce="+raw+"\n")
		_, err := NewStateMachine().ExecuteState(state, h)
		if err == nil || !strings.Contains(err.Error(), "down.bounce") || h.vars[0] != 0 || h.hitParams != nil {
			t.Fatal(raw, err, h.vars[0])
		}
	}
}
