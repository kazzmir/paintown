package parsers

import (
	"fmt"
	"io"
	"strings"
)

// DefData represents the relevant fields from a M.U.G.E.N .def file.
type DefData struct {
	Files map[string]string
}

// ParseDef parses a .def file and extracts file mappings.
func ParseDef(r io.Reader) (*DefData, error) {
	file, err := Parse(r)
	if err != nil {
		return nil, err
	}

	data := &DefData{
		Files: make(map[string]string),
	}

	for _, sec := range file.Sections {
		secName := strings.ToLower(strings.TrimSpace(sec.Name))
		if secName == "files" {
			for _, attr := range sec.Attributes {
				key := strings.ToLower(attr.ID.String())
				// MUGEN .def keys are like st, cns, st0, st1, etc.
				val := ""
				if attr.Value != nil {
					switch v := attr.Value.(type) {
					case StringValue:
						val = v.Val
					case KeywordValue:
						val = v.Val
					case *ValueList:
						// Handle comma separated lists in files (rare but possible)
						var sb strings.Builder
						for i, item := range v.Values {
							if i > 0 {
								sb.WriteString(",")
							}
							if sv, ok := item.(StringValue); ok {
								sb.WriteString(sv.Val)
							} else if kv, ok := item.(KeywordValue); ok {
								sb.WriteString(kv.Val)
							}
						}
						val = sb.String()
					default:
						val = fmt.Sprintf("%v", attr.Value)
					}
				}
				data.Files[key] = val
			}
		}
	}

	return data, nil
}
