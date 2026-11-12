package evaluator

import "testing"

func TestTupleUsesTriggerCommaGrammar(t *testing.T) {
	for _, source := range []string{"parent,var(2),0", "ifelse(StateType=S,3,4),floor(2.5)", "(enemynear,Pos X),0", "(fvar(0)),0"} {
		values, err := ParseReferenceTuple(source, 2)
		if err != nil {
			t.Fatal(source, err)
		}
		for _, value := range values {
			if err := ValidateKnownTriggers(value); err != nil {
				t.Fatal(source, err)
			}
		}
	}
	for _, source := range []string{"1,2,3", "1,", "1"} {
		if _, err := ParseReferenceTuple(source, 2); err == nil {
			t.Fatal("invalid tuple accepted", source)
		}
	}
	for _, source := range []string{"acing+3", "loor(2)", "unknown_call(2)"} {
		expr, err := ParseReference(source)
		if err != nil {
			t.Fatal(err)
		}
		if err := ValidateKnownTriggers(expr); err == nil {
			t.Fatal("unknown trigger accepted", source)
		}
	}
}
