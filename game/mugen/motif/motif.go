package motif

import (
	"fmt"
	"image/color"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/sff"
	"github.com/kazzmir/paintown/game/mugen/snd"
)

type Motif struct {
	Info struct {
		Name   string
		Author string
	}
	Files struct {
		Spr             string
		Snd             string
		LogoStoryboard  string
		IntroStoryboard string
		Select          string
		Fight           string
		FontFiles       []string
	}
	Music struct {
		TitleBGM      string
		TitleBGMLoop  bool
		SelectBGM     string
		SelectBGMLoop bool
		VSBGM         string
		VSBGMLoop     bool
	}
	TitleInfo struct {
		FadeInTime  int
		FadeOutTime int
		MenuPos     [2]float64
		ItemNames   map[string]string
		ItemFont    struct {
			Index int
			Bank  int
			Align int
		}
		ItemActiveFont struct {
			Index int
			Bank  int
			Align int
		}
		MenuSpacing      [2]float64
		VisibleItems     int
		VisibleMargins   [2]int
		CursorMoveSnd    [2]int
		CursorDoneSnd    [2]int
		CancelSnd        [2]int
		BoxCursorVisible bool
		BoxCursorCoords  [4]int
	}

	// Loaded Assets
	SpriteData *sff.SFF
	SoundBank  *snd.SoundBank
	Fonts      []*font.Font
	SystemAIR  *air.Data

	TitleBG *background.Background
}

func LoadMotif(path string, dataDir string) (*Motif, error) {
	absPath := resolvePath(path, filepath.Dir(path), dataDir)
	f, err := os.Open(absPath)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	ast, err := parsers.Parse(f)
	if err != nil {
		return nil, err
	}

	motif := &Motif{}
	motif.Files.FontFiles = make([]string, 11) // 1-10
	motif.Fonts = make([]*font.Font, 11)
	motif.TitleInfo.ItemNames = make(map[string]string)

	motifDir := filepath.Dir(absPath)

	for _, sec := range ast.Sections {
		switch strings.ToLower(sec.Name) {
		case "info":
			for _, attr := range sec.Attributes {
				switch strings.ToLower(attr.ID.String()) {
				case "name":
					motif.Info.Name = getString(attr.Value)
				case "author":
					motif.Info.Author = getString(attr.Value)
				}
			}
		case "files":
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				val := getString(attr.Value)
				switch id {
				case "spr":
					motif.Files.Spr = val
				case "snd":
					motif.Files.Snd = val
				case "logo.storyboard":
					motif.Files.LogoStoryboard = val
				case "intro.storyboard":
					motif.Files.IntroStoryboard = val
				case "select":
					motif.Files.Select = val
				case "fight":
					motif.Files.Fight = val
				default:
					if strings.HasPrefix(id, "font") {
						// e.g. font1
						var idx int
						fmt.Sscanf(id, "font%d", &idx)
						if idx >= 1 && idx <= 10 {
							motif.Files.FontFiles[idx] = val
						}
					}
				}
			}
		case "music":
			for _, attr := range sec.Attributes {
				switch strings.ToLower(attr.ID.String()) {
				case "title.bgm":
					motif.Music.TitleBGM = getString(attr.Value)
				case "title.bgm.loop":
					motif.Music.TitleBGMLoop = getBool(attr.Value)
				case "select.bgm":
					motif.Music.SelectBGM = getString(attr.Value)
				case "select.bgm.loop":
					motif.Music.SelectBGMLoop = getBool(attr.Value)
				case "vs.bgm":
					motif.Music.VSBGM = getString(attr.Value)
				case "vs.bgm.loop":
					motif.Music.VSBGMLoop = getBool(attr.Value)
				}
			}
		case "title info":
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch {
				case id == "fadein.time":
					motif.TitleInfo.FadeInTime = int(getFloat(attr.Value))
				case id == "fadeout.time":
					motif.TitleInfo.FadeOutTime = int(getFloat(attr.Value))
				case id == "menu.pos":
					motif.TitleInfo.MenuPos = getVector2(attr.Value)
				case strings.HasPrefix(id, "menu.itemname."):
					key := strings.TrimPrefix(id, "menu.itemname.")
					motif.TitleInfo.ItemNames[key] = getString(attr.Value)
				case id == "menu.item.font":
					motif.TitleInfo.ItemFont.Index, motif.TitleInfo.ItemFont.Bank, motif.TitleInfo.ItemFont.Align = getFontInfo(attr.Value)
				case id == "menu.item.active.font":
					motif.TitleInfo.ItemActiveFont.Index, motif.TitleInfo.ItemActiveFont.Bank, motif.TitleInfo.ItemActiveFont.Align = getFontInfo(attr.Value)
				case id == "menu.item.spacing":
					motif.TitleInfo.MenuSpacing = getVector2(attr.Value)
				case id == "menu.window.visibleitems":
					motif.TitleInfo.VisibleItems = int(getFloat(attr.Value))
				case id == "menu.window.margins.y":
					v := getVector2(attr.Value)
					motif.TitleInfo.VisibleMargins[0] = int(v[0])
					motif.TitleInfo.VisibleMargins[1] = int(v[1])
				case id == "menu.boxcursor.visible":
					motif.TitleInfo.BoxCursorVisible = getBool(attr.Value)
				case id == "menu.boxcursor.coords":
					motif.TitleInfo.BoxCursorCoords = getVector4(attr.Value)
				case id == "cursor.move.snd":
					v := getVector2(attr.Value)
					motif.TitleInfo.CursorMoveSnd = [2]int{int(v[0]), int(v[1])}
				case id == "cursor.done.snd":
					v := getVector2(attr.Value)
					motif.TitleInfo.CursorDoneSnd = [2]int{int(v[0]), int(v[1])}
				case id == "cancel.snd":
					v := getVector2(attr.Value)
					motif.TitleInfo.CancelSnd = [2]int{int(v[0]), int(v[1])}
				}
			}
		case "option info":
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "cursor.move.snd":
					v := getVector2(attr.Value)
					motif.TitleInfo.CursorMoveSnd = [2]int{int(v[0]), int(v[1])}
				case "cursor.done.snd":
					v := getVector2(attr.Value)
					motif.TitleInfo.CursorDoneSnd = [2]int{int(v[0]), int(v[1])}
				case "cancel.snd":
					v := getVector2(attr.Value)
					motif.TitleInfo.CancelSnd = [2]int{int(v[0]), int(v[1])}
				}
			}
		default:
			lowerName := strings.ToLower(sec.Name)
			if strings.HasPrefix(lowerName, "titlebgdef") {
				// Title background definition (bgclearcolor)
				if motif.TitleBG == nil {
					motif.TitleBG = &background.Background{}
				}
				for _, attr := range sec.Attributes {
					if strings.ToLower(attr.ID.String()) == "bgclearcolor" {
						v := getVector3(attr.Value)
						motif.TitleBG.BGClearColor = color.RGBA{R: uint8(v[0]), G: uint8(v[1]), B: uint8(v[2]), A: 255}
					}
				}
			} else if strings.HasPrefix(lowerName, "titlebg ") || strings.HasPrefix(lowerName, "titlebg") {
				// Parse individual background element
			}
		}
	}

	// Load SFF if available
	if motif.Files.Spr != "" {
		sprPath := resolvePath(motif.Files.Spr, motifDir, dataDir)
		if sprFile, err := os.Open(sprPath); err == nil {
			if sffData, err := sff.Parse(sprFile); err == nil {
				motif.SpriteData = sffData
			}
			sprFile.Close()
		}
	}

	// Load SND if available
	if motif.Files.Snd != "" {
		sndPath := resolvePath(motif.Files.Snd, motifDir, dataDir)
		if sndFile, err := os.Open(sndPath); err == nil {
			if soundBank, err := snd.Parse(sndFile); err == nil {
				motif.SoundBank = soundBank
			}
			sndFile.Close()
		}
	}

	// Load Fonts
	for i, fontFile := range motif.Files.FontFiles {
		if fontFile == "" {
			continue
		}
		fontPath := resolvePath(fontFile, motifDir, dataDir)
		if f, err := font.LoadFont(fontPath); err == nil {
			motif.Fonts[i] = f
		} else {
			fmt.Printf("Warning: Failed to load font %s: %v\n", fontFile, err)
		}
	}

	// Load System AIR (often embedded in system.def)
	if airData, err := air.ParseFromAST(ast); err == nil {
		motif.SystemAIR = airData
	}

	// Load Title Background
	motif.TitleBG = loadBackground(ast, "titlebg", motif.SpriteData, motif.SystemAIR)

	return motif, nil
}

func loadBackground(ast *parsers.File, prefix string, sffData *sff.SFF, airData *air.Data) *background.Background {
	bg := &background.Background{}
	prefix = strings.ToLower(prefix)

	for _, sec := range ast.Sections {
		lowerName := strings.ToLower(sec.Name)
		if !strings.HasPrefix(lowerName, prefix) || strings.HasSuffix(lowerName, "def") {
			continue
		}

		// Individual element
		common := background.CommonElement{
			Name:    sec.Name,
			Mask:    true,
			Delta:   [2]float64{1, 1},
			Spacing: [2]int{0, 0},
			Trans:   "none",
			Window:  [4]int{0, 0, 319, 239},
		}
		var elemType string
		var spriteNo [2]int
		var actionNo int

		for _, attr := range sec.Attributes {
			id := strings.ToLower(attr.ID.String())
			switch id {
			case "type":
				elemType = strings.ToLower(getString(attr.Value))
			case "spriteno":
				v := getVector2(attr.Value)
				spriteNo[0] = int(v[0])
				spriteNo[1] = int(v[1])
			case "actionno":
				actionNo = int(getFloat(attr.Value))
			case "start":
				common.Start = getVector2(attr.Value)
			case "delta":
				common.Delta = getVector2(attr.Value)
			case "velocity":
				common.Velocity = getVector2(attr.Value)
			case "mask":
				common.Mask = getBool(attr.Value)
			case "tile":
				v := getVector2(attr.Value)
				common.Tile[0] = int(v[0])
				common.Tile[1] = int(v[1])
			case "tilespacing":
				v := getVector2(attr.Value)
				common.Spacing[0] = int(v[0])
				common.Spacing[1] = int(v[1])
			case "xscale":
				// Handle both single value and vector (though typically vector)
				common := &common // shadowing
				_ = common        // unused
			case "trans":
				common.Trans = strings.ToLower(getString(attr.Value))
			case "alpha":
				v := getVector2(attr.Value)
				common.Alpha[0] = int(v[0])
				common.Alpha[1] = int(v[1])
			case "window":
				common.Window = getVector4(attr.Value)
			case "layerno":
				common.LayerNo = int(getFloat(attr.Value))
			}
		}

		var xscale [2]float64
		var yscale float64
		for _, attr := range sec.Attributes {
			id := strings.ToLower(attr.ID.String())
			if id == "xscale" {
				xscale = getVector2(attr.Value)
			} else if id == "yscale" {
				yscale = getFloat(attr.Value)
			}
		}

		switch elemType {
		case "normal":
			common.Type = background.Normal
			ne := &background.NormalElement{CommonElement: common}
			if sffData != nil {
				// Find sprite in SFF
				for i := range sffData.Sprites {
					spr := &sffData.Sprites[i]
					if int(spr.GroupNumber) == spriteNo[0] && int(spr.ImageNumber) == spriteNo[1] {
						ne.Sprite = spr
						break
					}
				}
			}
			bg.Elements = append(bg.Elements, ne)
		case "animation":
			common.Type = background.Animation
			ae := &background.AnimationElement{
				CommonElement: common,
				Sprites:       make(map[string]*sff.Sprite),
			}
			if airData != nil {
				if action, ok := airData.Actions[actionNo]; ok {
					ae.Player = air.NewPlayer(action)
					// Populate initial metadata for frames
					if sffData != nil {
						for _, el := range action.Elements {
							key := fmt.Sprintf("%d-%d", el.Group, el.Image)
							for j := range sffData.Sprites {
								spr := &sffData.Sprites[j]
								if spr.GroupNumber == uint16(el.Group) && spr.ImageNumber == uint16(el.Image) {
									ae.Sprites[key] = spr
									break
								}
							}
						}
					}
				}
			}
			bg.Elements = append(bg.Elements, ae)
		case "parallax":
			common.Type = background.Parallax
			pe := &background.ParallaxElement{
				CommonElement: common,
				XScale:        xscale,
				YScale:        yscale,
			}
			if sffData != nil {
				for i := range sffData.Sprites {
					spr := &sffData.Sprites[i]
					if int(spr.GroupNumber) == spriteNo[0] && int(spr.ImageNumber) == spriteNo[1] {
						pe.Sprite = spr
						break
					}
				}
			}
			bg.Elements = append(bg.Elements, pe)
		}
	}

	return bg
}

func resolvePath(path string, motifDir string, dataDir string) string {
	if path == "" {
		return ""
	}
	// 1. Check motif directory
	p := filepath.Join(motifDir, path)
	if _, err := os.Stat(p); err == nil {
		return p
	}
	// 2. Check data/ directory
	p = filepath.Join(dataDir, "data", path)
	if _, err := os.Stat(p); err == nil {
		return p
	}
	// 3. Check MUGEN root (dataDir)
	p = filepath.Join(dataDir, path)
	if _, err := os.Stat(p); err == nil {
		return p
	}
	return path // Return as is if not found (caller will handle error)
}

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

func getBool(v parsers.Value) bool {
	return getFloat(v) != 0
}

func getVector2(v parsers.Value) [2]float64 {
	if list, ok := v.(*parsers.ValueList); ok {
		if len(list.Values) >= 2 {
			return [2]float64{getFloat(list.Values[0]), getFloat(list.Values[1])}
		} else if len(list.Values) == 1 {
			return [2]float64{getFloat(list.Values[0]), 0}
		}
	}
	if v != nil {
		// Single value
		return [2]float64{getFloat(v), 0}
	}
	return [2]float64{0, 0}
}

func getVector4(v parsers.Value) [4]int {
	if list, ok := v.(*parsers.ValueList); ok {
		var res [4]int
		for i := 0; i < 4 && i < len(list.Values); i++ {
			res[i] = int(getFloat(list.Values[i]))
		}
		return res
	}
	if v != nil {
		return [4]int{int(getFloat(v)), 0, 0, 0}
	}
	return [4]int{0, 0, 0, 0}
}

func getVector3(v parsers.Value) [3]float64 {
	if list, ok := v.(*parsers.ValueList); ok {
		var res [3]float64
		for i := 0; i < 3 && i < len(list.Values); i++ {
			res[i] = getFloat(list.Values[i])
		}
		return res
	}
	if v != nil {
		return [3]float64{getFloat(v), 0, 0}
	}
	return [3]float64{0, 0, 0}
}

func getFontInfo(v parsers.Value) (int, int, int) {
	if list, ok := v.(*parsers.ValueList); ok {
		idx := 0
		bank := 0
		align := 0
		if len(list.Values) >= 1 {
			idx = int(getFloat(list.Values[0]))
		}
		if len(list.Values) >= 2 {
			bank = int(getFloat(list.Values[1]))
		}
		if len(list.Values) >= 3 {
			align = int(getFloat(list.Values[2]))
		}
		return idx, bank, align
	}
	return 0, 0, 0
}
