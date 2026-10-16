package cns

import (
	"fmt"
	"math"
	"sort"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/parsers"
)

// ConstantNames is the documented, case-insensitive Const trigger surface.
func ConstantNames() []string {
	names := make([]string, 0, len(constantSpecs))
	for name := range constantSpecs {
		names = append(names, name)
	}
	sort.Strings(names)
	return names
}

func ConstantSupported(name string) bool {
	spec, known := constantSpecs[strings.ToLower(name)]
	return known && !spec.unsupported
}

func (c *CNS) readConstantFields(ast *parsers.File) error {
	c.ConstantFields = make(map[string][]float64)
	types := make(map[string]bool)
	for _, spec := range constantSpecs {
		if spec.unsupported {
			continue
		}
		types[spec.source] = spec.integer || spec.fallMultiplier
	}
	for _, section := range ast.Sections {
		group := strings.ToLower(strings.TrimSpace(section.Name))
		if group != "data" && group != "size" && group != "velocity" && group != "movement" {
			continue
		}
		for _, attr := range section.Attributes {
			name := group + "." + strings.ToLower(attr.ID.String())
			integer, known := types[name]
			if !known {
				continue
			}
			parts := parsers.SplitList(attr.RawValue)
			values := make([]float64, len(parts))
			for i, part := range parts {
				n, err := strconv.ParseFloat(strings.TrimSpace(part), 64)
				if err != nil || math.IsNaN(n) || math.IsInf(n, 0) {
					return fmt.Errorf("line %d constant %s: invalid number %q", attr.Line, name, part)
				}
				if integer {
					n = math.Trunc(n)
					if n < math.MinInt32 || n > math.MaxInt32 {
						return fmt.Errorf("line %d constant %s outside int32 range", attr.Line, name)
					}
				} else {
					n = float64(float32(n))
					if math.IsInf(n, 0) {
						return fmt.Errorf("line %d constant %s outside float32 range", attr.Line, name)
					}
				}
				values[i] = n
			}
			c.ConstantFields[name] = values
		}
	}
	return nil
}

// LookupConstant returns a number and its fixed declared type, not the spelling
// of its source literal. Required-field validation for whole characters belongs
// to the loader; state-only CNS files may omit all constant groups.
func (c *CNS) LookupConstant(name string) (number float64, integer, found bool) {
	name = strings.ToLower(name)
	spec, ok := constantSpecs[name]
	if !ok || spec.unsupported || c == nil {
		return 0, false, false
	}
	values := c.ConstantFields[spec.source]
	if spec.component < len(values) {
		number = values[spec.component]
		if spec.fallMultiplier {
			number = float64(float32(100) / float32(number+100))
		}
		return number, spec.integer, true
	}
	// Existing defaults and optional fields exercised by the bundled character.
	// Do not invent values for missing required fields.
	switch name {
	case "data.power":
		if c.Data.Power == 0 {
			return 3000, true, true
		}
		return float64(c.Data.Power), true, true
	case "movement.yaccel":
		return float64(float32(c.Movement.YAccel)), false, true
	case "movement.stand.friction":
		return float64(float32(c.Movement.StandFriction)), false, true
	case "movement.crouch.friction":
		return float64(float32(c.Movement.CrouchFriction)), false, true
	}
	return 0, spec.integer, false
}
