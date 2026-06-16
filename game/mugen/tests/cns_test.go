package tests

import (
	"os"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestParseKFMCNS(t *testing.T) {
	path := "../../../data-new/mugen/chars/kfm/kfm.cns"

	f, err := os.Open(path)
	if err != nil {
		t.Skipf("Skipping TestParseKFMCNS, could not find %s: %v", path, err)
	}
	defer f.Close()

	cnsData, err := cns.Parse(f)
	if err != nil {
		t.Fatalf("Failed to parse CNS: %v", err)
	}

	if cnsData.Data.Life != 1000 {
		t.Errorf("Expected Life to be 1000, got %d", cnsData.Data.Life)
	}

	if len(cnsData.Velocity.RunFwd) < 2 {
		t.Errorf("Expected Velocity.RunFwd to have 2 dimensions, got %v", cnsData.Velocity.RunFwd)
	} else if cnsData.Velocity.RunFwd[0] != 4.6 {
		t.Errorf("Expected Velocity.RunFwd[0] = 4.6, got %f", cnsData.Velocity.RunFwd[0])
	}

	if len(cnsData.States) == 0 {
		t.Fatalf("Parsed 0 StateDefs, expected multiple states in %s", path)
	}

	// KFM has [Statedef 170]
	state170, ok := cnsData.States[170]
	if !ok {
		t.Fatalf("Expected Statedef 170 not found")
	}

	if state170.Type != "S" {
		t.Errorf("Expected State 170 type 'S', got '%s'", state170.Type)
	}

	if len(state170.Controllers) == 0 {
		t.Fatalf("Expected State 170 to have controllers")
	}

	t.Logf("Successfully parsed %d state definitions.", len(cnsData.States))
}
