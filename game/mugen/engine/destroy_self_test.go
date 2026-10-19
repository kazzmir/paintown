package engine

import "testing"

func TestDestroySelfValidatesBeforeParameterEffects(t *testing.T) {
	h := newParameterHost()
	state := stateSource(t, "[State 7, invalid removal]\ntype=DestroySelf\ntrigger1=1\nrecursive=var(1):=99\nremoveexplods=unknown_trigger(1)\n")
	if _, err := NewStateMachine().ExecuteState(state, h); err == nil || h.vars[1] != 0 {
		t.Fatal("invalid removal executed earlier parameter effects", err, h.vars[1])
	}
}
