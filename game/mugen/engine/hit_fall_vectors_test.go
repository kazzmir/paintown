package engine

import (
	"strconv"
	"strings"
	"testing"
)

type coordinateHitHost struct {
	*parameterHost
	width int
}

func (h *coordinateHitHost) LocalCoordinateWidth() int { return h.width }

func TestFallVectorDefaultsUseEachHostCoordinates(t *testing.T) {
	state := stateSource(t, "[State 7, hit]\ntype=HitDef\ntrigger1=1\n")
	machine := NewStateMachine()
	for _, width := range []int{320, 640, 1280} {
		host := &coordinateHitHost{parameterHost: newParameterHost(), width: width}
		if _, err := machine.ExecuteState(state, host); err != nil {
			t.Fatal(err)
		}
		if _, present := host.hitParams["fall.xvelocity"]; present {
			t.Fatal("omitted X must preserve velocity")
		}
		for key, base := range map[string]float32{"fall.yvelocity": -4.5, "yaccel": 0.35} {
			got, err := strconv.ParseFloat(host.hitParams[key], 32)
			if err != nil || float32(got) != base*float32(width)/320 {
				t.Fatal(width, key, got, err)
			}
		}
	}
	if state.Controllers[0].Params["fall.yvelocity"] != "" || state.Controllers[0].Params["yaccel"] != "" {
		t.Fatal("default values leaked into shared source")
	}
}

func TestFallVectorExplicitExpressionsAreNotDefaultScaled(t *testing.T) {
	state := stateSource(t, "[State 7, hit]\ntype=HitDef\ntrigger1=1\nfall.xvelocity=FVar(0)\nfall.yvelocity=FVar(1)\nyaccel=FVar(2)\n")
	machine := NewStateMachine()
	for _, width := range []int{320, 1280} {
		host := &coordinateHitHost{parameterHost: newParameterHost(), width: width}
		host.floats[0], host.floats[1], host.floats[2] = 2.5, -6.25, -0.5
		if _, err := machine.ExecuteState(state, host); err != nil {
			t.Fatal(err)
		}
		for key, want := range map[string]string{"fall.xvelocity": "2.5", "fall.yvelocity": "-6.25", "yaccel": "-0.5"} {
			if host.hitParams[key] != want {
				t.Fatal("explicit expression was rescaled", width, key, host.hitParams[key])
			}
		}
	}
	if state.Controllers[0].Params["fall.xvelocity"] != "FVar(0)" {
		t.Fatal("explicit source changed")
	}
}

func TestFallVectorValidationPrecedesStateEffects(t *testing.T) {
	for _, key := range []string{"fall.xvelocity", "fall.yvelocity", "yaccel"} {
		for _, raw := range []string{"unknown_fall_vector_trigger", "1,2"} {
			host := newParameterHost()
			state := stateSource(t, "[State 7, before]\ntype=VarSet\ntrigger1=1\nv=0\nvalue=99\n[State 7, hit]\ntype=HitDef\ntrigger1=1\n"+key+"="+raw+"\n")
			_, err := NewStateMachine().ExecuteState(state, host)
			if err == nil || !strings.Contains(err.Error(), key) || host.vars[0] != 0 || host.hitParams != nil {
				t.Fatal("invalid vector compiled or mutated host", key, raw, err, host.vars[0])
			}
		}
	}
}
