package character

import (
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
	var held []string
	for _, raw := range strings.Split(sequence, ",") {
		symbol := strings.TrimSpace(raw)
		keys := commandKeys(symbol)
		if len(keys) == 0 {
			return nil
		}
		isHold, release := strings.Contains(symbol, "/"), strings.Contains(symbol, "~")
		count := 1
		if release {
			s := strings.TrimLeft(symbol, ">/~$")
			n := 0
			for n < len(s) && s[n] >= '0' && s[n] <= '9' {
				n++
			}
			if n > 0 {
				value, err := strconv.Atoi(s[:n])
				if err != nil {
					return nil
				}
				count = max(1, value)
			}
		}
		if isHold {
			held = append(held, keys...)
		}
		steps = append(steps, CommandInput{Keys: append(append([]string(nil), held...), keys...), Time: count})
		if release || !isHold {
			steps = append(steps, CommandInput{Keys: append([]string(nil), held...), Time: 1})
		}
	}
	return steps
}
