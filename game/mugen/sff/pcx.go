package sff

import (
	"encoding/binary"
	"fmt"
	"image"
	"image/color"
	"io"
)

// DecodePCX decodes an 8-bit PCX image from the given byte slice.
// If the palette is not nil, it is applied directly to the image.
func DecodePCX(data []byte, palette color.Palette) (*image.Paletted, error) {
	return decodePCX(data, palette, true)
}

// SFF shared-palette sprites may omit the entire palette trailer. Their pixel
// bytes can coincidentally contain 0x0c at the usual trailer offset, so only
// strip a trailer when the enclosing archive declares an embedded palette.
func decodePCX(data []byte, palette color.Palette, embeddedPalette bool) (*image.Paletted, error) {
	width, height, bytesPerLine, err := pcxLayout(data)
	if err != nil {
		return nil, err
	}
	if palette == nil {
		palette = make(color.Palette, 256)
		for i := range palette {
			palette[i] = color.Black
		}
	}

	img := image.NewPaletted(image.Rect(0, 0, width, height), palette)
	pixelEnd := len(data)
	if embeddedPalette && len(data) >= 897 && data[len(data)-769] == 12 {
		pixelEnd -= 769
	}
	position, decoded := 128, 0
	expected := bytesPerLine * height
	for decoded < expected {
		if position >= pixelEnd {
			return nil, fmt.Errorf("PCX pixels: %w", io.ErrUnexpectedEOF)
		}
		value, count := data[position], 1
		position++
		if data[2] == 1 && value&0xc0 == 0xc0 {
			count = int(value & 0x3f)
			if count == 0 || count > expected-decoded {
				return nil, fmt.Errorf("invalid PCX run length %d", count)
			}
			if position >= pixelEnd {
				return nil, fmt.Errorf("PCX run: %w", io.ErrUnexpectedEOF)
			}
			value = data[position]
			position++
		}
		// Consume row padding without retaining a second full decoded plane.
		// A run may span padding and the next row, as in legacy SFF tools.
		for count > 0 {
			row, column := decoded/bytesPerLine, decoded%bytesPerLine
			span := min(count, bytesPerLine-column)
			if column < width {
				if int(value) >= len(palette) {
					return nil, fmt.Errorf("PCX row %d: pixel index %d outside palette of %d colors", row, value, len(palette))
				}
				pixels := img.Pix[row*img.Stride+column : row*img.Stride+column+min(span, width-column)]
				for i := range pixels {
					pixels[i] = value
				}
			}
			decoded += span
			count -= span
		}
	}
	return img, nil
}

// pcxLayout validates dimensions before callers allocate decoded pixels.
func pcxLayout(data []byte) (width, height, bytesPerLine int, err error) {
	if len(data) < 128 {
		return 0, 0, 0, fmt.Errorf("data too short for PCX header")
	}
	if data[0] != 10 || data[3] != 8 || data[65] != 1 || data[2] > 1 {
		return 0, 0, 0, fmt.Errorf("unsupported PCX format: manufacturer=%d encoding=%d bits=%d planes=%d", data[0], data[2], data[3], data[65])
	}

	// Read dimensions from header
	xmin := int(binary.LittleEndian.Uint16(data[4:6]))
	ymin := int(binary.LittleEndian.Uint16(data[6:8]))
	xmax := int(binary.LittleEndian.Uint16(data[8:10]))
	ymax := int(binary.LittleEndian.Uint16(data[10:12]))

	width = xmax - xmin + 1
	height = ymax - ymin + 1

	if width <= 0 || height <= 0 || width > 10000 || height > 10000 {
		return 0, 0, 0, fmt.Errorf("invalid PCX dimensions: %dx%d", width, height)
	}

	bytesPerLine = int(binary.LittleEndian.Uint16(data[66:68]))
	if bytesPerLine < width || int64(bytesPerLine)*int64(height) > maxSpritePixels {
		return 0, 0, 0, fmt.Errorf("invalid PCX row size %d for %dx%d", bytesPerLine, width, height)
	}
	return width, height, bytesPerLine, nil
}

// ExtractPalette reads the 768-byte palette from the end of the PCX payload.
// 8-bit PCX files have a palette at the end, preceded by a byte with value 12 (0x0C).
func ExtractPalette(data []byte, transparent bool) (color.Palette, error) {
	if len(data) >= 769 && data[len(data)-769] == 12 {
		paletteData := data[len(data)-768:]
		return createPalette(paletteData, transparent), nil
	}

	// Some MUGEN SFF tools might omit the 0x0C marker.
	if len(data) >= 768 {
		paletteData := data[len(data)-768:]
		// We can't be 100% sure it's a palette, but in SFF v1 with samePalette=0, it usually is.
		return createPalette(paletteData, transparent), nil
	}

	return nil, fmt.Errorf("not enough data for PCX palette (%d bytes)", len(data))
}

func createPalette(paletteData []byte, transparent bool) color.Palette {
	pal := make(color.Palette, 256)
	for i := 0; i < 256; i++ {
		pal[i] = color.RGBA{
			R: paletteData[i*3],
			G: paletteData[i*3+1],
			B: paletteData[i*3+2],
			A: 255,
		}
	}
	if transparent {
		// Index 0 is transparent in MUGEN
		c := color.NRGBAModel.Convert(pal[0]).(color.NRGBA)
		c.A = 0
		pal[0] = c
	}
	return pal
}

// ReadPaletteACT reads a 768-byte palette from r and reverses the color order.
// M.U.G.E.N .act files often store the transparent color at the end (index 255),
// so reversing the palette puts it at index 0 where M.U.G.E.N expects it.
func ReadPaletteACT(r io.Reader, transparent bool) (color.Palette, error) {
	data := make([]byte, 768)
	if _, err := io.ReadFull(r, data); err != nil {
		return nil, err
	}
	pal := make(color.Palette, 256)
	for i := 0; i < 256; i++ {
		// Reverse the color order: color i comes from file p	for i := 0; i < 256; i++ {
		// Reverse the color order: color i comes from file position 255-i
		srcIdx := (255 - i) * 3
		pal[i] = color.RGBA{
			R: data[srcIdx],
			G: data[srcIdx+1],
			B: data[srcIdx+2],
			A: 255,
		}
	}
	if transparent {
		// Index 0 is transparent in MUGEN
		c := color.NRGBAModel.Convert(pal[0]).(color.NRGBA)
		c.A = 0
		pal[0] = c
	}
	return pal, nil
}
