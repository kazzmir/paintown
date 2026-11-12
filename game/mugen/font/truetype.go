package font

import (
	"errors"
	"fmt"
	"image"
	"os"
	"path/filepath"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/resource"
	"golang.org/x/image/font"
	"golang.org/x/image/font/gofont/goregular"
	"golang.org/x/image/font/opentype"
	"golang.org/x/image/math/fixed"
)

type trueTypeFont struct {
	face   font.Face
	blend  bool
	glyphs map[rune]*Glyph
}

func loadTrueType(defPath, name string, f *Font, blend bool) (*Font, error) {
	name = filepath.FromSlash(strings.ReplaceAll(name, "\\", "/"))
	search := []string{filepath.Dir(defPath)}
	search = append(search, filepath.SplitList(os.Getenv("MUGEN_FONT_DIRS"))...)
	if windows := os.Getenv("WINDIR"); windows != "" {
		search = append(search, filepath.Join(windows, "Fonts"))
	}
	search = append(search, "/usr/share/fonts/truetype/msttcorefonts")
	path, err := resource.Resolve(name, search...)
	var data []byte
	if errors.Is(err, os.ErrNotExist) {
		data = goregular.TTF
		f.Fallback = name + " -> embedded Go Regular"
	} else if err != nil {
		return nil, fmt.Errorf("%s: TrueType file: %w", defPath, err)
	} else {
		data, err = os.ReadFile(path)
		if err != nil {
			return nil, fmt.Errorf("%s: TrueType file %s: %w", defPath, path, err)
		}
	}
	collection, err := opentype.ParseCollection(data)
	if err != nil {
		return nil, fmt.Errorf("%s: TrueType: %w", defPath, err)
	}
	parsed, err := collection.Font(0)
	if err != nil {
		return nil, fmt.Errorf("%s: TrueType face: %w", defPath, err)
	}
	face, err := opentype.NewFace(parsed, &opentype.FaceOptions{Size: float64(f.Height), DPI: 72, Hinting: font.HintingFull})
	if err != nil {
		return nil, fmt.Errorf("%s: TrueType rasterizer: %w", defPath, err)
	}
	f.ttf = &trueTypeFont{face: face, blend: blend, glyphs: make(map[rune]*Glyph)}
	return f, nil
}

func (f *trueTypeFont) kern(a, b rune) int { return f.face.Kern(a, b).Round() }
func (f *trueTypeFont) width(text string) int {
	w := 0
	var previous rune
	for _, r := range text {
		if previous != 0 {
			w += f.kern(previous, r)
		}
		advance, _ := f.face.GlyphAdvance(r)
		w += advance.Ceil()
		previous = r
	}
	return w
}

func (f *trueTypeFont) glyph(r rune, height int) *Glyph {
	if g, ok := f.glyphs[r]; ok {
		return g
	}
	bounds, advance, _ := f.face.GlyphBounds(r)
	rect := image.Rect(bounds.Min.X.Floor(), bounds.Min.Y.Floor(), bounds.Max.X.Ceil(), bounds.Max.Y.Ceil())
	if rect.Dx() > 8192 || rect.Dy() > 8192 || rect.Dx() < 0 || rect.Dy() < 0 {
		return nil
	}
	img := image.NewAlpha(image.Rect(0, 0, rect.Dx(), rect.Dy()))
	drawer := font.Drawer{Dst: img, Src: image.White, Face: f.face, Dot: fixed.P(-rect.Min.X, -rect.Min.Y)}
	drawer.DrawString(string(r))
	if !f.blend {
		for i, a := range img.Pix {
			if a >= 128 {
				img.Pix[i] = 255
			} else {
				img.Pix[i] = 0
			}
		}
	}
	g := &Glyph{Width: advance.Ceil(), Image: img, OffsetX: rect.Min.X, OffsetY: height + rect.Min.Y}
	f.glyphs[r] = g
	return g
}
