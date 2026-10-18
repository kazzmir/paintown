package evaluator

import "testing"

type chainIDFixture struct{ CharacterEnvironment }

func (*chainIDFixture) GetHitVar(string) float64 { return 7 }

func TestGetHitChainIDReturnsInteger(t *testing.T) {
	value := evaluate(t, "GetHitVar(chainid)", &chainIDFixture{})
	if value.Kind() != IntKind || value.Number() != 7 {
		t.Fatal("chainid did not reach the character binding as an integer", value)
	}
}

func TestGetHitVarRequiresBetaSelector(t *testing.T) {
	for _, source := range []string{"GetHitVar(chainid)", "Enemy,GetHitVar(chainid)", "GetHitVar(fall.envshake.phase)"} {
		if _, err := ParseReference(source); err != nil {
			t.Fatal(source, err)
		}
	}
	for _, source := range []string{"GetHitVar(hitid)", "Enemy,GetHitVar(hitid)", "GetHitVar(unknown_hit_selector)", "GetHitVar(Var(0))"} {
		if _, err := ParseReference(source); err == nil {
			t.Fatal("invalid selector accepted", source)
		}
	}
}
