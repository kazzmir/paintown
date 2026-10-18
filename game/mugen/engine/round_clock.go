package engine

// RoundSlowdown separates real GameTime ticks from KO simulation passes. The
// beta's integer debt produces 6/3/2 then fractional cadences as it recovers.
type RoundSlowdown struct {
	duration, remaining, elapsed int
	step, debt                   int
	first                        bool
}

func (s *RoundSlowdown) Start(duration int) {
	*s = RoundSlowdown{}
	if duration > 0 {
		s.duration, s.remaining = duration, duration
		s.step, s.debt, s.first = 500, 500, true
	}
}

func (s *RoundSlowdown) Clear() { *s = RoundSlowdown{} }

func (s *RoundSlowdown) BeginFrame() bool {
	if s.first {
		s.first = false
		return true
	}
	if s.step > 0 {
		if s.debt <= 0 {
			s.debt += s.step
		} else {
			s.debt -= 100
		}
	}
	return s.debt <= 0
}

func (s *RoundSlowdown) Advance() {
	if s.remaining <= 0 {
		return
	}
	for _, change := range []struct{ at, step int }{
		{5, 200}, {s.duration / 2, 100}, {s.duration * 7 / 10, 50}, {s.duration * 9 / 10, 25},
	} {
		if s.elapsed == change.at {
			s.step, s.debt = change.step, change.step
		}
	}
	s.remaining--
	s.elapsed++
	if s.remaining == 0 {
		s.Clear()
	}
}

// Real ticks continue while KO slowdown suppresses the simulation. Pending
// requests, pause duration, palette and shake clocks belong to simulation.
func (t *Timeline) SkipSimulationFrame() { t.GameTime++ }
