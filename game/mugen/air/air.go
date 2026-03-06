package air

import (
	"io"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/parsers"
)

type Box struct {
	X1 int
	Y1 int
	X2 int
	Y2 int
}

type Element struct {
	Group   int
	Image   int
	XOffset int
	YOffset int
	Time    int
	Flags   string
	Clsn1   []Box
	Clsn2   []Box
}

type Action struct {
	ID        int
	LoopStart int
	Elements  []Element
}

type Data struct {
	Actions map[int]*Action
}

func Parse(r io.Reader) (*Data, error) {
	ast, err := parsers.Parse(r)
	if err != nil {
		return nil, err
	}
	return ParseFromAST(ast)
}

func ParseFromAST(ast *parsers.File) (*Data, error) {
	data := &Data{
		Actions: make(map[int]*Action),
	}

	for _, section := range ast.Sections {
		name := strings.ToLower(strings.TrimSpace(section.Name))
		if strings.HasPrefix(name, "begin action ") {
			idStr := strings.TrimSpace(strings.TrimPrefix(name, "begin action "))
			actionID, err := strconv.Atoi(idStr)
			if err != nil {
				continue // Skip malformed action headers
			}

			action := &Action{
				ID:        actionID,
				LoopStart: -1,
				Elements:  make([]Element, 0),
			}

			var defaultClsn1 []Box
			var defaultClsn2 []Box

			// For active boxes spanning just the next frame
			var nextClsn1 []Box
			var nextClsn2 []Box

			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())

				if key == "clsn1default" {
					if kv, ok := attr.Value.(parsers.KeywordValue); ok {
						count, _ := strconv.Atoi(kv.Val)
						defaultClsn1 = make([]Box, count)
					}
				} else if key == "clsn2default" {
					if kv, ok := attr.Value.(parsers.KeywordValue); ok {
						count, _ := strconv.Atoi(kv.Val)
						defaultClsn2 = make([]Box, count)
					}
				} else if key == "clsn1" {
					if kv, ok := attr.Value.(parsers.KeywordValue); ok {
						count, _ := strconv.Atoi(kv.Val)
						nextClsn1 = make([]Box, count)
					}
				} else if key == "clsn2" {
					if kv, ok := attr.Value.(parsers.KeywordValue); ok {
						count, _ := strconv.Atoi(kv.Val)
						nextClsn2 = make([]Box, count)
					}
				} else if strings.HasPrefix(key, "clsn1[") {
					b := parseBox(attr.Value)
					idx := parseIndex(key)
					if idx >= 0 && idx < len(nextClsn1) {
						nextClsn1[idx] = b
					} else if idx >= 0 && idx < len(defaultClsn1) {
						defaultClsn1[idx] = b
					}
				} else if strings.HasPrefix(key, "clsn2[") {
					b := parseBox(attr.Value)
					idx := parseIndex(key)
					if idx >= 0 && idx < len(nextClsn2) {
						nextClsn2[idx] = b
					} else if idx >= 0 && idx < len(defaultClsn2) {
						defaultClsn2[idx] = b
					}
				} else if key == "loopstart" {
					action.LoopStart = len(action.Elements)
				} else {
					// An animation element row: Group, Image, X, Y, Time, Flags
					if listPointer, ok := attr.Value.(*parsers.ValueList); ok {
						el := parseElement(attr.ID.String(), *listPointer)
						// Apply clsn bounds
						clsn1 := defaultClsn1
						if nextClsn1 != nil {
							clsn1 = nextClsn1
							nextClsn1 = nil
						}
						clsn2 := defaultClsn2
						if nextClsn2 != nil {
							clsn2 = nextClsn2
							nextClsn2 = nil
						}
						el.Clsn1 = append([]Box(nil), clsn1...) // Clone just in case
						el.Clsn2 = append([]Box(nil), clsn2...)

						action.Elements = append(action.Elements, el)
					}
				}
			}

			data.Actions[actionID] = action
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
	return ""
}

func parseBox(val parsers.Value) Box {
	b := Box{}
	if list, ok := val.(*parsers.ValueList); ok && len(list.Values) >= 4 {
		b.X1 = getAsInt(list.Values[0])
		b.Y1 = getAsInt(list.Values[1])
		b.X2 = getAsInt(list.Values[2])
		b.Y2 = getAsInt(list.Values[3])
	}
	return b
}

func parseIndex(key string) int {
	start := strings.Index(key, "[")
	end := strings.Index(key, "]")
	if start != -1 && end != -1 && end > start {
		idx, _ := strconv.Atoi(key[start+1 : end])
		return idx
	}
	return -1
}

func parseElement(groupID string, list parsers.ValueList) Element {
	el := Element{}
	el.Group, _ = strconv.Atoi(strings.TrimSpace(groupID))

	if len(list.Values) > 0 {
		el.Image = getAsInt(list.Values[0])
	}
	if len(list.Values) > 1 {
		el.XOffset = getAsInt(list.Values[1])
	}
	if len(list.Values) > 2 {
		el.YOffset = getAsInt(list.Values[2])
	}
	if len(list.Values) > 3 {
		el.Time = getAsInt(list.Values[3])
	}
	if len(list.Values) > 4 {
		el.Flags = getAsString(list.Values[4])
	}
	return el
}

type Player struct {
	Action      *Action
	CurrentIdx  int
	TickCounter int
}

func NewPlayer(action *Action) *Player {
	return &Player{
		Action: action,
	}
}

func (p *Player) Update() {
	if p.Action == nil || len(p.Action.Elements) == 0 {
		return
	}

	el := p.Action.Elements[p.CurrentIdx]
	if el.Time == -1 {
		return // Infinite frame
	}

	p.TickCounter++
	if p.TickCounter >= el.Time {
		p.TickCounter = 0
		p.CurrentIdx++
		if p.CurrentIdx >= len(p.Action.Elements) {
			p.CurrentIdx = 0 // Loop
		}
	}
}

func (p *Player) CurrentElement() *Element {
	if p.Action == nil || p.CurrentIdx < 0 || p.CurrentIdx >= len(p.Action.Elements) {
		return nil
	}
	return &p.Action.Elements[p.CurrentIdx]
}
