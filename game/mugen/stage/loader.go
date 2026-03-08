package stage

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
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

	consumed := make(map[string]bool)

	for _, sec := range ast.Sections {
		name := strings.ToLower(sec.Name)
		switch {
		case name == "info":
			consumed[name] = true
			for _, attr := range sec.Attributes {
				if strings.ToLower(attr.ID.String()) == "name" {
					stage.Name = background.GetString(attr.Value)
				}
			}
		case name == "camera":
			consumed[name] = true
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "startx":
					stage.Camera.StartX = background.GetFloat(attr.Value)
				case "starty":
					stage.Camera.StartY = background.GetFloat(attr.Value)
				case "boundleft":
					stage.Camera.BoundLeft = background.GetFloat(attr.Value)
				case "boundright":
					stage.Camera.BoundRight = background.GetFloat(attr.Value)
				case "boundhigh":
					stage.Camera.BoundHigh = background.GetFloat(attr.Value)
				case "boundlow":
					stage.Camera.BoundLow = background.GetFloat(attr.Value)
				case "verticalfollow":
					stage.Camera.VerticalFollow = background.GetFloat(attr.Value)
				case "floortension":
					stage.Camera.FloorTension = background.GetFloat(attr.Value)
				case "tension":
					stage.Camera.Tension = background.GetFloat(attr.Value)
				}
			}
		case name == "playerinfo":
			consumed[name] = true
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "p1startx":
					stage.PlayerInfo.P1StartX = background.GetFloat(attr.Value)
				case "p1starty":
					stage.PlayerInfo.P1StartY = background.GetFloat(attr.Value)
				case "p1facing":
					stage.PlayerInfo.P1Facing = int(background.GetFloat(attr.Value))
				case "p2startx":
					stage.PlayerInfo.P2StartX = background.GetFloat(attr.Value)
				case "p2starty":
					stage.PlayerInfo.P2StartY = background.GetFloat(attr.Value)
				case "p2facing":
					stage.PlayerInfo.P2Facing = int(background.GetFloat(attr.Value))
				}
			}
		case name == "stageinfo":
			consumed[name] = true
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "zoffset":
					stage.StageInfo.ZOffset = background.GetFloat(attr.Value)
				case "autoturn":
					stage.StageInfo.AutoTurn = background.GetBool(attr.Value)
				case "resetbg":
					stage.StageInfo.ResetBG = background.GetBool(attr.Value)
				}
			}
		case name == "bound":
			consumed[name] = true
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "screenleft":
					stage.Bound.ScreenLeft = background.GetFloat(attr.Value)
				case "screenright":
					stage.Bound.ScreenRight = background.GetFloat(attr.Value)
				}
			}
		case name == "shadow":
			consumed[name] = true
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "intensity":
					stage.Shadow.Intensity = int(background.GetFloat(attr.Value))
				case "color":
					v := getIntList(attr.Value)
					if len(v) >= 3 {
						stage.Shadow.Color = [3]int{v[0], v[1], v[2]}
					}
				case "yscale":
					stage.Shadow.YScale = background.GetFloat(attr.Value)
				case "fade.range":
					v := background.GetVector2(attr.Value)
					stage.Shadow.FadeRange = [2]float64{v[0], v[1]}
				}
			}
		case name == "reflection":
			consumed[name] = true
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "intensity":
					stage.Reflection.Intensity = int(background.GetFloat(attr.Value))
				}
			}
		case name == "bgdef":
			var sprPath string
			for _, attr := range sec.Attributes {
				if strings.ToLower(attr.ID.String()) == "spr" {
					sprPath = background.GetString(attr.Value)
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
							key := fmt.Sprintf("%d-%d", spr.GroupNumber, spr.ImageNumber)
							if spr.Image != nil {
								w, h := spr.Image.Bounds().Dx(), spr.Image.Bounds().Dy()
								fmt.Printf("Cached sprite %s: %dx%d axis=%d,%d\n", key, w, h, spr.XAxis, spr.YAxis)
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
				} else {
					fmt.Printf("Warning: failed to open sprite file %s for stage\n", fullSprPath)
				}
			}
		case strings.HasPrefix(name, "bgctrldef"):
			currentDef = &background.BGCtrlDef{}
			stage.Background.Controllers = append(stage.Background.Controllers, currentDef)
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "looptime":
					currentDef.Looptime = int(background.GetFloat(attr.Value))
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
					ctrl.Type = background.GetString(attr.Value)
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

	// Parse animation data from AST (stages often embed AIR in .def)
	airData, _ := air.ParseFromAST(ast)
	if airData != nil {
		fmt.Printf("Loaded %d actions from stage AIR data\n", len(airData.Actions))
	} else {
		fmt.Printf("No AIR data found in stage AST\n")
	}

	// Load background elements
	stage.Background.LoadFromAST(ast, "bg", stage.SpriteData, airData)

	return stage, nil
}

func getIntList(v parsers.Value) []int {
	if list, ok := v.(*parsers.ValueList); ok {
		res := make([]int, 0, len(list.Values))
		for _, val := range list.Values {
			res = append(res, int(background.GetFloat(val)))
		}
		return res
	}
	if v != nil {
		return []int{int(background.GetFloat(v))}
	}
	return nil
}

// Helpers are now used from background package
