package tests

import (
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/parsers"
)

func TestLexer(t *testing.T) {
	input := `
; This is a comment
[Info]
name = "Kung Fu Man"
displayname = "Kung Fu Man"
versiondate = 04,14,2001
mugenversion = 04,14,2001
author = "Elecbyte"
pal.defaults = 1,2,3,4,5 ; Another comment
`
	lexer := parsers.NewLexer(strings.NewReader(input))

	expected := []parsers.TokenType{
		parsers.TokenNewline,
		parsers.TokenSectionStart, parsers.TokenIdentifier, parsers.TokenSectionEnd, parsers.TokenNewline,
		parsers.TokenIdentifier, parsers.TokenAssign, parsers.TokenString, parsers.TokenNewline,
		parsers.TokenIdentifier, parsers.TokenAssign, parsers.TokenString, parsers.TokenNewline,
		parsers.TokenIdentifier, parsers.TokenAssign, parsers.TokenNumber, parsers.TokenComma, parsers.TokenNumber, parsers.TokenComma, parsers.TokenNumber, parsers.TokenNewline,
		parsers.TokenIdentifier, parsers.TokenAssign, parsers.TokenNumber, parsers.TokenComma, parsers.TokenNumber, parsers.TokenComma, parsers.TokenNumber, parsers.TokenNewline,
		parsers.TokenIdentifier, parsers.TokenAssign, parsers.TokenString, parsers.TokenNewline,
		parsers.TokenIdentifier, parsers.TokenAssign, parsers.TokenNumber, parsers.TokenComma, parsers.TokenNumber, parsers.TokenComma, parsers.TokenNumber, parsers.TokenComma, parsers.TokenNumber, parsers.TokenComma, parsers.TokenNumber, parsers.TokenNewline,
		parsers.TokenEOF,
	}

	for i, exp := range expected {
		tok := lexer.NextToken()
		if tok.Type != exp {
			t.Errorf("token %d: expected %v, got %v (lit: %q)", i+1, exp, tok.Type, tok.Lit)
		}
	}
}

func TestParser(t *testing.T) {
	input := `
[Info]
name = "Kung Fu Man"
displayname="Kung Fu Man"
versiondate = 04.14.2001 ; some date
localcoord = 320, 240

[Files]
cmd = kfm.cmd ; Command set
cns = kfm.cns ; Constants
`
	file, err := parsers.Parse(strings.NewReader(input))
	if err != nil {
		t.Fatalf("Parse error: %v", err)
	}

	if len(file.Sections) != 2 {
		t.Fatalf("expected 2 sections, got %d", len(file.Sections))
	}

	info := file.Sections[0]
	if info.Name != "Info" {
		t.Errorf("expected section name Info, got %s", info.Name)
	}

	if len(info.Attributes) != 4 {
		t.Errorf("expected 4 attributes in Info, got %d", len(info.Attributes))
	}

	files := file.Sections[1]
	if files.Name != "Files" {
		t.Errorf("expected section name Files, got %s", files.Name)
	}

	if len(files.Attributes) != 2 {
		t.Errorf("expected 2 attributes in Files, got %d", len(files.Attributes))
	}

	// Verify a specific attribute
	cmdAttr := files.Attributes[0]
	if cmdAttr.ID.String() != "cmd" {
		t.Errorf("expected attribute ID cmd, got %s", cmdAttr.ID.String())
	}
	if kv, ok := cmdAttr.Value.(parsers.KeywordValue); !ok || kv.Val != "kfm.cmd" {
		t.Errorf("expected attribute value kfm.cmd, got %v", cmdAttr.Value)
	}
}

func TestSystemDef(t *testing.T) {
	// The user requested testing against paintown-data/mugen/data/big/system.def
	// Let's use the local data-new directory
	path := "../../../data-new/mugen/data/big/system.def"

	file, err := os.Open(path)
	if err != nil {
		t.Skipf("Skipping system.def test, file not found: %v", err)
	}
	defer file.Close()

	parsedFile, err := parsers.Parse(file)
	if err != nil {
		t.Fatalf("Failed to parse system.def: %v", err)
	}

	if len(parsedFile.Sections) == 0 {
		t.Errorf("Expected system.def to have sections, got 0")
	}
	t.Logf("Successfully parsed system.def with %d sections", len(parsedFile.Sections))
}

func TestAllDefFiles(t *testing.T) {
	basePath := "../../../data-new/mugen"

	// Check if directory exists before walking
	if _, err := os.Stat(basePath); os.IsNotExist(err) {
		t.Skipf("Skipping TestAllDefFiles, directory %s not found", basePath)
	}

	successCount := 0
	errCount := 0

	err := filepath.Walk(basePath, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if !info.IsDir() && strings.HasSuffix(strings.ToLower(info.Name()), ".def") {
			file, err := os.Open(path)
			if err != nil {
				t.Errorf("Failed to open %s: %v", path, err)
				errCount++
				return nil
			}
			defer file.Close()

			_, parseErr := parsers.Parse(file)
			if parseErr != nil {
				t.Errorf("Failed to parse %s: %v", path, parseErr)
				errCount++
			} else {
				successCount++
			}
		}
		return nil
	})

	if err != nil {
		t.Fatalf("Error walking directory %s: %v", basePath, err)
	}

	t.Logf("Successfully parsed %d .def files. Failed: %d", successCount, errCount)
	if errCount > 0 {
		t.Errorf("Failed to parse %d .def files", errCount)
	}
}
