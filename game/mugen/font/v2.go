package font

import (
	"bytes"
	"fmt"
	"image"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

func loadV2(path string, data []byte) (*Font, error) {
	ast, err := parsers.Parse(bytes.NewReader(data))
	if err != nil {
		return nil, fmt.Errorf("%s: %w", path, err)
	}
	values := make(map[string]string)
	version := ""
	for _, section := range ast.Sections {
		for _, a := range section.Attributes {
			if strings.EqualFold(section.Name, "fnt v2") && strings.EqualFold(a.ID.String(), "fntversion") {
				version = a.RawValue
			}
			if strings.EqualFold(section.Name, "def") {
				values[strings.ToLower(a.ID.String())] = strings.Trim(strings.TrimSpace(a.RawValue), "\"")
			}
		}
	}
	f := &Font{Type: Variable, fontV2: true, Glyphs: make(map[rune]*Glyph), bankGlyphs: make(map[int]map[rune]*Glyph)}
	var major, minor int
	if _, err := fmt.Sscanf(strings.ReplaceAll(version, ",", " "), "%d %d", &major, &minor); err != nil || major != 2 || minor != 0 {
		return nil, fmt.Errorf("%s: unsupported FNT v2 version %q", path, version)
	}
	for _, spec := range []struct {
		key      string
		x, y     *int
		required bool
	}{{"size", &f.Width, &f.Height, true}, {"spacing", &f.SpacingX, &f.SpacingY, false}, {"offset", &f.OffsetX, &f.OffsetY, false}} {
		raw, present := values[spec.key]
		if !present {
			if spec.required {
				return nil, fmt.Errorf("%s: missing font %s", path, spec.key)
			}
			continue
		}
		parts := parsers.SplitList(raw)
		if len(parts) != 2 {
			return nil, fmt.Errorf("%s: invalid font %s = %s", path, spec.key, raw)
		}
		x, e1 := strconv.Atoi(strings.TrimSpace(parts[0]))
		y, e2 := strconv.Atoi(strings.TrimSpace(parts[1]))
		if e1 != nil || e2 != nil {
			return nil, fmt.Errorf("%s: invalid font %s = %s", path, spec.key, raw)
		}
		*spec.x, *spec.y = x, y
	}
	if f.Width < 0 || f.Height <= 0 || f.Height > 2048 {
		return nil, fmt.Errorf("%s: invalid font size %dx%d", path, f.Width, f.Height)
	}
	file := values["file"]
	if file == "" {
		return nil, fmt.Errorf("%s: missing font file", path)
	}
	switch strings.ToLower(values["type"]) {
	case "truetype":
		blend := values["blend"] != "0"
		return loadTrueType(path, file, f, blend)
	case "bitmap":
		bankType := strings.ToLower(values["banktype"])
		if bankType == "" {
			bankType = "palette"
		}
		if bankType != "palette" && bankType != "sprite" {
			return nil, fmt.Errorf("%s: invalid BankType %q", path, bankType)
		}
		resource := filepath.Join(filepath.Dir(path), filepath.FromSlash(strings.ReplaceAll(file, "\\", "/")))
		source, err := os.Open(resource)
		if err != nil {
			return nil, fmt.Errorf("%s: font sprite file: %w", path, err)
		}
		defer source.Close()
		archive, err := sff.Parse(source)
		if err != nil {
			return nil, fmt.Errorf("%s: %w", path, err)
		}
		if archive.Version[0] != 2 || len(archive.Sprites) == 0 {
			return nil, fmt.Errorf("%s: bitmap font requires nonempty SFF v2", path)
		}
		makeGlyph := func(index, palette int) (*Glyph, error) {
			s := archive.Sprites[index]
			img, err := archive.SpriteImage(index, palette, true)
			if err != nil {
				return nil, err
			}
			return &Glyph{Width: img.Bounds().Dx() - int(s.XAxis), Image: img, OffsetX: -int(s.XAxis), OffsetY: -int(s.YAxis)}, nil
		}
		for i, s := range archive.Sprites {
			if s.ImageNumber < 32 || s.ImageNumber > 126 {
				continue
			}
			if bankType == "palette" && s.GroupNumber != 0 {
				continue
			}
			g, err := makeGlyph(i, -1)
			if err != nil {
				return nil, err
			}
			bank := int(s.GroupNumber)
			if f.bankGlyphs[bank] == nil {
				f.bankGlyphs[bank] = make(map[rune]*Glyph)
			}
			f.bankGlyphs[bank][rune(s.ImageNumber)] = g
		}
		if bankType == "palette" {
			base := f.bankGlyphs[0]
			f.Glyphs = base
			for p, palette := range archive.Palettes {
				if palette.GroupNumber != 0 {
					continue
				}
				glyphs := make(map[rune]*Glyph)
				for i, s := range archive.Sprites {
					if s.GroupNumber == 0 && s.ImageNumber >= 32 && s.ImageNumber <= 126 {
						g, err := makeGlyph(i, p)
						if err != nil {
							return nil, fmt.Errorf("%s bank %d: %w", path, palette.ItemNumber, err)
						}
						glyphs[rune(s.ImageNumber)] = g
					}
				}
				f.bankGlyphs[int(palette.ItemNumber)] = glyphs
			}
			// Invalid bank uses the first sprite's palette, not necessarily bank 0.
			if _, indexed := archive.Sprites[0].Image.(*image.Paletted); indexed {
				f.Glyphs = make(map[rune]*Glyph)
				for i, s := range archive.Sprites {
					if s.GroupNumber == 0 && s.ImageNumber >= 32 && s.ImageNumber <= 126 {
						g, err := makeGlyph(i, archive.Sprites[0].PaletteIndex)
						if err != nil {
							return nil, err
						}
						f.Glyphs[rune(s.ImageNumber)] = g
					}
				}
			}
		} else {
			f.Glyphs = f.bankGlyphs[0]
		}
		if len(f.Glyphs) == 0 {
			return nil, fmt.Errorf("%s: no default-bank ASCII glyphs", path)
		}
		return f, nil
	default:
		return nil, fmt.Errorf("%s: unsupported font type %q", path, values["type"])
	}
}
