package engine

import (
	"strings"
	"testing"
)

func TestHitDefLiteralDefaultsAndPrefixes(t *testing.T) {
	for _, test := range []struct{ source, key, want string }{
		{"", "animtype", "Light"}, {"", "air.animtype", "Light"},
		{"", "fall.animtype", "Back"}, {"", "hitflag", "MAF"},
		{"", "guardflag", ""}, {"", "affectteam", "E"},
		{"", "ground.type", "High"}, {"", "air.type", "High"},
		{"animtype=Medium", "air.animtype", "Medium"},
		{"air.animtype=UPjunk", "fall.animtype", "Up"},
		{"ground.type=Trip", "air.type", "Trip"},
		{"animtype=HARDjunk", "animtype", "Hard"},
		{"affectteam=enemy", "affectteam", "E"},
		{"hitflag=M?A!F", "hitflag", "MAF"},
		{"guardflag=M?", "guardflag", "M"},
		{"hitflag=?", "hitflag", ""},
		{"attr=S,NA,XX", "attr", "S,NA"},
		{"attr=S,XX,NA", "attr", ""},
		{"attr=S,NAjunk", "attr", ""},
		{"attr=\"S,NA\"", "attr", ""},
	} {
		h := newParameterHost()
		runController(t, h, "HitDef", test.source)
		if h.hitParams[test.key] != test.want {
			t.Fatalf("%q %s=%q, want %q", test.source, test.key, h.hitParams[test.key], test.want)
		}
	}
}

func TestHitDefLiteralFailureIsAtomic(t *testing.T) {
	for _, key := range []string{"affectteam", "animtype", "air.animtype", "fall.animtype", "ground.type", "air.type", "attr"} {
		raw := "?"
		if key == "attr" {
			raw = "X,NA"
		}
		h := newParameterHost()
		state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, hit]\ntype=HitDef\ntrigger1=1\n"+key+"="+raw+"\n")
		_, err := NewStateMachine().ExecuteState(state, h)
		if err == nil || !strings.Contains(err.Error(), key) || h.vars[0] != 0 || h.hitParams != nil {
			t.Fatal(key, err, h.vars[0])
		}
	}
}

func TestHitDefLiteralSourceIsImmutable(t *testing.T) {
	state := stateSource(t, "[State 7, hit]\ntype=HitDef\ntrigger1=1\nattr=s,na\nanimtype=HARDjunk\n")
	machine := NewStateMachine()
	for i := 0; i < 2; i++ {
		h := newParameterHost()
		if _, err := machine.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
		h.hitParams["animtype"] = "corrupted"
	}
	if state.Controllers[0].Params["animtype"] != "HARDjunk" || state.Controllers[0].Params["attr"] != "s,na" {
		t.Fatal("literal compilation mutated shared source")
	}
}
