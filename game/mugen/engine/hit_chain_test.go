package engine

import (
	"strconv"
	"strings"
	"testing"
)

func TestHitChainOptionalIntegerPrograms(t *testing.T) {
	for _, key := range []string{"chainid", "nochainid", "air.juggle"} {
		state := stateSource(t, "[State 7, chain]\ntype=HitDef\ntrigger1=1\n"+key+"=FVar(0)\n")
		machine := NewStateMachine()
		for _, value := range []float64{7.75, -1.75, 0} {
			h := newParameterHost()
			h.floats[0] = value
			if _, err := machine.ExecuteState(state, h); err != nil {
				t.Fatal(key, err)
			}
			want := strconv.Itoa(int(value))
			if key == "nochainid" {
				want += ",-1"
			}
			if h.hitParams[key] != want || state.Controllers[0].Params[key] != "FVar(0)" {
				t.Fatal(key, h.hitParams[key], want)
			}
		}
		for _, raw := range []string{"1.25", "unknown_chain_trigger", "1,2,3"} {
			h := newParameterHost()
			state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, chain]\ntype=HitDef\ntrigger1=1\n"+key+"="+raw+"\n")
			_, err := machine.ExecuteState(state, h)
			if err == nil || !strings.Contains(err.Error(), key) || h.vars[0] != 0 || h.hitParams != nil {
				t.Fatal(key, raw, err, h.vars[0])
			}
		}
	}
}
