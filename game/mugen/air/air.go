package air

import (
	"fmt"
	"io"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/logger"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

type Box struct {
	X1 int
	Y1 int
	X2 int
	Y2 int
}

type Element struct {
	Group       int
	Image       int
	XOffset     int
	YOffset     int
	Time        int
	Flags       string
	Clsn1       []Box
	Clsn2       []Box
	Interpolate bool
	FlipX       bool
	FlipY       bool
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

			consumed := make(map[*parsers.Attribute]bool)

			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())

				if key == "clsn1default" {
					consumed[attr] = true
					count := getAsInt(attr.Value)
					defaultClsn1 = make([]Box, count)
					nextClsn1 = nil // Reset frame-specific when default is set
				} else if key == "clsn2default" {
					consumed[attr] = true
					count := getAsInt(attr.Value)
					defaultClsn2 = make([]Box, count)
					nextClsn2 = nil
				} else if key == "clsn1" {
					consumed[attr] = true
					count := getAsInt(attr.Value)
					nextClsn1 = make([]Box, count)
				} else if key == "clsn2" {
					consumed[attr] = true
					count := getAsInt(attr.Value)
					nextClsn2 = make([]Box, count)
				} else if strings.HasPrefix(key, "clsn1[") {
					consumed[attr] = true
					b := parseBox(attr.Value)
					idx := parseIndex(key)
					if idx >= 0 && idx < len(nextClsn1) {
						nextClsn1[idx] = b
					} else if idx >= 0 && idx < len(defaultClsn1) {
						defaultClsn1[idx] = b
					}
				} else if strings.HasPrefix(key, "clsn2[") {
					consumed[attr] = true
					b := parseBox(attr.Value)
					idx := parseIndex(key)
					if idx >= 0 && idx < len(nextClsn2) {
						nextClsn2[idx] = b
					} else if idx >= 0 && idx < len(defaultClsn2) {
						defaultClsn2[idx] = b
					}
				} else if key == "loopstart" {
					consumed[attr] = true
					action.LoopStart = len(action.Elements)
				} else {
					// An animation element row: Group, Image, X, Y, Time, Flags
					if listPointer, ok := attr.Value.(*parsers.ValueList); ok {
						consumed[attr] = true
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

			// Log unresolved
			for _, attr := range section.Attributes {
				if !consumed[attr] {
					logger.LogUnresolved(fmt.Sprintf("[Action %d] %s = %v", actionID, attr.ID.String(), attr.Value))
				}
			}

			data.Actions[actionID] = action
		} else {
			logger.LogUnresolved(fmt.Sprintf("Unhandled AIR Section: [%s]", section.Name))
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
		f := strings.ToUpper(el.Flags)
		el.FlipX = strings.Contains(f, "H")
		el.FlipY = strings.Contains(f, "V")
	}
	if len(list.Values) > 5 {
		el.Interpolate = strings.ToLower(getAsString(list.Values[5])) == "interpolate"
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

	p.TickCounter++
	for {
		el := p.Action.Elements[p.CurrentIdx]
		if el.Time == -1 {
			return // Infinite frame
		}
		if p.TickCounter <= el.Time {
			break
		}

		// Advance frame
		p.TickCounter -= el.Time
		p.CurrentIdx++
		if p.CurrentIdx >= len(p.Action.Elements) {
			if p.Action.LoopStart != -1 {
				p.CurrentIdx = p.Action.LoopStart
			} else {
				p.CurrentIdx = 0
			}
		}
	}
}

func (p *Player) CurrentElement() *Element {
	if p.Action == nil || p.CurrentIdx < 0 || p.CurrentIdx >= len(p.Action.Elements) {
		return nil
	}
	return &p.Action.Elements[p.CurrentIdx]
}

func (p *Player) NextElement() *Element {
	if p.Action == nil || len(p.Action.Elements) == 0 {
		return nil
	}
	nextIdx := p.CurrentIdx + 1
	if nextIdx >= len(p.Action.Elements) {
		if p.Action.LoopStart != -1 {
			nextIdx = p.Action.LoopStart
		} else {
			nextIdx = 0
		}
	}
	return &p.Action.Elements[nextIdx]
}

func (p *Player) GetInterpolationFactor() float64 {
	el := p.CurrentElement()
	if el == nil || !el.Interpolate || el.Time <= 0 {
		return 0
	}
	return float64(p.TickCounter) / float64(el.Time)
}
