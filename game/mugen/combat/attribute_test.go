package combat

import "testing"

func TestAttributeFilters(t *testing.T) {
	for _, test := range []struct {
		filter, hit string
		match       bool
	}{
		{"SA,AP", "A,HP", true}, {"SA,AP", "C,NP", false}, {"SA,AP", "S,NA", false},
		{"SCA,AA,AT,AP", "C,ST", true}, {"SCA,NA,SA", "S,SA", true}, {"SCA,NA,SA", "S,HA", false},
		{"C,AT", "C,NT", true}, {"C,AT", "C,NA", false},
	} {
		filter, err := ParseAttributes(test.filter)
		if err != nil {
			t.Fatal(err)
		}
		hit, err := ParseAttributes(test.hit)
		if err != nil {
			t.Fatal(err)
		}
		if filter.Matches(hit) != test.match {
			t.Fatal(test)
		}
	}
	for _, source := range []string{"S", "S,XX", "L,NA", "S,NA,", "S,N", "S,NAX"} {
		if _, err := ParseAttributes(source); err == nil {
			t.Fatal("invalid attributes accepted", source)
		}
	}
	if filter, err := ParseAttributes(""); err != nil || !filter.Empty() {
		t.Fatal("blank override could not clear", filter, err)
	}
}
