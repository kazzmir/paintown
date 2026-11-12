package tests

import (
	"bytes"
	"os"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/logger"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

func TestEmbeddedParserDiagnosticsRespectSectionOwnership(t *testing.T) {
	ast, err := parsers.Parse(strings.NewReader(`[Info]
name=Example
[SceneDef]
spr=example.sff
[TitleBGDef]
[TitleBG Example]
type=normal
spriteno=0,0
unsupported_bg=1
[Begin Action 240]
unsupported_air=1
0,0,0,0,-1
`))
	if err != nil {
		t.Fatal(err)
	}
	var output bytes.Buffer
	logger.SetOutput(&output)
	defer logger.SetOutput(os.Stderr)
	animations, err := air.ParseFromAST(ast)
	if err != nil {
		t.Fatal(err)
	}
	bg := &background.Background{}
	bg.LoadFromAST(ast, "titlebg", nil, animations)
	text := output.String()
	if strings.Contains(text, "Unhandled") || !strings.Contains(text, "unsupported_air") || !strings.Contains(text, "unsupported_bg") {
		t.Fatalf("normal sections must be ignored while owned unknown fields stay diagnosed: %s", text)
	}
}
