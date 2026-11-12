package background

import (
	"strings"

	"github.com/kazzmir/paintown/game/mugen/parsers"
)

func integerList(value parsers.Value) []int {
	if list, ok := value.(*parsers.ValueList); ok {
		result := make([]int, len(list.Values))
		for i, value := range list.Values {
			result[i] = int(GetFloat(value))
		}
		return result
	}
	return []int{int(GetFloat(value))}
}

// Section labels are descriptive; each controller belongs to the preceding
// definition. Stages, motifs and storyboards use the same grouping/clock rules.
func (b *Background) loadControllers(ast *parsers.File, prefix string) {
	var definition *BGCtrlDef
	for _, section := range ast.Sections {
		name := strings.ToLower(section.Name)
		if strings.HasPrefix(name, prefix+"ctrldef") {
			definition = &BGCtrlDef{}
			b.Controllers = append(b.Controllers, definition)
			for _, attribute := range parsers.FirstAttributes(section) {
				switch strings.ToLower(attribute.ID.String()) {
				case "looptime":
					definition.Looptime = int(GetFloat(attribute.Value))
				case "ctrlid", "eventid":
					definition.CtrlIDs = integerList(attribute.Value)
				}
			}
		} else if strings.HasPrefix(name, prefix+"ctrl") {
			if definition == nil {
				definition = &BGCtrlDef{}
				b.Controllers = append(b.Controllers, definition)
			}
			controller := &BGCtrl{Params: make(map[string]parsers.Value)}
			definition.Ctrls = append(definition.Ctrls, controller)
			for _, attribute := range parsers.FirstAttributes(section) {
				switch strings.ToLower(attribute.ID.String()) {
				case "type":
					controller.Type = GetString(attribute.Value)
				case "time":
					values := integerList(attribute.Value)
					if len(values) > 0 {
						controller.Time = [2]int{values[0], values[0]}
					}
					if len(values) > 1 {
						controller.Time[1] = values[1]
					}
					if len(values) > 2 {
						controller.Looptime = values[2]
					}
				case "ctrlid":
					controller.CtrlIDs = integerList(attribute.Value)
				default:
					controller.Params[strings.ToLower(attribute.ID.String())] = attribute.Value
				}
			}
		}
	}
}
