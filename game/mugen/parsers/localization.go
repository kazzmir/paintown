package parsers

import "strings"

// Localize selects complete definition groups, falling back to undecorated
// English groups. Replacements keep the base group's position and the selected
// attributes' source lines. The original AST is not changed. Standalone AIR
// readers do not call this; localized Files can select a different AIR bank.
func Localize(file *File, languages ...string) *File {
	if file == nil {
		return nil
	}
	language := "en"
	if len(languages) > 0 && strings.TrimSpace(languages[0]) != "" {
		language = strings.ToLower(strings.TrimSpace(languages[0]))
	}
	decorated := func(name string) (code, base string, localized bool) {
		name = strings.TrimSpace(name)
		if len(name) > 3 && name[2] == '.' {
			code = strings.ToLower(name[:2])
			if code[0] >= 'a' && code[0] <= 'z' && code[1] >= 'a' && code[1] <= 'z' {
				return code, strings.TrimSpace(name[3:]), true
			}
		}
		return "", name, false
	}
	// Background definitions can be localized; individual elements cannot.
	var backgrounds []string
	for _, section := range file.Sections {
		_, base, _ := decorated(section.Name)
		name := strings.ToLower(base)
		if strings.HasSuffix(name, "bgdef") {
			backgrounds = append(backgrounds, strings.TrimSuffix(name, "def"))
		}
	}
	exempt := func(name string) bool {
		name = strings.ToLower(name)
		if name == "command" || strings.HasPrefix(name, "state ") || strings.HasPrefix(name, "statedef ") {
			return true
		}
		for _, prefix := range backgrounds {
			if strings.HasPrefix(name, prefix) && !strings.HasSuffix(name, "def") && !strings.HasPrefix(name, prefix+"ctrl") {
				return true
			}
		}
		return false
	}
	groups := map[string][]*Section{}
	for _, section := range file.Sections {
		code, base, localized := decorated(section.Name)
		// CNS keeps quote banks for its existing runtime language selector.
		if localized && code == language && !exempt(base) && !strings.EqualFold(base, "quotes") {
			copy := *section
			copy.Name = base
			key := strings.ToLower(base)
			groups[key] = append(groups[key], &copy)
		}
	}
	baseNames := map[string]bool{}
	for _, section := range file.Sections {
		_, base, localized := decorated(section.Name)
		if !localized {
			baseNames[strings.ToLower(base)] = true
		}
	}
	result := &File{}
	emitted := map[string]bool{}
	for _, section := range file.Sections {
		code, base, localized := decorated(section.Name)
		key := strings.ToLower(base)
		if key == "quotes" {
			result.Sections = append(result.Sections, section)
			continue
		}
		if localized {
			// Localized-only groups stay at their authored location. Groups
			// with a base are emitted there, preserving scene/controller order.
			if code == language && !baseNames[key] && !emitted[key] {
				result.Sections = append(result.Sections, groups[key]...)
				emitted[key] = true
			}
			continue
		}
		if replacement, exists := groups[key]; exists {
			if !emitted[key] {
				result.Sections = append(result.Sections, replacement...)
				emitted[key] = true
			}
		} else {
			result.Sections = append(result.Sections, section)
		}
	}
	return result
}
