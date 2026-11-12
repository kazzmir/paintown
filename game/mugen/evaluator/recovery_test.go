package evaluator

import "testing"

type recoveryEnvironment struct {
	*fixtureEnvironment
	ready bool
}

func (e *recoveryEnvironment) GetCanRecover() bool { return e.ready }

func TestCanRecoverUsesReceiverEligibility(t *testing.T) {
	for _, ready := range []bool{false, true} {
		env := &recoveryEnvironment{fixtureEnvironment: &fixtureEnvironment{}, ready: ready}
		value := evaluate(t, "CanRecover", env)
		if value.Kind() != IntKind || value.Bool() != ready {
			t.Fatal("recovery eligibility lost", ready, value)
		}
	}
}
