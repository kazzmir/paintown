package sff

import (
	"bytes"
	"encoding/binary"
	"image"
	"image/color"
	"os"
	"testing"
)

func formatFixture(t *testing.T) []byte {
	t.Helper()
	b, err := os.ReadFile("testdata/formats.sff")
	if err != nil {
		t.Fatal(err)
	}
	return b
}

func TestReferenceEncodedFormats(t *testing.T) {
	archive, err := Parse(bytes.NewReader(formatFixture(t)))
	if err != nil {
		t.Fatal(err)
	}
	formats := []byte{3, 10, 11, 12, 0, 0}
	depths := []byte{5, 8, 24, 32, 24, 32}
	if len(archive.Sprites) != 6 {
		t.Fatal(len(archive.Sprites))
	}
	for i, s := range archive.Sprites {
		if s.Format != formats[i] || s.ColorDepth != depths[i] {
			t.Fatalf("sprite %d format/depth=%d/%d", i, s.Format, s.ColorDepth)
		}
		for y := 0; y < s.Image.Bounds().Dy(); y++ {
			for x := 0; x < s.Image.Bounds().Dx(); x++ {
				var want color.NRGBA
				if i < 2 {
					index := (x/4 + y) % 31
					if i == 1 {
						index = (x*13 + y*7) % 256
					}
					want = color.NRGBA{byte(index * 17 % 256), byte(index * 53 % 256), byte(index * 97 % 256), 255}
				} else {
					want = color.NRGBA{byte(x * 31), byte(y * 67), 75, 255}
					if i == 3 || i == 5 {
						want.A = byte(x * 31)
					}
				}
				got := color.NRGBAModel.Convert(s.ImageOpaque.At(x, y)).(color.NRGBA)
				// Fully transparent PNG pixels have no visible RGB requirement.
				if got.A != want.A || (want.A != 0 && got != want) {
					t.Fatalf("sprite %d pixel %d,%d=%v want %v", i, x, y, got, want)
				}
				if i < 2 {
					masked := color.NRGBAModel.Convert(s.Image.At(x, y)).(color.NRGBA)
					if index := s.Image.(*image.Paletted).ColorIndexAt(x, y); index == 0 && masked.A != 0 {
						t.Fatal("index zero is not masked")
					}
				}
			}
		}
	}
	if archive.Sprites[0].XAxis != -3 || archive.Sprites[1].YAxis != -4 {
		t.Fatal("signed axes")
	}
}

func TestIndexedPackets(t *testing.T) {
	for _, tc := range []struct {
		name       string
		format     byte
		data, want []byte
	}{
		{"rle8", 2, []byte{0x43, 7, 8}, []byte{7, 7, 7, 8}},
		{"rle5", 3, []byte{1, 0x81, 9, 0x23}, []byte{9, 9, 3, 3}},
		{"lz5-overlap", 4, []byte{2, 0x21, 3, 0}, []byte{1, 1, 1, 1, 1}},
		{"lz5-long", 4, []byte{2, 0x21, 0, 0, 0}, []byte{1, 1, 1, 1}},
		{"lz5-recycle", 4, []byte{0x1e, 0x21, 1, 0, 1, 0, 1, 0, 1}, bytes.Repeat([]byte{1}, 9)},
	} {
		t.Run(tc.name, func(t *testing.T) {
			got, err := decodeIndexed(tc.format, tc.data, len(tc.want))
			if err != nil {
				t.Fatal(err)
			}
			if !bytes.Equal(got, tc.want) {
				t.Fatalf("%v want %v", got, tc.want)
			}
		})
	}
	for _, tc := range []struct {
		format byte
		data   []byte
		count  int
	}{
		{2, []byte{0x43}, 3}, {2, []byte{0x43, 1}, 2}, {2, []byte{1}, 2},
		{3, []byte{1, 0x80}, 2}, {3, []byte{4, 0}, 2},
		{4, []byte{1, 1, 0}, 2}, {4, []byte{0, 0}, 8}, {4, []byte{1, 0, 0}, 3},
	} {
		if _, err := decodeIndexed(tc.format, tc.data, tc.count); err == nil {
			t.Fatalf("accepted malformed format %d: %v", tc.format, tc.data)
		}
	}
}

func TestV2LinksAndBlocks(t *testing.T) {
	b := formatFixture(t)
	so := int(binary.LittleEndian.Uint32(b[36:40]))
	po := int(binary.LittleEndian.Uint32(b[44:48]))
	// Turn sprite 1 into a link to sprite 0 with independent axes/palette.
	binary.LittleEndian.PutUint16(b[so+28+12:], 0)
	binary.LittleEndian.PutUint32(b[so+28+20:], 0)
	archive, err := Parse(bytes.NewReader(b))
	if err != nil {
		t.Fatal(err)
	}
	if !archive.Sprites[1].Linked || archive.Sprites[1].Image.Bounds() != archive.Sprites[0].Image.Bounds() || archive.Sprites[1].YAxis != -4 {
		t.Fatal("linked metadata/pixels")
	}
	// A palette link resolves through the palette table, not sprite table.
	binary.LittleEndian.PutUint32(b[po+16+12:], 0)
	binary.LittleEndian.PutUint16(b[po+16+6:], 0)
	if _, err = Parse(bytes.NewReader(b)); err != nil {
		t.Fatal(err)
	}
	// Put all literal data in tdata without changing its bytes.
	b = formatFixture(t)
	copy(b[60:68], b[52:60])
	for i := 0; i < 6; i++ {
		binary.LittleEndian.PutUint16(b[so+i*28+26:], 1)
	}
	if _, err = Parse(bytes.NewReader(b)); err != nil {
		t.Fatal(err)
	}
}

func TestPaletteSelectionDoesNotMutateArchive(t *testing.T) {
	archive, err := Parse(bytes.NewReader(formatFixture(t)))
	if err != nil {
		t.Fatal(err)
	}
	before := archive.Sprites[0].Image
	selected, err := archive.SpriteImage(0, 1, true)
	if err != nil {
		t.Fatal(err)
	}
	if selected == before || archive.Sprites[0].Image != before {
		t.Fatal("palette selection changed shared image")
	}
	if _, err := archive.SpriteImage(1, 0, true); err == nil {
		t.Fatal("8-bit pixels outside 32-color palette must fail")
	}
	if _, err := archive.SpriteImage(0, 99, true); err == nil {
		t.Fatal("missing palette accepted")
	}
}

func TestV2RejectsMalformedTablesAndLinks(t *testing.T) {
	for _, mutate := range []func([]byte){
		func(b []byte) { binary.LittleEndian.PutUint32(b[40:], 0xffffffff) },
		func(b []byte) { binary.LittleEndian.PutUint32(b[52:], 0xffffffff) },
		func(b []byte) {
			so := binary.LittleEndian.Uint32(b[36:])
			binary.LittleEndian.PutUint32(b[so+20:], 0)
			binary.LittleEndian.PutUint16(b[so+12:], 0)
		},
		func(b []byte) {
			so := binary.LittleEndian.Uint32(b[36:])
			binary.LittleEndian.PutUint32(b[so+20:], 0)
			binary.LittleEndian.PutUint16(b[so+12:], 65535)
		},
		func(b []byte) {
			po := binary.LittleEndian.Uint32(b[44:])
			binary.LittleEndian.PutUint32(b[po+12:], 0)
			binary.LittleEndian.PutUint16(b[po+6:], 0)
		},
		func(b []byte) { so := binary.LittleEndian.Uint32(b[36:]); b[so+14] = 1 },
		func(b []byte) {
			so := binary.LittleEndian.Uint32(b[36:])
			binary.LittleEndian.PutUint32(b[so+16:], 0xffffffff)
		},
	} {
		b := formatFixture(t)
		mutate(b)
		if _, err := Parse(bytes.NewReader(b)); err == nil {
			t.Fatal("accepted malformed SFF")
		}
	}
}

func FuzzIndexedPackets(f *testing.F) {
	f.Add(byte(4), []byte{2, 0x21, 3, 0}, uint16(5))
	f.Add(byte(3), []byte{1, 0x81, 9, 0x23}, uint16(4))
	f.Fuzz(func(t *testing.T, format byte, data []byte, count uint16) {
		if len(data) > 65536 {
			return
		}
		_, _ = decodeIndexed(2+format%3, data, int(count))
	})
}
