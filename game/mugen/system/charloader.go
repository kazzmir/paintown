package main

import (
	"image"
	"image/color"
	"os"
	"path/filepath"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

// CharacterPortrait holds portrait images and metadata for a character.
type CharacterPortrait struct {
	Name            string        // display name
	DefPath         string        // absolute path to the .def file
	IntroStoryboard string        // absolute path to intro storyboard (may be empty)
	SmallPortrait   *ebiten.Image // group 9000, image 0 - used in grid cells
	BigPortrait     *ebiten.Image // group 9000, image 1 - used in face panel
}

// loadCharacters reads select.def and loads portrait sprites for each character.
// dataDir is the root mugen data directory (e.g. "data-new/mugen").
func loadCharacters(selectDefPath string, charsDir string) []*CharacterPortrait {
	f, err := os.Open(selectDefPath)
	if err != nil {
		return nil
	}
	defer f.Close()

	sd, err := parsers.ParseSelectDef(f)
	if err != nil {
		return nil
	}

	var chars []*CharacterPortrait
	for _, entry := range sd.Characters {
		if entry.Name == "randomselect" {
			chars = append(chars, &CharacterPortrait{Name: "Random"})
			continue
		}

		cp := &CharacterPortrait{Name: entry.Name}

		// Resolve path to character .def file
		charDefPath := resolveCharPath(entry.Name, charsDir)
		if charDefPath == "" {
			chars = append(chars, cp)
			continue
		}
		cp.DefPath = charDefPath

		// Parse the character .def to find its SFF file
		sffPath := findCharSFF(charDefPath)
		if sffPath == "" {
			chars = append(chars, cp)
			continue
		}

		// Parse the SFF and extract portraits
		sffFile, err := os.Open(sffPath)
		if err != nil {
			chars = append(chars, cp)
			continue
		}
		charSFF, err := sff.Parse(sffFile)
		sffFile.Close()
		if err != nil {
			chars = append(chars, cp)
			continue
		}

		// Read display name and arcade info
		cp.Name = readCharDisplayName(charDefPath, entry.Name)
		cp.IntroStoryboard = readCharIntroStoryboard(charDefPath)

		// Extract portrait sprites
		for i := range charSFF.Sprites {
			sp := &charSFF.Sprites[i]
			if sp.GroupNumber != 9000 || sp.Image == nil {
				continue
			}
			img := imageToEbiten(sp.Image)
			if img == nil {
				continue
			}
			switch sp.ImageNumber {
			case 0:
				cp.SmallPortrait = img
			case 1:
				cp.BigPortrait = img
			}
		}

		chars = append(chars, cp)
	}
	return chars
}

// resolveCharPath finds the character's .def file path.
func resolveCharPath(name string, charsDir string) string {
	// name can be "kfm" or "kfm/alt-kfm.def"
	if strings.HasSuffix(strings.ToLower(name), ".def") {
		p := filepath.Join(charsDir, name)
		if _, err := os.Stat(p); err == nil {
			return p
		}
		return ""
	}
	// Try <charsDir>/<name>/<name>.def
	p := filepath.Join(charsDir, name, name+".def")
	if _, err := os.Stat(p); err == nil {
		return p
	}
	return ""
}

// findCharSFF parses a character .def file and returns the resolved SFF path.
func findCharSFF(defPath string) string {
	f, err := os.Open(defPath)
	if err != nil {
		return ""
	}
	defer f.Close()

	ast, err := parsers.Parse(f)
	if err != nil {
		return ""
	}

	charDir := filepath.Dir(defPath)
	for _, sec := range ast.Sections {
		if strings.ToLower(strings.TrimSpace(sec.Name)) == "files" {
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				if id == "sprite" || id == "spr" {
					val := ""
					if sv, ok := attr.Value.(parsers.StringValue); ok {
						val = sv.Val
					} else if kv, ok := attr.Value.(parsers.KeywordValue); ok {
						val = kv.Val
					}
					val = strings.TrimSpace(val)
					if val == "" {
						continue
					}
					p := filepath.Join(charDir, val)
					if _, err := os.Stat(p); err == nil {
						return p
					}
				}
			}
		}
	}
	return ""
}

// readCharDisplayName reads the "displayname" or "name" from a character .def.
func readCharDisplayName(defPath, fallback string) string {
	f, err := os.Open(defPath)
	if err != nil {
		return fallback
	}
	defer f.Close()

	ast, err := parsers.Parse(f)
	if err != nil {
		return fallback
	}

	for _, sec := range ast.Sections {
		if strings.ToLower(strings.TrimSpace(sec.Name)) == "info" {
			for _, attr := range sec.Attributes {
				id := strings.ToLower(attr.ID.String())
				if id == "displayname" || id == "name" {
					if sv, ok := attr.Value.(parsers.StringValue); ok && sv.Val != "" {
						return sv.Val
					}
					if kv, ok := attr.Value.(parsers.KeywordValue); ok && kv.Val != "" {
						return kv.Val
					}
				}
			}
		}
	}
	return fallback
}

// readCharIntroStoryboard reads the intro.storyboard path from the [Arcade] section.
// Returns an absolute path, or "" if not found.
func readCharIntroStoryboard(defPath string) string {
	f, err := os.Open(defPath)
	if err != nil {
		return ""
	}
	defer f.Close()

	ast, err := parsers.Parse(f)
	if err != nil {
		return ""
	}

	charDir := filepath.Dir(defPath)
	for _, sec := range ast.Sections {
		if strings.ToLower(strings.TrimSpace(sec.Name)) == "arcade" {
			for _, attr := range sec.Attributes {
				if strings.ToLower(attr.ID.String()) == "intro.storyboard" {
					val := ""
					if sv, ok := attr.Value.(parsers.StringValue); ok {
						val = sv.Val
					} else if kv, ok := attr.Value.(parsers.KeywordValue); ok {
						val = kv.Val
					}
					val = strings.TrimSpace(val)
					if val == "" {
						continue
					}
					p := filepath.Join(charDir, val)
					if _, err := os.Stat(p); err == nil {
						return p
					}
				}
			}
		}
	}
	return ""
}

// imageToEbiten converts a standard image.Image to an *ebiten.Image.
func imageToEbiten(img image.Image) *ebiten.Image {
	if img == nil {
		return nil
	}
	b := img.Bounds()
	if b.Dx() == 0 || b.Dy() == 0 {
		return nil
	}
	result := ebiten.NewImage(b.Dx(), b.Dy())
	pix := make([]byte, 4*b.Dx()*b.Dy())
	for y := b.Min.Y; y < b.Max.Y; y++ {
		for x := b.Min.X; x < b.Max.X; x++ {
			r, g, B, a := color.RGBAModel.Convert(img.At(x, y)).RGBA()
			idx := ((y-b.Min.Y)*b.Dx() + (x - b.Min.X)) * 4
			pix[idx+0] = byte(r >> 8)
			pix[idx+1] = byte(g >> 8)
			pix[idx+2] = byte(B >> 8)
			pix[idx+3] = byte(a >> 8)
		}
	}
	result.WritePixels(pix)
	return result
}
