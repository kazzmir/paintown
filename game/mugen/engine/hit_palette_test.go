package engine

import (
	"strings"
	"testing"
)

func TestHitPaletteDefaultsRuntimeAndSource(t *testing.T) {
	h := newParameterHost()
	runController(t, h, "HitDef", "")
	for key, want := range map[string]string{"palfx.time": "0", "palfx.mul": "256,256,256", "palfx.add": "0,0,0"} {
		if h.hitParams[key] != want {
			t.Fatal(key, h.hitParams)
		}
	}
	state := stateSource(t, "[State 7, palette]\ntype=HitDef\ntrigger1=1\npalfx.time=FVar(0)\npalfx.mul=FVar(1),256,64\npalfx.add=20,-40,5\n")
	machine := NewStateMachine()
	for i := 0; i < 2; i++ {
		h := newParameterHost()
		h.floats[0], h.floats[1] = -1.75, 128.75
		if _, err := machine.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
		if h.hitParams["palfx.time"] != "-1" || h.hitParams["palfx.mul"] != "128,256,64" || state.Controllers[0].Params["palfx.time"] != "FVar(0)" {
			t.Fatal(h.hitParams)
		}
	}
}

func TestHitPaletteErrorsAreAtomic(t *testing.T) {
	for key, values := range map[string][]string{
		"palfx.time": {"1.25", "unknown_palette_trigger", "1,2"},
		"palfx.mul":  {"128", "128,256", "1.25,256,256", "unknown_palette_trigger,256,256"},
		"palfx.add":  {"1", "1,2", "0,0,1.25", "0,0,unknown_palette_trigger"},
	} {
		for _, raw := range values {
			h := newParameterHost()
			state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, palette]\ntype=HitDef\ntrigger1=1\n"+key+"="+raw+"\n")
			_, err := NewStateMachine().ExecuteState(state, h)
			if err == nil || !strings.Contains(err.Error(), key) || h.vars[0] != 0 || h.hitParams != nil {
				t.Fatal(key, raw, err, h.vars[0])
			}
		}
	}
}
