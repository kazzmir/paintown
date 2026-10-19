package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/combat"
)

// These fields are literal masks/enums in the beta, never numeric expressions.
var hitDefLiteralKeys = []string{"attr", "hitflag", "guardflag", "affectteam", "animtype", "air.animtype", "fall.animtype", "ground.type", "air.type"}

func compileHitDefLiterals(ctrl *cns.StateController) (map[string]string, error) {
	result := map[string]string{}
	attr, err := compileHitDefAttr(ctrl.Params["attr"])
	if err != nil {
		return nil, err
	}
	result["attr"] = attr
	flags := func(key, allowed, fallback string) string {
		raw, present := ctrl.Params[key]
		if !present {
			return fallback
		}
		var out strings.Builder
		for _, letter := range strings.ToUpper(raw) {
			if strings.ContainsRune(allowed, letter) && !strings.ContainsRune(out.String(), letter) {
				out.WriteRune(letter)
			}
		}
		return out.String()
	}
	result["hitflag"] = flags("hitflag", "HLMAFD+-", "MAF")
	result["guardflag"] = flags("guardflag", "HLMA", "")
	enum := func(key, fallback string, names map[byte]string) (string, error) {
		raw, present := ctrl.Params[key]
		if !present {
			return fallback, nil
		}
		raw = strings.ToUpper(strings.TrimSpace(raw))
		if len(raw) > 0 {
			if value, ok := names[raw[0]]; ok {
				return value, nil
			}
		}
		return "", fmt.Errorf("%s: invalid literal %q", key, raw)
	}
	animations := map[byte]string{'L': "Light", 'M': "Medium", 'H': "Hard", 'B': "Back", 'U': "Up", 'D': "DiagUp"}
	types := map[byte]string{'H': "High", 'L': "Low", 'T': "Trip", 'N': "None"}
	for _, field := range []struct {
		key, fallback string
		names         map[byte]string
	}{
		{"affectteam", "E", map[byte]string{'E': "E", 'F': "F", 'B': "B"}},
		{"animtype", "Light", animations},
		{"air.animtype", "", animations},
		{"fall.animtype", "", animations},
		{"ground.type", "High", types},
		{"air.type", "", types},
	} {
		fallback := field.fallback
		switch field.key {
		case "air.animtype":
			fallback = result["animtype"]
		case "fall.animtype":
			fallback = "Back"
			if result["air.animtype"] == "Up" || result["air.animtype"] == "DiagUp" {
				fallback = result["air.animtype"]
			}
		case "air.type":
			fallback = result["ground.type"]
		}
		value, err := enum(field.key, fallback, field.names)
		if err != nil {
			return nil, err
		}
		result[field.key] = value
	}
	return result, nil
}

// HitDef stops at the first unusable attack class, retaining earlier classes.
// Protection filters deliberately use the stricter combat parser instead.
func compileHitDefAttr(source string) (string, error) {
	raw := strings.ToUpper(strings.TrimSpace(source))
	if strings.HasPrefix(raw, "\"") || !strings.Contains(raw, ",") {
		return "", nil
	}
	parts := strings.Split(raw, ",")
	posture := strings.TrimSpace(parts[0])
	if _, err := combat.ParseAttributes(posture + ",NA"); err != nil {
		return "", fmt.Errorf("attr: %w", err)
	}
	valid := []string{posture}
	for _, part := range parts[1:] {
		part = strings.TrimSpace(part)
		if len(part) != 2 || !strings.ContainsRune("NSH", rune(part[0])) || !strings.ContainsRune("ATP", rune(part[1])) {
			break
		}
		valid = append(valid, part)
	}
	if len(valid) < 2 {
		return "", nil
	}
	return strings.Join(valid, ","), nil
}
