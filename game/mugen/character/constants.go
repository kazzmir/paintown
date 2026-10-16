package character

import "github.com/kazzmir/paintown/game/mugen/evaluator"

func (c *Character) LookupConstant(name string) (evaluator.Value, bool) {
	if c.StateFile == nil {
		return evaluator.Value{}, false
	}
	n, integer, ok := c.StateFile.LookupConstant(name)
	if !ok {
		return evaluator.Value{}, false
	}
	if integer {
		return evaluator.Int(int32(n)), true
	}
	return evaluator.Float(float32(n)), true
}
