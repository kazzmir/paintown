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
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/logger"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/resource"
	"github.com/kazzmir/paintown/game/mugen/sff"
	"github.com/kazzmir/paintown/game/mugen/snd"
)

type Motif struct {
	Source string
	Info   struct {
		Name       string
		Author     string
		LocalCoord coord.Space
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

func LoadMotif(path string, dataDir string) (*Motif, error) {
	absPath, err := resource.Resolve(path, dataDir, filepath.Join(dataDir, "data"), ".")
	if err != nil {
		return nil, fmt.Errorf("motif DEF: %w", err)
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
	fileLines := make(map[string]int)
	defData, err := parsers.ParseDefFromAST(ast)
	if err != nil {
		return nil, fmt.Errorf("%s: %w", absPath, err)
	}
	local, err := coord.Parse(defData.Info["localcoord"])
	if err != nil {
		return nil, fmt.Errorf("%s [Info] localcoord (line %d): %w", absPath, defData.InfoLines["localcoord"], err)
	}

	motif := &Motif{Source: absPath}
	motif.Info.LocalCoord = local
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
				fileLines[id] = attr.Line
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
						idx, err := strconv.Atoi(strings.TrimPrefix(id, "font"))
						if err == nil && idx >= 1 && idx <= 10 {
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
		case "select info":
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "fadein.time":
					motif.SelectInfo.FadeInTime = int(getFloat(attr.Value))
				case "fadeout.time":
					motif.SelectInfo.FadeOutTime = int(getFloat(attr.Value))
				case "rows":
					motif.SelectInfo.Rows = int(getFloat(attr.Value))
				case "columns":
					motif.SelectInfo.Columns = int(getFloat(attr.Value))
				case "wrapping":
					motif.SelectInfo.Wrapping = getBool(attr.Value)
				case "pos":
					v := getVector2(attr.Value)
					motif.SelectInfo.Pos = [2]int{int(v[0]), int(v[1])}
				case "cell.size":
					v := getVector2(attr.Value)
					motif.SelectInfo.CellSize = [2]int{int(v[0]), int(v[1])}
				case "cell.spacing":
					motif.SelectInfo.CellSpacing = int(getFloat(attr.Value))
				case "cell.bg.spr":
					v := getVector2(attr.Value)
					motif.SelectInfo.CellBgSpr = [2]int{int(v[0]), int(v[1])}
				case "cell.random.spr":
					v := getVector2(attr.Value)
					motif.SelectInfo.CellRandSpr = [2]int{int(v[0]), int(v[1])}
				case "p1.cursor.startcell":
					v := getVector2(attr.Value)
					motif.SelectInfo.P1StartCell = [2]int{int(v[0]), int(v[1])}
				case "p2.cursor.startcell":
					v := getVector2(attr.Value)
					motif.SelectInfo.P2StartCell = [2]int{int(v[0]), int(v[1])}
				case "title.offset":
					v := getVector2(attr.Value)
					motif.SelectInfo.TitleOffset = [2]int{int(v[0]), int(v[1])}
				case "title.font":
					i, b, a := getFontInfo(attr.Value)
					motif.SelectInfo.TitleFont = [3]int{i, b, a}
				case "p1.face.offset":
					motif.SelectInfo.P1FaceOffset = getVector2(attr.Value)
				case "p2.face.offset":
					motif.SelectInfo.P2FaceOffset = getVector2(attr.Value)
				case "p1.face.scale":
					motif.SelectInfo.P1FaceScale = getVector2(attr.Value)
				case "p2.face.scale":
					motif.SelectInfo.P2FaceScale = getVector2(attr.Value)
				case "p1.name.offset":
					v := getVector2(attr.Value)
					motif.SelectInfo.P1NameOffset = [2]int{int(v[0]), int(v[1])}
				case "p2.name.offset":
					v := getVector2(attr.Value)
					motif.SelectInfo.P2NameOffset = [2]int{int(v[0]), int(v[1])}
				case "p1.name.font":
					i, b, a := getFontInfo(attr.Value)
					motif.SelectInfo.P1NameFont = [3]int{i, b, a}
				case "p2.name.font":
					i, b, a := getFontInfo(attr.Value)
					motif.SelectInfo.P2NameFont = [3]int{i, b, a}
				case "cursor.move.snd":
					v := getVector2(attr.Value)
					motif.SelectInfo.CursorMoveSnd = [2]int{int(v[0]), int(v[1])}
				case "cursor.done.snd":
					v := getVector2(attr.Value)
					motif.SelectInfo.CursorDoneSnd = [2]int{int(v[0]), int(v[1])}
				case "cancel.snd":
					v := getVector2(attr.Value)
					motif.SelectInfo.CancelSnd = [2]int{int(v[0]), int(v[1])}
				}
			}
		case "vs screen":
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "time":
					motif.VSScreen.Time = int(getFloat(attr.Value))
				case "fadein.time":
					motif.VSScreen.FadeInTime = int(getFloat(attr.Value))
				case "fadeout.time":
					motif.VSScreen.FadeOutTime = int(getFloat(attr.Value))
				case "p1.pos":
					v := getVector2(attr.Value)
					motif.VSScreen.P1Pos = [2]int{int(v[0]), int(v[1])}
				case "p2.pos":
					v := getVector2(attr.Value)
					motif.VSScreen.P2Pos = [2]int{int(v[0]), int(v[1])}
				case "p1.name.pos":
					v := getVector2(attr.Value)
					motif.VSScreen.P1NamePos = [2]int{int(v[0]), int(v[1])}
				case "p2.name.pos":
					v := getVector2(attr.Value)
					motif.VSScreen.P2NamePos = [2]int{int(v[0]), int(v[1])}
				case "p1.name.font":
					i, b, a := getFontInfo(attr.Value)
					motif.VSScreen.P1NameFont = [3]int{i, b, a}
				case "p2.name.font":
					i, b, a := getFontInfo(attr.Value)
					motif.VSScreen.P2NameFont = [3]int{i, b, a}
				}
			}
		case "option info":
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				switch id {
				case "cursor.move.snd":
					v := getVector2(attr.Value)
					motif.OptionInfo.CursorMoveSnd = [2]int{int(v[0]), int(v[1])}
				case "cursor.done.snd":
					v := getVector2(attr.Value)
					motif.OptionInfo.CursorDoneSnd = [2]int{int(v[0]), int(v[1])}
				case "cancel.snd":
					v := getVector2(attr.Value)
					motif.OptionInfo.CancelSnd = [2]int{int(v[0]), int(v[1])}
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
			} else if strings.HasPrefix(lowerName, "titlebg ") || lowerName == "titlebg" {
				// Individual title bg elements are handled via LoadFromAST
			} else if strings.HasPrefix(lowerName, "selectbgdef") {
				if motif.SelectBG == nil {
					motif.SelectBG = &background.Background{}
				}
				for _, attr := range sec.Attributes {
					if strings.ToLower(attr.ID.String()) == "bgclearcolor" {
						v := getVector3(attr.Value)
						motif.SelectBG.BGClearColor = color.RGBA{R: uint8(v[0]), G: uint8(v[1]), B: uint8(v[2]), A: 255}
					}
				}
			} else if strings.HasPrefix(lowerName, "versusbgdef") {
				if motif.VsBG == nil {
					motif.VsBG = &background.Background{}
				}
				for _, attr := range sec.Attributes {
					if strings.ToLower(attr.ID.String()) == "bgclearcolor" {
						v := getVector3(attr.Value)
						motif.VsBG.BGClearColor = color.RGBA{R: uint8(v[0]), G: uint8(v[1]), B: uint8(v[2]), A: 255}
					}
				}
			} else if strings.HasPrefix(lowerName, "optionbgdef") {
				if motif.OptionBG == nil {
					motif.OptionBG = &background.Background{}
				}
				for _, attr := range sec.Attributes {
					if strings.ToLower(attr.ID.String()) == "bgclearcolor" {
						v := getVector3(attr.Value)
						motif.OptionBG.BGClearColor = color.RGBA{R: uint8(v[0]), G: uint8(v[1]), B: uint8(v[2]), A: 255}
					}
				}
			}
		}
	}

	assetError := func(key string, err error) error {
		return fmt.Errorf("%s [Files] %s (line %d): %w", absPath, key, fileLines[key], err)
	}
	resolveAsset := func(key, name string, fontFile bool) (string, error) {
		directories := []string{motifDir, filepath.Join(dataDir, "data"), dataDir}
		if fontFile {
			// The bundled system.def specifies system, program, data, font.
			directories = []string{motifDir, dataDir, filepath.Join(dataDir, "data"), filepath.Join(dataDir, "font")}
		}
		resolved, err := resource.Resolve(name, directories...)
		if err != nil {
			return "", assetError(key, err)
		}
		return resolved, nil
	}
	for _, reference := range []struct {
		key  string
		path *string
	}{
		{"logo.storyboard", &motif.Files.LogoStoryboard}, {"intro.storyboard", &motif.Files.IntroStoryboard},
		{"select", &motif.Files.Select}, {"fight", &motif.Files.Fight},
	} {
		if *reference.path == "" {
			continue
		}
		resolved, err := resolveAsset(reference.key, *reference.path, false)
		if err != nil {
			return nil, err
		}
		*reference.path = resolved
	}
	// Declared assets must load; a partial motif hides a broken installation.
	if motif.Files.Spr != "" {
		sprPath, err := resolveAsset("spr", motif.Files.Spr, false)
		if err != nil {
			return nil, err
		}
		sprFile, err := os.Open(sprPath)
		if err != nil {
			return nil, assetError("spr", err)
		}
		motif.SpriteData, err = sff.Parse(sprFile)
		sprFile.Close()
		if err != nil {
			return nil, assetError("spr", fmt.Errorf("%s: %w", sprPath, err))
		}
	}

	// Load SND if available
	if motif.Files.Snd != "" {
		sndPath, err := resolveAsset("snd", motif.Files.Snd, false)
		if err != nil {
			return nil, err
		}
		sndFile, err := os.Open(sndPath)
		if err != nil {
			return nil, assetError("snd", err)
		}
		motif.SoundBank, err = snd.Parse(sndFile)
		sndFile.Close()
		if err != nil {
			return nil, assetError("snd", fmt.Errorf("%s: %w", sndPath, err))
		}
	}

	// Load Fonts
	for i, fontFile := range motif.Files.FontFiles {
		if fontFile == "" {
			continue
		}
		key := fmt.Sprintf("font%d", i)
		fontPath, err := resolveAsset(key, fontFile, true)
		if err != nil {
			return nil, err
		}
		loaded, err := font.LoadFont(fontPath)
		if err != nil {
			return nil, assetError(key, fmt.Errorf("%s: %w", fontPath, err))
		}
		motif.Fonts[i] = loaded
		if loaded.Fallback != "" {
			logger.LogUnresolved(fmt.Sprintf("font %s: %s", fontFile, loaded.Fallback))
		}
	}

	// Load System AIR (often embedded in system.def)
	motif.SystemAIR, err = air.ParseFromAST(ast)
	if err != nil {
		return nil, fmt.Errorf("%s embedded AIR: %w", absPath, err)
	}

	// Each motif background may declare a separate sprite archive.
	for _, spec := range []struct {
		prefix string
		target **background.Background
	}{
		{"titlebg", &motif.TitleBG}, {"selectbg", &motif.SelectBG},
		{"versusbg", &motif.VsBG}, {"optionbg", &motif.OptionBG},
	} {
		if *spec.target == nil {
			*spec.target = &background.Background{}
		}
		bg := *spec.target
		bg.LocalCoord = local
		archive := motif.SpriteData
		var filename string
		var line int
		for _, section := range ast.Sections {
			if !strings.EqualFold(section.Name, spec.prefix+"def") {
				continue
			}
			for _, attr := range section.Attributes {
				if strings.EqualFold(attr.ID.String(), "spr") {
					filename, line = getString(attr.Value), attr.Line
				}
			}
		}
		if filename != "" {
			loadError := func(err error) error {
				return fmt.Errorf("%s [%sdef] spr (line %d): %w", absPath, spec.prefix, line, err)
			}
			spritePath, err := resource.Resolve(filename, motifDir, filepath.Join(dataDir, "data"), dataDir)
			if err != nil {
				return nil, loadError(err)
			}
			file, err := os.Open(spritePath)
			if err != nil {
				return nil, loadError(err)
			}
			archive, err = sff.Parse(file)
			file.Close()
			if err != nil {
				return nil, loadError(fmt.Errorf("%s: %w", spritePath, err))
			}
			bg.SpriteData = archive
		}
		bg.LoadFromAST(ast, spec.prefix, archive, motif.SystemAIR)
	}

	return motif, nil
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
