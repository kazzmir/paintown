package sff

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"io"
)

// Palette retains SFF's group/item identity, including linked palette nodes.
type Palette struct {
	GroupNumber, ItemNumber uint16
	Linked                  bool
	Colors                  color.Palette // Opaque colors; indexed sprite masking is separate.
}

const maxSpritePixels = 64 << 20
const maxArchivePixels = 512 << 20

type v2Sprite struct {
	group, item, width, height, x, y, link uint16
	format, depth                          byte
	offset, length                         uint32
	palette, flags                         uint16
}

// readRegion checks before allocation and never reads beyond a declared block.
func readRegion(r io.ReadSeeker, size, offset, length int64) ([]byte, error) {
	if offset < 0 || length < 0 || offset > size || length > size-offset {
		return nil, fmt.Errorf("region %d+%d outside file/block size %d", offset, length, size)
	}
	if _, err := r.Seek(offset, io.SeekStart); err != nil {
		return nil, err
	}
	b := make([]byte, int(length))
	_, err := io.ReadFull(r, b)
	return b, err
}

func parseV2(r io.ReadSeeker, h []byte) (*SFF, error) {
	size, err := r.Seek(0, io.SeekEnd)
	if err != nil {
		return nil, err
	}
	u32 := func(i int) int64 { return int64(binary.LittleEndian.Uint32(h[i : i+4])) }
	so, count, po, pals := u32(36), u32(40), u32(44), u32(48)
	lo, ll, to, tl := u32(52), u32(56), u32(60), u32(64)
	if count > (size/28) || pals > (size/16) {
		return nil, fmt.Errorf("SFF v2 table counts exceed file size")
	}
	for _, block := range [][2]int64{{lo, ll}, {to, tl}} {
		if block[0] > size || block[1] > size-block[0] {
			return nil, fmt.Errorf("SFF v2 data block outside file")
		}
	}
	sprites, err := readRegion(r, size, so, count*28)
	if err != nil {
		return nil, fmt.Errorf("SFF sprite table: %w", err)
	}
	palettes, err := readRegion(r, size, po, pals*16)
	if err != nil {
		return nil, fmt.Errorf("SFF palette table: %w", err)
	}
	out := &SFF{Version: [4]byte{h[15], h[14], h[13], h[12]}, Sprites: make([]Sprite, int(count)), Palettes: make([]Palette, int(pals))}
	// Palette and sprite links may form chains. Iterative resolution is bounded
	// by the table size, detecting cycles without recursive stack growth.
	palState := make([]byte, int(pals))
	for start := 0; start < int(pals); start++ {
		if palState[start] == 2 {
			continue
		}
		chain := []int{}
		idx := start
		for {
			if idx < 0 || idx >= int(pals) {
				return nil, fmt.Errorf("palette %d: invalid linked index %d", start, idx)
			}
			if palState[idx] == 2 {
				break
			}
			if palState[idx] == 1 {
				return nil, fmt.Errorf("palette %d: cyclic link", start)
			}
			palState[idx] = 1
			chain = append(chain, idx)
			p := palettes[idx*16 : (idx+1)*16]
			u16 := func(i int) uint16 { return binary.LittleEndian.Uint16(p[i : i+2]) }
			n, offset, length := int(u16(4)), int64(binary.LittleEndian.Uint32(p[8:12])), int64(binary.LittleEndian.Uint32(p[12:16]))
			out.Palettes[idx] = Palette{GroupNumber: u16(0), ItemNumber: u16(2), Linked: length == 0}
			if length == 0 {
				idx = int(u16(6))
				continue
			}
			if n < 1 || n > 256 || length != int64(n*4) {
				return nil, fmt.Errorf("palette %d: invalid color count/length %d/%d", idx, n, length)
			}
			if offset > ll || length > ll-offset {
				return nil, fmt.Errorf("palette %d outside ldata", idx)
			}
			data, err := readRegion(r, size, lo+offset, length)
			if err != nil {
				return nil, fmt.Errorf("palette %d: %w", idx, err)
			}
			colors := make(color.Palette, n)
			for c := range colors {
				colors[c] = color.NRGBA{R: data[c*4], G: data[c*4+1], B: data[c*4+2], A: 255}
			}
			out.Palettes[idx].Colors = colors
			palState[idx] = 2
			break
		}
		for j := len(chain) - 1; j >= 0; j-- {
			i := chain[j]
			if palState[i] == 2 {
				continue
			}
			p := palettes[i*16 : (i+1)*16]
			out.Palettes[i].Colors = out.Palettes[int(binary.LittleEndian.Uint16(p[6:8]))].Colors
			palState[i] = 2
		}
	}
	if len(out.Palettes) > 0 {
		out.SharedPalette = maskedPalette(out.Palettes[0].Colors)
	}
	nodes := make([]v2Sprite, int(count))
	for i := range nodes {
		p := sprites[i*28 : (i+1)*28]
		u16 := func(j int) uint16 { return binary.LittleEndian.Uint16(p[j : j+2]) }
		nodes[i] = v2Sprite{u16(0), u16(2), u16(4), u16(6), u16(8), u16(10), u16(12), p[14], p[15], binary.LittleEndian.Uint32(p[16:20]), binary.LittleEndian.Uint32(p[20:24]), u16(24), u16(26)}
	}
	state := make([]byte, int(count))
	budget := int64(0)
	for start := range nodes {
		if state[start] == 2 {
			continue
		}
		chain := []int{}
		idx := start
		for {
			if idx < 0 || idx >= len(nodes) {
				return nil, fmt.Errorf("sprite %d: invalid linked index %d", start, idx)
			}
			if state[idx] == 2 {
				break
			}
			if state[idx] == 1 {
				return nil, fmt.Errorf("sprite %d: cyclic link", start)
			}
			state[idx] = 1
			chain = append(chain, idx)
			n := nodes[idx]
			if n.length > 256<<20 {
				return nil, fmt.Errorf("sprite %d: excessive encoded length %d", idx, n.length)
			}
			out.Sprites[idx] = Sprite{GroupNumber: n.group, ImageNumber: n.item, XAxis: int16(n.x), YAxis: int16(n.y), Linked: n.length == 0, PaletteIndex: int(n.palette), Format: n.format, ColorDepth: n.depth}
			if n.length == 0 {
				idx = int(n.link)
				continue
			}
			pixels := int64(n.width) * int64(n.height)
			budget += pixels
			if pixels == 0 || pixels > maxSpritePixels || budget > maxArchivePixels {
				return nil, fmt.Errorf("sprite %d (%d,%d): invalid/excessive dimensions %dx%d", idx, n.group, n.item, n.width, n.height)
			}
			base, block := lo, ll
			if n.flags&1 != 0 {
				base, block = to, tl
			}
			if int64(n.offset) > block || int64(n.length) > block-int64(n.offset) {
				return nil, fmt.Errorf("sprite %d (%d,%d) outside data block", idx, n.group, n.item)
			}
			data, err := readRegion(r, size, base+int64(n.offset), int64(n.length))
			if err != nil {
				return nil, err
			}
			img, opaque, err := decodeV2(n, data, out.Palettes)
			if err != nil {
				return nil, fmt.Errorf("sprite %d (%d,%d) format %d: %w", idx, n.group, n.item, n.format, err)
			}
			out.Sprites[idx].Image, out.Sprites[idx].ImageOpaque = img, opaque
			state[idx] = 2
			break
		}
		for j := len(chain) - 1; j >= 0; j-- {
			i := chain[j]
			if state[i] == 2 {
				continue
			}
			linked := out.Sprites[int(nodes[i].link)]
			out.Sprites[i].Image, out.Sprites[i].ImageOpaque = linked.Image, linked.ImageOpaque
			// Linked pixels retain the link node's palette when indexed.
			if pixels, ok := linked.Image.(*image.Paletted); ok {
				if int(nodes[i].palette) >= len(out.Palettes) {
					return nil, fmt.Errorf("sprite %d: invalid palette %d", i, nodes[i].palette)
				}
				img, opaque, err := indexedImages(pixels.Pix, pixels.Bounds().Dx(), pixels.Bounds().Dy(), out.Palettes[nodes[i].palette].Colors)
				if err != nil {
					return nil, fmt.Errorf("sprite %d: %w", i, err)
				}
				out.Sprites[i].Image, out.Sprites[i].ImageOpaque = img, opaque
			}
			state[i] = 2
		}
	}
	return out, nil
}

func maskedPalette(p color.Palette) color.Palette {
	masked := append(color.Palette(nil), p...)
	if len(masked) > 0 {
		c := color.NRGBAModel.Convert(masked[0]).(color.NRGBA)
		c.A = 0
		masked[0] = c
	}
	return masked
}

func opaquePalette(p color.Palette) color.Palette {
	if len(p) == 0 {
		return nil
	}
	opaque := append(color.Palette(nil), p...)
	for i, c := range opaque {
		n := color.NRGBAModel.Convert(c).(color.NRGBA)
		n.A = 255
		opaque[i] = n
	}
	return opaque
}

func indexedImages(pixels []byte, w, h int, p color.Palette) (image.Image, image.Image, error) {
	for _, index := range pixels {
		if int(index) >= len(p) {
			return nil, nil, fmt.Errorf("pixel index %d outside palette of %d colors", index, len(p))
		}
	}
	r := image.Rect(0, 0, w, h)
	return &image.Paletted{Pix: pixels, Stride: w, Rect: r, Palette: maskedPalette(p)}, &image.Paletted{Pix: pixels, Stride: w, Rect: r, Palette: p}, nil
}

func decodeV2(n v2Sprite, data []byte, pals []Palette) (image.Image, image.Image, error) {
	w, h := int(n.width), int(n.height)
	count := w * h
	if n.format != 0 {
		if len(data) < 4 {
			return nil, nil, fmt.Errorf("missing decoded length")
		}
		expected := count
		if n.depth == 24 || n.depth == 32 {
			expected *= int(n.depth) / 8
		}
		if int64(binary.LittleEndian.Uint32(data)) != int64(expected) {
			return nil, nil, fmt.Errorf("decoded length differs from dimensions/depth")
		}
		data = data[4:]
	}
	var pixels []byte
	var err error
	switch n.format {
	case 0:
		if n.depth == 24 || n.depth == 32 {
			channels := int(n.depth) / 8
			if len(data) != count*channels {
				return nil, nil, fmt.Errorf("raw true-color length mismatch")
			}
			img := image.NewNRGBA(image.Rect(0, 0, w, h))
			for i := 0; i < count; i++ {
				copy(img.Pix[i*4:i*4+3], data[i*channels:i*channels+3])
				img.Pix[i*4+3] = 255
				if channels == 4 {
					img.Pix[i*4+3] = data[i*channels+3]
				}
			}
			return img, img, nil
		}
		if n.depth != 8 && n.depth != 5 {
			return nil, nil, fmt.Errorf("raw color depth %d is unsupported", n.depth)
		}
		if len(data) != count {
			return nil, nil, fmt.Errorf("raw length %d, want %d", len(data), count)
		}
		pixels = data
	case 2, 3, 4:
		if n.depth != 8 && n.depth != 5 {
			return nil, nil, fmt.Errorf("invalid indexed color depth %d", n.depth)
		}
		pixels, err = decodeIndexed(n.format, data, count)
		if err != nil {
			return nil, nil, err
		}
	case 10, 11, 12:
		config, err := png.DecodeConfig(bytes.NewReader(data))
		if err != nil {
			return nil, nil, err
		}
		if config.Width != w || config.Height != h {
			return nil, nil, fmt.Errorf("PNG dimensions differ from sprite header")
		}
		img, err := png.Decode(bytes.NewReader(data))
		if err != nil {
			return nil, nil, err
		}
		if n.format == 10 {
			paletted, ok := img.(*image.Paletted)
			if !ok || n.depth != 8 {
				return nil, nil, fmt.Errorf("PNG8 requires an indexed 8-bit image")
			}
			pixels = paletted.Pix
		} else {
			if (n.format == 11 && n.depth != 24) || (n.format == 12 && n.depth != 32) {
				return nil, nil, fmt.Errorf("PNG color depth mismatch")
			}
			return img, img, nil
		}
	default:
		return nil, nil, fmt.Errorf("unsupported compression format")
	}
	if int(n.palette) >= len(pals) {
		return nil, nil, fmt.Errorf("palette %d not found", n.palette)
	}
	return indexedImages(pixels, w, h, pals[n.palette].Colors)
}

func decodeIndexed(format byte, data []byte, count int) ([]byte, error) {
	out := make([]byte, 0, count)
	pos := 0
	read := func() (byte, error) {
		if pos >= len(data) {
			return 0, io.ErrUnexpectedEOF
		}
		b := data[pos]
		pos++
		return b, nil
	}
	run := func(value byte, n int) error {
		if n <= 0 || n > count-len(out) {
			return fmt.Errorf("run length %d exceeds remaining pixels %d", n, count-len(out))
		}
		for i := 0; i < n; i++ {
			out = append(out, value)
		}
		return nil
	}
	copyRun := func(offset, n int) error {
		if offset <= 0 || offset > len(out) || n > count-len(out) {
			return fmt.Errorf("invalid backreference offset=%d length=%d at pixel %d", offset, n, len(out))
		}
		for i := 0; i < n; i++ {
			out = append(out, out[len(out)-offset])
		}
		return nil
	}
	var recycled byte
	shorts := 0
	for pos < len(data) {
		switch format {
		case 2:
			b, _ := read()
			value, n := b, 1
			if b&0xc0 == 0x40 {
				var err error
				value, err = read()
				if err != nil {
					return nil, err
				}
				n = int(b & 63)
			}
			if err := run(value, n); err != nil {
				return nil, err
			}
		case 3:
			a, _ := read()
			b, err := read()
			if err != nil {
				return nil, err
			}
			value := byte(0)
			if b&128 != 0 {
				value, err = read()
				if err != nil {
					return nil, err
				}
			}
			if err := run(value, int(a)+1); err != nil {
				return nil, err
			}
			for j := 0; j < int(b&127); j++ {
				v, err := read()
				if err != nil {
					return nil, err
				}
				if err := run(v&31, int(v>>5)+1); err != nil {
					return nil, err
				}
			}
		case 4:
			control, _ := read()
			for bit := 0; bit < 8 && pos < len(data); bit++ {
				b, _ := read()
				if control&(1<<bit) == 0 {
					n := int(b >> 5)
					if n == 0 {
						v, err := read()
						if err != nil {
							return nil, err
						}
						n = int(v) + 8
					}
					if err := run(b&31, n); err != nil {
						return nil, err
					}
				} else {
					n, offset := int(b&63)+1, 0
					if b&63 == 0 {
						v, err := read()
						if err != nil {
							return nil, err
						}
						offset = (int(b)<<2 | int(v)) + 1
						v, err = read()
						if err != nil {
							return nil, err
						}
						n = int(v) + 3
					} else {
						recycled = recycled<<2 | b>>6
						shorts++
						if shorts == 4 {
							offset = int(recycled) + 1
							shorts = 0
							recycled = 0
						} else {
							v, err := read()
							if err != nil {
								return nil, err
							}
							offset = int(v) + 1
						}
					}
					if err := copyRun(offset, n); err != nil {
						return nil, err
					}
				}
			}
		default:
			return nil, fmt.Errorf("invalid indexed compression %d", format)
		}
	}
	if len(out) != count {
		return nil, fmt.Errorf("decoded %d pixels, want %d", len(out), count)
	}
	return out, nil
}
