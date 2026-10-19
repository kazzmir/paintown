package air

import (
	"strings"
	"testing"
)

func TestAIRDrawTypesRetainBlendWeights(t *testing.T) {
	for _, test := range []struct {
		raw, mode    string
		source, dest int
	}{{"A", "alpha", 256, 256}, {"A1", "alpha", 256, 128}, {"AS224D32", "alpha", 224, 32}, {"S", "sub", 256, 256}, {"", "", 0, 0}} {
		data, err := Parse(strings.NewReader("[Begin Action 7]\n0,0,0,0,2,H," + test.raw + "\n"))
		if err != nil {
			t.Fatal(err)
		}
		el := data.Actions[7].Elements[0]
		if el.Blend != test.mode || el.AlphaSrc != test.source || el.AlphaDst != test.dest || !el.FlipX {
			t.Fatal("draw type lost", test.raw, el)
		}
	}
	for _, raw := range []string{"bad", "ASxD2", "AS12"} {
		_, err := Parse(strings.NewReader("[Begin Action 7]\n0,0,0,0,2,," + raw + "\n"))
		if err == nil || !strings.Contains(err.Error(), "action 7 line 2") {
			t.Fatal("invalid draw type lacks context", err)
		}
	}
}
