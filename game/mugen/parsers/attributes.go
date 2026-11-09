package parsers

import "strings"

// FirstAttributes reads ordinary definition fields in source order without
// changing their AST or source locations. Call it only for metadata: command
// entries, state triggers and separate BG/controller instances remain ordered.
func FirstAttributes(sections ...*Section) []*Attribute {
	seen := make(map[string]bool)
	var fields []*Attribute
	for _, section := range sections {
		for _, field := range section.Attributes {
			key := strings.ToLower(field.ID.String())
			if !seen[key] {
				seen[key] = true
				fields = append(fields, field)
			}
		}
	}
	return fields
}
