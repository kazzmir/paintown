package engine

import "testing"

type afterImageHost struct {
	*parameterHost
	image    AfterImageParameters
	duration int
}

func (h *afterImageHost) SetAfterImage(p AfterImageParameters) error { h.image = p; return nil }
func (h *afterImageHost) SetAfterImageTime(n int) error              { h.duration = n; return nil }

func TestAfterImageIgnoredRedIsValidatedWithoutRuntimeEvaluation(t *testing.T) {
	h := &afterImageHost{parameterHost: newParameterHost()}
	state := stateSource(t, `[State 7, image]
type=AfterImage
trigger1=1
time=0
palbright=(Var(0):=99),((Var(1):=Var(1)+1)*0+4),5
palpostbright=((Var(1):=Var(1)+1)*0+6),7,8
`)
	if _, err := NewStateMachine().ExecuteState(state, h); err != nil {
		t.Fatal(err)
	}
	if h.vars[0] != 0 || h.vars[1] != 2 || h.image.Time != 0 || h.image.Bright != [3]int{30, 4, 5} || h.image.PostBright != [3]int{6, 7, 8} {
		t.Fatal(h.vars, h.image)
	}
	for _, raw := range []string{"1.5,0,0", "UnknownAfterImage,0,0"} {
		h := &afterImageHost{parameterHost: newParameterHost()}
		state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=23\n[State 7, bad]\ntype=AfterImage\ntrigger1=1\npalbright="+raw+"\n")
		if _, err := NewStateMachine().ExecuteState(state, h); err == nil || h.vars[0] != 0 {
			t.Fatal(raw, err, h.vars)
		}
	}
}

func TestAfterImageTimeCanonicalFieldIgnoresUnusedAlias(t *testing.T) {
	for _, text := range []string{"time=0\nvalue=UnknownAfterImage", "value=1.5\ntime=0", "value=0", ""} {
		h := &afterImageHost{parameterHost: newParameterHost(), duration: 91}
		state := stateSource(t, "[State 7, duration]\ntype=AfterImageTime\ntrigger1=1\n"+text+"\n")
		if _, err := NewStateMachine().ExecuteState(state, h); err != nil || h.duration != 0 {
			t.Fatal(text, err, h.duration)
		}
	}
}
