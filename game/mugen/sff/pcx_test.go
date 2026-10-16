package sff

import (
	"bytes"
	"encoding/binary"
	"image/color"
	"testing"
)

func tinyPCX() []byte {
	b := make([]byte, 128)
	b[0], b[2], b[3], b[65] = 10, 1, 8, 1
	binary.LittleEndian.PutUint16(b[8:], 1) // 2x1, two-byte row
	binary.LittleEndian.PutUint16(b[66:], 2)
	b = append(b, 0, 1, 12)
	for i := 0; i < 256; i++ {
		b = append(b, byte(i+7), byte(i+8), byte(i+9))
	}
	return b
}

func TestPCXStrictPixelsAndMasking(t *testing.T) {
	b := tinyPCX()
	p, err := ExtractPalette(b, true)
	if err != nil {
		t.Fatal(err)
	}
	img, err := DecodePCX(b, p)
	if err != nil {
		t.Fatal(err)
	}
	if img.ColorIndexAt(1, 0) != 1 {
		t.Fatal("decoded indices")
	}
	c := color.NRGBAModel.Convert(img.At(0, 0)).(color.NRGBA)
	if c != (color.NRGBA{7, 8, 9, 0}) {
		t.Fatalf("transparent RGB lost: %v", c)
	}
	for _, mutate := range []func([]byte) []byte{
		func(b []byte) []byte { return b[:129] },
		func(b []byte) []byte { b[128] = 0xc0; return b },
		func(b []byte) []byte { b[128] = 0xc3; return b },
		func(b []byte) []byte { b[65] = 3; return b },
		func(b []byte) []byte { b[66] = 1; return b },
	} {
		if _, err := DecodePCX(mutate(tinyPCX()), p); err == nil {
			t.Fatal("accepted malformed PCX")
		}
	}
}

func tinyV1() []byte {
	pcx := tinyPCX()
	b := make([]byte, 512)
	copy(b, []byte("ElecbyteSpr\x00"))
	b[15] = 1
	binary.LittleEndian.PutUint32(b[20:], 2)
	binary.LittleEndian.PutUint32(b[24:], 512)
	sub := make([]byte, 32)
	binary.LittleEndian.PutUint32(sub, uint32(512+32+len(pcx)))
	binary.LittleEndian.PutUint32(sub[4:], uint32(len(pcx)))
	b = append(b, sub...)
	b = append(b, pcx...)
	link := make([]byte, 32)
	link[14] = 1
	link[8] = 3
	return append(b, link...)
}

func TestV1LinksAndFailures(t *testing.T) {
	archive, err := Parse(bytes.NewReader(tinyV1()))
	if err != nil {
		t.Fatal(err)
	}
	if len(archive.Sprites) != 2 || !archive.Sprites[1].Linked || archive.Sprites[1].XAxis != 3 || archive.Sprites[1].Image == nil {
		t.Fatal("v1 link metadata")
	}
	_, _, _, a := archive.Sprites[1].ImageOpaque.At(0, 0).RGBA()
	if a != 65535 {
		t.Fatal("opaque link color zero")
	}
	for _, mutate := range []func([]byte) []byte{
		func(b []byte) []byte { return b[:len(b)-1] },
		func(b []byte) []byte { binary.LittleEndian.PutUint32(b[20:], 3); return b },
		func(b []byte) []byte { binary.LittleEndian.PutUint32(b[512:], 512); return b },
		func(b []byte) []byte { b[len(b)-32+16] = 5; return b },
		func(b []byte) []byte { b[544] = 0; return b },
	} {
		if _, err := Parse(bytes.NewReader(mutate(tinyV1()))); err == nil {
			t.Fatal("accepted malformed v1")
		}
	}
}
