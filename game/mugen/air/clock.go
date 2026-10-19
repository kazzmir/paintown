package air

import (
	"fmt"
	"math"
)

func (p *Player) loopStart() int {
	if p.Action.LoopStart >= 0 {
		return p.Action.LoopStart
	}
	return 0
}

// Reset uses one-based CNS element indices. Action data stays immutable.
func (p *Player) Reset(element int) {
	p.CurrentIdx, p.TickCounter, p.Elapsed, p.time = 0, 0, 0, 0
	p.Error = nil
	p.boundary = false
	p.nonfinalInfinite = false
	if p.Action == nil || len(p.Action.Elements) == 0 {
		return
	}
	if p.Action.LoopStart < -1 || p.Action.LoopStart >= len(p.Action.Elements) {
		p.Error = fmt.Errorf("AIR action %d: invalid loop start", p.Action.ID)
		return
	}
	p.starts = make([]int64, len(p.Action.Elements))
	var total, start int64
	for i, el := range p.Action.Elements {
		if el.Time < -1 || int64(el.Time) > math.MaxInt32 {
			p.Error = fmt.Errorf("AIR action %d element %d: invalid duration %d", p.Action.ID, i+1, el.Time)
			return
		}
		p.starts[i] = start
		start += int64(el.Time)
		if el.Time == -1 {
			total = -1
			p.nonfinalInfinite = i != len(p.Action.Elements)-1 || p.nonfinalInfinite
		} else {
			total += int64(el.Time)
		}
	}
	p.total = total
	p.loopTime = -1
	if total >= 0 {
		p.loopTime = total - p.starts[p.loopStart()]
		if p.loopTime <= 0 {
			p.Error = fmt.Errorf("AIR action %d: loop has no positive-duration element", p.Action.ID)
			return
		}
	}
	if element < 1 || element > len(p.starts) {
		p.Error = fmt.Errorf("AIR action %d: invalid/unreachable initial element %d", p.Action.ID, element)
		return
	}
	p.CurrentIdx = element - 1
	p.time = p.starts[p.CurrentIdx]
	p.Elapsed = int(p.time)
	p.skipZero()
	if p.nonfinalInfinite {
		p.selectNonfinalFrame()
	}
}

// The beta accepts a nonfinal -1 despite the AIR manual's restriction. That
// entry resets total duration to -1, while later durations add to it and element
// start queries retain their cumulative offsets. Select by that measured clock;
// the negative-length element has no display interval.
func (p *Player) selectNonfinalFrame() {
	if p.total >= 0 && p.time >= p.total {
		p.CurrentIdx, p.TickCounter, p.boundary = p.loopStart(), 0, true
		p.skipZero()
		return
	}
	for index, element := range p.Action.Elements {
		start := p.starts[index]
		infinite := element.Time == -1 && index == len(p.Action.Elements)-1
		if p.time >= start && (infinite || (element.Time >= 0 && p.time < start+int64(element.Time))) {
			p.CurrentIdx, p.TickCounter = index, int(p.time-start)
			return
		}
	}
	p.Error = fmt.Errorf("AIR action %d: no element at time %d", p.Action.ID, p.time)
}

func (p *Player) skipZero() {
	for visited := 0; visited < len(p.Action.Elements); visited++ {
		if p.Action.Elements[p.CurrentIdx].Time != 0 {
			return
		}
		p.CurrentIdx++
		if p.CurrentIdx == len(p.Action.Elements) {
			p.CurrentIdx = p.loopStart()
			p.boundary = true
		}
	}
	p.Error = fmt.Errorf("AIR action %d: zero-duration cycle", p.Action.ID)
}

func (p *Player) advance() {
	if p.Error != nil || p.CurrentElement() == nil {
		return
	}
	if p.boundary {
		p.time = p.starts[p.loopStart()]
		p.boundary = false
	}
	p.time++
	p.Elapsed++
	if p.nonfinalInfinite {
		p.selectNonfinalFrame()
		return
	}
	p.TickCounter++
	el := p.CurrentElement()
	if el.Time == -1 || p.TickCounter < el.Time {
		return
	}
	p.TickCounter = 0
	p.CurrentIdx++
	if p.CurrentIdx == len(p.Action.Elements) {
		p.CurrentIdx = p.loopStart()
		p.boundary = true
	}
	p.skipZero()
}

// AnimTime preserves the reference's end-boundary tick: the display has wrapped
// to LoopStart while the logical clock still equals the old action's end.
func (p *Player) AnimTime() int {
	if p.CurrentElement() == nil || p.Error != nil {
		return 0
	}
	if p.total < 0 {
		return int(p.time + 1)
	}
	return int(p.time - p.total)
}

func (p *Player) ElemTime(element int) (int, bool) {
	if p.Error != nil || element < 1 || element > len(p.starts) {
		return 0, false
	}
	return int(p.time - p.starts[element-1]), true
}

func (p *Player) ElemNo(offset int) (int, bool) {
	if p.Error != nil || p.CurrentElement() == nil {
		return 0, false
	}
	t := p.time + int64(offset)
	loop := p.loopStart()
	// Once the current display lies in a finite looping portion, historical
	// queries assume that portion extends indefinitely into the past.
	if p.total >= 0 && (t >= p.total || (p.CurrentIdx >= loop && t < p.starts[loop])) {
		start := p.starts[loop]
		t = start + ((t-start)%p.loopTime+p.loopTime)%p.loopTime
	}
	if t < 0 {
		return 0, false
	}
	for i, el := range p.Action.Elements {
		start := p.starts[i]
		if t >= start && ((el.Time == -1 && i == len(p.Action.Elements)-1) || (el.Time >= 0 && t < start+int64(el.Time))) {
			return i + 1, true
		}
	}
	return 0, false
}
