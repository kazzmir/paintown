package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

// StateLoopLimit is the beta's displayed loop limit. Its post-increment > check
// permits 2502 positive executions when the final pass does not transition.
const StateLoopLimit = 2500

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
	for i := 0; i < StateLoopLimit+2; i++ {
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
	err := fmt.Errorf("State machine stuck in loop (stopped after %d loops): %d", StateLoopLimit, env.GetStateNo())
	if state := states[env.GetStateNo()]; state != nil {
		return stateError(state, err)
	}
	return err
}
