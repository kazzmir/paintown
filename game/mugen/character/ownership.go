package character

import (
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func (c *Character) currentStateOwner() *Character {
	if c.stateOwner != nil {
		return c.stateOwner
	}
	return c
}

func (c *Character) PreparePositiveState() error {
	if c.pendingMissingEntry {
		c.pendingMissingEntry = false
		// Undefined external states still reset entry persistence/counters,
		// while retaining the actor's current entry attributes and animation.
		c.BeginStateEntry(false, false, false)
	}
	if state := c.pendingStateEntry; state != nil {
		c.pendingStateEntry = nil
		if err := c.SM.EnterState(state, c.stateEnvironment(), engine.EntryOverrides{}); err != nil {
			c.RuntimeError = err
		}
	}
	return c.RuntimeError
}

func (c *Character) MissingStateAllowed(number int) bool {
	return c.allowMissingState && c.StateNo == number
}

// ResolveState keeps negative definitions with the actor, while positive
// definitions follow the current state owner. Native foreign states suppress
// -3 and -1; -2 still runs in the current owner's coordinate units.
func (c *Character) ResolveState(number int) (*cns.StateDef, engine.Environment) {
	owner := c.currentStateOwner()
	if number < 0 {
		if owner != c && (number == -3 || number == -1) {
			return nil, c
		}
		owner = c
	}
	if owner.StateFile == nil {
		return nil, c
	}
	return owner.StateFile.States[number], c.stateEnvironment()
}

// Character storage stays in its own profile. Only CNS evaluation and numeric
// position/velocity boundaries use the active state owner's profile.
type stateContext interface {
	engine.Environment
	engine.EntryEnvironment
}

func (c *Character) stateEnvironment() stateContext {
	owner := c.currentStateOwner()
	if owner == c {
		return c
	}
	return &stateEnvironment{redirectEnvironment: redirectEnvironment{Character: c, width: owner.LocalCoordinateWidth()}}
}

type stateEnvironment struct{ redirectEnvironment }

func (s *stateEnvironment) storageVector(x, y float64) (float64, float64) {
	scale := float32(s.Character.LocalCoordinateWidth()) / float32(s.width)
	return float64(float32(x) * scale), float64(float32(y) * scale)
}
func (s *stateEnvironment) SetPosition(x, y float64) {
	s.Character.SetPosition(s.storageVector(x, y))
}
func (s *stateEnvironment) AddPosition(x, y float64) {
	s.Character.AddPosition(s.storageVector(x, y))
}
func (s *stateEnvironment) SetVelocity(x, y float64) {
	s.Character.SetVelocity(s.storageVector(x, y))
}
func (s *stateEnvironment) AddVelocity(x, y float64) {
	s.Character.AddVelocity(s.storageVector(x, y))
}
func (s *stateEnvironment) SetTargetVelocity(p engine.TargetVelocityParameters) error {
	p.X, p.Y = s.storageVector(p.X, p.Y)
	return s.Character.SetTargetVelocity(p)
}
func (s *stateEnvironment) BindTargets(p engine.TargetBindParameters) error {
	p.X, p.Y = s.storageVector(p.X, p.Y)
	return s.Character.BindTargets(p)
}
func (s *stateEnvironment) Redirect(name string, id evaluator.Value, hasID bool) evaluator.EvaluatorEnvironment {
	destination := s.Character.Redirect(name, id, hasID)
	if proxy, ok := destination.(*redirectEnvironment); ok {
		return &redirectEnvironment{Character: proxy.Character, width: s.width}
	}
	return destination
}
