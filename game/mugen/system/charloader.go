package main

import (
	"fmt"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"image"
	"os"
	"path/filepath"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/resource"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

type CharacterPortrait struct {
	Name, DefPath, IntroStoryboard, EndingStoryboard string
	Empty                                            bool
	CoordinateWidth                                  int
	Portraits                                        map[[2]int]portraitImage
	SmallAxis, BigAxis                               [2]int16
	Entry                                            parsers.CharacterEntry
	SmallPortrait, BigPortrait                       *ebiten.Image
}

func loadCharacters(selectDefPath, charsDir string, packs *resource.CharacterPacks, languages ...string) ([]*CharacterPortrait, []string, error) {
	f, err := os.Open(selectDefPath)
	if err != nil {
		return nil, nil, err
	}
	defer f.Close()
	sd, err := parsers.ParseSelectDef(f, languages...)
	if err != nil {
		return nil, nil, fmt.Errorf("%s: %w", selectDefPath, err)
	}
	var chars []*CharacterPortrait
	var stages []string
	seen := map[string]bool{}
	addStage := func(name string) {
		if name != "" && !strings.EqualFold(name, "random") && !seen[name] {
			stages = append(stages, name)
			seen[name] = true
		}
	}
	for _, entry := range sd.Characters {
		cp := &CharacterPortrait{Name: entry.Name, Entry: *entry}
		if strings.EqualFold(entry.Name, "empty") {
			// Keep the grid position without resolving a character or adding
			// its stage/music to the playable roster.
			cp.Name, cp.Empty = "", true
			chars = append(chars, cp)
			continue
		}
		if strings.EqualFold(entry.Name, "randomselect") {
			cp.Name = "Random"
			chars = append(chars, cp)
			continue
		}
		cp.DefPath, err = packs.Resolve(entry.Name, charsDir)
		if err != nil {
			return nil, nil, fmt.Errorf("%s character %q: %w", selectDefPath, entry.Name, err)
		}
		language := ""
		if len(languages) > 0 {
			language = languages[0]
		}
		loaded, err := loadCharacterPortraitLanguage(cp.DefPath, language)
		if err != nil {
			return nil, nil, err
		}
		for _, path := range []*string{&loaded.IntroStoryboard, &loaded.EndingStoryboard} {
			if *path != "" {
				*path, err = resource.Resolve(*path, filepath.Dir(loaded.DefPath))
				if err != nil {
					return nil, nil, fmt.Errorf("%s [Arcade] storyboard: %w", loaded.DefPath, err)
				}
			}
		}
		loaded.Entry = cp.Entry
		cp = loaded
		chars = append(chars, cp)
		if entry.IncludeStage {
			addStage(entry.Stage)
		}
	}
	for _, name := range sd.ExtraStages {
		addStage(name)
	}
	return chars, stages, nil
}

type portraitImage struct {
	Image *ebiten.Image
	Axis  [2]int16
}

func loadCharacterPortrait(defPath string, extra ...[2]int) (*CharacterPortrait, error) {
	return loadCharacterPortraitLanguage(defPath, "", extra...)
}

func loadCharacterPortraitLanguage(defPath, language string, extra ...[2]int) (*CharacterPortrait, error) {
	cp := &CharacterPortrait{DefPath: defPath, Portraits: map[[2]int]portraitImage{}}
	file, err := os.Open(cp.DefPath)
	if err != nil {
		return nil, err
	}
	ast, err := parsers.Parse(file)
	file.Close()
	if err != nil {
		return nil, fmt.Errorf("%s: %w", cp.DefPath, err)
	}
	ast = parsers.Localize(ast, language)
	def, err := parsers.ParseDefFromAST(ast)
	if err != nil {
		return nil, err
	}
	cp.Name = def.Info["displayname"]
	if cp.Name == "" {
		cp.Name = def.Info["name"]
	}
	if cp.Name == "" {
		cp.Name = filepath.Base(defPath)
	}
	dir := filepath.Dir(cp.DefPath)
	// Selection resolves these paths when it needs story playback. VS only
	// consumes portrait assets, including for rematches of temporary DEFs.
	for _, section := range ast.Sections {
		if !strings.EqualFold(section.Name, "arcade") {
			continue
		}
		for _, a := range section.Attributes {
			if strings.EqualFold(a.ID.String(), "ending.storyboard") {
				cp.EndingStoryboard = background.GetString(a.Value)
			}
			if strings.EqualFold(a.ID.String(), "intro.storyboard") {
				cp.IntroStoryboard = background.GetString(a.Value)
			}
		}
	}

	path, err := resource.Resolve(def.Files["sprite"], dir)
	if err != nil {
		return nil, fmt.Errorf("%s [Files] sprite (line %d): %w", cp.DefPath, def.FileLines["sprite"], err)
	}
	file, err = os.Open(path)
	if err != nil {
		return nil, err
	}
	archive, err := sff.ParsePortraits(file, extra...)
	file.Close()
	if err != nil {
		return nil, fmt.Errorf("%s: %w", path, err)
	}
	for _, sprite := range archive.Sprites {
		if sprite.Image != nil {
			pair := [2]int{int(sprite.GroupNumber), int(sprite.ImageNumber)}
			cp.Portraits[pair] = portraitImage{ebiten.NewImageFromImage(sff.UploadPixels(sprite.Image)), [2]int16{sprite.XAxis, sprite.YAxis}}
		}
		if sprite.GroupNumber == 9000 && sprite.Image != nil {
			switch sprite.ImageNumber {
			case 0:
				cp.SmallPortrait = cp.Portraits[[2]int{9000, int(sprite.ImageNumber)}].Image
				cp.SmallAxis = [2]int16{sprite.XAxis, sprite.YAxis}
			case 1:
				cp.BigPortrait = cp.Portraits[[2]int{9000, int(sprite.ImageNumber)}].Image
				cp.BigAxis = [2]int16{sprite.XAxis, sprite.YAxis}
			}
		}
	}

	local, err := coord.Parse(def.Info["localcoord"])
	if err != nil {
		return nil, fmt.Errorf("%s [Info] localcoord: %w", defPath, err)
	}
	cp.CoordinateWidth = local.Width
	return cp, nil
}

// Portrait dimensions and axes use the character's units, not the motif's.
func (c *CharacterPortrait) drawPortrait(screen *ebiten.Image, pair [2]int, position, scale [2]float64, facing, motifWidth int, window *[4]int) {
	if c == nil {
		return
	}
	sprite, ok := c.Portraits[pair]
	if !ok || sprite.Image == nil {
		return
	}
	if window != nil {
		bounds := image.Rect(window[0], window[1], window[2]+1, window[3]+1).Intersect(screen.Bounds())
		if bounds.Empty() {
			return
		}
		screen = screen.SubImage(bounds).(*ebiten.Image)
	}
	width := c.CoordinateWidth
	if width <= 0 {
		width = 320
	}
	ratio := float64(motifWidth) / float64(width)
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Translate(-float64(sprite.Axis[0]), -float64(sprite.Axis[1]))
	op.GeoM.Scale(scale[0]*ratio*float64(facing), scale[1]*ratio)
	op.GeoM.Translate(position[0], position[1])
	screen.DrawImage(sprite.Image, op)
}
