package parsers

import (
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/coord"
)

func TestStoryboardLocalCoord(t *testing.T) {
	for _, test := range []struct {
		source string
		want   coord.Space
	}{
		{"[SceneDef]\nspr=intro.sff\n", coord.Default()},
		{"[Info]\nlocalcoord=640,480\n", coord.Space{Width: 640, Height: 480}},
		{"[INFO]\nLOCALCOORD=1280,720 ; HD\n", coord.Space{Width: 1280, Height: 720}},
	} {
		story, err := ParseStoryboard(strings.NewReader(test.source))
		if err != nil || story.LocalCoord != test.want {
			t.Fatalf("%q: story=%+v err=%v", test.source, story, err)
		}
	}
	for _, raw := range []string{"0,240", "320,-1", "320", "320,240,1", "bad,240"} {
		_, err := ParseStoryboard(strings.NewReader("[Info]\nlocalcoord=" + raw + "\n"))
		if err == nil || !strings.Contains(err.Error(), "[Info] localcoord (line 2)") {
			t.Fatalf("invalid dimensions %q: %v", raw, err)
		}
	}
}
