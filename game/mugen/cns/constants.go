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
				// The beta accepts S-prefixed numeric [Data] spark defaults but
				// coerces them to zero; only HitDef supports own-bank prefixes.
				trimmed := strings.TrimSpace(part)
				if err != nil && (name == "data.sparkno" || name == "data.guard.sparkno") && len(trimmed) > 1 && (trimmed[0] == 'S' || trimmed[0] == 's') {
					if _, suffixErr := strconv.ParseInt(trimmed[1:], 10, 32); suffixErr == nil {
						n, err = 0, nil
					}
				}
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
	return c.LookupConstantInSpace(name, 320)
}

// Omitted coordinate constants are beta defaults expressed in 320p. Authored
// values already use the character's units; recovery multipliers are unitless.
func (c *CNS) LookupConstantInSpace(name string, width int) (number float64, integer, found bool) {
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
		return float64(float32(c.Movement.YAccel) * float32(width) / 320), false, true
	case "movement.stand.friction":
		return float64(float32(c.Movement.StandFriction)), false, true
	case "movement.crouch.friction":
		return float64(float32(c.Movement.CrouchFriction)), false, true
	}
	if defaults, ok := optionalConstantDefaults[spec.source]; ok && spec.component < len(defaults) {
		number = float64(float32(defaults[spec.component]))
		if spec.source != "velocity.air.gethit.airrecover.mul" {
			number = float64(float32(number) * float32(width) / 320)
		}
		return number, spec.integer, true
	}
	return 0, spec.integer, false
}

// Beta defaults for fields added in 1.0. Pre-1.0 characters legitimately omit
// these while using the current common states for friction, falls and recovery.
// Authored values (including zero) above always take precedence.
var optionalConstantDefaults = map[string][]float64{
	"movement.stand.friction.threshold":                  {2},
	"movement.crouch.friction.threshold":                 {.05},
	"movement.air.gethit.groundlevel":                    {25},
	"movement.air.gethit.groundrecover.ground.threshold": {-20},
	"movement.air.gethit.groundrecover.groundlevel":      {10},
	"movement.air.gethit.airrecover.threshold":           {-1},
	"movement.air.gethit.airrecover.yaccel":              {.35},
	"movement.air.gethit.trip.groundlevel":               {15},
	"movement.down.bounce.offset":                        {0, 20},
	"movement.down.bounce.yaccel":                        {.4},
	"movement.down.bounce.groundlevel":                   {12},
	"movement.down.friction.threshold":                   {.05},
	"velocity.air.gethit.groundrecover":                  {-.15, -3.5},
	"velocity.air.gethit.airrecover.mul":                 {.5, .2},
	"velocity.air.gethit.airrecover.add":                 {0, -4.5},
	"velocity.air.gethit.airrecover.back":                {-1},
	"velocity.air.gethit.airrecover.fwd":                 {0},
	"velocity.air.gethit.airrecover.up":                  {-2},
	"velocity.air.gethit.airrecover.down":                {1.5},
}
