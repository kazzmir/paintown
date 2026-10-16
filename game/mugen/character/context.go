package character

import "github.com/kazzmir/paintown/game/mugen/evaluator"

func (c *Character) LookupIdentifier(name string) (evaluator.Value, bool) {
	switch name {
	case "p2life", "p2stateno", "p2statetype", "p2movetype":
		if len(c.Target) == 0 {
			return evaluator.Value{}, true
		}
		other := c.Target[0]
		switch name {
		case "p2life":
			return evaluator.Int(int32(other.Life)), true
		case "p2stateno":
			return evaluator.Int(int32(other.StateNo)), true
		case "p2statetype":
			return evaluator.EvaluateValue(&evaluator.Identifier{Name: "statetype"}, other), true
		default:
			return evaluator.EvaluateValue(&evaluator.Identifier{Name: "movetype"}, other), true
		}
	case "palno":
		return evaluator.Int(int32(c.PaletteNo)), true
	case "ailevel":
		return evaluator.Int(int32(c.AILevel)), true
	case "lifemax":
		return evaluator.Int(int32(c.MaxLife)), true
	case "powermax":
		return evaluator.Int(int32(c.MaxPower)), true
	}
	return evaluator.Value{}, false
}
