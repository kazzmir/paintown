package character

import (
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestRecoveryCommandDoesNotInventTransitions(t *testing.T) {
	for _, enabled := range []bool{false, true} {
		c := fixtureCharacter(t, "type=A\nmovetype=H\nctrl=0\n[State 7, idle]\ntype=Null\ntrigger1=1\n[Statedef 5040]\ntype=A\nmovetype=I\nphysics=N\n[State 5040, recovered]\ntype=Null\ntrigger1=1\n")
		c.HitFall = true
		c.GetHit = cns.ParseHitDef(map[string]string{"fall": "1", "fall.recovertime": "4"})
		c.GetHit.FallRecover = enabled
		commands, err := cmd.Parse(strings.NewReader("[Command]\nname=\"recovery\"\ncommand=/x\ntime=1\nbuffer.time=1\n"))
		if err != nil {
			t.Fatal(err)
		}
		c.CmdFile, c.Commands = commands, NewCommandBuffer(120)
		c.InputSource = func(int, int) []string { return []string{"ButtonX"} }
		for tick := 1; tick <= 4; tick++ {
			c.Update()
			if c.RuntimeError != nil {
				t.Fatal(c.RuntimeError)
			}
			if got, want := c.GetCanRecover(), enabled && tick == 4; got != want {
				t.Fatalf("enabled=%v tick=%d: CanRecover=%v want=%v", enabled, tick, got, want)
			}
			want := 7
			if c.StateNo != want {
				t.Fatalf("enabled=%v tick=%d: state=%d want=%d", enabled, tick, c.StateNo, want)
			}
		}
	}
}
