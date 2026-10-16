package character

import (
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestCharacterUsesDeclaredMaximumPower(t *testing.T) {
	for _, source := range []string{"[Data]\npower=1500", "[Data]\npower=0", "[Data]\nlife=1000"} {
		file, err := cns.Parse(strings.NewReader(source))
		if err != nil {
			t.Fatal(err)
		}
		c := NewCharacter(file, nil, nil)
		want := file.Data.Power
		c.SetPower(9999)
		if c.MaxPower != want || c.Power != want {
			t.Fatalf("%s: maximum=%d power=%d want %d", source, c.MaxPower, c.Power, want)
		}
	}
}
