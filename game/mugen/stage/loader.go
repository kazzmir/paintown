package stage

import (
	"fmt"
	"math"
	"os"
	"path/filepath"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/resource"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

func LoadStage(path string, dataDir string) (*Stage, error) {
	absPath, err := resource.Resolve(path, dataDir, filepath.Join(dataDir, "data"), ".")
	if err != nil {
		return nil, fmt.Errorf("stage DEF: %w", err)
	}
	f, err := os.Open(absPath)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	ast, err := parsers.Parse(f)
	if err != nil {
		return nil, fmt.Errorf("%s: %w", absPath, err)
	}

	stage := &Stage{
		Source:       absPath,
		SpriteImages: make(map[string]*background.SpriteImages),
	}
	stage.StageInfo.LocalCoord = coord.Default()
	stageDir := filepath.Dir(absPath)

	var currentDef *background.BGCtrlDef

	consumed := make(map[string]bool)
	screenLeftPresent, screenRightPresent := false, false

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
				case "cuthigh":
					stage.Camera.CutHigh = background.GetFloat(attr.Value)
				case "cutlow":
					stage.Camera.CutLow = background.GetFloat(attr.Value)
				case "overdrawhigh":
					stage.Camera.OverdrawHigh = background.GetFloat(attr.Value)
				case "overdrawlow":
					stage.Camera.OverdrawLow = background.GetFloat(attr.Value)
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
				case "p1startz":
					stage.PlayerInfo.P1StartZ = background.GetFloat(attr.Value)
				case "p1facing":
					stage.PlayerInfo.P1Facing = int(background.GetFloat(attr.Value))
				case "p2startx":
					stage.PlayerInfo.P2StartX = background.GetFloat(attr.Value)
				case "p2starty":
					stage.PlayerInfo.P2StartY = background.GetFloat(attr.Value)
				case "p2startz":
					stage.PlayerInfo.P2StartZ = background.GetFloat(attr.Value)
				case "p2facing":
					stage.PlayerInfo.P2Facing = int(background.GetFloat(attr.Value))
				case "leftbound":
					stage.PlayerInfo.LeftBound = background.GetFloat(attr.Value)
				case "rightbound":
					stage.PlayerInfo.RightBound = background.GetFloat(attr.Value)
				case "topbound":
					stage.PlayerInfo.TopBound = background.GetFloat(attr.Value)
				case "botbound":
					stage.PlayerInfo.BotBound = background.GetFloat(attr.Value)
				}
			}
		case name == "stageinfo":
			consumed[name] = true
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "localcoord":
					local, err := coord.Parse(attr.RawValue)
					if err != nil {
						return nil, fmt.Errorf("%s [StageInfo] localcoord (line %d): %w", absPath, attr.Line, err)
					}
					stage.StageInfo.LocalCoord = local
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
					screenLeftPresent = true
					stage.Bound.ScreenLeft = math.Trunc(background.GetFloat(attr.Value))
				case "screenright":
					screenRightPresent = true
					stage.Bound.ScreenRight = math.Trunc(background.GetFloat(attr.Value))
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
			var sprLine int
			for _, attr := range sec.Attributes {
				if strings.EqualFold(attr.ID.String(), "spr") {
					sprPath = background.GetString(attr.Value)
					sprLine = attr.Line
				}
			}
			if sprPath != "" {
				assetError := func(err error) error {
					return fmt.Errorf("%s [BGDef] spr (line %d): %w", absPath, sprLine, err)
				}
				fullSprPath, err := resource.Resolve(sprPath, stageDir, filepath.Join(dataDir, "data"), dataDir)
				if err != nil {
					return nil, assetError(err)
				}
				sprFile, err := os.Open(fullSprPath)
				if err != nil {
					return nil, assetError(err)
				}
				stage.SpriteData, err = sff.Parse(sprFile)
				sprFile.Close()
				if err != nil {
					return nil, assetError(fmt.Errorf("%s: %w", fullSprPath, err))
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

	if !screenLeftPresent || !screenRightPresent {
		return nil, fmt.Errorf("%s [Bound]: screenleft and screenright are required", absPath)
	}

	// Parse animation data from AST (stages often embed AIR in .def)
	airData, err := air.ParseFromAST(ast)
	if err != nil {
		return nil, fmt.Errorf("%s embedded AIR: %w", absPath, err)
	}

	// Load background elements
	stage.Background.LocalCoord = stage.StageInfo.LocalCoord
	stage.Background.SpriteData = stage.SpriteData
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
