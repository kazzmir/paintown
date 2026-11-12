package parsers

import (
	"io"
	"strings"
)

// DefData represents the relevant fields from a M.U.G.E.N .def file.
type DefData struct {
	Files     map[string]string
	FileLines map[string]int
	Info      map[string]string
	InfoLines map[string]int
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
