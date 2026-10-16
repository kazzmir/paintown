package font

import (
	"bufio"
	"bytes"
	"encoding/binary"
	"fmt"
	"image"
	"image/color"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

type Alignment int

const (
	AlignLeft Alignment = iota
	AlignRight
	AlignCenter
)

type FontType int

const (
	Fixed FontType = iota
	Variable
)

type Font struct {
	Type     FontType
	Width    int
	Height   int
	SpacingX int
	SpacingY int
	Colors   int
	OffsetX  int
	OffsetY  int

	Glyphs   map[rune]*Glyph
	Palettes []color.Palette

	imageCache map[int]map[rune]*ebiten.Image
	bankGlyphs map[int]map[rune]*Glyph
	fontV2     bool
	ttf        *trueTypeFont
	// Fallback records an explicit substitution when a Windows font is absent.
	Fallback string
}

type Glyph struct {
	X                int
	Width            int
	Image            image.Image
	OffsetX, OffsetY int
}

func LoadFont(path string) (*Font, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	if strings.EqualFold(filepath.Ext(path), ".def") {
		return loadV2(path, data)
	}

	if len(data) < 32 {
		return nil, fmt.Errorf("font file too short")
	}

	sig := string(data[:12])
	if !strings.HasPrefix(sig, "ElecbyteFnt") {
		return nil, fmt.Errorf("invalid font signature: %s", sig)
	}

	// versionHi := binary.LittleEndian.Uint16(data[12:14])
	// versionLo := binary.LittleEndian.Uint16(data[14:16])

	pcxOffset := binary.LittleEndian.Uint32(data[16:20])
	pcxSize := binary.LittleEndian.Uint32(data[20:24])
	txtOffset := binary.LittleEndian.Uint32(data[24:28])
	txtSize := binary.LittleEndian.Uint32(data[28:32])

	if uint64(pcxOffset)+uint64(pcxSize) > uint64(len(data)) || uint64(txtOffset)+uint64(txtSize) > uint64(len(data)) {
		return nil, fmt.Errorf("font data exceeds file size")
	}

	pcxData := data[pcxOffset : pcxOffset+pcxSize]
	txtData := data[txtOffset : txtOffset+txtSize]

	// Extract primary palette and decode PCX
	mainPal, err := sff.ExtractPalette(pcxData, true)
	if err != nil {
		return nil, fmt.Errorf("failed to extract palette: %v", err)
	}

	fullImg, err := sff.DecodePCX(pcxData, mainPal)
	if err != nil {
		return nil, fmt.Errorf("failed to decode PCX: %v", err)
	}

	fnt := &Font{
		Glyphs: make(map[rune]*Glyph),
	}

	// Parse TXT metadata
	scanner := bufio.NewScanner(bytes.NewReader(txtData))
	section := ""

	lineIdx := 0
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" || strings.HasPrefix(line, ";") {
			continue
		}

		if strings.HasPrefix(line, "[") && strings.HasSuffix(line, "]") {
			section = strings.ToLower(line[1 : len(line)-1])
			continue
		}

		switch section {
		case "def":
			parts := strings.SplitN(line, "=", 2)
			if len(parts) < 2 {
				continue
			}
			key := strings.ToLower(strings.TrimSpace(parts[0]))
			val := strings.TrimSpace(parts[1])

			switch key {
			case "type":
				if strings.ToLower(val) == "variable" {
					fnt.Type = Variable
				} else {
					fnt.Type = Fixed
				}
			case "size":
				val = strings.ReplaceAll(val, ",", " ")
				fmt.Sscanf(val, "%d %d", &fnt.Width, &fnt.Height)
			case "spacing":
				val = strings.ReplaceAll(val, ",", " ")
				fmt.Sscanf(val, "%d %d", &fnt.SpacingX, &fnt.SpacingY)
			case "colors":
				fnt.Colors, _ = strconv.Atoi(val)
			case "offset":
				val = strings.ReplaceAll(val, ",", " ")
				fmt.Sscanf(val, "%d %d", &fnt.OffsetX, &fnt.OffsetY)
			}
		case "map":
			// Map format: char [start_x width]
			// start_x and width only present for Variable fonts
			parts := strings.Fields(line)
			if len(parts) == 0 {
				continue
			}

			rawChar := parts[0]
			var char rune
			if strings.HasPrefix(rawChar, "0x") {
				hex, _ := strconv.ParseInt(rawChar[2:], 16, 32)
				char = rune(hex)
			} else {
				char = rune(rawChar[0])
			}

			startX := lineIdx * fnt.Width
			glyphWidth := fnt.Width

			if fnt.Type == Variable && len(parts) >= 3 {
				x, _ := strconv.Atoi(parts[1])
				w, _ := strconv.Atoi(parts[2])
				startX = x
				glyphWidth = w
			}

			// Clip glyph from fullImg
			if startX < 0 || startX >= fullImg.Bounds().Max.X || glyphWidth <= 0 || glyphWidth > 8192 || fnt.Height <= 0 || fnt.Height > fullImg.Bounds().Max.Y {
				return nil, fmt.Errorf("font glyph %q outside PCX or invalid size", char)
			}
			rect := image.Rect(startX, 0, startX+glyphWidth, fnt.Height)
			if rect.Max.X > fullImg.Bounds().Max.X {
				// Stock f-6x8f.fnt's last cell includes one byte of row
				// padding beyond xmax. Clip pixels while retaining its advance.
				rect.Max.X = fullImg.Bounds().Max.X
			}

			glyphImg := fullImg.SubImage(rect)
			fnt.Glyphs[char] = &Glyph{
				X:     startX,
				Width: glyphWidth,
				Image: glyphImg,
			}
			lineIdx++
		}
	}
	if err := scanner.Err(); err != nil {
		return nil, fmt.Errorf("font metadata: %w", err)
	}
	if len(fnt.Glyphs) == 0 {
		return nil, fmt.Errorf("font has no mapped glyphs")
	}

	// Generate extra palettes for banks
	if fnt.Colors > 0 {
		// MUGEN fonts often have multiple palettes at the end of the PCX.
		// Each bank is Colors * 3 bytes.
		// font.cpp: pcx + pcxsize - ((bank+1) * colors * 3)
		if fnt.Colors > 256 {
			return nil, fmt.Errorf("font colors %d exceeds palette", fnt.Colors)
		}
		fnt.Palettes = make([]color.Palette, 256/fnt.Colors)
		for b := range fnt.Palettes {
			palOffset := len(pcxData) - ((b + 1) * fnt.Colors * 3)
			if palOffset >= 0 && palOffset+fnt.Colors*3 <= len(pcxData) {
				palData := pcxData[palOffset : palOffset+fnt.Colors*3]
				bankPal := make(color.Palette, 256)
				// Copy main palette first
				copy(bankPal, mainPal)
				// Overwrite the last 'Colors' entries
				startIdx := 256 - fnt.Colors
				for i := 0; i < fnt.Colors; i++ {
					bankPal[startIdx+i] = color.RGBA{
						R: palData[i*3],
						G: palData[i*3+1],
						B: palData[i*3+2],
						A: 255,
					}
				}
				// Ensure index 0 is transparent as per MUGEN rules
				bankPal[0] = color.Transparent
				fnt.Palettes[b] = bankPal
			}
		}
	}

	return fnt, nil
}

func (f *Font) getGlyphImage(char rune, bank int) *ebiten.Image {
	if f.imageCache == nil {
		f.imageCache = make(map[int]map[rune]*ebiten.Image)
	}
	if f.imageCache[bank] == nil {
		f.imageCache[bank] = make(map[rune]*ebiten.Image)
	}

	if img, ok := f.imageCache[bank][char]; ok {
		return img
	}

	glyph := f.Glyph(char, bank)
	if glyph == nil {
		return nil
	}

	// Apply palette if bank is valid
	var pal color.Palette
	if bank >= 0 && bank < len(f.Palettes) && f.Palettes[bank] != nil {
		pal = f.Palettes[bank]
	}

	// Create new image with bank palette
	rect := glyph.Image.Bounds()
	if rect.Dx() <= 0 || rect.Dy() <= 0 {
		return nil
	}

	// Create a zero-origin image for the glyph
	targetRect := image.Rect(0, 0, rect.Dx(), rect.Dy())
	var img image.Image = glyph.Image
	if p, ok := glyph.Image.(*image.Paletted); ok {
		if pal == nil || f.fontV2 {
			pal = p.Palette
		}
		indexed := image.NewPaletted(targetRect, pal)
		for y := 0; y < rect.Dy(); y++ {
			srcStart := p.PixOffset(rect.Min.X, rect.Min.Y+y)
			dstStart := indexed.PixOffset(0, y)
			copy(indexed.Pix[dstStart:dstStart+rect.Dx()], p.Pix[srcStart:srcStart+rect.Dx()])
		}
		img = indexed
	}

	eImg := ebiten.NewImageFromImage(img)
	f.imageCache[bank][char] = eImg
	return eImg
}

func (f *Font) TextWidth(text string) int {
	return f.TextWidthForBank(text, 0)
}

func (f *Font) TextWidthForBank(text string, bank int) int {
	if f.ttf != nil {
		return f.ttf.width(text)
	}
	w := 0
	for _, char := range text {
		if glyph := f.Glyph(char, bank); glyph != nil {
			w += glyph.Width + f.SpacingX
		} else {
			w += f.Width + f.SpacingX
		}
	}
	return w
}

func (f *Font) Draw(screen *ebiten.Image, text string, x, y int, bank int, align Alignment) {
	width := f.TextWidthForBank(text, bank)
	startX := x + f.OffsetX
	// MUGEN treats Y as the baseline (bottom of the font cell), so we subtract
	// the font height to get the top-left origin for glyph rendering.
	startY := y + f.OffsetY - f.Height

	switch align {
	case AlignCenter:
		startX -= width / 2
	case AlignRight:
		startX -= width
	}

	currentX := startX
	var previous rune
	for _, char := range text {
		img := f.getGlyphImage(char, bank)
		glyph := f.Glyph(char, bank)
		if f.ttf != nil && previous != 0 {
			currentX += f.ttf.kern(previous, char)
		}

		if img != nil {
			op := &ebiten.DrawImageOptions{}
			op.GeoM.Translate(float64(currentX+glyph.OffsetX), float64(startY+glyph.OffsetY))
			op.Filter = ebiten.FilterNearest
			screen.DrawImage(img, op)
		}

		if glyph != nil {
			currentX += glyph.Width
			if f.ttf == nil {
				currentX += f.SpacingX
			}
		} else {
			currentX += f.Width + f.SpacingX
		}
		previous = char
	}
}

// Glyph returns CPU pixels/metrics for the requested bank; missing characters
// remain absent. V2 invalid banks use the documented default bank/palette.
func (f *Font) Glyph(char rune, bank int) *Glyph {
	if f.ttf != nil {
		return f.ttf.glyph(char, f.Height)
	}
	if f.fontV2 {
		if glyphs, ok := f.bankGlyphs[bank]; ok {
			return glyphs[char]
		}
	}
	return f.Glyphs[char]
}
