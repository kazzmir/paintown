package parsers

import (
	"io"
	"strings"
)

// StoryboardDef holds parsed data from a storyboard .def file.
type StoryboardDef struct {
	SprFile    string
	StartScene int
	Scenes     []*SceneDef
}

// SceneDef holds parsed data for a single scene section.
type SceneDef struct {
	Name            string
	EndTime         int
	FadeInTime      int
	FadeOutTime     int
	FadeInColor     [3]int
	FadeOutColor    [3]int
	BGName          string
	ClearColor      [3]int
	ClearColorSet   bool
	DefaultPosition [2]int
	DefaultPosSet   bool
	MusicFile       string
	MusicLoop       bool
	Layers          []*LayerDef
}

// LayerDef holds parsed data for a layer within a scene.
type LayerDef struct {
	Index     int
	AnimNo    int // action number
	OffsetX   int
	OffsetY   int
	StartTime int
}

// ParseStoryboard parses a MUGEN storyboard .def file.
func ParseStoryboard(r io.Reader) (*StoryboardDef, error) {
	ast, err := Parse(r)
	if err != nil {
		return nil, err
	}
	return ParseStoryboardFromAST(ast)
}

// ParseStoryboardFromAST builds a StoryboardDef from an already-parsed AST.
func ParseStoryboardFromAST(ast *File) (*StoryboardDef, error) {
	sb := &StoryboardDef{}

	for _, sec := range ast.Sections {
		name := strings.ToLower(strings.TrimSpace(sec.Name))

		switch {
		case name == "scenedef":
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "spr":
					sb.SprFile = getString(attr.Value)
				case "startscene":
					sb.StartScene = int(getFloat(attr.Value))
				}
			}

		case strings.HasPrefix(name, "scene"):
			scene := &SceneDef{
				Name:      sec.Name,
				MusicLoop: true,
				Layers:    make([]*LayerDef, 10),
			}
			// Initialise 10 layers
			for i := range scene.Layers {
				scene.Layers[i] = &LayerDef{Index: i, AnimNo: -1}
			}

			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch {
				case id == "end.time":
					scene.EndTime = int(getFloat(attr.Value))
				case id == "fadein.time":
					scene.FadeInTime = int(getFloat(attr.Value))
				case id == "fadeout.time":
					scene.FadeOutTime = int(getFloat(attr.Value))
				case id == "fadein.col":
					v := getVector3(attr.Value)
					scene.FadeInColor = [3]int{int(v[0]), int(v[1]), int(v[2])}
				case id == "fadeout.col":
					v := getVector3(attr.Value)
					scene.FadeOutColor = [3]int{int(v[0]), int(v[1]), int(v[2])}
				case id == "bg.name":
					scene.BGName = getString(attr.Value)
				case id == "clearcolor":
					v := getVector3(attr.Value)
					scene.ClearColor = [3]int{int(v[0]), int(v[1]), int(v[2])}
					scene.ClearColorSet = true
				case id == "layerall.pos":
					v := getVector2(attr.Value)
					scene.DefaultPosition = [2]int{int(v[0]), int(v[1])}
					scene.DefaultPosSet = true
				case id == "bgm":
					scene.MusicFile = getString(attr.Value)
				case id == "bgm.loop":
					scene.MusicLoop = getBool(attr.Value)
				default:
					// Check for layer0.anim, layer1.offset, layer0.starttime etc
					if layerIdx, ok := parseLayerAttr(id, "anim"); ok {
						scene.Layers[layerIdx].AnimNo = int(getFloat(attr.Value))
					} else if layerIdx, ok := parseLayerAttr(id, "offset"); ok {
						v := getVector2(attr.Value)
						scene.Layers[layerIdx].OffsetX = int(v[0])
						scene.Layers[layerIdx].OffsetY = int(v[1])
					} else if layerIdx, ok := parseLayerAttr(id, "starttime"); ok {
						scene.Layers[layerIdx].StartTime = int(getFloat(attr.Value))
					}
				}
			}
			sb.Scenes = append(sb.Scenes, scene)
		}
	}

	return sb, nil
}

// parseLayerAttr checks if id matches "layer<N>.<suffix>" and returns the layer index.
func parseLayerAttr(id, suffix string) (int, bool) {
	prefix := "layer"
	full := prefix + suffix // e.g. "layeranim" - not used directly
	_ = full
	if !strings.HasPrefix(id, prefix) {
		return -1, false
	}
	rest := strings.TrimPrefix(id, prefix) // e.g. "0.anim" or "0.offset"
	dotIdx := strings.Index(rest, ".")
	if dotIdx < 0 {
		return -1, false
	}
	numStr := rest[:dotIdx]
	attrPart := rest[dotIdx+1:]
	if attrPart != suffix {
		return -1, false
	}
	idx := 0
	for _, c := range numStr {
		if c < '0' || c > '9' {
			return -1, false
		}
		idx = idx*10 + int(c-'0')
	}
	if idx < 0 || idx >= 10 {
		return -1, false
	}
	return idx, true
}

// helper used by storyboard parser — re-uses the motif helper functions
func getString(v Value) string {
	if s, ok := v.(StringValue); ok {
		return s.Val
	}
	if k, ok := v.(KeywordValue); ok {
		return k.Val
	}
	return ""
}

func getFloat(v Value) float64 {
	if n, ok := v.(NumberValue); ok {
		return n.Val
	}
	return 0
}

func getBool(v Value) bool {
	return getFloat(v) != 0
}

func getVector2(v Value) [2]float64 {
	if list, ok := v.(*ValueList); ok {
		if len(list.Values) >= 2 {
			return [2]float64{getFloat(list.Values[0]), getFloat(list.Values[1])}
		} else if len(list.Values) == 1 {
			return [2]float64{getFloat(list.Values[0]), 0}
		}
	}
	if v != nil {
		return [2]float64{getFloat(v), 0}
	}
	return [2]float64{}
}

func getVector3(v Value) [3]float64 {
	if list, ok := v.(*ValueList); ok {
		var res [3]float64
		for i := 0; i < 3 && i < len(list.Values); i++ {
			res[i] = getFloat(list.Values[i])
		}
		return res
	}
	if v != nil {
		return [3]float64{getFloat(v), 0, 0}
	}
	return [3]float64{}
}
