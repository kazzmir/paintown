package tests

import (
	"os"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/sff"
)

func TestParseSystemSFF(t *testing.T) {
	sffPath := "../../../data-new/mugen/data/system.sff"

	f, err := os.Open(sffPath)
	if err != nil {
		t.Skipf("Skipping TestParseSystemSFF, could not open %s: %v", sffPath, err)
	}
	defer f.Close()

	sffFile, err := sff.Parse(f)
	if err != nil {
		t.Fatalf("Failed to parse SFF: %v", err)
	}

	if len(sffFile.Sprites) == 0 {
		t.Errorf("Parsed 0 sprites, expected many")
	}

	successCount := 0
	for idx, spr := range sffFile.Sprites {
		if spr.Image != nil {
			bounds := spr.Image.Bounds()
			if bounds.Dx() > 0 && bounds.Dy() > 0 {
				successCount++
			} else {
				t.Errorf("Sprite %d (Group %d, Item %d) has 0-size bounds", idx, spr.GroupNumber, spr.ImageNumber)
			}
		} else if !spr.Linked {
			t.Errorf("Sprite %d (Group %d, Item %d) is not linked but has no image", idx, spr.GroupNumber, spr.ImageNumber)
		}
	}

	t.Logf("Successfully parsed %d total sprites (%d with valid PCX images).", len(sffFile.Sprites), successCount)
}
