package sff

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"image"
	"image/color"
	"io"
)

// DecodePCX decodes an 8-bit PCX image from the given byte slice.
// If the palette is not nil, it is applied directly to the image.
func DecodePCX(data []byte, palette color.Palette) (*image.Paletted, error) {
	if len(data) < 128 {
		return nil, fmt.Errorf("data too short for PCX header")
	}

	// Read dimensions from header
	xmin := int(binary.LittleEndian.Uint16(data[4:6]))
	ymin := int(binary.LittleEndian.Uint16(data[6:8]))
	xmax := int(binary.LittleEndian.Uint16(data[8:10]))
	ymax := int(binary.LittleEndian.Uint16(data[10:12]))

	width := xmax - xmin + 1
	height := ymax - ymin + 1

	if width <= 0 || height <= 0 || width > 10000 || height > 10000 {
		return nil, fmt.Errorf("invalid PCX dimensions: %dx%d", width, height)
	}

	bytesPerLine := int(binary.LittleEndian.Uint16(data[66:68]))

	if palette == nil {
		palette = make(color.Palette, 256)
		// default to black
	}

	img := image.NewPaletted(image.Rect(0, 0, width, height), palette)

	// Decode RLE
	r := bytes.NewReader(data[128:])

	expectedBytes := bytesPerLine * height
	pixelData := make([]byte, expectedBytes)
	idx := 0

	for idx < expectedBytes {
		b, err := r.ReadByte()
		if err != nil {
			break
		}

		if (b & 0xC0) == 0xC0 {
			runLength := int(b & 0x3F)
			colorByte, err := r.ReadByte()
			if err != nil {
				break
			}
			for i := 0; i < runLength && idx < expectedBytes; i++ {
				pixelData[idx] = colorByte
				idx++
			}
		} else {
			pixelData[idx] = b
			idx++
		}
	}

	for y := 0; y < height; y++ {
		srcStart := y * bytesPerLine
		dstStart := y * img.Stride
		// copy up to width bytes (bytesPerLine is often padded to even number)
		copy(img.Pix[dstStart:dstStart+width], pixelData[srcStart:srcStart+width])
	}

	return img, nil
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
		pal[0] = color.Transparent
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
		pal[0] = color.Transparent
	}
	return pal, nil
}
