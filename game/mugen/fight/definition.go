// Package fight loads fight-screen resources and layout definitions.
package fight

import (
	"bytes"
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/resource"
	"github.com/kazzmir/paintown/game/mugen/sff"
	"github.com/kazzmir/paintown/game/mugen/snd"
)

// Definition retains immutable CPU assets and raw layout attributes. Runtime
// animation clocks, bar state and GPU images belong to the fight instance.
type Definition struct {
	Source           string
	AST              *parsers.File
	Files            *parsers.DefData
	Sprites          *sff.SFF
	Sounds           *snd.SoundBank
	Fonts            [11]*font.Font
	Animations       *air.Data
	EffectSprites    *sff.SFF
	EffectAnimations *air.Data
	CommonSounds     *snd.SoundBank
}

// Attribute returns the last matching attribute in a named layout section.
func (d *Definition) Attribute(section, name string) *parsers.Attribute {
	var found *parsers.Attribute
	for _, group := range d.AST.Sections {
		if !strings.EqualFold(strings.TrimSpace(group.Name), section) {
			continue
		}
		for _, attribute := range group.Attributes {
			if strings.EqualFold(attribute.ID.String(), name) {
				found = attribute
			}
		}
	}
	return found
}

func Load(path, root string) (*Definition, error) {
	source, err := resource.Resolve(path, root, filepath.Join(root, "data"), ".")
	if err != nil {
		return nil, fmt.Errorf("fight DEF: %w", err)
	}
	data, err := os.ReadFile(source)
	if err != nil {
		return nil, err
	}
	ast, err := parsers.Parse(bytes.NewReader(data))
	if err != nil {
		return nil, fmt.Errorf("%s: %w", source, err)
	}
	files, err := parsers.ParseDefFromAST(ast)
	if err != nil {
		return nil, fmt.Errorf("%s: %w", source, err)
	}
	definition := &Definition{Source: source, AST: ast, Files: files}
	dir := filepath.Dir(source)
	context := func(key string, err error) error {
		return fmt.Errorf("%s [Files] %s (line %d): %w", source, key, files.FileLines[key], err)
	}
	resolve := func(key string, fontFile bool) (string, error) {
		directories := []string{dir, filepath.Join(root, "data"), root}
		if fontFile {
			directories = []string{dir, root, filepath.Join(root, "data"), filepath.Join(root, "font")}
		}
		resolved, err := resource.Resolve(files.Files[key], directories...)
		if err != nil {
			return "", context(key, err)
		}
		return resolved, nil
	}
	loadSprites := func(key string) (*sff.SFF, error) {
		path, err := resolve(key, false)
		if err != nil {
			return nil, err
		}
		file, err := os.Open(path)
		if err != nil {
			return nil, context(key, err)
		}
		defer file.Close()
		archive, err := sff.Parse(file)
		if err != nil {
			return nil, context(key, fmt.Errorf("%s: %w", path, err))
		}
		return archive, nil
	}
	loadSounds := func(key string) (*snd.SoundBank, error) {
		path, err := resolve(key, false)
		if err != nil {
			return nil, err
		}
		file, err := os.Open(path)
		if err != nil {
			return nil, context(key, err)
		}
		defer file.Close()
		bank, err := snd.Parse(file)
		if err != nil {
			return nil, context(key, fmt.Errorf("%s: %w", path, err))
		}
		return bank, nil
	}
	definition.Sprites, err = loadSprites("sff")
	if err != nil {
		return nil, err
	}
	if files.Files["fightfx.sff"] != "" {
		definition.EffectSprites, err = loadSprites("fightfx.sff")
		if err != nil {
			return nil, err
		}
	}
	if files.Files["snd"] != "" {
		definition.Sounds, err = loadSounds("snd")
		if err != nil {
			return nil, err
		}
	}
	if files.Files["common.snd"] != "" {
		definition.CommonSounds, err = loadSounds("common.snd")
		if err != nil {
			return nil, err
		}
	}
	for index := 1; index <= 10; index++ {
		key := fmt.Sprintf("font%d", index)
		if files.Files[key] == "" {
			continue
		}
		path, err := resolve(key, true)
		if err != nil {
			return nil, err
		}
		definition.Fonts[index], err = font.LoadFont(path)
		if err != nil {
			return nil, context(key, fmt.Errorf("%s: %w", path, err))
		}
	}
	definition.Animations, err = air.ParseFromAST(ast)
	if err != nil {
		return nil, fmt.Errorf("%s embedded AIR: %w", source, err)
	}
	if files.Files["fightfx.air"] != "" {
		path, err := resolve("fightfx.air", false)
		if err != nil {
			return nil, err
		}
		data, err := os.ReadFile(path)
		if err != nil {
			return nil, context("fightfx.air", err)
		}
		definition.EffectAnimations, err = air.Parse(bytes.NewReader(data))
		if err != nil {
			return nil, context("fightfx.air", fmt.Errorf("%s: %w", path, err))
		}
	}
	return definition, nil
}
