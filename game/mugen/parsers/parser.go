package parsers

import (
	"bufio"
	"fmt"
	"io"
	"strings"
)

// Parse reads the line-oriented DEF/CNS/CMD/AIR container, preserving source
// expressions rather than lexing their operators as filenames or numbers.
func Parse(r io.Reader) (*File, error) {
	file := &File{}
	var section *Section
	scanner := bufio.NewScanner(r)
	scanner.Buffer(make([]byte, 4096), 1024*1024)
	lineNo := 0
	for scanner.Scan() {
		lineNo++
		line := scanner.Text()
		if lineNo == 1 {
			line = strings.TrimPrefix(line, "\ufeff")
		}
		column := len(line) - len(strings.TrimLeft(line, " \t")) + 1
		line = strings.TrimSpace(StripComment(line))
		if line == "" {
			continue
		}
		if strings.Count(line, "\"")%2 != 0 {
			return nil, fmt.Errorf("line %d:%d: unterminated string", lineNo, column)
		}
		if line[0] == '[' {
			end := strings.IndexByte(line, ']')
			if end < 0 || strings.TrimSpace(line[end+1:]) != "" || strings.TrimSpace(line[1:end]) == "" {
				return nil, fmt.Errorf("line %d:%d: malformed section %q", lineNo, column, line)
			}
			section = &Section{Name: strings.TrimSpace(line[1:end]), Line: lineNo, Column: column}
			file.Sections = append(file.Sections, section)
			continue
		}
		if section == nil {
			return nil, fmt.Errorf("line %d:%d: attribute outside section", lineNo, column)
		}
		attr := &Attribute{Line: lineNo, Column: column}
		key, raw, assigned := containerAttribute(line)
		key = strings.Join(strings.Fields(key), "")
		if key == "" {
			return nil, fmt.Errorf("line %d:%d: empty attribute name", lineNo, column)
		}
		attr.ID = &Identifier{Parts: []string{key}}
		if assigned {
			attr.RawValue = strings.TrimSpace(raw)
			attr.Value = containerValue(attr.RawValue)
		}
		section.Attributes = append(section.Attributes, attr)
	}
	if err := scanner.Err(); err != nil {
		return nil, fmt.Errorf("line %d: %w", lineNo+1, err)
	}
	return file, nil
}

func containerAttribute(line string) (key, value string, assigned bool) {
	depth := 0
	quoted := false
	for i, ch := range line {
		if ch == '"' {
			quoted = !quoted
		}
		if quoted {
			continue
		}
		switch ch {
		case '(', '[':
			depth++
		case ')', ']':
			depth--
		case '=', ',':
			// An AIR/select row's first comma precedes option assignments.
			if depth == 0 {
				return line[:i], line[i+1:], true
			}
		case ':':
			name := strings.ToLower(strings.TrimSpace(line[:i]))
			if depth == 0 && (name == "clsn1" || name == "clsn2" || name == "clsn1default" || name == "clsn2default") {
				return line[:i], line[i+1:], true
			}
		}
	}
	return line, "", false
}
