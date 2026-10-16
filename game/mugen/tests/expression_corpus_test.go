package tests

import (
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/evaluator"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

// Syntax coverage is deliberately separate from evaluating world-bound triggers.
func TestStockTriggerSyntax(t *testing.T) {
	root := referenceRoot(t)
	count := 0
	err := filepath.WalkDir(root, func(path string, entry os.DirEntry, err error) error {
		if err != nil {
			return err
		}
		if entry.IsDir() || (filepath.Ext(path) != ".cns" && filepath.Ext(path) != ".cmd") {
			return nil
		}
		f, err := os.Open(path)
		if err != nil {
			return err
		}
		file, parseErr := parsers.Parse(f)
		f.Close()
		if parseErr != nil {
			t.Errorf("%s: %v", path, parseErr)
			return nil
		}
		for _, section := range file.Sections {
			for _, attr := range section.Attributes {
				if !strings.HasPrefix(strings.ToLower(attr.ID.String()), "trigger") {
					continue
				}
				count++
				if _, err := evaluator.Parse(attr.RawValue); err != nil {
					t.Errorf("%s:%d: [%s] %s = %s: %v", path, attr.Line, section.Name, attr.ID.String(), attr.RawValue, err)
				}
			}
		}
		return nil
	})
	if err != nil {
		t.Fatal(err)
	}
	if count == 0 {
		t.Fatal("no reference trigger expressions were checked")
	}
	t.Logf("parsed %d stock trigger expressions; world-bound behavior is tested separately", count)
}
