package engine

import "testing"

type screenBoundHost struct {
	*parameterHost
	parameters ScreenBoundParameters
	calls      int
}

func (h *screenBoundHost) SetScreenBound(p ScreenBoundParameters) error {
	h.parameters = p
	h.calls++
	return nil
}

func TestScreenBoundExpressionsAreHostLocalAndCompileBeforeEffects(t *testing.T) {
	state := stateSource(t, "[State 7, screen]\ntype=ScreenBound\ntrigger1=1\nvalue=var(0)\nmovecamera=var(0),!var(0)\n")
	machine := NewStateMachine()
	a, b := &screenBoundHost{parameterHost: newParameterHost()}, &screenBoundHost{parameterHost: newParameterHost()}
	b.vars[0] = 2
	for _, h := range []*screenBoundHost{a, b} {
		if _, err := machine.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
	}
	if a.parameters != (ScreenBoundParameters{MoveY: true}) || b.parameters != (ScreenBoundParameters{Bound: true, MoveX: true}) || a.calls != 1 || b.calls != 1 {
		t.Fatal("ScreenBound shared values across hosts", a.parameters, b.parameters)
	}
	if state.Controllers[0].Params["movecamera"] != "var(0),!var(0)" {
		t.Fatal("ScreenBound mutated shared controller source")
	}
	bad := stateSource(t, "[State 7, bad screen]\ntype=ScreenBound\ntrigger1=1\nvalue=var(1):=99\nmovecamera=1,unknown_trigger(1)\n")
	if _, err := machine.ExecuteState(bad, a); err == nil || a.vars[1] != 0 || a.calls != 1 {
		t.Fatal("malformed later parameter allowed partial ScreenBound effects", err, a.vars[1], a.calls)
	}
	defaults := stateSource(t, "[State 7, defaults]\ntype=ScreenBound\ntrigger1=1\n")
	if _, err := machine.ExecuteState(defaults, a); err != nil {
		t.Fatal(err)
	}
	if a.parameters != (ScreenBoundParameters{}) {
		t.Fatal("ScreenBound omitted parameter defaults", a.parameters)
	}
}
