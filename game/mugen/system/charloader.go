package main

import (
	"fmt"
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
	Entry                                            parsers.CharacterEntry
	SmallPortrait, BigPortrait                       *ebiten.Image
}

func loadCharacters(selectDefPath, charsDir string, packs *resource.CharacterPacks) ([]*CharacterPortrait, []string, error) {
	f, err := os.Open(selectDefPath)
	if err != nil {
		return nil, nil, err
	}
	defer f.Close()
	sd, err := parsers.ParseSelectDef(f)
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
		file, err := os.Open(cp.DefPath)
		if err != nil {
			return nil, nil, err
		}
		ast, err := parsers.Parse(file)
		file.Close()
		if err != nil {
			return nil, nil, fmt.Errorf("%s: %w", cp.DefPath, err)
		}
		def, err := parsers.ParseDefFromAST(ast)
		if err != nil {
			return nil, nil, err
		}
		cp.Name = def.Info["displayname"]
		if cp.Name == "" {
			cp.Name = def.Info["name"]
		}
		if cp.Name == "" {
			cp.Name = entry.Name
		}
		dir := filepath.Dir(cp.DefPath)
		for _, section := range ast.Sections {
			if strings.EqualFold(section.Name, "arcade") {
				for _, a := range section.Attributes {
					if strings.EqualFold(a.ID.String(), "ending.storyboard") && background.GetString(a.Value) != "" {
						cp.EndingStoryboard, err = resource.Resolve(background.GetString(a.Value), dir)
						if err != nil {
							return nil, nil, fmt.Errorf("%s [Arcade] ending.storyboard: %w", cp.DefPath, err)
						}
					}
					if strings.EqualFold(a.ID.String(), "intro.storyboard") && background.GetString(a.Value) != "" {
						cp.IntroStoryboard, err = resource.Resolve(background.GetString(a.Value), dir)
						if err != nil {
							return nil, nil, fmt.Errorf("%s [Arcade] intro.storyboard (line %d): %w", cp.DefPath, a.Line, err)
						}
					}
				}
			}
		}
		path, err := resource.Resolve(def.Files["sprite"], dir)
		if err != nil {
			return nil, nil, fmt.Errorf("%s [Files] sprite (line %d): %w", cp.DefPath, def.FileLines["sprite"], err)
		}
		file, err = os.Open(path)
		if err != nil {
			return nil, nil, err
		}
		archive, err := sff.ParsePortraits(file)
		file.Close()
		if err != nil {
			return nil, nil, fmt.Errorf("%s: %w", path, err)
		}
		for _, sprite := range archive.Sprites {
			if sprite.GroupNumber == 9000 && sprite.Image != nil {
				switch sprite.ImageNumber {
				case 0:
					cp.SmallPortrait = ebiten.NewImageFromImage(sff.UploadPixels(sprite.Image))
				case 1:
					cp.BigPortrait = ebiten.NewImageFromImage(sff.UploadPixels(sprite.Image))
				}
			}
		}
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
