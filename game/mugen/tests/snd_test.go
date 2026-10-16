package tests

import (
	"bytes"
	"io/fs"
	"os"
	"path/filepath"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/snd"
)

func TestAllStockSounds(t *testing.T) {
	files, sounds := 0, 0
	err := filepath.WalkDir(referenceRoot(t), func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			return err
		}
		if d.IsDir() || filepath.Ext(path) != ".snd" {
			return nil
		}
		f, err := os.Open(path)
		if err != nil {
			return err
		}
		defer f.Close()
		bank, err := snd.Parse(f)
		if err != nil {
			t.Errorf("%s: %v", path, err)
			return nil
		}
		for _, sound := range bank.Sounds {
			if sound.Wave == nil {
				t.Fatal("missing WAV metadata")
			}
			pcm, err := sound.Wave.DecodePCM()
			if err != nil {
				t.Fatalf("%s (%d,%d): %v", path, sound.Group, sound.Item, err)
			}
			frames, valid := sound.Wave.Frames()
			if !valid || frames == 0 || len(pcm) != frames*int(sound.Wave.Channels) {
				t.Fatal("missing PCM frames")
			}
			sounds++
		}
		files++
		return nil
	})
	if err != nil {
		t.Fatal(err)
	}
	if files != 5 || sounds != 45 {
		t.Fatalf("decoded %d banks/%d sounds, want 5/45", files, sounds)
	}
}

func TestParseKFMSND(t *testing.T) {
	path := referencePath(t, "chars", "kfm", "kfm.snd")

	f, err := os.Open(path)
	if err != nil {
		t.Fatalf("required reference fixture: %v", err)
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
