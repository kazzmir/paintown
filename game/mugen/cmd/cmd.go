package cmd

import (
	"fmt"
	"io"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/parsers"
)

type Command struct {
	Name       string
	Sequence   string
	Time       int
	BufferTime int
}

type StateController struct {
	Name     string
	Type     string
	Value    int
	Triggers []string
}

type Data struct {
	Commands         map[string][]Command
	StateControllers []StateController
	Remap            map[string]string // logical button -> physical button; blank disables it
}

// Parse extracts [Command] definitions and [State -1, x] controllers from a M.U.G.E.N .cmd file.
func Parse(r io.Reader, languages ...string) (*Data, error) {
	ast, err := parsers.Parse(r)
	if err != nil {
		return nil, err
	}

	ast = parsers.Localize(ast, languages...)
	data := &Data{
		Commands:         make(map[string][]Command),
		StateControllers: make([]StateController, 0),
	}

	// [Defaults] section values
	defaultTime := 15
	defaultBufferTime := 1
	seenDefaults := map[string]bool{}
	// Defaults apply to every command regardless of section order. Like
	// ordinary definition lookup, the first authored setting wins.
	for _, section := range ast.Sections {
		if !strings.EqualFold(strings.TrimSpace(section.Name), "defaults") {
			continue
		}
		for _, attr := range section.Attributes {
			key := strings.ToLower(attr.ID.String())
			if seenDefaults[key] {
				continue
			}
			seenDefaults[key] = true
			switch key {
			case "command.time":
				defaultTime = max(1, getAsInt(attr.Value))
			case "command.buffer.time":
				defaultBufferTime = max(1, min(30, getAsInt(attr.Value)))
			}
		}
	}

	for _, section := range ast.Sections {
		secName := strings.ToLower(strings.TrimSpace(section.Name))

		if secName == "remap" {
			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())
				if buttonKey(key) == "" {
					continue
				}
				if _, exists := data.Remap[key]; exists {
					continue
				}
				physical := strings.ToLower(getAsString(attr.Value))
				if physical != "" && buttonKey(physical) == "" {
					return nil, fmt.Errorf("[Remap] %s (line %d): invalid button %q", key, attr.Line, physical)
				}
				if data.Remap == nil {
					data.Remap = make(map[string]string)
				}
				data.Remap[key] = physical
			}
		} else if secName == "command" {
			cmd := Command{
				Time:       defaultTime,
				BufferTime: defaultBufferTime,
			}

			seen := map[string]bool{}
			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())
				if seen[key] {
					continue
				}
				seen[key] = true

				if key == "name" {
					// Names are quoted inside strings usually, so we must unwrap StringValue
					if sv, ok := attr.Value.(parsers.StringValue); ok {
						cmd.Name = sv.Val
					} else {
						cmd.Name = getAsString(attr.Value)
					}
				} else if key == "command" {
					// In MUGEN, command blocks are tricky since they use commas, e.g. command = ~D, DF, F, a
					// Therefore the AST parser groups them into a ValueList
					cmd.Sequence = renderCmdSequence(attr.Value)
				} else if key == "time" {
					cmd.Time = getAsInt(attr.Value)
				} else if key == "buffer.time" {
					cmd.BufferTime = getAsInt(attr.Value)
				}
			}

			cmd.Time = max(1, cmd.Time)
			cmd.BufferTime = max(1, min(30, cmd.BufferTime))
			if cmd.Name != "" && cmd.Sequence != "" {
				data.Commands[cmd.Name] = append(data.Commands[cmd.Name], cmd)
			}

		} else if strings.HasPrefix(secName, "state -1") {
			// e.g. [State -1, Stand Light Punch]
			ctrl := StateController{
				Triggers: make([]string, 0),
			}

			// Extract controller name from bracket
			// "state -1, stand light punch"
			parts := strings.SplitN(section.Name, ",", 2)
			if len(parts) > 1 {
				ctrl.Name = strings.TrimSpace(parts[1])
			}

			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())

				if key == "type" {
					ctrl.Type = getAsString(attr.Value)
				} else if key == "value" {
					ctrl.Value = getAsInt(attr.Value)
				} else if strings.HasPrefix(key, "trigger") {
					// trigger1 = command = "x"
					// We just keep the raw statement for evaluation by the game engine later
					// However, the AST has already tokenized the rhs. Let's dump it back to a string form.
					// Actually, trigger values often have complex ops e.g. trigger1 = statetype != A
					// We'll reconstruct the raw string for now
					strVal := attr.RawValue
					if strVal == "" {
						strVal = renderRawValue(attr.Value)
					}
					ctrl.Triggers = append(ctrl.Triggers, fmt.Sprintf("%s = %s", attr.ID.String(), strVal))
				}
			}

			data.StateControllers = append(data.StateControllers, ctrl)
		}
	}

	return data, nil
}

func getAsInt(v parsers.Value) int {
	if nv, ok := v.(parsers.NumberValue); ok {
		return int(nv.Val)
	}
	if kv, ok := v.(parsers.KeywordValue); ok {
		val, _ := strconv.Atoi(strings.TrimSpace(kv.Val))
		return val
	}
	if sv, ok := v.(parsers.StringValue); ok {
		val, _ := strconv.Atoi(strings.TrimSpace(sv.Val))
		return val
	}
	return 0
}

func getAsString(v parsers.Value) string {
	if kv, ok := v.(parsers.KeywordValue); ok {
		return strings.TrimSpace(kv.Val)
	}
	if sv, ok := v.(parsers.StringValue); ok {
		return strings.TrimSpace(sv.Val)
	}
	// Number representation fallback
	if nv, ok := v.(parsers.NumberValue); ok {
		return fmt.Sprintf("%v", nv.Val)
	}
	return ""
}

// renderCmdSequence reconstructs the ~D, DF, F string exactly over the AST values
func renderCmdSequence(v parsers.Value) string {
	if list, ok := v.(*parsers.ValueList); ok {
		var parts []string
		for _, item := range list.Values {
			parts = append(parts, getAsString(item))
		}
		return strings.Join(parts, ", ")
	}
	return getAsString(v)
}

// renderRawValue naively strings together lists or complex objects
func renderRawValue(v parsers.Value) string {
	if list, ok := v.(*parsers.ValueList); ok {
		var parts []string
		for _, item := range list.Values {
			parts = append(parts, renderRawValue(item))
		}
		return strings.Join(parts, ", ")
	}

	if sv, ok := v.(parsers.StringValue); ok {
		return fmt.Sprintf("\"%s\"", sv.Val)
	}

	return getAsString(v)
}
