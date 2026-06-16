package tests

import (
	"os"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
)

func TestParseFightAir(t *testing.T) {
	// Let's test against fight.def/fight.air equivalent
	path := "../../../data-new/mugen/data/mugen1/fight.def" // this folder has a fight.def but we really want an air file

	// Looking for any .air file in local data-new directory. system.def is accompanied by system.sff, maybe system.air? No, system.def contains animations internally.
	// Oh, wait. fightfx.air is standard in data/
	path = "../../../data-new/mugen/data/fightfx.air"

	f, err := os.Open(path)
	if err != nil {
		t.Skipf("Skipping TestParseFightAir, could not find %s: %v", path, err)
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
