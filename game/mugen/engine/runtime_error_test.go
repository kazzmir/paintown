package engine

import (
	"strings"
	"testing"
)

func TestMalformedBottomTriggerStopsBeforeLaterEffects(t *testing.T) {
	h := newParameterHost()
	state := stateSource(t, "[State 7, fatal trigger]\ntype=LifeAdd\ntrigger1=~(parent,time)\nvalue=1\n[State 7, later effect]\ntype=LifeAdd\ntrigger1=1\nvalue=100\n")
	_, err := NewStateMachine().ExecuteState(state, h)
	if err == nil || !strings.Contains(err.Error(), "malformed expression") || !strings.Contains(err.Error(), "state 7 controller 0") {
		t.Fatal("fatal trigger became false or lost source context", err)
	}
	if h.life != 0 {
		t.Fatal("effects executed after malformed trigger", h.life)
	}
}
