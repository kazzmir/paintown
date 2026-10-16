package engine

import (
	"strings"
	"sync"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

type host struct {
	Environment
	life        int
	vars        [60]int
	paused      bool
	state       int
	changes     int
	persistence map[string]int
}

func newHost() *host                             { return &host{state: 7, persistence: make(map[string]int)} }
func (h *host) GetTime() int                     { return 1 } // Entry scheduling is tested separately.
func (h *host) GetStateNo() int                  { return h.state }
func (h *host) IsPaused() bool                   { return h.paused }
func (h *host) GetVar(i int) int                 { return h.vars[i] }
func (h *host) SetVar(i, v int)                  { h.vars[i] = v }
func (h *host) AddLife(v int)                    { h.life += v }
func (h *host) GetPersistence(key string) int    { return h.persistence[key] }
func (h *host) SetPersistence(key string, v int) { h.persistence[key] = v }
func (h *host) ChangeState(state, ctrl, anim int) {
	h.state = state
	h.changes++
	for key := range h.persistence {
		if strings.HasPrefix(key, "pos") {
			delete(h.persistence, key)
		}
	}
}

func stateSource(t *testing.T, source string) *cns.StateDef {
	t.Helper()
	file, err := cns.Parse(strings.NewReader("[Statedef 7]\n" + source))
	if err != nil {
		t.Fatal(err)
	}
	return file.States[7]
}
func lifeController(triggers string) string {
	return "[State 7, life]\ntype=LifeAdd\nvalue=1\n" + triggers + "\n"
}

func TestNamedStateDiagnostics(t *testing.T) {
	for _, test := range []struct{ name, body string }{
		{"compile", "[State 7, broken]\ntype=Unknown\ntrigger1=1\n"},
		{"runtime", "[State 7, broken]\ntype=LifeAdd\ntrigger1=1\nvalue=1/0\n"},
	} {
		t.Run(test.name, func(t *testing.T) {
			file, err := cns.ParseNamed(strings.NewReader("[Statedef 7]\n"+test.body), "fighter/states.cns")
			if err != nil {
				t.Fatal(err)
			}
			_, err = NewStateMachine().ExecuteState(file.States[7], newHost())
			if err == nil {
				t.Fatal("expected diagnosed failure")
			}
			for _, want := range []string{"fighter/states.cns", "state 7", "broken", "line 2"} {
				if !strings.Contains(err.Error(), want) {
					t.Errorf("missing %q in %v", want, err)
				}
			}
		})
	}
}

func TestTriggerGroups(t *testing.T) {
	cases := []struct {
		name, triggers string
		life, var0     int
	}{
		{"all-and-numbered", "triggerall=1\ntrigger1=1", 1, 0},
		{"all-first", "trigger1=var(0):=1\ntriggerall=0", 0, 0},
		{"repeated-and", "trigger1=1\ntrigger1=0", 0, 0},
		{"groups-or", "trigger1=0\ntrigger2=1", 1, 0},
		{"number-order", "trigger2=var(0):=2\ntrigger1=var(0):=1", 1, 1},
		{"stop-at-gap", "trigger1=0\ntrigger3=1", 0, 0},
		{"bottom-is-false", "trigger1=1/0\ntrigger2=1", 1, 0},
	}
	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			h := newHost()
			sm := NewStateMachine()
			changed, err := sm.ExecuteState(stateSource(t, lifeController(tc.triggers)), h)
			if err != nil {
				t.Fatal(err)
			}
			if changed || h.life != tc.life || h.vars[0] != tc.var0 {
				t.Fatalf("changed=%v life=%d var0=%d", changed, h.life, h.vars[0])
			}
		})
	}
}

func TestPersistenceCooldown(t *testing.T) {
	h := newHost()
	sm := NewStateMachine()
	state := stateSource(t, lifeController("trigger1=var(0)\npersistent=2"))
	for i, value := range []int{1, 0, 1, 0, 1, 1} {
		h.vars[0] = value
		if _, err := sm.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
		want := []int{1, 1, 2, 2, 3, 3}[i]
		if h.life != want {
			t.Fatalf("tick %d: life=%d, want %d", i, h.life, want)
		}
	}
}

func TestOnceAndReentry(t *testing.T) {
	h := newHost()
	sm := NewStateMachine()
	state := stateSource(t, lifeController("trigger1=var(1):=var(1)+1\npersistent=0"))
	for i := 0; i < 3; i++ {
		if _, err := sm.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
	}
	if h.life != 1 || h.vars[1] != 1 {
		t.Fatalf("life=%d, trigger evaluations=%d", h.life, h.vars[1])
	}
	h.ChangeState(7, -1, -1)
	if _, err := sm.ExecuteState(state, h); err != nil {
		t.Fatal(err)
	}
	if h.life != 2 || h.vars[1] != 2 {
		t.Fatal("reentry did not reset one-shot persistence")
	}
}

func TestIgnoreHitPause(t *testing.T) {
	h := newHost()
	h.paused = true
	sm := NewStateMachine()
	state := stateSource(t, lifeController("trigger1=1")+lifeController("trigger1=1\nignorehitpause=1"))
	if _, err := sm.ExecuteState(state, h); err != nil {
		t.Fatal(err)
	}
	if h.life != 1 {
		t.Fatalf("paused life=%d, want 1", h.life)
	}
	h.paused = false
	if _, err := sm.ExecuteState(state, h); err != nil {
		t.Fatal(err)
	}
	if h.life != 3 {
		t.Fatalf("unpaused life=%d, want 3", h.life)
	}
}

func TestTransitionAbortsControllers(t *testing.T) {
	h := newHost()
	sm := NewStateMachine()
	state := stateSource(t, "[State 7, self transition]\ntype=ChangeState\ntrigger1=1\nvalue=7\n"+lifeController("trigger1=1"))
	changed, err := sm.ExecuteState(state, h)
	if err != nil {
		t.Fatal(err)
	}
	if !changed || h.changes != 1 || h.life != 0 {
		t.Fatalf("changed=%v calls=%d life=%d", changed, h.changes, h.life)
	}
}

func TestCompilationFailsBeforeSideEffects(t *testing.T) {
	if err := NewStateMachine().CompileState(cns.NewStateDef(7)); err == nil {
		t.Fatal("accepted a state with no controllers")
	}
	for _, bad := range []string{
		"type=UnknownController\ntrigger1=1", "type=LifeAdd\ntriggerall=1", "type=LifeAdd\ntrigger1=1\ntrigger1=time @ 1",
		"type=LifeAdd\ntrigger0=1", "type=LifeAdd\ntriggerwhat=1", "type=LifeAdd\ntrigger1=1\npersistent=time+1",
		"type=LifeAdd\ntrigger1=1\nignorehitpause=var(0)",
	} {
		t.Run(bad, func(t *testing.T) {
			h := newHost()
			sm := NewStateMachine()
			state := stateSource(t, lifeController("trigger1=1")+"[State 7, bad]\n"+bad+"\nvalue=1\n")
			for i := 0; i < 2; i++ {
				_, err := sm.ExecuteState(state, h)
				if err == nil || !strings.Contains(err.Error(), "state 7 controller 1") || !strings.Contains(err.Error(), "line ") {
					t.Fatalf("lost error/context: %v", err)
				}
				if h.life != 0 {
					t.Fatal("invalid state partially executed")
				}
			}
		})
	}
}

func TestNullAndSharedPrograms(t *testing.T) {
	sm := NewStateMachine()
	state := stateSource(t, "[State 7, explicit null]\ntype=Null\ntrigger1=var(0):=var(0)+1\n"+lifeController("trigger1=1"))
	var wg sync.WaitGroup
	for i := 0; i < 8; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			h := newHost()
			for n := 0; n < 100; n++ {
				if _, err := sm.ExecuteState(state, h); err != nil {
					t.Error(err)
					return
				}
			}
			if h.life != 100 || h.vars[0] != 100 {
				t.Errorf("shared state leaked runtime data: life=%d var0=%d", h.life, h.vars[0])
			}
		}()
	}
	wg.Wait()
}
