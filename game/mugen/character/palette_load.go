package character

import (
	"fmt"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/resource"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

// V1's external ACT banks become immutable shared palette banks. Sprite nodes
// retain whether they use the common palette; independent portraits/effects
// keep their embedded PCX colors, including links to either kind of sprite.
func (p *Player) loadLegacyPalettes() error {
	if p.SFF.Version[0] != 1 || p.Definition == nil {
		return nil
	}
	for number := 1; number <= 12; number++ {
		key := "pal" + strconv.Itoa(number)
		name := p.Definition.Files[key]
		if strings.TrimSpace(name) == "" {
			continue
		}
		context := func(err error) error {
			return fmt.Errorf("%s [Files] %s (line %d): %w", p.DefPath, key, p.Definition.FileLines[key], err)
		}
		path, err := resource.Resolve(name, filepath.Dir(p.DefPath))
		if err != nil {
			return context(err)
		}
		file, err := os.Open(path)
		if err != nil {
			return context(err)
		}
		colors, err := sff.ReadPaletteACT(file, false)
		file.Close()
		if err != nil {
			return context(fmt.Errorf("%s: %w", path, err))
		}
		bank := sff.Palette{GroupNumber: 1, ItemNumber: uint16(number), Colors: colors}
		if number == 1 && len(p.SFF.Palettes) > 0 {
			p.SFF.Palettes[0] = bank
			p.SFF.SharedPalette = colors
		} else {
			p.SFF.Palettes = append(p.SFF.Palettes, bank)
		}
	}
	// Rebuild only shared default images after pal1 replaces the embedded bank.
	// SpriteImage keeps index-zero masking separate from opaque ACT colors.
	if len(p.SFF.Palettes) > 0 && p.SFF.Palettes[0].ItemNumber == 1 {
		for index := range p.SFF.Sprites {
			sprite := &p.SFF.Sprites[index]
			if sprite.PaletteIndex != 0 {
				continue
			}
			var err error
			sprite.Image, err = p.SFF.SpriteImage(index, 0, true)
			if err != nil {
				return err
			}
			sprite.ImageOpaque, err = p.SFF.SpriteImage(index, 0, false)
			if err != nil {
				return err
			}
		}
	}
	return nil
}

// BindPalette creates round-local mutable mappings over immutable sprite data.
// Pre-1.0 V1 outfits are chosen by the engine; modern characters retain
// authored state5900/RemapPal, as specified by the beta upgrading guide.
// An unavailable V1 outfit falls back to the DEF's ordered pal.defaults, then
// the first available bank. No unrelated ACT is guessed from directory order.
func (p *Player) BindPalette(c *Character, number int) {
	if number < 1 || number > 12 {
		number = 1
	}
	c.Palettes = palette.New(p.SFF)
	c.PaletteNo = number
	if p.SFF == nil || p.SFF.Version[0] != 1 {
		return
	}
	if p.Definition != nil {
		version, err := strconv.ParseFloat(strings.TrimSpace(p.Definition.Info["mugenversion"]), 64)
		if err == nil && version >= 1 && version < 2 {
			return
		}
	}

	available := map[int]bool{}
	for _, bank := range p.SFF.Palettes {
		if bank.GroupNumber == 1 && bank.ItemNumber >= 1 && bank.ItemNumber <= 12 {
			available[int(bank.ItemNumber)] = true
		}
	}
	if !available[number] {
		number = 0
		if p.Definition != nil {
			for _, raw := range strings.Split(p.Definition.Info["pal.defaults"], ",") {
				n, err := strconv.Atoi(strings.TrimSpace(raw))
				if err == nil && available[n] {
					number = n
					break
				}
			}
		}
		if number == 0 {
			for n := 1; n <= 12; n++ {
				if available[n] {
					number = n
					break
				}
			}
		}
		if number == 0 {
			number = 1
		}
	}
	c.PaletteNo = number
	if number != 1 {
		if err := c.Palettes.SelectLegacy(palette.ID{1, number}); err != nil {
			c.EvaluationWarning("initial palette: " + err.Error())
		}
	}
}
