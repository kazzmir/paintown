package character

import "github.com/kazzmir/paintown/game/mugen/evaluator"

func (c *Character) LookupIdentifier(name string) (evaluator.Value, bool) {
	switch name {
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
