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
	"github.com/kazzmir/paintown/game/mugen/sound"
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
		Music:        sound.DefaultMusic(true),
		SpriteImages: make(map[string]*background.SpriteImages),
	}
	stage.StageInfo.LocalCoord = coord.Default()
	stage.StageInfo.ResetBG = true
	stage.StageInfo.AutoTurn = true
	stage.StageInfo.XScale, stage.StageInfo.YScale = 1, 1
	stage.Camera.StartZoom, stage.Camera.ZoomOut, stage.Camera.ZoomIn = 1, 1, 1
	stage.Camera.RoundBackground = true
	stage.Shadow.Intensity, stage.Shadow.YScale = 128, .4
	stageDir := filepath.Dir(absPath)

	consumed := make(map[string]bool)
	screenLeftPresent, screenRightPresent := false, false

	for _, sec := range ast.Sections {
		name := strings.ToLower(sec.Name)
		switch {
		case name == "info":
			consumed[name] = true
			for _, attr := range sec.Attributes {
				switch strings.ToLower(attr.ID.String()) {
				case "name":
					stage.Name = background.GetString(attr.Value)
				case "displayname":
					stage.DisplayName = background.GetString(attr.Value)
				case "author":
					stage.Author = background.GetString(attr.Value)
				case "mugenversion":
					value := attr.Value
					if list, ok := value.(*parsers.ValueList); ok && len(list.Values) > 0 {
						value = list.Values[0]
					}
					version := background.GetFloat(value)
					stage.Camera.RoundBackground = version < 1 || version >= 1000
				}
			}
		case name == "music":
			consumed[name] = true
			for _, attr := range sec.Attributes {
				key := map[string]string{"bgmusic": "file", "bgmvolume": "volume", "bgmloopstart": "loopstart", "bgmloopend": "loopend"}[strings.ToLower(attr.ID.String())]
				if key != "" {
					if err := stage.Music.SetParameter(key, attr.RawValue); err != nil {
						return nil, fmt.Errorf("%s [Music] line %d: %w", absPath, attr.Line, err)
					}
				} else if strings.EqualFold(attr.ID.String(), "bgvolume") {
					stage.Music.Volume = max(0, min(200, 100+int(background.GetFloat(attr.Value))*100/255))
				}
			}
		case name == "camera":
			consumed[name] = true
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "startx":
					stage.Camera.StartX = math.Trunc(background.GetFloat(attr.Value))
				case "starty":
					stage.Camera.StartY = math.Trunc(background.GetFloat(attr.Value))
				case "startzoom":
					stage.Camera.StartZoom = background.GetFloat(attr.Value)
				case "zoomout":
					stage.Camera.ZoomOut = background.GetFloat(attr.Value)
				case "zoomin":
					stage.Camera.ZoomIn = background.GetFloat(attr.Value)
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
				case "tensionhigh":
					value := background.GetFloat(attr.Value)
					stage.Camera.TensionHigh = &value
				case "tensionlow":
					value := background.GetFloat(attr.Value)
					stage.Camera.TensionLow = &value
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
			// The beta initializes hires defaults before reading explicit scale
			// overrides, independent of their order in the DEF (native423361).
			for _, attr := range sec.Attributes {
				if strings.EqualFold(attr.ID.String(), "hires") {
					stage.StageInfo.HiRes = background.GetBool(attr.Value)
					if stage.StageInfo.HiRes {
						stage.StageInfo.XScale, stage.StageInfo.YScale = .5, .5
					}
				}
			}
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
				case "zoffsetlink":
					id := int(background.GetFloat(attr.Value))
					stage.StageInfo.ZOffsetLink = &id
				case "autoturn":
					stage.StageInfo.AutoTurn = background.GetBool(attr.Value)
				case "xscale":
					stage.StageInfo.XScale = background.GetFloat(attr.Value)
				case "yscale":
					stage.StageInfo.YScale = background.GetFloat(attr.Value)
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
					v := background.GetVector3(attr.Value)
					stage.Shadow.Color = [3]int{int(v[0]), int(v[1]), int(v[2])}
				case "xshear":
					stage.Shadow.XShear = background.GetFloat(attr.Value)
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
				case "reflect":
					stage.Reflection.Intensity = 0
					if background.GetBool(attr.Value) {
						stage.Reflection.Intensity = 128
					}
				case "intensity":
					stage.Reflection.Intensity = int(background.GetFloat(attr.Value))
				}
			}
		case name == "bgdef":
			var sprPath string
			var sprLine int
			for _, attr := range sec.Attributes {
				switch strings.ToLower(attr.ID.String()) {
				case "spr":
					sprPath = background.GetString(attr.Value)
					sprLine = attr.Line
				case "debugbg":
					stage.DebugBG = math.Trunc(background.GetFloat(attr.Value)) != 0
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
		}
	}

	if !screenLeftPresent || !screenRightPresent {
		return nil, fmt.Errorf("%s [Bound]: screenleft and screenright are required", absPath)
	}
	for _, zoom := range []float64{stage.Camera.StartZoom, stage.Camera.ZoomOut, stage.Camera.ZoomIn} {
		if zoom <= 0 || math.IsNaN(zoom) || math.IsInf(zoom, 0) {
			return nil, fmt.Errorf("%s [Camera]: zoom scales must be positive and finite", absPath)
		}
	}
	for _, scale := range []float64{stage.StageInfo.XScale, stage.StageInfo.YScale} {
		if scale == 0 || math.IsNaN(scale) || math.IsInf(scale, 0) {
			return nil, fmt.Errorf("%s [StageInfo]: scales must be nonzero and finite", absPath)
		}
	}
	if stage.Camera.ZoomOut > stage.Camera.ZoomIn {
		return nil, fmt.Errorf("%s [Camera]: zoomout exceeds zoomin", absPath)
	}

	// Parse animation data from AST (stages often embed AIR in .def)
	airData, err := air.ParseFromAST(ast)
	if err != nil {
		return nil, fmt.Errorf("%s embedded AIR: %w", absPath, err)
	}

	// Load background elements
	stage.Background.LocalCoord = stage.StageInfo.LocalCoord
	stage.Background.SpriteData = stage.SpriteData
	stage.resetBackground = func() {
		stage.Background.Elements = nil
		stage.Background.Controllers = nil
		stage.Background.LoadFromAST(ast, "bg", stage.SpriteData, airData)
	}
	stage.resetBackground()

	return stage, nil
}
