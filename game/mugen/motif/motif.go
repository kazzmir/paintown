package motif

import (
	"fmt"
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

	SelectInfo struct {
		FadeInTime    int
		FadeOutTime   int
		Rows          int
		Columns       int
		Wrapping      bool
		Pos           [2]int
		CellSize      [2]int
		CellSpacing   int
		CellBgSpr     [2]int
		CellRandSpr   [2]int
		P1StartCell   [2]int
		P2StartCell   [2]int
		TitleOffset   [2]int
		TitleFont     [3]int
		P1FaceOffset  [2]float64
		P2FaceOffset  [2]float64
		P1FaceScale   [2]float64
		P2FaceScale   [2]float64
		P1NameOffset  [2]int
		P2NameOffset  [2]int
		P1NameFont    [3]int
		P2NameFont    [3]int
		CursorMoveSnd [2]int
		CursorDoneSnd [2]int
		CancelSnd     [2]int
	}

	VSScreen struct {
		Time        int
		FadeInTime  int
		FadeOutTime int
		P1Pos       [2]int
		P2Pos       [2]int
		P1NamePos   [2]int
		P2NamePos   [2]int
		P1NameFont  [3]int
		P2NameFont  [3]int
	}

	OptionInfo struct {
		CursorMoveSnd [2]int
		CursorDoneSnd [2]int
		CancelSnd     [2]int
	}

	// Loaded Assets
	SpriteData *sff.SFF
	SoundBank  *snd.SoundBank
	Fonts      []*font.Font
	SystemAIR  *air.Data

	TitleBG  *background.Background
	SelectBG *background.Background
	VsBG     *background.Background
	OptionBG *background.Background
}

func (b *Background) LoadFromAST(ast *parsers.File, prefix string, sffData *sff.SFF, airData *air.Data) {
	prefix = strings.ToLower(prefix)

	for _, sec := range ast.Sections {
		lowerName := strings.ToLower(sec.Name)
		if !strings.HasPrefix(lowerName, prefix) || strings.HasSuffix(lowerName, "def") {
			continue
		}

		// Individual element
		common := CommonElement{
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
		case "normal", "":
			common.Type = Normal
			ne := &NormalElement{CommonElement: common}
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
			b.Elements = append(b.Elements, ne)
		case "animation":
			common.Type = Animation
			ae := &AnimationElement{
				CommonElement: common,
				Sprites:       make(map[string]*sff.Sprite),
			}
			if airData != nil {
				if action, ok := airData.Actions[actionNo]; ok {
					ae.Player = air.NewPlayer(action)
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
			b.Elements = append(b.Elements, ae)
		case "parallax":
			common.Type = Parallax
			pe := &ParallaxElement{
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
			b.Elements = append(b.Elements, pe)
		}
	}
}

func ResolvePath(path string, motifDir string, dataDir string) string {
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
	return path
}

// Helper methods from motif.go adapted for background package
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
		return [2]float64{getFloat(v), 0}
	}
	return [2]float64{0, 0}
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
