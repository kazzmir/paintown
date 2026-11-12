package engine

import "testing"

type widthHost struct {
	*parameterHost
	parameters WidthParameters
	calls      int
}

func (h *widthHost) SetWidth(p WidthParameters) error {
	h.parameters = p
	h.calls++
	return nil
}

func TestWidthShorthandEvaluatesOncePerComponentAndHost(t *testing.T) {
	state := stateSource(t, "[State 7, width]\ntype=Width\ntrigger1=1\nvalue=(var(0):=var(0)+1),(var(0):=var(0)+1)\n")
	a, b := &widthHost{parameterHost: newParameterHost()}, &widthHost{parameterHost: newParameterHost()}
	b.vars[0] = 20
	machine := NewStateMachine()
	for _, h := range []*widthHost{a, b} {
		if _, err := machine.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
	}
	if a.parameters != (WidthParameters{Edge: [2]float64{1, 2}, Player: [2]float64{1, 2}}) || b.parameters != (WidthParameters{Edge: [2]float64{21, 22}, Player: [2]float64{21, 22}}) || a.vars[0] != 2 || b.vars[0] != 22 || a.calls != 1 || b.calls != 1 {
		t.Fatal("Width repeated shorthand effects or shared host values", a.parameters, b.parameters, a.vars[0], b.vars[0])
	}
	if state.Controllers[0].Params["value"] != "(var(0):=var(0)+1),(var(0):=var(0)+1)" {
		t.Fatal("Width mutated source")
	}
	bad := stateSource(t, "[State 7, invalid]\ntype=Width\ntrigger1=1\nedge=var(1):=99,0\nplayer=0,unknown_trigger(1)\n")
	if _, err := machine.ExecuteState(bad, a); err == nil || a.vars[1] != 0 || a.calls != 1 {
		t.Fatal("malformed later Width argument allowed partial effects", err, a.vars[1], a.calls)
	}
}
