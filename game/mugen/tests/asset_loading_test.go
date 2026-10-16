package tests

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/motif"
	"github.com/kazzmir/paintown/game/mugen/stage"
)

func TestStockMotifAssets(t *testing.T) {
	for _, name := range []string{"system.def", "mugen1/system.def", "big/system.def", "kfm/system.def"} {
		t.Run(name, func(t *testing.T) {
			loaded, err := motif.LoadMotif(filepath.Join("data", name), referenceRoot(t))
			if err != nil {
				t.Fatal(err)
			}
			if loaded.SpriteData == nil || len(loaded.SpriteData.Sprites) == 0 || loaded.SoundBank == nil || len(loaded.SoundBank.Sounds) == 0 {
				t.Fatal("motif main sprite/sound archives were not loaded")
			}
			for i, file := range loaded.Files.FontFiles {
				if file != "" && (loaded.Fonts[i] == nil || loaded.Fonts[i].Glyph('A', 0) == nil) {
					t.Errorf("font %d %s is unavailable", i, file)
				}
			}
		})
	}
}

func TestStockStageAssets(t *testing.T) {
	for _, name := range []string{"kfm", "stage0", "stage0-720"} {
		t.Run(name, func(t *testing.T) {
			loaded, err := stage.LoadStage("stages/"+name+".def", referenceRoot(t))
			if err != nil {
				t.Fatal(err)
			}
			if loaded.SpriteData == nil || len(loaded.SpriteData.Sprites) == 0 || len(loaded.SpriteImages) == 0 || len(loaded.Background.Elements) == 0 {
				t.Fatal("stage sprite/background resources were not loaded")
			}
			for _, images := range loaded.SpriteImages {
				if images.Opaque != nil && images.Opaque != images.Masked {
					images.Opaque.Dispose()
				}
				if images.Masked != nil {
					images.Masked.Dispose()
				}
			}
		})
	}
}

func TestMotifAndStageAssetFailures(t *testing.T) {
	for _, kind := range []string{"spr", "snd", "font1", "stage-spr"} {
		for _, mode := range []string{"missing", "malformed"} {
			t.Run(kind+"/"+mode, func(t *testing.T) {
				dir := t.TempDir()
				path := filepath.Join(dir, "probe.def")
				definition := "[Files]\n" + kind + "=broken.asset\n"
				if kind == "stage-spr" {
					definition = "[BGDef]\nspr=broken.asset\n"
				}
				writeResource(t, path, []byte(definition))
				if mode == "malformed" {
					writeResource(t, filepath.Join(dir, "broken.asset"), []byte("invalid archive"))
				}
				var err error
				if kind == "stage-spr" {
					_, err = stage.LoadStage(path, dir)
				} else {
					_, err = motif.LoadMotif(path, dir)
				}
				if err == nil {
					t.Fatal("broken asset returned a successful object")
				}
				for _, want := range []string{path, "line 2", "broken.asset"} {
					if !strings.Contains(err.Error(), want) {
						t.Errorf("missing %q in %v", want, err)
					}
				}
			})
		}
	}
}

func TestMotifFontDirectoryOrderAndHeightKey(t *testing.T) {
	root := t.TempDir()
	dir := filepath.Join(root, "data", "theme")
	path := filepath.Join(dir, "system.def")
	writeResource(t, path, []byte("[Files]\nfont1=sample.def\nfont1.height=13\n"))
	// Stock system.def documents system, program, data, font search priority.
	locations := []string{dir, root, filepath.Join(root, "data"), filepath.Join(root, "font")}
	for i, location := range locations {
		body := fmt.Sprintf("[FNT v2]\nfntversion=2,0\n[Def]\nType=bitmap\nSize=4,%d\nFile=%s\n", 6+i, referencePath(t, "font", "f-4x6.sff"))
		writeResource(t, filepath.Join(location, "sample.def"), []byte(body))
	}
	for i, location := range locations {
		loaded, err := motif.LoadMotif(path, root)
		if err != nil {
			t.Fatal(err)
		}
		if loaded.Files.FontFiles[1] != "sample.def" || loaded.Fonts[1] == nil || loaded.Fonts[1].Height != 6+i {
			t.Fatalf("font lookup %d used wrong resource: %v", i, loaded.Files.FontFiles)
		}
		if err := os.Remove(filepath.Join(location, "sample.def")); err != nil {
			t.Fatal(err)
		}
	}
}

func TestBitmapFontChildResourceNamesAndErrors(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "font.def")
	writeResource(t, path, []byte("[FNT v2]\nfntversion=2,0\n[Def]\nType=bitmap\nSize=4,6\nFile=ASSETS\\Glyphs.SFF\n"))
	child := filepath.Join(dir, "assets", "glyphs.sff")
	data, err := os.ReadFile(referencePath(t, "font", "f-4x6.sff"))
	if err != nil {
		t.Fatal(err)
	}
	writeResource(t, child, data)
	loaded, err := font.LoadFont(path)
	if err != nil || loaded.Glyph('A', 0) == nil {
		t.Fatalf("relative Windows/case child: %v", err)
	}
	writeResource(t, child, []byte("invalid archive"))
	_, err = font.LoadFont(path)
	if err == nil || !strings.Contains(err.Error(), path) || !strings.Contains(err.Error(), child) {
		t.Fatalf("lost parent/child context: %v", err)
	}
}

func TestKFMTitleBackgroundArchive(t *testing.T) {
	loaded, err := motif.LoadMotif("data/kfm/system.def", referenceRoot(t))
	if err != nil {
		t.Fatal(err)
	}
	bg := loaded.TitleBG
	if bg == nil || bg.SpriteData == nil || bg.SpriteData == loaded.SpriteData || len(bg.Elements) == 0 {
		t.Fatal("KFM title override was not loaded independently")
	}
	first, ok := bg.Elements[0].(*background.NormalElement)
	if !ok || first.Sprite == nil {
		t.Fatal("title image did not bind its override sprite")
	}
	found := false
	for i := range bg.SpriteData.Sprites {
		if first.Sprite == &bg.SpriteData.Sprites[i] {
			found = true
		}
	}
	if !found {
		t.Fatal("title element references an archive belonging to another screen")
	}
}

func TestMotifBackgroundResourceErrors(t *testing.T) {
	for _, prefix := range []string{"TitleBG", "SelectBG", "VersusBG", "OptionBG"} {
		for _, mode := range []string{"missing", "malformed"} {
			t.Run(prefix+"/"+mode, func(t *testing.T) {
				dir := t.TempDir()
				path := filepath.Join(dir, "system.def")
				writeResource(t, path, []byte("["+prefix+"Def]\nspr=broken.sff\n"))
				if mode == "malformed" {
					writeResource(t, filepath.Join(dir, "broken.sff"), []byte("invalid archive"))
				}
				_, err := motif.LoadMotif(path, dir)
				if err == nil || !strings.Contains(err.Error(), path) || !strings.Contains(strings.ToLower(err.Error()), strings.ToLower(prefix+"def")) || !strings.Contains(err.Error(), "line 2") || !strings.Contains(err.Error(), "broken.sff") {
					t.Fatalf("missing background resource context: %v", err)
				}
			})
		}
	}
}
