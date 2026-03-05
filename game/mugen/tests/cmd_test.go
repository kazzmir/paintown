package tests

import (
	"os"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cmd"
)

func TestParseKFMCMD(t *testing.T) {
	path := "../../../data-new/mugen/chars/kfm/kfm.cmd"

	f, err := os.Open(path)
	if err != nil {
		t.Skipf("Skipping TestParseKFMCMD, could not find %s: %v", path, err)
	}
	defer f.Close()

	cmdData, err := cmd.Parse(f)
	if err != nil {
		t.Fatalf("Failed to parse CMD: %v", err)
	}

	if len(cmdData.Commands) == 0 {
		t.Fatalf("Parsed 0 commands, expected multiple input configurations in %s", path)
	}

	// KFM has numerous inputs. Let's check for standard ones.
	if _, ok := cmdData.Commands["QCF_x"]; !ok {
		t.Errorf("Expected command 'QCF_x' not found")
	}

	totalControllers := len(cmdData.StateControllers)
	if totalControllers == 0 {
		t.Fatalf("Parsed 0 StateControllers, expected -1 states in %s", path)
	}

	t.Logf("Successfully parsed %d unique named commands and %d [-1] StateControllers.", len(cmdData.Commands), totalControllers)
}
