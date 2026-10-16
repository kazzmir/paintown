package character

import "github.com/kazzmir/paintown/game/mugen/engine"

// Compatibility names keep existing callers on the shared, headless executor.
type Environment = engine.Environment
type ControllerFunc = engine.ControllerFunc
type StateMachine = engine.StateMachine

func NewStateMachine() *StateMachine { return engine.NewStateMachine() }
