// Package combat contains display-independent combat value types.
package combat

import (
	"fmt"
	"strings"
)

type Attributes struct {
	States  uint8
	Attacks uint16
}

func ParseAttributes(source string) (Attributes, error) {
	return parseAttributes(source, false)
}

// Protection controllers also accept a posture-only or attack-only mask.
// An absent component stays empty; NotHitBy complements both components.
func ParseProtectionAttributes(source string) (Attributes, error) {
	return parseAttributes(source, true)
}

func parseAttributes(source string, partial bool) (Attributes, error) {
	var result Attributes
	if strings.TrimSpace(source) == "" {
		return result, nil
	}
	parts := strings.Split(strings.ToUpper(source), ",")
	if len(parts) < 2 && !partial {
		return result, fmt.Errorf("hit attributes require state types and attack classes")
	}
	for _, letter := range strings.TrimSpace(parts[0]) {
		switch letter {
		case 'S':
			result.States |= 1
		case 'C':
			result.States |= 2
		case 'A':
			result.States |= 4
		default:
			return Attributes{}, fmt.Errorf("invalid hit state type %q", letter)
		}
	}
	for _, part := range parts[1:] {
		part = strings.TrimSpace(part)
		if len(part) != 2 {
			return Attributes{}, fmt.Errorf("invalid hit attack class %q", part)
		}
		for level, letter := range "NSH" {
			if part[0] != 'A' && part[0] != byte(letter) {
				continue
			}
			kind := strings.IndexByte("ATP", part[1])
			if kind < 0 {
				return Attributes{}, fmt.Errorf("invalid hit attack class %q", part)
			}
			result.Attacks |= 1 << uint(level*3+kind)
		}
		if !strings.ContainsRune("NSHA", rune(part[0])) {
			return Attributes{}, fmt.Errorf("invalid hit attack class %q", part)
		}
	}
	if !partial && (result.States == 0 || result.Attacks == 0) {
		return Attributes{}, fmt.Errorf("empty hit attribute component")
	}
	return result, nil
}

func (a Attributes) Matches(hit Attributes) bool {
	return a.States&hit.States != 0 && a.Attacks&hit.Attacks != 0
}
func (a Attributes) Empty() bool { return a.States == 0 || a.Attacks == 0 }

func (a Attributes) Complement() Attributes {
	return Attributes{States: ^a.States & 7, Attacks: ^a.Attacks & 511}
}

type HitFilter struct {
	Attributes Attributes
	Time       int
}

type HitOverride struct {
	Attributes          Attributes
	StateNo, Time, Slot int
	ForceAir            bool
}
