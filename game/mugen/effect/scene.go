// Package effect owns display-independent, one-shot fight animations.
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
	Source         Source
	Action, Facing int
	X, Y           float64 // common game units
}

type Instance struct {
	Request
	Clock *air.Player
}

// Queue belongs to the simulation goroutine, as does the scene that drains it.
type Queue struct{ requests []Request }

func (q *Queue) Push(request Request) error {
	if len(q.requests) >= 16384 {
		return fmt.Errorf("undrained effect queue exceeds 16384 entries")
	}
	q.requests = append(q.requests, request)
	return nil
}

func (q *Queue) Drain() []Request {
	requests := q.requests
	q.requests = nil
	return requests
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

// Advance runs before this tick's contacts. Newly queued effects retain their
// first display frame until the following tick; every animation plays one pass.
func (s *Scene) Advance() error {
	keep := s.Active[:0]
	for _, instance := range s.Active {
		instance.Clock.Update()
		if instance.Clock.Error != nil {
			return instance.Clock.Error
		}
		if instance.Clock.AnimTime() != 0 {
			keep = append(keep, instance)
		}
	}
	clear(s.Active[len(keep):])
	s.Active = keep
	return nil
}

func (s *Scene) Flush() error {
	for _, request := range s.Queue.Drain() {
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
		clock := air.NewPlayer(action)
		if clock.Error != nil {
			return clock.Error
		}
		s.Active = append(s.Active, &Instance{Request: request, Clock: clock})
	}
	return nil
}

func (s *Scene) Clear() {
	s.Active = nil
	s.Queue.Drain()
}
