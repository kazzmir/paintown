package air

import (
	"fmt"
	"io"
	"math"
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
	Group                 int
	Image                 int
	XOffset               int
	YOffset               int
	Time                  int
	Flags                 string
	Clsn1                 []Box
	Clsn2                 []Box
	Interpolate           bool // legacy inline offset interpolation
	Interpolation         uint8
	HasScale              bool
	ScaleX, ScaleY, Angle float64
	FlipX                 bool
	FlipY                 bool
	Blend                 string
	AlphaSrc, AlphaDst    int
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
	// Count declarations and per-element copies before allocating. A corrupt
	// count must produce a load error, never a negative make length or an OOM.
	const maxBoxes = 1 << 20
	allocatedBoxes := 0
	reserveBoxes := func(count int) error {
		if count < 0 || count > maxBoxes-allocatedBoxes {
			return fmt.Errorf("collision-box allocation exceeds %d boxes per AIR file", maxBoxes)
		}
		allocatedBoxes += count
		return nil
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
			var wrapInterpolation uint8

			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())

				// The INI parser groups ":" into identifiers instead of treating it as an assignment.
				// For AIR files which use "Clsn1: 1", the ID becomes "clsn1:1" and Value is nil.
				// We manually split this here to properly process it.
				if strings.Contains(key, ":") && attr.Value == nil {
					parts := strings.SplitN(key, ":", 2)
					key = strings.TrimSpace(parts[0])
					attr.Value = parsers.StringValue{Val: strings.TrimSpace(parts[1])}
				}

				count := 0
				if key == "clsn1default" || key == "clsn2default" || key == "clsn1" || key == "clsn2" {
					rawCount := strings.TrimSpace(attr.RawValue)
					if rawCount == "" {
						rawCount = getAsString(attr.Value)
					}
					value, err := strconv.ParseFloat(rawCount, 64)
					if number, ok := attr.Value.(parsers.NumberValue); ok {
						value, err = number.Val, nil
					}
					if err != nil || math.IsNaN(value) || math.IsInf(value, 0) || value < 0 || value > maxBoxes {
						return nil, fmt.Errorf("AIR action %d line %d: invalid %s count %q", actionID, attr.Line, key, attr.RawValue)
					}
					count = int(value)
					if err := reserveBoxes(count); err != nil {
						return nil, fmt.Errorf("AIR action %d line %d: %w", actionID, attr.Line, err)
					}
				}

				if key == "clsn1default" {
					consumed[attr] = true
					defaultClsn1 = make([]Box, count)
					nextClsn1 = nil // Reset frame-specific when default is set
				} else if key == "clsn2default" {
					consumed[attr] = true
					defaultClsn2 = make([]Box, count)
					nextClsn2 = nil
				} else if key == "clsn1" {
					consumed[attr] = true
					nextClsn1 = make([]Box, count)
				} else if key == "clsn2" {
					consumed[attr] = true
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
				} else if kind, ok := interpolationKind(key); ok {
					consumed[attr] = true
					if len(action.Elements) == 0 {
						wrapInterpolation |= kind
					} else {
						action.Elements[len(action.Elements)-1].Interpolation |= kind
					}
				} else if key == "loopstart" {
					consumed[attr] = true
					action.LoopStart = len(action.Elements)
				} else {
					// An animation element row: Group, Image, X, Y, Time, Flags
					if listPointer, ok := attr.Value.(*parsers.ValueList); ok {
						consumed[attr] = true
						el, err := parseElement(attr.ID.String(), *listPointer)
						if err != nil {
							return nil, fmt.Errorf("AIR action %d line %d: %w", actionID, attr.Line, err)
						}
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
						if err := reserveBoxes(len(clsn1) + len(clsn2)); err != nil {
							return nil, fmt.Errorf("AIR action %d line %d: %w", actionID, attr.Line, err)
						}
						el.Clsn1 = append([]Box(nil), clsn1...) // Clone just in case
						el.Clsn2 = append([]Box(nil), clsn2...)

						action.Elements = append(action.Elements, el)
					}
				}
			}

			if len(action.Elements) > 0 {
				action.Elements[len(action.Elements)-1].Interpolation |= wrapInterpolation
			}

			// Log unresolved
			for _, attr := range section.Attributes {
				if !consumed[attr] {
					logger.LogUnresolved(fmt.Sprintf("[Action %d] %s = %v", actionID, attr.ID.String(), attr.Value))
				}
			}

			// The beta resolves duplicate action numbers to their first
			// definition. Still validate later blocks and charge their boxes
			// against the allocation limit before discarding them.
			if _, exists := data.Actions[actionID]; !exists {
				data.Actions[actionID] = action
			}
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

func parseElement(groupID string, list parsers.ValueList) (Element, error) {
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
		draw := strings.ToUpper(getAsString(list.Values[5]))
		el.Interpolate = draw == "INTERPOLATE"
		switch draw {
		case "A", "AI":
			el.Blend, el.AlphaSrc, el.AlphaDst = "alpha", 256, 256
		case "A1":
			el.Blend, el.AlphaSrc, el.AlphaDst = "alpha", 256, 128
		case "S", "SUB":
			el.Blend, el.AlphaSrc, el.AlphaDst = "sub", 256, 256
		default:
			if strings.HasPrefix(draw, "AS") {
				parts := strings.SplitN(draw[2:], "D", 2)
				if len(parts) == 2 {
					source, sourceErr := strconv.Atoi(parts[0])
					dest, destErr := strconv.Atoi(parts[1])
					if sourceErr == nil && destErr == nil {
						el.Blend, el.AlphaSrc, el.AlphaDst = "alpha", source, dest
					}
				}
			}
			if draw != "" && draw != "INTERPOLATE" && el.Blend == "" {
				return el, fmt.Errorf("invalid draw type %q", draw)
			}
		}
	}
	if len(list.Values) > 6 {
		el.HasScale = true
		el.ScaleX = 1
		el.ScaleY = 1
		if list.Values[6] != nil {
			el.ScaleX = getAsFloat(list.Values[6])
		}
	}
	if len(list.Values) > 7 && list.Values[7] != nil {
		el.ScaleY = getAsFloat(list.Values[7])
	}
	if len(list.Values) > 8 {
		el.Angle = getAsFloat(list.Values[8])
	}
	return el, nil
}

type Player struct {
	Action           *Action
	CurrentIdx       int
	TickCounter      int
	Elapsed          int
	Error            error
	time             int64
	starts           []int64
	total, loopTime  int64
	boundary         bool
	nonfinalInfinite bool
}

func NewPlayer(action *Action) *Player {
	p := &Player{Action: action}
	p.Reset(1)
	return p
}

func (p *Player) Update() {
	p.advance()
}

func (p *Player) CurrentElement() *Element {
	if p.Error != nil || p.Action == nil || p.CurrentIdx < 0 || p.CurrentIdx >= len(p.Action.Elements) {
		return nil
	}
	return &p.Action.Elements[p.CurrentIdx]
}

func (p *Player) NextElement() *Element {
	if p.Error != nil || p.Action == nil || len(p.Action.Elements) == 0 {
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
	if nextIdx < 0 || nextIdx >= len(p.Action.Elements) {
		return nil
	}
	return &p.Action.Elements[nextIdx]
}

func (p *Player) GetInterpolationFactor() float64 {
	el := p.CurrentElement()
	if el == nil || (!el.Interpolate && el.Interpolation == 0) || el.Time <= 0 {
		return 0
	}
	return float64(p.TickCounter) / float64(el.Time)
}

const (
	interpolateOffset uint8 = 1 << iota
	interpolateBlend
	interpolateScale
	interpolateAngle
)

func interpolationKind(key string) (uint8, bool) {
	key = strings.ReplaceAll(strings.ToLower(key), " ", "")
	switch key {
	case "interpolateoffset":
		return interpolateOffset, true
	case "interpolateblend":
		return interpolateBlend, true
	case "interpolatescale":
		return interpolateScale, true
	case "interpolateangle":
		return interpolateAngle, true
	}
	return 0, false
}
func getAsFloat(v parsers.Value) float64 {
	if n, ok := v.(parsers.NumberValue); ok {
		return n.Val
	}
	n, _ := strconv.ParseFloat(getAsString(v), 64)
	return n
}

// Visual samples draw-only properties. Collision boxes and animation/state
// clocks remain discrete; all consumers share the same AIR interpolation.
type Visual struct {
	Element                     *Element
	X, Y, ScaleX, ScaleY, Angle float64
	AlphaSrc, AlphaDst          int
}

func (a *Action) Sample(index, tick int) Visual {
	v := Visual{ScaleX: 1, ScaleY: 1}
	if a == nil || index < 0 || index >= len(a.Elements) {
		return v
	}
	el := &a.Elements[index]
	v.Element = el
	v.X = float64(el.XOffset)
	v.Y = float64(el.YOffset)
	if el.HasScale {
		v.ScaleX = el.ScaleX
		v.ScaleY = el.ScaleY
	}
	v.Angle = el.Angle
	v.AlphaSrc = el.AlphaSrc
	v.AlphaDst = el.AlphaDst
	flags := el.Interpolation
	if el.Interpolate {
		flags |= interpolateOffset
	}
	if flags == 0 || el.Time <= 0 {
		return v
	}
	nextIndex := index + 1
	if nextIndex >= len(a.Elements) {
		nextIndex = a.LoopStart
		if nextIndex < 0 {
			nextIndex = 0
		}
	}
	if nextIndex < 0 || nextIndex >= len(a.Elements) {
		return v
	}
	next := &a.Elements[nextIndex]
	factor := min(1, max(0, float64(tick)/float64(el.Time)))
	lerp := func(x, y float64) float64 { return x + (y-x)*factor }
	if flags&interpolateOffset != 0 {
		v.X = lerp(v.X, float64(next.XOffset))
		v.Y = lerp(v.Y, float64(next.YOffset))
	}
	if flags&interpolateScale != 0 {
		nx, ny := 1.0, 1.0
		if next.HasScale {
			nx, ny = next.ScaleX, next.ScaleY
		}
		v.ScaleX = lerp(v.ScaleX, nx)
		v.ScaleY = lerp(v.ScaleY, ny)
	}
	if flags&interpolateAngle != 0 {
		v.Angle = lerp(v.Angle, next.Angle)
	}
	if flags&interpolateBlend != 0 && el.Blend == "alpha" && next.Blend == "alpha" {
		v.AlphaSrc = int(lerp(float64(el.AlphaSrc), float64(next.AlphaSrc)))
		v.AlphaDst = int(lerp(float64(el.AlphaDst), float64(next.AlphaDst)))
	}
	return v
}
func (p *Player) Visual() Visual {
	if p == nil || p.Error != nil {
		return Visual{ScaleX: 1, ScaleY: 1}
	}
	return p.Action.Sample(p.CurrentIdx, p.TickCounter)
}
