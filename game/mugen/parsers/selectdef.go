package parsers

import (
	"io"
	"strings"
)

// CharacterEntry represents a single character entry in select.def.
type CharacterEntry struct {
	Name         string // character directory/name, e.g. "kfm"
	Stage        string // stage .def, e.g. "stages/kfm.def" or "random"
	Music        string // optional override music
	IncludeStage bool   // whether stage appears in vs/stage-select
	Order        int    // arcade order priority (1-10)
}

// SelectDef holds parsed data from select.def.
type SelectDef struct {
	Characters  []*CharacterEntry
	ExtraStages []string
	// Options
	ArcadeMaxMatches [10]int
	TeamMaxMatches   [10]int
}

// ParseSelectDef parses a MUGEN select.def file.
func ParseSelectDef(r io.Reader) (*SelectDef, error) {
	ast, err := Parse(r)
	if err != nil {
		return nil, err
	}
	return ParseSelectDefFromAST(ast)
}

// ParseSelectDefFromAST builds a SelectDef from a parsed AST.
func ParseSelectDefFromAST(ast *File) (*SelectDef, error) {
	sd := &SelectDef{}
	// Default includes stage
	for i := range sd.ArcadeMaxMatches {
		sd.ArcadeMaxMatches[i] = 0
	}

	for _, sec := range ast.Sections {
		name := strings.ToLower(strings.TrimSpace(sec.Name))

		switch name {
		case "characters":
			for _, attr := range sec.Attributes {
				// attr.ID = character name (or "randomselect")
				charName := strings.TrimSpace(attr.ID.String())
				if strings.ToLower(charName) == "randomselect" {
					// Placeholder for the random select icon slot
					sd.Characters = append(sd.Characters, &CharacterEntry{
						Name:         "randomselect",
						IncludeStage: false,
					})
					continue
				}

				entry := &CharacterEntry{
					Name:         charName,
					IncludeStage: true,
					Order:        1,
				}

				// Parse the comma-separated values after the character name
				// Value can be: stage, music=..., includestage=0, order=N
				if attr.Value != nil {
					parseCharValues(attr.Value, entry)
				}

				sd.Characters = append(sd.Characters, entry)
			}

		case "extrastages":
			for _, attr := range sec.Attributes {
				stage := strings.TrimSpace(attr.ID.String())
				if stage != "" {
					sd.ExtraStages = append(sd.ExtraStages, stage)
				}
			}

		case "options":
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "arcade.maxmatches":
					parseMaxMatches(attr.Value, &sd.ArcadeMaxMatches)
				case "team.maxmatches":
					parseMaxMatches(attr.Value, &sd.TeamMaxMatches)
				}
			}
		}
	}

	return sd, nil
}

// parseCharValues parses the values following a character name in [Characters].
// The format is: stage, param=value, param=value, ...
// e.g. "stages/kfm.def, music=sound/song.mp3, includestage=0, order=3"
func parseCharValues(v Value, entry *CharacterEntry) {
	var values []Value

	if list, ok := v.(*ValueList); ok {
		values = list.Values
	} else {
		values = []Value{v}
	}

	// First non-key=value item is the stage
	stageSet := false
	for _, val := range values {
		str := ""
		if kv, ok := val.(KeywordValue); ok {
			str = kv.Val
		} else if sv, ok := val.(StringValue); ok {
			str = sv.Val
		} else if nv, ok := val.(NumberValue); ok {
			_ = nv
		}
		str = strings.TrimSpace(str)
		if str == "" {
			continue
		}
		if eqIdx := strings.Index(str, "="); eqIdx >= 0 {
			key := strings.ToLower(strings.TrimSpace(str[:eqIdx]))
			valStr := strings.TrimSpace(str[eqIdx+1:])
			switch key {
			case "music":
				entry.Music = valStr
			case "includestage":
				entry.IncludeStage = valStr != "0"
			case "order":
				n := 0
				for _, c := range valStr {
					if c >= '0' && c <= '9' {
						n = n*10 + int(c-'0')
					}
				}
				entry.Order = n
			}
		} else if !stageSet {
			entry.Stage = str
			stageSet = true
		}
	}
}

// parseMaxMatches parses "6,1,1,0,0,0,0,0,0,0" into an array.
func parseMaxMatches(v Value, out *[10]int) {
	var values []Value
	if list, ok := v.(*ValueList); ok {
		values = list.Values
	} else if v != nil {
		values = []Value{v}
	}
	for i := 0; i < 10 && i < len(values); i++ {
		out[i] = int(getFloat(values[i]))
	}
}
