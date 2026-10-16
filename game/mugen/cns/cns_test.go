package cns

import (
	"strings"
	"testing"
)

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
