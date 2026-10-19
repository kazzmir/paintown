package cns

import "testing"

func TestHitRecoveryDefaultZeroAndNegative(t *testing.T) {
	for _, test := range []struct {
		params map[string]string
		want   int
	}{
		{nil, 4},
		{map[string]string{"fall.recovertime": "0"}, 0},
		{map[string]string{"fall.recovertime": "-3"}, 4},
		{map[string]string{"fall.recovertime": "7"}, 7},
	} {
		if hit := ParseHitDef(test.params); hit.FallRecoverTime != test.want {
			t.Fatal(test.params, hit.FallRecoverTime, test.want)
		}
	}
}
