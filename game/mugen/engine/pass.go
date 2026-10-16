package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

// RunStatePass runs negative states in order, then the active positive-state
// chain. A transition aborts its controller block; later negative levels still
// execute before the positive chain. The caller owns pause/physics/time phases.
// State ownership/helper eligibility remains work for the world adapter.
func (sm *StateMachine) RunStatePass(states map[int]*cns.StateDef, env Environment) error {
	failure := func() error {
		if host, ok := env.(interface{ StateExecutionError() error }); ok {
			return host.StateExecutionError()
		}
		return nil
	}
	if err := failure(); err != nil {
		return err
	}
	for _, level := range []int{-3, -2, -1} {
		if state := states[level]; state != nil {
			if _, err := sm.ExecuteState(state, env); err != nil {
				return err
			}
			if err := failure(); err != nil {
				return err
			}
		}
	}
	return sm.RunPositivePass(states, env)
}

// RunPositivePass continues the current state after an engine transition without
// running negative states a second time in the same tick.
func (sm *StateMachine) RunPositivePass(states map[int]*cns.StateDef, env Environment) error {
	// Retain the existing diagnosed cap until its exact reference limit is
	// measured. This is a runaway guard, not a conformance claim about the cap.
	for i := 0; i < 10; i++ {
		state := states[env.GetStateNo()]
		if state == nil {
			return fmt.Errorf("active state %d is missing", env.GetStateNo())
		}
		changed, err := sm.ExecuteState(state, env)
		if err != nil {
			return err
		}
		if host, ok := env.(interface{ StateExecutionError() error }); ok {
			if err := host.StateExecutionError(); err != nil {
				return err
			}
		}
		if !changed {
			return nil
		}
	}
	return fmt.Errorf("exceeded 10 state transitions in one tick")
}
