package font

import (
	"image"
	"os"
	"path/filepath"
	"testing"

	"golang.org/x/image/font/gofont/goregular"
)

func fontDef(t *testing.T, body string) string {
	t.Helper()
	path := filepath.Join(t.TempDir(), "font.def")
	if err := os.WriteFile(path, []byte("[FNT v2]\nfntversion=2,00\n[Def]\n"+body), 0600); err != nil {
		t.Fatal(err)
	}
	return path
}

func TestTrueTypeLocalUTF8AndBlend(t *testing.T) {
	path := fontDef(t, "Type=truetype\nSize=9,20\nFile=font.ttf\nSpacing=100,2\nBlend=1\n")
	if err := os.WriteFile(filepath.Join(filepath.Dir(path), "FONT.TTF"), goregular.TTF, 0600); err != nil {
		t.Fatal(err)
	}
	f, err := LoadFont(path)
	if err != nil {
		t.Fatal(err)
	}
	if f.Fallback != "" {
		t.Fatal(f.Fallback)
	}
	if f.TextWidth("AV") <= 0 || f.TextWidth("AV") >= 50 {
		t.Fatal("TrueType must ignore bitmap horizontal spacing")
	}
	if f.Glyph('é', 0) == nil || f.TextWidth("éA") <= 0 {
		t.Fatal("UTF-8 glyphs")
	}
	gray := false
	for _, a := range f.Glyph('A', 0).Image.(*image.Alpha).Pix {
		if a > 0 && a < 255 {
			gray = true
		}
	}
	if !gray {
		t.Fatal("blended text has no antialiasing")
	}
	path = fontDef(t, "Type=truetype\nSize=9,20\nFile=__missing_font__.ttf\nBlend=0\n")
	mono, err := LoadFont(path)
	if err != nil {
		t.Fatal(err)
	}
	if mono.Fallback == "" {
		t.Fatal("missing font substitution is not observable")
	}
	for _, a := range mono.Glyph('A', 0).Image.(*image.Alpha).Pix {
		if a != 0 && a != 255 {
			t.Fatal("unblended text contains partial alpha")
		}
	}
}

func TestInvalidFonts(t *testing.T) {
	for _, body := range []string{"Type=bitmap\nSize=3,6", "Type=unknown\nSize=3,6\nFile=a.sff", "Type=bitmap\nSize=3\nFile=a.sff", "Type=truetype\nSize=3,-1\nFile=a.ttf"} {
		if _, err := LoadFont(fontDef(t, body)); err == nil {
			t.Fatal("accepted invalid font", body)
		}
	}
	path := filepath.Join(t.TempDir(), "bad.fnt")
	b := make([]byte, 32)
	copy(b, []byte("ElecbyteFnt"))
	for i := 16; i < 24; i++ {
		b[i] = 255
	}
	if err := os.WriteFile(path, b, 0600); err != nil {
		t.Fatal(err)
	}
	if _, err := LoadFont(path); err == nil {
		t.Fatal("accepted wrapped binary offsets")
	}
}
