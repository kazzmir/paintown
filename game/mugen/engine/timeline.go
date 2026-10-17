package engine

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
	owner *PauseActor
}

// Timeline is simulation-owned. Requests take effect after the issuing tick;
// nested entity adapters share its frame instead of advancing GameTime again.
type Timeline struct {
	GameTime              int32
	pause, super          pauseClock
	pending, pendingSuper *pauseClock
	depth                 int
}

func (t *Timeline) BeginFrame() { t.depth++ }

func (t *Timeline) EndFrame() {
	t.depth--
	if t.depth != 0 {
		return
	}
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
	clock := &pauseClock{parameters, owner}
	if super {
		t.pendingSuper = clock
	} else {
		t.pending = clock
	}
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
