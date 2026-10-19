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

type fallShakeFixture struct{ CharacterEnvironment }

func (*fallShakeFixture) GetHitVar(string) float64 { return 4.75 }

func TestGetHitFallShakeValueTypes(t *testing.T) {
	for _, field := range []string{"time", "ampl", "freq", "phase"} {
		value := evaluate(t, "GetHitVar(fall.envshake."+field+")", &fallShakeFixture{})
		integer := field == "time" || field == "ampl"
		if integer && (value.Kind() != IntKind || value.Number() != 4) {
			t.Fatal(field, value)
		}
		if !integer && (value.Kind() != FloatKind || value.Number() != 4.75) {
			t.Fatal(field, value)
		}
	}
}

func TestGetHitVarRequiresBetaSelector(t *testing.T) {
	for _, source := range []string{"GetHitVar(chainid)", "Enemy,GetHitVar(chainid)", "GetHitVar(fall.envshake.phase)"} {
		if _, err := ParseReference(source); err != nil {
			t.Fatal(source, err)
		}
	}
	for _, source := range []string{"GetHitVar(hitid)", "Enemy,GetHitVar(hitid)", "GetHitVar(zoff)", "Enemy,GetHitVar(zoff)", "GetHitVar(unknown_hit_selector)", "GetHitVar(Var(0))"} {
		if _, err := ParseReference(source); err == nil {
			t.Fatal("invalid selector accepted", source)
		}
	}
}

func TestGetHitMetadataValueTypes(t *testing.T) {
	for _, field := range []string{"xveladd", "yveladd", "hitcount", "fallcount", "recovertime"} {
		value := evaluate(t, "GetHitVar("+field+")", &fallShakeFixture{})
		integer := field == "hitcount" || field == "fallcount" || field == "recovertime"
		if integer && (value.Kind() != IntKind || value.Number() != 4) {
			t.Fatal(field, value)
		}
		if !integer && (value.Kind() != FloatKind || value.Number() != 4.75) {
			t.Fatal(field, value)
		}
	}
}
