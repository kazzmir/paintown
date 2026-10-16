package evaluator

import "testing"

func TestReferenceConstantValidation(t *testing.T) {
	for _, source := range []string{"Const(data.life)", "var(0):=Const(size.height)", "helper(Const(data.life)),Const(data.attack)", "1=[Const(data.life),Const(data.power)]"} {
		if _, err := ParseReference(source); err != nil {
			t.Fatalf("%s: %v", source, err)
		}
	}
	for _, source := range []string{"Const()", "Const(data.life,data.power)", "Const(1+2)", "1+Const(data.unknown)", "Const(movement.jump.changeanim.threshold)"} {
		if _, err := ParseReference(source); err == nil {
			t.Fatal("accepted invalid/unavailable constant", source)
		}
	}
}
