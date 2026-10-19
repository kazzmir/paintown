package character

import (
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func (c *Character) allowsHitFlags(hit cns.HitDef) bool {
	flags := strings.ToUpper(hit.HitFlag)
	if flags == "" && !hit.HitFlagSpecified {
		// Programmatic definitions keep the same omission default as CNS.
		flags = "MAF"
	}
	if strings.Contains(flags, "+") && c.MoveType != "H" {
		return false
	}
	if strings.Contains(flags, "-") && c.MoveType == "H" {
		return false
	}
	switch c.StateType {
	case "S":
		return strings.ContainsAny(flags, "HM")
	case "C":
		return strings.ContainsAny(flags, "LM")
	case "A":
		return strings.Contains(flags, "A") && (c.MoveType != "H" || !c.HitFall || strings.Contains(flags, "F"))
	case "L":
		return strings.Contains(flags, "D")
	}
	return false
}
