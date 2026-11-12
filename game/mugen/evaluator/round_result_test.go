package evaluator

import "testing"

func TestWinLoseSuffixVariantsAreKnown(t *testing.T) {
	for _, name := range []string{"Win", "WinKO", "WinTime", "WinPerfect", "Lose", "LoseKO", "LoseTime", "DrawGame"} {
		expr, err := ParseReference(name)
		if err != nil {
			t.Fatal(name, err)
		}
		if err := ValidateKnownTriggers(expr); err != nil {
			t.Fatal(name, err)
		}
	}
	expr, err := ParseReference("WinUnknown")
	if err == nil && ValidateKnownTriggers(expr) == nil {
		t.Fatal("unknown result suffix accepted")
	}
}

