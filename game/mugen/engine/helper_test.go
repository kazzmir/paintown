package engine

import "testing"

type helperHost struct {
	*parameterHost
	parameters HelperParameters
	calls      int
}

func (h *helperHost) CreateHelper(p HelperParameters) error {
	h.parameters = p
	h.calls++
	return nil
}

func TestHelperCompilesBeforeEffectsAndKeepsHostSizeValues(t *testing.T) {
	state := stateSource(t, "[State 7, helper]\ntype=Helper\ntrigger1=1\nid=123+var(0)\nstateno=800\nkeyctrl=var(0)\npos=3*var(0),-6\nsize.ground.front=8+var(0)\nname=\"unit helper\"\n")
	a, b := &helperHost{parameterHost: newParameterHost()}, &helperHost{parameterHost: newParameterHost()}
	b.vars[0] = 1
	machine := NewStateMachine()
	for _, h := range []*helperHost{a, b} {
		if _, err := machine.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
	}
	if a.parameters.ID != 123 || a.parameters.KeyControl || b.parameters.ID != 124 || !b.parameters.KeyControl || a.parameters.Size["size.ground.front"][0] != 8 || b.parameters.Size["size.ground.front"][0] != 9 || b.parameters.X != 3 || b.parameters.Y != -6 {
		t.Fatal("shared Helper host values", a.parameters, b.parameters)
	}
	a.parameters.Size["size.ground.front"][0] = 99
	if b.parameters.Size["size.ground.front"][0] != 9 || state.Controllers[0].Params["size.ground.front"] != "8+var(0)" {
		t.Fatal("Helper shared mutable size/source data")
	}
	bad := stateSource(t, "[State 7, invalid]\ntype=Helper\ntrigger1=1\nid=var(1):=99\nsize.head.pos=1,unknown_trigger(1)\n")
	if _, err := machine.ExecuteState(bad, a); err == nil || a.vars[1] != 0 || a.calls != 1 {
		t.Fatal("invalid later Helper parameter executed partial effects", err, a.vars[1], a.calls)
	}
}
