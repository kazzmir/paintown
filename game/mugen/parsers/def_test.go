package parsers

import (
	"strings"
	"testing"
)

func TestDefMetadataAndFileSource(t *testing.T) {
	data, err := ParseDef(strings.NewReader(`[Info]
name="A; B"
mugenversion=1.1
localcoord=1280, 720
[Files]
sprite = "Assets\A File.SFF" ; retained as one filename
st = 100
st9 = 1.1
sound =
`))
	if err != nil {
		t.Fatal(err)
	}
	for key, want := range map[string]string{"name": "A; B", "mugenversion": "1.1", "localcoord": "1280, 720"} {
		if data.Info[key] != want {
			t.Errorf("Info %s=%q want %q", key, data.Info[key], want)
		}
	}
	for key, want := range map[string]string{"sprite": `Assets\A File.SFF`, "st": "100", "st9": "1.1", "sound": ""} {
		if got, exists := data.Files[key]; !exists || got != want {
			t.Errorf("Files %s=%q present=%v want %q", key, got, exists, want)
		}
	}
	if data.FileLines["sprite"] != 6 || data.FileLines["sound"] != 9 {
		t.Fatal(data.FileLines)
	}
}
