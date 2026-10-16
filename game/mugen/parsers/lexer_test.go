package parsers

import (
	"strings"
	"testing"
)

func TestUTF8BOM(t *testing.T) {
	file, err := Parse(strings.NewReader("\ufeff[Info]\r\nname = \"fixture\"\r\n"))
	if err != nil {
		t.Fatal(err)
	}
	if len(file.Sections) != 1 || file.Sections[0].Name != "Info" {
		t.Fatalf("BOM changed section parsing: %+v", file)
	}
	if got := file.Sections[0].Attributes[0].Value.(StringValue).Val; got != "fixture" {
		t.Fatalf("name = %q", got)
	}
}
