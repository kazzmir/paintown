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
	"github.com/kazzmir/paintown/game/mugen/sound"
)

// SelectCursor keeps authored cursor sprites/actions; clocks belong to selection.
type SelectCursor struct {
	ActiveSprite, DoneSprite       *[2]int
	ActiveAnimation, DoneAnimation *int
	Blink                          bool
}

type Portrait struct {
	Sprite        [2]int
	Offset, Scale [2]float64
	Facing        int
	Window        *[4]int
}

type Motif struct {
	Source string
	AST    *parsers.File
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
	Music map[string]sound.MusicSpec

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
		FadeInTime                         int
		FadeOutTime                        int
		Rows                               int
		Columns                            int
		Wrapping                           bool
		Pos                                [2]int
		CellSize                           [2]int
		CellSpacing                        int
		CellBgSpr                          [2]int
		CellRandSpr                        [2]int
		PortraitSpr                        [2]int
		P1FaceSpr, P2FaceSpr               [2]int
		PortraitScale                      [2]float64
		PortraitOffset                     [2]float64
		P1FaceWindow, P2FaceWindow         *[4]int
		ShowEmptyBoxes, MoveOverEmptyBoxes bool
		P1Cursor, P2Cursor                 SelectCursor
		P1FaceFacing, P2FaceFacing         int
		P1StartCell                        [2]int
		P2StartCell                        [2]int
		TitleOffset                        [2]int
		TitleFont                          [3]int
		P1FaceOffset                       [2]float64
		P2FaceOffset                       [2]float64
		P1FaceScale                        [2]float64
		P2FaceScale                        [2]float64
		P1NameOffset                       [2]int
		P2NameOffset                       [2]int
		P1NameFont                         [3]int
		P2NameFont                         [3]int
		CursorMoveSnd                      [2]int
		CursorDoneSnd                      [2]int
		CancelSnd                          [2]int
	}

	VSScreen struct {
		Time                         int
		FadeInTime                   int
		FadeOutTime                  int
		P1Pos                        [2]int
		P2Pos                        [2]int
		P1NamePos                    [2]int
		P2NamePos                    [2]int
		P1NameFont                   [3]int
		P2NameFont                   [3]int
		P1Portrait, P2Portrait       Portrait
		P1NameSpacing, P2NameSpacing [2]int
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

	TitleBG   *background.Background
	SelectBG  *background.Background
	VsBG      *background.Background
	OptionBG  *background.Background
	VictoryBG *background.Background
}

func LoadMotif(path string, dataDir string, languages ...string) (*Motif, error) {
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
	ast = parsers.Localize(ast, languages...)
	fileLines := make(map[string]int)
	defData, err := parsers.ParseDefFromAST(ast)
	if err != nil {
		return nil, fmt.Errorf("%s: %w", absPath, err)
	}
	local, err := coord.Parse(defData.Info["localcoord"])
	if err != nil {
		return nil, fmt.Errorf("%s [Info] localcoord (line %d): %w", absPath, defData.InfoLines["localcoord"], err)
	}

	motif := &Motif{Source: absPath, AST: ast, Music: map[string]sound.MusicSpec{"title": sound.DefaultMusic(true), "select": sound.DefaultMusic(true), "vs": sound.DefaultMusic(false), "victory": sound.DefaultMusic(false)}}
	motif.Info.LocalCoord = local
	motif.Files.FontFiles = make([]string, 11) // 1-10
	motif.Fonts = make([]*font.Font, 11)
	motif.TitleInfo.ItemNames = make(map[string]string)
	motif.SelectInfo.CellBgSpr, motif.SelectInfo.CellRandSpr = [2]int{-1, -1}, [2]int{-1, -1}
	motif.SelectInfo.PortraitSpr = [2]int{9000, 0}
	motif.SelectInfo.P1FaceSpr, motif.SelectInfo.P2FaceSpr = [2]int{9000, 1}, [2]int{9000, 1}
	motif.SelectInfo.PortraitScale = [2]float64{1, 1}
	motif.SelectInfo.P1FaceScale, motif.SelectInfo.P2FaceScale = [2]float64{1, 1}, [2]float64{1, 1}
	motif.SelectInfo.P1FaceFacing, motif.SelectInfo.P2FaceFacing = 1, 1
	motif.SelectInfo.ShowEmptyBoxes, motif.SelectInfo.MoveOverEmptyBoxes = true, true
	motif.SelectInfo.P2Cursor.Blink = true
	motif.VSScreen.Time = 150
	motif.VSScreen.P1Portrait = Portrait{Sprite: [2]int{9000, 1}, Scale: [2]float64{1, 1}, Facing: 1}
	motif.VSScreen.P2Portrait = Portrait{Sprite: [2]int{9000, 1}, Scale: [2]float64{1, 1}, Facing: -1}

	motifDir := filepath.Dir(absPath)

	groups := make(map[string][]*parsers.Section)
	for _, section := range ast.Sections {
		name := strings.ToLower(section.Name)
		switch name {
		case "info", "files", "music", "title info", "select info", "vs screen", "option info":
			groups[name] = append(groups[name], section)
		default:
			if strings.HasSuffix(name, "bgdef") {
				groups[name] = append(groups[name], section)
			}
		}
	}
	readGroups := make(map[string]bool)
	for _, original := range ast.Sections {
		sec := original
		name := strings.ToLower(sec.Name)
		if group := groups[name]; len(group) > 0 {
			if readGroups[name] {
				continue
			}
			readGroups[name] = true
			copy := *sec
			copy.Attributes = parsers.FirstAttributes(group...)
			sec = &copy
		}
		switch name {
		case "info":
			for _, attr := range sec.Attributes {
				if defData.InfoLines[strings.ToLower(attr.ID.String())] != attr.Line {
					continue
				}
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
				if defData.FileLines[id] != attr.Line {
					continue
				}
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
				screen, suffix, ok := strings.Cut(strings.ToLower(attr.ID.String()), ".bgm")
				if !ok {
					continue
				}
				key := map[string]string{"": "file", ".loop": "loop", ".volume": "volume", ".loopstart": "loopstart", ".loopend": "loopend"}[suffix]
				if key == "" {
					continue
				}
				track, present := motif.Music[screen]
				if !present {
					track = sound.DefaultMusic(false)
				}
				if err := track.SetParameter(key, attr.RawValue); err != nil {
					return nil, fmt.Errorf("%s [Music] line %d: %w", absPath, attr.Line, err)
				}
				motif.Music[screen] = track
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
			seen := map[string]bool{}
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				// Screenpacks put alternative layouts below the active one.
				// The beta takes the first value of each attribute.
				if seen[id] {
					continue
				}
				seen[id] = true
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
				case "portrait.spr", "p1.face.spr", "p2.face.spr":
					v := getVector2(attr.Value)
					pair := [2]int{int(v[0]), int(v[1])}
					switch id {
					case "portrait.spr":
						motif.SelectInfo.PortraitSpr = pair
					case "p1.face.spr":
						motif.SelectInfo.P1FaceSpr = pair
					case "p2.face.spr":
						motif.SelectInfo.P2FaceSpr = pair
					}
				case "portrait.scale":
					motif.SelectInfo.PortraitScale = getVector2(attr.Value)
				case "portrait.offset":
					motif.SelectInfo.PortraitOffset = getVector2(attr.Value)
				case "p1.face.window", "p2.face.window":
					window := background.GetVector4(attr.Value)
					if id == "p1.face.window" {
						motif.SelectInfo.P1FaceWindow = &window
					} else {
						motif.SelectInfo.P2FaceWindow = &window
					}
				case "showemptyboxes":
					motif.SelectInfo.ShowEmptyBoxes = getBool(attr.Value)
				case "moveoveremptyboxes":
					motif.SelectInfo.MoveOverEmptyBoxes = getBool(attr.Value)
				case "p1.face.facing":
					motif.SelectInfo.P1FaceFacing = int(getFloat(attr.Value))
				case "p2.face.facing":
					motif.SelectInfo.P2FaceFacing = int(getFloat(attr.Value))
				case "p1.cursor.active.spr", "p2.cursor.active.spr", "p1.cursor.done.spr", "p2.cursor.done.spr",
					"p1.cursor.active.anim", "p2.cursor.active.anim", "p1.cursor.done.anim", "p2.cursor.done.anim", "p1.cursor.blink", "p2.cursor.blink":
					cursor := &motif.SelectInfo.P1Cursor
					if strings.HasPrefix(id, "p2.") {
						cursor = &motif.SelectInfo.P2Cursor
					}
					switch {
					case strings.HasSuffix(id, ".blink"):
						cursor.Blink = getBool(attr.Value)
					case strings.HasSuffix(id, ".spr"):
						v := getVector2(attr.Value)
						pair := [2]int{int(v[0]), int(v[1])}
						if strings.Contains(id, ".active.") {
							cursor.ActiveSprite = &pair
						} else {
							cursor.DoneSprite = &pair
						}
					default:
						value := int(getFloat(attr.Value))
						if strings.Contains(id, ".active.") {
							cursor.ActiveAnimation = &value
						} else {
							cursor.DoneAnimation = &value
						}
					}
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
			seen := map[string]bool{}
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				if seen[id] {
					continue
				}
				seen[id] = true
				portrait := &motif.VSScreen.P1Portrait
				if strings.HasPrefix(id, "p2.") {
					portrait = &motif.VSScreen.P2Portrait
				}
				switch id {
				case "p1.spr", "p2.spr":
					v := getVector2(attr.Value)
					portrait.Sprite = [2]int{int(v[0]), int(v[1])}
				case "p1.offset", "p2.offset", "p1.pos", "p2.pos":
					portrait.Offset = getVector2(attr.Value)
				case "p1.scale", "p2.scale":
					portrait.Scale = getVector2(attr.Value)
				case "p1.facing", "p2.facing":
					portrait.Facing = int(getFloat(attr.Value))
				case "p1.window", "p2.window":
					v := background.GetVector4(attr.Value)
					portrait.Window = &v
				case "time":
					motif.VSScreen.Time = int(getFloat(attr.Value))
				case "fadein.time":
					motif.VSScreen.FadeInTime = int(getFloat(attr.Value))
				case "fadeout.time":
					motif.VSScreen.FadeOutTime = int(getFloat(attr.Value))
				case "p1.name.pos", "p1.name.offset":
					v := getVector2(attr.Value)
					motif.VSScreen.P1NamePos = [2]int{int(v[0]), int(v[1])}
				case "p2.name.pos", "p2.name.offset":
					v := getVector2(attr.Value)
					motif.VSScreen.P2NamePos = [2]int{int(v[0]), int(v[1])}
				case "p1.name.font":
					i, b, a := getFontInfo(attr.Value)
					motif.VSScreen.P1NameFont = [3]int{i, b, a}
				case "p2.name.font":
					i, b, a := getFontInfo(attr.Value)
					motif.VSScreen.P2NameFont = [3]int{i, b, a}
				case "p1.name.spacing":
					v := getVector2(attr.Value)
					motif.VSScreen.P1NameSpacing = [2]int{int(v[0]), int(v[1])}
				case "p2.name.spacing":
					v := getVector2(attr.Value)
					motif.VSScreen.P2NameSpacing = [2]int{int(v[0]), int(v[1])}
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
		loaded, err := font.LoadFontWithHeight(fontPath, defData.Files[key+".height"], languages...)
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
		{"victorybg", &motif.VictoryBG},
	} {
		if *spec.target == nil {
			*spec.target = &background.Background{}
		}
		bg := *spec.target
		bg.LocalCoord = local
		archive := motif.SpriteData
		var filename string
		var line int
		declared := false
		for _, attr := range parsers.FirstAttributes(groups[spec.prefix+"def"]...) {
			switch strings.ToLower(attr.ID.String()) {
			case "spr":
				filename, line, declared = getString(attr.Value), attr.Line, true
			case "bgclearcolor":
				v := getVector3(attr.Value)
				bg.BGClearColor = color.RGBA{R: uint8(v[0]), G: uint8(v[1]), B: uint8(v[2]), A: 255}
			}
		}
		if declared {
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
		}
		bg.SpriteData = archive
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
