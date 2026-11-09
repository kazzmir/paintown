package character

import (
	"slices"
	"strconv"
	"strings"
)

// CommandInput holds one input snapshot for a duration, without allocating a
// separate frame for each tick of an authored charge.
type CommandInput struct {
	Keys []string
	Time int
}

// CommandInputs plans logical CMD input. AI uses the same matcher and CNS as
// humans; long charges take constant plan storage and play one tick at a time.
func CommandInputs(sequence string) []CommandInput {
	var steps []CommandInput
	var held, current []string
	add := func(keys []string, extra ...string) []string {
		out := append([]string(nil), keys...)
		for _, key := range extra {
			if !slices.Contains(out, key) {
				out = append(out, key)
			}
		}
		return out
	}
	remove := func(keys, unwanted []string) []string {
		var out []string
		for _, key := range keys {
			if !slices.Contains(unwanted, key) {
				out = append(out, key)
			}
		}
		return out
	}
	for _, raw := range strings.Split(sequence, ",") {
		symbol := strings.TrimSpace(raw)
		strict := strings.HasPrefix(symbol, ">")
		symbol = strings.TrimPrefix(symbol, ">")
		heldChord := strings.HasPrefix(symbol, "/")
		if heldChord {
			symbol = strings.TrimPrefix(symbol, "/")
		}
		var pressed, released, holding []string
		charge := 1
		for _, rawPart := range strings.Split(symbol, "+") {
			part := strings.TrimSpace(rawPart)
			keys := commandKeys(part)
			if len(keys) == 0 {
				return nil
			}
			switch {
			case heldChord || strings.HasPrefix(part, "/"):
				holding = add(holding, keys...)
			case strings.HasPrefix(part, "~"):
				released = add(released, keys...)
				digits := strings.TrimPrefix(part, "~")
				n := 0
				for n < len(digits) && digits[n] >= '0' && digits[n] <= '9' {
					n++
				}
				if n > 0 {
					value, err := strconv.Atoi(digits[:n])
					if err != nil {
						return nil
					}
					charge = max(charge, value)
				}
			default:
				pressed = add(pressed, keys...)
			}
		}
		// Prepare release components independently; ordinary presses occur only
		// on the completion frame. Strict steps retain unrelated previous inputs.
		base := held
		if strict {
			base = current
		}
		preparation := add(remove(base, pressed), holding...)
		preparation = add(preparation, released...)
		steps = append(steps, CommandInput{Keys: preparation, Time: charge})
		current = add(remove(preparation, released), pressed...)
		held = add(held, holding...)
		steps = append(steps, CommandInput{Keys: current, Time: 1})
	}
	// Release transient keys before a future plan; persistent holds remain live.
	steps = append(steps, CommandInput{Keys: held, Time: 1})
	return steps
}
