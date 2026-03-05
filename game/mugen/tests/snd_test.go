package tests

import (
	"bytes"
	"os"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/snd"
)

func TestParseKFMSND(t *testing.T) {
	path := "../../../data-new/mugen/chars/kfm/kfm.snd"

	f, err := os.Open(path)
	if err != nil {
		t.Skipf("Skipping TestParseKFMSND, could not find %s: %v", path, err)
	}
	defer f.Close()

	bank, err := snd.Parse(f)
	if err != nil {
		t.Fatalf("Failed to parse SND: %v", err)
	}

	if len(bank.Sounds) == 0 {
		t.Fatalf("Parsed 0 sounds from %s", path)
	}

	// In the previous hexdump we noticed offset 512 (first sound) contained RIFF WAV formats.
	firstSound := bank.Sounds[0]
	if !bytes.HasPrefix(firstSound.Data, []byte("RIFF")) {
		t.Errorf("Expected first sound byte data to be a RIFF WAV file, got header bytes: %v", firstSound.Data[:4])
	}

	// Access via map verification
	mapSound, ok := bank.Map["0-0"]
	if !ok || len(mapSound.Data) == 0 {
		t.Logf("Sound '0-0' not found, available length %d", len(bank.Map))
	}

	t.Logf("Successfully extracted %d sounds from binary SND.", len(bank.Sounds))
}
