package engine

import (
	"encoding/json"
	"os"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type passHost struct {
	*entryHost
	time, previous int
	sm             *StateMachine
	states         map[int]*cns.StateDef
}

func (h *passHost) GetTime() int { return h.time }
func (h *passHost) LookupIdentifier(name string) (evaluator.Value, bool) {
	if name == "prevstateno" {
		return evaluator.Int(int32(h.previous)), true
	}
	return evaluator.Value{}, false
}
func (h *passHost) Transition(state int, overrides EntryOverrides, _ bool) error {
	h.previous = h.state
	h.time = 0
	h.host.ChangeState(state, -1, -1)
	return h.sm.EnterState(h.states[state], h, overrides)
}

func referencePassHost(t *testing.T, name string) *passHost {
	t.Helper()
	path := "../tests/testdata/reference/" + name
	data, err := os.ReadFile(path + ".cns")
	if err != nil {
		t.Fatal(err)
	}
	file, err := cns.Parse(strings.NewReader(string(data)))
	if err != nil {
		t.Fatal(err)
	}
	if commands, err := os.ReadFile(path + ".cmd"); err == nil {
		cmd, err := cns.Parse(strings.NewReader(string(commands)))
		if err != nil {
			t.Fatal(err)
		}
		for id, state := range cmd.States {
			file.States[id] = state
		}
	}
	h := &passHost{entryHost: newEntryHost(), sm: NewStateMachine(), states: file.States}
	if err := h.Transition(0, EntryOverrides{}, false); err != nil {
		t.Fatal(err)
	}
	return h
}

func compareReferenceVariables(t *testing.T, name string, vars [60]int) {
	t.Helper()
	data, err := os.ReadFile("../tests/testdata/reference/" + name + ".json")
	if err != nil {
		t.Fatal(err)
	}
	var trace struct {
		Captures []struct{ Variables map[string]int }
	}
	if err := json.Unmarshal(data, &trace); err != nil {
		t.Fatal(err)
	}
	for _, capture := range trace.Captures {
		for key, want := range capture.Variables {
			index, err := strconv.Atoi(key)
			if err != nil {
				t.Fatal(err)
			}
			if vars[index] != want {
				t.Errorf("%s var(%d)=%d, reference=%d", name, index, vars[index], want)
			}
		}
	}
}

func TestStatePassReferenceOrder(t *testing.T) {
	h := referencePassHost(t, "state-order")
	if err := h.sm.RunStatePass(h.states, h); err != nil {
		t.Fatal(err)
	}
	compareReferenceVariables(t, "state-order", h.vars)
	if h.state != 31 || h.previous != 30 || h.time != 0 {
		t.Fatal("positive chain did not finish on entry tick")
	}
}

func TestPersistenceReferenceTrace(t *testing.T) {
	h := referencePassHost(t, "persistence")
	for tick := 0; tick < 40; tick++ {
		if err := h.sm.RunStatePass(h.states, h); err != nil {
			t.Fatal(err)
		}
		h.time++
	}
	compareReferenceVariables(t, "persistence", h.vars)
}

func TestStatePassRunawayAndMissingState(t *testing.T) {
	h := &passHost{entryHost: newEntryHost(), sm: NewStateMachine()}
	h.states = map[int]*cns.StateDef{7: stateSource(t, "[State 7, repeat]\ntype=ChangeState\ntrigger1=1\nvalue=7\n")}
	if err := h.sm.RunStatePass(h.states, h); err == nil || !strings.Contains(err.Error(), "transitions") {
		t.Fatal(err)
	}
	h.state = 404
	if err := h.sm.RunStatePass(h.states, h); err == nil || !strings.Contains(err.Error(), "404") {
		t.Fatal(err)
	}
}
