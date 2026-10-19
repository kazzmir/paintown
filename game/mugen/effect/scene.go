// Package effect models fight animation instances shared with the renderer.
package effect

import (
	"fmt"
	"math"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

type Source struct {
	Sprites    *sff.SFF
	Animations *air.Data
	Palette    *palette.State
	Scale      float64 // common game units per sprite pixel
}

type Request struct {
	Source                        Source
	Action, Facing                int
	X, Y                          float64 // common game units
	PauseMoveTime, SuperMoveTime  int
	IgnorePause, IgnoreSuperPause bool
	RemoveTime                    *int // nil retains the one-pass spark policy
	SpritePriority, VFacing       int
	ScreenSpace                   bool
	OnTop                         bool
	ScaleX, ScaleY                float64
	HasScale                      bool
}

type Instance struct {
	Request
	Clock               *air.Player
	Active, OwnerDriven bool
	Age                 int
}

// Queue belongs to the simulation goroutine, as does the scene that drains it.
type queuedRequest struct {
	request  Request
	instance *Instance
}

type Queue struct{ requests []queuedRequest }

func (q *Queue) Push(request Request) error {
	if len(q.requests) >= 16384 {
		return fmt.Errorf("undrained effect queue exceeds 16384 entries")
	}
	q.requests = append(q.requests, queuedRequest{request: request})
	return nil
}

func (q *Queue) PushOwned(instance *Instance) error {
	if instance == nil {
		return fmt.Errorf("nil owned effect")
	}
	if len(q.requests) >= 16384 {
		return fmt.Errorf("undrained effect queue exceeds 16384 entries")
	}
	q.requests = append(q.requests, queuedRequest{instance: instance})
	return nil
}

func (q *Queue) Drain() []Request {
	requests := make([]Request, 0, len(q.requests))
	for _, entry := range q.requests {
		if entry.instance != nil {
			requests = append(requests, entry.instance.Request)
		} else {
			requests = append(requests, entry.request)
		}
	}
	q.requests = nil
	return requests
}

func (instance *Instance) Advance(paused, superPaused bool) error {
	if !instance.Active {
		return nil
	}
	if superPaused {
		if !instance.IgnoreSuperPause && instance.SuperMoveTime <= 0 {
			return nil
		}
		if !instance.IgnoreSuperPause {
			instance.SuperMoveTime--
		}
	} else if paused {
		if !instance.IgnorePause && instance.PauseMoveTime <= 0 {
			return nil
		}
		if !instance.IgnorePause {
			instance.PauseMoveTime--
		}
	}
	instance.Clock.Update()
	if instance.Clock.Error != nil {
		return instance.Clock.Error
	}
	instance.Age++
	lifetime := -2
	if instance.RemoveTime != nil {
		lifetime = *instance.RemoveTime
	}
	if (lifetime >= 0 && instance.Age >= lifetime) || (lifetime == -2 && instance.Clock.AnimTime() == 0) {
		instance.Active = false
	}
	return nil
}

type Scene struct {
	Queue  Queue
	Active []*Instance
	Limit  int
}

func NewScene(limit int) *Scene {
	if limit <= 0 {
		limit = 256
	}
	return &Scene{Limit: limit}
}

// Advance runs before this tick's contacts. Scene-owned cues play one pass by
// default; owned Explod clocks are advanced by the simulation instead. New
// requests retain their first display frame until their next animation update.
func (s *Scene) Advance() error {
	return s.AdvanceWithPause(false, false)
}

func (s *Scene) AdvanceWithPause(paused, superPaused bool) error {
	keep := s.Active[:0]
	for _, instance := range s.Active {
		if !instance.OwnerDriven {
			if err := instance.Advance(paused, superPaused); err != nil {
				return err
			}
		}
		if instance.Active {
			keep = append(keep, instance)
		}
	}
	clear(s.Active[len(keep):])
	s.Active = keep
	return nil
}

func (s *Scene) Flush() error {
	entries := s.Queue.requests
	s.Queue.requests = nil
	for _, entry := range entries {
		request, instance := entry.request, entry.instance
		if instance != nil {
			if !instance.Active {
				continue
			}
			request = instance.Request
		}
		if len(s.Active) >= s.Limit {
			return fmt.Errorf("fight effect limit %d reached", s.Limit)
		}
		if request.Source.Sprites == nil || request.Source.Animations == nil {
			return fmt.Errorf("effect resources unavailable")
		}
		for _, value := range []float64{request.X, request.Y, request.Source.Scale} {
			if math.IsNaN(value) || math.IsInf(value, 0) {
				return fmt.Errorf("nonfinite effect position/scale")
			}
		}
		if request.Source.Scale <= 0 || (request.Facing != 1 && request.Facing != -1) {
			return fmt.Errorf("invalid effect scale/facing")
		}
		action := request.Source.Animations.Actions[request.Action]
		if action == nil || len(action.Elements) == 0 {
			return fmt.Errorf("effect action %d is unavailable", request.Action)
		}
		if instance == nil {
			clock := air.NewPlayer(action)
			if clock.Error != nil {
				return clock.Error
			}
			instance = &Instance{Request: request, Clock: clock, Active: true}
		} else if instance.Clock == nil {
			return fmt.Errorf("owned effect has no animation clock")
		}
		s.Active = append(s.Active, instance)
	}
	return nil
}

func (s *Scene) Clear() {
	for _, instance := range s.Active {
		instance.Active = false
	}
	for _, entry := range s.Queue.requests {
		if entry.instance != nil {
			entry.instance.Active = false
		}
	}
	s.Active = nil
	s.Queue.Drain()
}
