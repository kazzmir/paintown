package tests

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"os"
	"path/filepath"
	"runtime"
	"testing"
)

func referenceRoot(t *testing.T) string {
	t.Helper()
	root := os.Getenv("MUGEN_REFERENCE_DIR")
	if root == "" {
		_, source, _, ok := runtime.Caller(0)
		if !ok {
			t.Fatal("cannot locate reference test source")
		}
		root = filepath.Join(filepath.Dir(source), "..", "temp", "mugen-1.1b1")
	}
	if info, err := os.Stat(root); err != nil || !info.IsDir() {
		t.Fatalf("required MUGEN 1.1 beta 1 corpus is missing at %s; extract the distribution into game/mugen/temp/mugen-1.1b1 or set MUGEN_REFERENCE_DIR", root)
	}
	return root
}

func referencePath(t *testing.T, parts ...string) string {
	t.Helper()
	return filepath.Join(append([]string{referenceRoot(t)}, parts...)...)
}

// This pins required fixtures without distributing Elecbyte's content in Git.
func TestReferenceManifest(t *testing.T) {
	_, source, _, _ := runtime.Caller(0)
	manifest, err := os.ReadFile(filepath.Join(filepath.Dir(source), "testdata", "reference-manifest.json"))
	if err != nil {
		t.Fatal(err)
	}
	var entries []struct {
		Path   string
		Size   int64
		SHA256 string
	}
	if err := json.Unmarshal(manifest, &entries); err != nil {
		t.Fatal(err)
	}
	if len(entries) == 0 {
		t.Fatal("empty reference manifest")
	}
	root := referenceRoot(t)
	for _, entry := range entries {
		t.Run(entry.Path, func(t *testing.T) {
			data, err := os.ReadFile(filepath.Join(root, filepath.FromSlash(entry.Path)))
			if err != nil {
				t.Fatal(err)
			}
			sum := sha256.Sum256(data)
			if int64(len(data)) != entry.Size || hex.EncodeToString(sum[:]) != entry.SHA256 {
				t.Fatalf("reference changed: %s; restore the original 1.1 beta 1 fixture", entry.Path)
			}
		})
	}
}
