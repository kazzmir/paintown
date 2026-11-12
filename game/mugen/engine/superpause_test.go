package engine

import (
	"testing"
)

type superPauseHost struct {
	*entryHost
	requests []SuperPauseParameters
}

func (h *superPauseHost) SuperPauseGame(request SuperPauseParameters) error {
	h.requests = append(h.requests, request)
	return nil
}

func TestCompiledSuperPauseDefaultsAndHostIsolation(t *testing.T) {
	sm := NewStateMachine()
	defaults := stateSource(t, "[State 7, super]\ntype=SuperPause\ntrigger1=1\n")
	h := &superPauseHost{entryHost: newEntryHost()}
	if _, err := sm.ExecuteState(defaults, h); err != nil {
		t.Fatal(err)
	}
	got := h.requests[0]
	if got.Time != 30 || got.Animation != 30 || got.SoundGroup != -1 || !got.Darken || !got.Unhittable || !got.PauseBackground || got.TargetDefenceMul != 0 {
		t.Fatal("SuperPause defaults", got)
	}
	source := stateSource(t, "[State 7, super]\ntype=SuperPause\ntrigger1=1\ntime=var(0)+3\nmovetime=1\nanim=S(9000+var(0))\nsound=F6\npos=1.5,-var(0)\npoweradd=-2\ndarken=0\nunhittable=0\np2defmul=2.5\n")
	for _, value := range []int{2, 5} {
		h := &superPauseHost{entryHost: newEntryHost()}
		h.vars[0] = value
		if _, err := sm.ExecuteState(source, h); err != nil {
			t.Fatal(err)
		}
		got := h.requests[0]
		if got.Time != value+3 || got.MoveTime != 1 || got.Animation != 9000+value || !got.OwnAnimation || got.OwnSound || got.SoundGroup != 6 || got.SoundItem != 0 || got.Position != [2]float64{1.5, -float64(value)} || got.PowerAdd != -2 || got.Darken || got.Unhittable || got.TargetDefenceMul != 2.5 {
			t.Fatal("SuperPause typed values", got)
		}
	}
	if source.Controllers[0].Params["anim"] != "S(9000+var(0))" {
		t.Fatal("compilation mutated shared resource source")
	}
}
