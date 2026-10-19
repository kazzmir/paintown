package cns

import "testing"

func TestHitTimeOmissionDiffersFromExplicitZero(t *testing.T) {
	// Native air-omitted and down-omitted-contact both record20; their
	// explicit-zero controls record0. Direct adapters keep the same defaults
	// as the compiled production path.
	for _, test := range []struct {
		params    map[string]string
		air, down int
	}{
		{nil, 20, 20},
		{map[string]string{"air.hittime": "0", "down.hittime": "0"}, 0, 0},
		{map[string]string{"air.hittime": "3", "down.hittime": "4"}, 3, 4},
	} {
		hit := ParseHitDef(test.params)
		if hit.AirHitTime != test.air || hit.DownHitTime != test.down {
			t.Fatalf("%v: air=%d down=%d; want %d/%d", test.params, hit.AirHitTime, hit.DownHitTime, test.air, test.down)
		}
	}
}
