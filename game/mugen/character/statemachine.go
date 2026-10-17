package character

import "github.com/kazzmir/paintown/game/mugen/engine"

// Compatibility names keep existing callers on the shared, headless executor.
type Environment = engine.Environment
type ControllerFunc = engine.ControllerFunc
type StateMachine = engine.StateMachine

func NewStateMachine() *StateMachine { return engine.NewStateMachine() }

func (c *Character) StateExecutionError() error { return c.RuntimeError }

func (c *Character) StateExecutionStopped() bool { return c.destroyed }
func (c *Character) EvaluationFailure(err error) { c.RuntimeError = err }
