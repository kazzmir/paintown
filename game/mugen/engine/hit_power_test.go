package engine

import (
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/combat"
)

type powerRuleHost struct {
	*parameterHost
	rules combat.PowerRules
}

func (h *powerRuleHost) GetHitPowerRules() combat.PowerRules { return h.rules }

func TestHitPowerDefaultExpressionsRepeat(t *testing.T) {
	h := newParameterHost()
	runController(t, h, "HitDef", "damage=(var(29):=var(29)+1),0")
	// Native order-defaults records damage1, attacker1, receiver2 and five
	// counter increments. The defaults cannot read cached integer damage.
	if h.hitParams["damage"] != "1,0" || !strings.HasPrefix(h.hitParams["getpower"], "1,") || !strings.HasPrefix(h.hitParams["givepower"], "2,") || h.vars[29] != 5 {
		t.Fatal("native repeated power defaults", h.hitParams, h.vars[29])
	}
}

func TestHitPowerRulesRemainHostLocalIncludingZero(t *testing.T) {
	state := stateSource(t, "[State 7, hit]\ntype=HitDef\ntrigger1=1\ndamage=10,3\n")
	sm := NewStateMachine()
	for _, test := range []struct {
		rules     combat.PowerRules
		get, give string
	}{{combat.DefaultPowerRules(), "7", "6"}, {combat.PowerRules{}, "0", "0"}, {combat.PowerRules{Attack: -.7, GetHit: -.6}, "-7", "-6"}} {
		h := &powerRuleHost{parameterHost: newParameterHost(), rules: test.rules}
		if _, err := sm.ExecuteState(state, h); err != nil {
			t.Fatal(err)
		}
		if got := h.hitParams["getpower"]; !strings.HasPrefix(got, test.get+",") {
			t.Fatal("attacker default lost host rules", got, test)
		}
		if got := h.hitParams["givepower"]; !strings.HasPrefix(got, test.give+",") {
			t.Fatal("receiver default lost host rules", got, test)
		}
	}
}
