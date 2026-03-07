package stage

import (
	"fmt"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

func LoadStage(path string, dataDir string) (*Stage, error) {
	absPath := background.ResolvePath(path, filepath.Dir(path), dataDir)
	f, err := os.Open(absPath)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	ast, err := parsers.Parse(f)
	if err != nil {
		return nil, err
	}

	stage := &Stage{
		SpriteImages: make(map[string]*background.SpriteImages),
	}
	stageDir := filepath.Dir(absPath)

	var currentDef *background.BGCtrlDef

	for _, sec := range ast.Sections {
		name := strings.ToLower(sec.Name)
		switch {
		case name == "info":
			for _, attr := range sec.Attributes {
				if strings.ToLower(attr.ID.String()) == "name" {
					stage.Name = getString(attr.Value)
				}
			}
		case name == "camera":
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "startx":
					stage.Camera.StartX = getFloat(attr.Value)
				case "starty":
					stage.Camera.StartY = getFloat(attr.Value)
				case "boundleft":
					stage.Camera.BoundLeft = getFloat(attr.Value)
				case "boundright":
					stage.Camera.BoundRight = getFloat(attr.Value)
				case "boundhigh":
					stage.Camera.BoundHigh = getFloat(attr.Value)
				case "boundlow":
					stage.Camera.BoundLow = getFloat(attr.Value)
				case "verticalfollow":
					stage.Camera.VerticalFollow = getFloat(attr.Value)
				case "floortension":
					stage.Camera.FloorTension = getFloat(attr.Value)
				case "tension":
					stage.Camera.Tension = getFloat(attr.Value)
				}
			}
		case name == "playerinfo":
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "p1startx":
					stage.PlayerInfo.P1StartX = getFloat(attr.Value)
				case "p1starty":
					stage.PlayerInfo.P1StartY = getFloat(attr.Value)
				case "p1facing":
					stage.PlayerInfo.P1Facing = int(getFloat(attr.Value))
				case "p2startx":
					stage.PlayerInfo.P2StartX = getFloat(attr.Value)
				case "p2starty":
					stage.PlayerInfo.P2StartY = getFloat(attr.Value)
				case "p2facing":
					stage.PlayerInfo.P2Facing = int(getFloat(attr.Value))
				}
			}
		case name == "stageinfo":
			for _, attr := range sec.Attributes {
				if strings.ToLower(attr.ID.String()) == "zoffset" {
					stage.StageInfo.ZOffset = getFloat(attr.Value)
				}
			}
		case name == "bgdef":
			var sprPath string
			for _, attr := range sec.Attributes {
				if strings.ToLower(attr.ID.String()) == "spr" {
					sprPath = getString(attr.Value)
				}
			}
			if sprPath != "" {
				fullSprPath := background.ResolvePath(sprPath, stageDir, dataDir)
				if sprFile, err := os.Open(fullSprPath); err == nil {
					if sffData, err := sff.Parse(sprFile); err == nil {
						stage.SpriteData = sffData
						// Cache sprites
						for i := range sffData.Sprites {
							spr := &sffData.Sprites[i]
							if spr.Image != nil {
								key := fmt.Sprintf("%d-%d", spr.GroupNumber, spr.ImageNumber)
								si := &background.SpriteImages{
									Masked: ebiten.NewImageFromImage(spr.Image),
								}
								if spr.ImageOpaque != nil {
									si.Opaque = ebiten.NewImageFromImage(spr.ImageOpaque)
								} else {
									si.Opaque = si.Masked
								}
								stage.SpriteImages[key] = si
							}
						}
					}
					sprFile.Close()
				}
			}
		case strings.HasPrefix(name, "bgctrldef"):
			currentDef = &background.BGCtrlDef{}
			stage.Background.Controllers = append(stage.Background.Controllers, currentDef)
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "looptime":
					currentDef.Looptime = int(getFloat(attr.Value))
				case "ctrlid":
					currentDef.CtrlIDs = getIntList(attr.Value)
				}
			}
		case strings.HasPrefix(name, "bgctrl"):
			if currentDef == nil {
				// MUGEN allows BGCtrl without preceding BGCtrlDef sometimes (implicitly group all)
				currentDef = &background.BGCtrlDef{}
				stage.Background.Controllers = append(stage.Background.Controllers, currentDef)
			}
			ctrl := &background.BGCtrl{
				Params: make(map[string]parsers.Value),
			}
			currentDef.Ctrls = append(currentDef.Ctrls, ctrl)
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "type":
					ctrl.Type = getString(attr.Value)
				case "time":
					t := getIntList(attr.Value)
					if len(t) >= 1 {
						ctrl.Time[0] = t[0]
						if len(t) >= 2 {
							ctrl.Time[1] = t[1]
						} else {
							ctrl.Time[1] = t[0]
						}
						if len(t) >= 3 {
							ctrl.Looptime = t[2]
						}
					}
				case "ctrlid":
					ctrl.CtrlIDs = getIntList(attr.Value)
				default:
					ctrl.Params[id] = attr.Value
				}
			}
		}
	}

	// Load background elements
	stage.Background.LoadFromAST(ast, "bg", stage.SpriteData, nil)

	return stage, nil
}

func getIntList(v parsers.Value) []int {
	if list, ok := v.(*parsers.ValueList); ok {
		res := make([]int, 0, len(list.Values))
		for _, val := range list.Values {
			res = append(res, int(getFloat(val)))
		}
		return res
	}
	if v != nil {
		return []int{int(getFloat(v))}
	}
	return nil
}

// Helpers duplicated from background (could be exported if needed frequently, but fine for now)
func getFloat(v parsers.Value) float64 {
	if n, ok := v.(parsers.NumberValue); ok {
		return n.Val
	}
	if s, ok := v.(parsers.StringValue); ok {
		val, _ := strconv.ParseFloat(strings.TrimSpace(s.Val), 64)
		return val
	}
	if k, ok := v.(parsers.KeywordValue); ok {
		val, _ := strconv.ParseFloat(strings.TrimSpace(k.Val), 64)
		return val
	}
	return 0
}

func getString(v parsers.Value) string {
	if v == nil {
		return ""
	}
	if s, ok := v.(parsers.StringValue); ok {
		return s.Val
	}
	if k, ok := v.(parsers.KeywordValue); ok {
		return k.Val
	}
	return ""
}
