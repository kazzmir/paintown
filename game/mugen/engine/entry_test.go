package engine

import (
	"testing"
)

type entryHost struct {
	*host
	stateType, moveType, physics                               string
	ctrl                                                       bool
	anim, animChanges, power, priority, juggle, entries, turns int
	x, y                                                       float64
	persist                                                    [3]bool
}

func newEntryHost() *entryHost {
	return &entryHost{host: newHost(), stateType: "A", moveType: "A", physics: "A", ctrl: true, anim: 42, priority: 7, juggle: 4, x: 2, y: -3}
}
func (h *entryHost) GetTime() int                 { return 0 }
func (h *entryHost) GetCtrl() bool                { return h.ctrl }
func (h *entryHost) GetAnim() int                 { return h.anim }
func (h *entryHost) BeginStateEntry(a, b, c bool) { h.entries++; h.persist = [3]bool{a, b, c} }
func (h *entryHost) SetJuggleCost(n int)          { h.juggle = n }
func (h *entryHost) FaceOpponent()                { h.turns++ }
func (h *entryHost) SetStateType(s string)        { h.stateType = s }
func (h *entryHost) SetMoveType(s string)         { h.moveType = s }
func (h *entryHost) SetPhysics(s string)          { h.physics = s }
func (h *entryHost) SetCtrl(v bool)               { h.ctrl = v }
func (h *entryHost) SetSprPriority(n int)         { h.priority = n }
func (h *entryHost) ChangeAnim(n int)             { h.anim = n; h.animChanges++ }
func (h *entryHost) SetVelocity(x, y float64)     { h.x = x; h.y = y }
func (h *entryHost) AddPower(n int)               { h.power += n }

func TestEntryDefaults(t *testing.T) {
	h := newEntryHost()
	sm := NewStateMachine()
	state := stateSource(t, "[State 7, idle]\ntype=Null\ntrigger1=1\n")
	if err := sm.EnterState(state, h, EntryOverrides{}); err != nil {
		t.Fatal(err)
	}
	if h.stateType != "S" || h.moveType != "I" || h.physics != "N" {
		t.Fatalf("defaults = %s/%s/%s", h.stateType, h.moveType, h.physics)
	}
	if !h.ctrl || h.anim != 42 || h.animChanges != 0 || h.priority != 7 || h.x != 2 || h.y != -3 || h.juggle != 4 {
		t.Fatalf("omitted attributes mutated: %+v", h)
	}
	if h.persist != [3]bool{} || h.entries != 1 {
		t.Fatalf("entry reset = %+v", h)
	}
}

func TestEntryExpressionsAfterTransitionParameters(t *testing.T) {
	h := newEntryHost()
	h.vars[0] = 3
	sm := NewStateMachine()
	state := stateSource(t, `type=U
movetype=U
physics=U
anim=200+var(0)
ctrl=0
velset=(var(1):=1)+1.0,(var(1):=var(1)+1)
poweradd=10+var(0)
juggle=5
facep2=1
hitdefpersist=1
movehitpersist=0
hitcountpersist=1
sprpriority=var(0)+2
[State 7, idle]
type=Null
trigger1=1
`)
	if err := sm.EnterState(state, h, EntryOverrides{HasAnim: true, Anim: 999, HasCtrl: true, Ctrl: true}); err != nil {
		t.Fatal(err)
	}
	if h.stateType != "A" || h.moveType != "A" || h.physics != "A" {
		t.Fatal("U did not preserve entry enums")
	}
	if h.anim != 203 || h.animChanges != 2 || h.ctrl || h.power != 13 || h.priority != 5 || h.juggle != 5 || h.turns != 1 || h.x != 2 || h.y != 2 || h.vars[1] != 2 {
		t.Fatalf("entry = %+v", h)
	}
	if h.persist != [3]bool{true, false, true} {
		t.Fatalf("persist flags = %v", h.persist)
	}
	for i := 0; i < 3; i++ {
		if _, err := sm.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
	}
	if h.entries != 1 || h.animChanges != 2 || h.power != 13 {
		t.Fatalf("Time=0 execution reapplied entry: %+v", h)
	}
}

func TestEntryIntegerConversionWarns(t *testing.T) {
	h := &warningEntryHost{entryHost: newEntryHost()}
	state := stateSource(t, "ctrl=1.9\npoweradd=3.9\n[State 7, idle]\ntype=Null\ntrigger1=1\n")
	if err := NewStateMachine().EnterState(state, h, EntryOverrides{}); err != nil {
		t.Fatal(err)
	}
	if !h.ctrl || h.power != 3 || len(h.warnings) != 2 {
		t.Fatalf("ctrl=%v power=%d warnings=%v", h.ctrl, h.power, h.warnings)
	}
}

type warningEntryHost struct {
	*entryHost
	warnings []string
}

func (h *warningEntryHost) EvaluationWarning(s string) { h.warnings = append(h.warnings, s) }

func TestInvalidEntry(t *testing.T) {
	for _, params := range []string{"type=Q", "anim=1@2", "velset=1,2,3", "anim=1/0"} {
		t.Run(params, func(t *testing.T) {
			h := newEntryHost()
			sm := NewStateMachine()
			state := stateSource(t, params+"\n[State 7, idle]\ntype=Null\ntrigger1=1\n")
			if err := sm.EnterState(state, h, EntryOverrides{}); err == nil {
				t.Fatal("accepted invalid entry")
			}
			if h.entries != 0 || h.animChanges != 0 {
				t.Fatal("invalid entry applied attributes")
			}
		})
	}
}
