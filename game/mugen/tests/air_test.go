package tests

import (
	"os"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
)

func TestParseFightAir(t *testing.T) {
	path := referencePath(t, "data", "fightfx.air")

	f, err := os.Open(path)
	if err != nil {
		t.Fatalf("required reference fixture: %v", err)
	}
	defer f.Close()

	animData, err := air.Parse(f)
	if err != nil {
		t.Fatalf("Failed to parse AIR: %v", err)
	}

	if len(animData.Actions) == 0 {
		t.Fatalf("Parsed 0 actions, expected animations in fightfx.air")
	}

	totalFrames := 0
	for id, action := range animData.Actions {
		if action.ID != id {
			t.Errorf("Action ID mismatch: key %d, val %d", id, action.ID)
		}
		totalFrames += len(action.Elements)
	}

	t.Logf("Successfully parsed %d animations comprising %d total frames (elements).", len(animData.Actions), totalFrames)
}
