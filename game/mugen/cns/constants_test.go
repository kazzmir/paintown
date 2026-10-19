package cns

import (
	"strings"
	"testing"
)

func TestTypedConstantFields(t *testing.T) {
	file, err := Parse(strings.NewReader(`[Data]
life=900
power=2000
fall.defence_up=50
[Size]
head.pos=-2,-70
[Velocity]
jump.neu=0,-8.4
[Movement]
air.gethit.airrecover.yaccel=.35
`))
	if err != nil {
		t.Fatal(err)
	}
	for _, tc := range []struct {
		name    string
		number  float64
		integer bool
	}{
		{"data.life", 900, true}, {"data.power", 2000, true},
		{"data.fall.defence_mul", float64(float32(100.0 / 150)), false},
		{"size.head.pos.y", -70, true}, {"velocity.jump.y", float64(float32(-8.4)), false},
		{"movement.air.gethit.airrecover.yaccel", float64(float32(.35)), false},
	} {
		n, integer, found := file.LookupConstant(tc.name)
		if !found || n != tc.number || integer != tc.integer {
			t.Errorf("%s=%g,%v,%v want %g,%v", tc.name, n, integer, found, tc.number, tc.integer)
		}
	}
	if len(ConstantNames()) != 74 {
		t.Fatal("documented constant catalogue changed")
	}
	if ConstantSupported("movement.jump.changeanim.threshold") || ConstantSupported("data.unknown") {
		t.Fatal("reference-rejected name advertised as available")
	}
}

func TestInvalidConstantFields(t *testing.T) {
	for _, source := range []string{"[Data]\nlife=oops", "[Data]\npower=2147483648", "[Size]\nxscale=1e99", "[Velocity]\nrun.fwd=4.6,broken"} {
		if _, err := Parse(strings.NewReader(source)); err == nil || !strings.Contains(err.Error(), "line 2 constant") {
			t.Fatalf("%s: %v", source, err)
		}
	}
}
