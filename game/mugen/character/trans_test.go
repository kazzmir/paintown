package character

import (
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

func TestTransparencyExpiresOnActiveFrameAndSurvivesWorldFreeze(t *testing.T) {
	file, err := cns.Parse(strings.NewReader("[Statedef 0]\nphysics=N\n[State 0, half]\ntype=Trans\ntrigger1=Time=0\ntrans=add\nalpha=128,0\n"))
	if err != nil {
		t.Fatal(err)
	}
	c := NewCharacter(file, nil, nil)
	c.Life = 1000
	c.Update()
	want := engine.Transparency{Enabled: true, Mode: "alpha", Source: 128, Destination: 0}
	if c.RuntimeError != nil || c.DrawTransparency != want {
		t.Fatal(c.RuntimeError, c.DrawTransparency)
	}
	c.worldFrozen = true
	c.runStateTick()
	if c.DrawTransparency != want {
		t.Fatal("frozen actor lost its rendered override", c.DrawTransparency)
	}
	c.worldFrozen = false
	c.Update()
	if c.RuntimeError != nil || c.DrawTransparency.Enabled {
		t.Fatal(c.RuntimeError, c.DrawTransparency)
	}
}

func TestTransparencyDefaultStillEvaluatesBothExpressions(t *testing.T) {
	c := NewCharacter(&cns.CNS{}, nil, nil)
	c.Trans(map[string]string{"trans": "add", "alpha": "128,0"})
	want := c.DrawTransparency
	c.Trans(map[string]string{"trans": "default", "alpha": "(Var(0):=1),(Var(0):=Var(0)*10+2)"})
	if c.RuntimeError != nil || c.IntVars[0] != 12 || c.DrawTransparency != want {
		t.Fatal(c.RuntimeError, c.IntVars[0], c.DrawTransparency)
	}
}
