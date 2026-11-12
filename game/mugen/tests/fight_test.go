package tests

import (
	"fmt"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/fight"
	"github.com/kazzmir/paintown/game/mugen/motif"
)

func TestStockFightResources(t *testing.T) {
	for _, path := range []string{"data/fight.def", "data/mugen1/fight.def"} {
		t.Run(path, func(t *testing.T) {
			data, err := fight.Load(path, referenceRoot(t))
			if err != nil {
				t.Fatal(err)
			}
			if data.Sprites == nil || len(data.Sprites.Sprites) == 0 || data.Sounds == nil || data.CommonSounds == nil || data.EffectSprites == nil || data.EffectAnimations == nil || len(data.EffectAnimations.Actions) == 0 || len(data.Animations.Actions) == 0 {
				t.Fatal("declared fight/HUD/common-effect resources were not loaded")
			}
			for i := 1; i <= 10; i++ {
				if data.Files.Files[fmt.Sprintf("font%d", i)] != "" && data.Fonts[i] == nil {
					t.Errorf("declared fight font %d was discarded", i)
				}
			}
			for _, field := range []struct{ section, key string }{{"Lifebar", "p1.pos"}, {"Powerbar", "p2.pos"}, {"Time", "pos"}, {"Round", "start.waittime"}} {
				attribute := data.Attribute(field.section, field.key)
				if attribute == nil || attribute.RawValue == "" || attribute.Line <= 0 {
					t.Errorf("layout source lost: [%s] %s", field.section, field.key)
				}
			}
		})
	}
}

func TestMotifFightResourceHandoff(t *testing.T) {
	for _, path := range []string{"data/system.def", "data/mugen1/system.def", "data/big/system.def", "data/kfm/system.def"} {
		t.Run(path, func(t *testing.T) {
			m, err := motif.LoadMotif(path, referenceRoot(t))
			if err != nil {
				t.Fatal(err)
			}
			data, err := fight.Load(m.Files.Fight, referenceRoot(t))
			if err != nil || data.Source != m.Files.Fight {
				t.Fatalf("motif's shared/relative fight definition was not preserved: %v", err)
			}
		})
	}
}

func TestFightDeclaredResourceFailures(t *testing.T) {
	for _, key := range []string{"sff", "snd", "font1", "fightfx.sff", "fightfx.air", "common.snd"} {
		for _, mode := range []string{"missing", "malformed"} {
			t.Run(key+"/"+mode, func(t *testing.T) {
				dir := t.TempDir()
				path := filepath.Join(dir, "fight.def")
				body := "[Files]\nsff=" + referencePath(t, "data", "fight.sff") + "\n"
				if key == "sff" {
					body = "[Files]\n"
				}
				body += key + "=broken.asset\n"
				writeResource(t, path, []byte(body))
				if mode == "malformed" {
					writeResource(t, filepath.Join(dir, "broken.asset"), []byte("[invalid\n"))
				}
				_, err := fight.Load(path, referenceRoot(t))
				if err == nil || !strings.Contains(err.Error(), path) || !strings.Contains(err.Error(), "[Files] "+key) || !strings.Contains(err.Error(), "broken.asset") {
					t.Fatalf("declared fight resource failure was hidden: %v", err)
				}
			})
		}
	}
}
