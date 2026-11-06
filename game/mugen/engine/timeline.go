package engine

import "github.com/kazzmir/paintown/game/mugen/palette"

// PauseActor is an identity independent of input/player IDs. Helpers can share
// input slots while keeping distinct pause movement allowances.
type PauseActor struct{ token byte }

func NewPauseActor() *PauseActor { return new(PauseActor) }

type PauseParameters struct {
	Time, MoveTime, EndCommandBufferTime int
	PauseBackground                      bool
}

type pauseClock struct {
	PauseParameters
	owner              *PauseActor
	darken, unhittable bool
}

// Timeline is simulation-owned. Requests take effect after the issuing tick;
// nested entity adapters share its frame instead of advancing GameTime again.
type Timeline struct {
	EnvironmentColor                         EnvironmentColorParameters
	AllPalette, BackgroundPalette            *palette.State
	GlobalNoShadow                           bool // Reset every game tick, including paused frames.
	NoBackground, NoForeground, NoBarDisplay bool
	NoKO                                     bool
	GameTime                                 int32
	roundEffectOrigin                        int32
	TicksPerSecond                           int32 // Zero uses the standalone default of 60.
	pause, super                             pauseClock
	pending, pendingSuper                    *pauseClock
	depth                                    int
	shake                                    ShakeParameters
	pendingShake                             *ShakeParameters
}

// RoundEffectTime includes fight-call and paused game passes. Match's internal
// phase splits do not all reset the native spacing clock.
func (t *Timeline) RoundEffectTime() int32 { return t.GameTime - t.roundEffectOrigin + 1 }

func (t *Timeline) ResetRoundEffectClock() { t.roundEffectOrigin = t.GameTime }

func (t *Timeline) BeginFrame() {
	if t.depth == 0 {
		t.GlobalNoShadow = false
		t.NoBackground, t.NoForeground, t.NoBarDisplay = false, false, false
		t.NoKO = false
		t.AdvanceGlobalPalette()
		t.AdvanceEnvironmentColor()
	}
	t.depth++
}

// Global palette clocks keep running during CNS Pause and SuperPause.
func (t *Timeline) AdvanceGlobalPalette() {
	if t.AllPalette != nil {
		t.AllPalette.AdvanceHitEffect()
	}
	if t.BackgroundPalette != nil {
		t.BackgroundPalette.AdvanceHitEffect()
	}
}

func (t *Timeline) SetGlobalPalette(p PaletteFXParameters) {
	state := &t.AllPalette
	if p.Scope == PaletteFXBackground {
		state = &t.BackgroundPalette
	}
	if *state == nil {
		*state = palette.New(nil)
	}
	(*state).SetEffect(palette.Effect{Time: p.Time, Color: p.Color, Period: p.Period, Add: p.Add, Mul: p.Mul, SinAdd: p.SinAdd, Invert: p.Invert})
}

func (t *Timeline) EndFrame() {
	t.depth--
	if t.depth != 0 {
		return
	}
	t.advanceShake()
	clock := &t.pause
	if t.super.Time > 0 {
		clock = &t.super
	}
	if clock.Time > 0 {
		clock.Time--
		if clock.MoveTime > 0 {
			clock.MoveTime--
		}
	}
	if t.pending != nil {
		t.pause, t.pending = *t.pending, nil
	}
	if t.pendingSuper != nil {
		t.super, t.pendingSuper = *t.pendingSuper, nil
	}
	t.GameTime++
}

func (t *Timeline) RequestPause(owner *PauseActor, parameters PauseParameters, super bool) {
	parameters.Time = max(0, parameters.Time)
	if parameters.MoveTime < 0 || parameters.MoveTime > parameters.Time {
		parameters.MoveTime = 0
	}
	parameters.EndCommandBufferTime = max(0, min(parameters.Time, parameters.EndCommandBufferTime))
	clock := &pauseClock{PauseParameters: parameters, owner: owner}
	if super {
		t.pendingSuper = clock
	} else {
		t.pending = clock
	}
}

func (t *Timeline) RequestSuperPause(owner *PauseActor, parameters SuperPauseParameters) {
	t.RequestPause(owner, parameters.PauseParameters, true)
	t.pendingSuper.darken, t.pendingSuper.unhittable = parameters.Darken, parameters.Unhittable
}

func (t *Timeline) SuperPaused() bool { return t.super.Time > 0 }

func (t *Timeline) Darkened() bool { return t.SuperPaused() && t.super.darken }

func (t *Timeline) SceneTint() [3]float32 {
	if t.Darkened() {
		return [3]float32{.3125, .3125, .5}
	}
	return [3]float32{1, 1, 1}
}

func (t *Timeline) ActorTint(actor *PauseActor) [3]float32 {
	if t.Darkened() && t.super.owner != actor {
		return [3]float32{.5, .5, .75}
	}
	return [3]float32{1, 1, 1}
}

func (t *Timeline) SuperUnhittable(actor *PauseActor) bool {
	return t.SuperPaused() && t.super.owner == actor && t.super.unhittable
}

func (t *Timeline) activePause() *pauseClock {
	if t.super.Time > 0 {
		return &t.super
	}
	return &t.pause
}

func (t *Timeline) Paused() bool { return t.activePause().Time > 0 }

func (t *Timeline) Frozen(actor *PauseActor) bool {
	clock := t.activePause()
	return clock.Time > 0 && (clock.owner != actor || clock.MoveTime == 0)
}

func (t *Timeline) BackgroundFrozen() bool {
	clock := t.activePause()
	return clock.Time > 0 && clock.PauseBackground
}

func (t *Timeline) BufferCommands(actor *PauseActor) bool {
	clock := t.activePause()
	return t.Frozen(actor) && clock.Time <= clock.EndCommandBufferTime
}

func (t *Timeline) ClearPauses() {
	t.pause, t.super = pauseClock{}, pauseClock{}
	t.pending, t.pendingSuper = nil, nil
}
