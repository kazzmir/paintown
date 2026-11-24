package parsers

import (
	"io"
	"strings"
)

// DefData represents the relevant fields from a M.U.G.E.N .def file.
type DefData struct {
	Files         map[string]string
	FileLines     map[string]int
	Info          map[string]string
	InfoLines     map[string]int
	PaletteKeymap PaletteKeymap
}

// PaletteKeymap follows selection-button order a,b,c,x,y,z, then Start plus
// those buttons. A zero value represents the legacy identity mapping, allowing
// synthetic portraits to retain the same defaults as a DEF without the group.
type PaletteKeymap [12]int

func (m PaletteKeymap) Palette(button int) int {
	if button < 1 || button > len(m) {
		return 1 // Enter-only selection is a port convenience, not an attack key.
	}
	if palette := m[button-1]; palette != 0 {
		return palette
	}
	return button
}

// ParseDef parses a .def file and extracts file mappings.
func ParseDef(r io.Reader, languages ...string) (*DefData, error) {
	file, err := Parse(r)
	if err != nil {
		return nil, err
	}
	return ParseDefFromAST(Localize(file, languages...))
}

// ParseDefFromAST extracts mappings without reparsing an embedded definition.
func ParseDefFromAST(file *File) (*DefData, error) {
	data := &DefData{
		Files:     make(map[string]string),
		FileLines: make(map[string]int),
		Info:      make(map[string]string),
		InfoLines: make(map[string]int),
	}
	var keymapGroups []*Section
	for _, section := range file.Sections {
		if strings.EqualFold(strings.TrimSpace(section.Name), "palette keymap") {
			keymapGroups = append(keymapGroups, section)
		}
	}
	if len(keymapGroups) > 0 {
		// A present group initializes every key to 1; it does not inherit
		// the absent group's identity mapping. Native then clamps atoi values.
		for index := range data.PaletteKeymap {
			data.PaletteKeymap[index] = 1
		}
		keys := [...]string{"a", "b", "c", "x", "y", "z", "a2", "b2", "c2", "x2", "y2", "z2"}
		for _, attribute := range FirstAttributes(keymapGroups...) {
			for index, key := range keys {
				if strings.EqualFold(attribute.ID.String(), key) {
					data.PaletteKeymap[index] = selectionPaletteNumber(attribute.RawValue)
					break
				}
			}
		}
	}

	for _, sec := range file.Sections {
		secName := strings.ToLower(strings.TrimSpace(sec.Name))
		if secName == "info" {
			for _, attr := range sec.Attributes {
				key := strings.ToLower(attr.ID.String())
				if _, exists := data.Info[key]; exists {
					continue
				}
				text := strings.TrimSpace(attr.RawValue)
				if value, ok := attr.Value.(StringValue); ok {
					text = value.Val
				}
				data.Info[key] = text
				data.InfoLines[key] = attr.Line
			}
		}
		if secName == "files" {
			for _, attr := range sec.Attributes {
				key := strings.ToLower(attr.ID.String())
				// Native DEF lookup keeps the first value, including an
				// explicit blank, and diagnostics belong to that source line.
				if _, exists := data.Files[key]; exists {
					continue
				}
				val := strings.TrimSpace(attr.RawValue)
				if quoted, ok := attr.Value.(StringValue); ok {
					val = quoted.Val
				}
				data.Files[key] = val
				data.FileLines[key] = attr.Line
			}
		}
	}

	return data, nil
}

// Only the clamped 1..12 result of native integer-prefix conversion is needed;
// saturating early avoids overflow without rejecting otherwise loadable DEFs.
func selectionPaletteNumber(raw string) int {
	raw = strings.TrimSpace(raw)
	if strings.HasPrefix(raw, "-") {
		return 1
	}
	raw = strings.TrimPrefix(raw, "+")
	number := 0
	for _, char := range raw {
		if char < '0' || char > '9' {
			break
		}
		number = min(12, number*10+int(char-'0'))
	}
	return max(1, number)
}
