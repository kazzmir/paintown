package cns

import (
	"os"
	"strings"
	"testing"
)

func TestReferenceStateBlockBinding(t *testing.T) {
	data, err := os.ReadFile("../tests/testdata/reference/state-binding.cns")
	if err != nil {
		t.Fatal(err)
	}
	file, err := ParseNamed(strings.NewReader(string(data)), "state-binding.cns")
	if err != nil {
		t.Fatal(err)
	}
	state := file.States[0]
	if len(file.States) != 3 || state == nil || len(state.Controllers) != 2 {
		t.Fatalf("states=%v state 0=%+v", file.States, state)
	}
	if state.Source != "state-binding.cns" || state.Line != 11 || state.Controllers[0].Params["value"] != "111" || state.Controllers[1].Params["value"] != "222" {
		t.Fatalf("first definition or controller ownership lost: %+v", state)
	}
	file, err = Parse(strings.NewReader("[State 123, orphan]\ntype=Null\ntrigger1=1\n"))
	if err != nil || len(file.States) != 0 {
		t.Fatalf("orphan invented a state: %+v, %v", file, err)
	}
}

func TestStateExpressionsPreserved(t *testing.T) {
	data, err := Parse(strings.NewReader(`[Statedef 200]
anim = 200 + var(1)
ctrl = time != 0
[State 200, expression]
type = VarSet
trigger1 = command = "a;b"
trigger1 = time!=0 && var(1):=5.0
v = var(2)
value = ifelse(var(1), 1.0, 2)
text = "quoted text"
`))
	if err != nil {
		t.Fatal(err)
	}
	state := data.States[200]
	if state.Parameters["anim"] != "200 + var(1)" || state.Parameters["ctrl"] != "time != 0" {
		t.Fatalf("entry expressions lost: %#v", state.Parameters)
	}
	ctrl := state.Controllers[0]
	if ctrl.Line != 4 || state.Line != 1 {
		t.Fatalf("locations = %d / %d", state.Line, ctrl.Line)
	}
	want := []string{`trigger1 = command = "a;b"`, `trigger1 = time!=0 && var(1):=5.0`}
	if len(ctrl.Triggers) != 2 || ctrl.Triggers[0] != want[0] || ctrl.Triggers[1] != want[1] {
		t.Fatalf("triggers changed: %#v", ctrl.Triggers)
	}
	if ctrl.Params["text"] != `"quoted text"` || ctrl.Params["value"] != "ifelse(var(1), 1.0, 2)" {
		t.Fatalf("parameters changed: %#v", ctrl.Params)
	}
}
