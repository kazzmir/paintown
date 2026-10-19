package parsers

import (
	"errors"
	"io"
	"strings"
	"testing"
)

func TestExpressionSourcePreserved(t *testing.T) {
	inputs := []string{
		`time!=0 && var(1):=5.0`,
		`ifelse(command = "a;b", 1.0, var(2))`,
		`(time = [1,3)) || (parent, var(0))`,
		`"text, with; punctuation"`,
		`7.0`,
		`data\chars\sprite.sff`,
	}
	for _, input := range inputs {
		t.Run(input, func(t *testing.T) {
			file, err := Parse(strings.NewReader("[State 0, Test]\n  trigger1 = " + input + " ; comment\n"))
			if err != nil {
				t.Fatal(err)
			}
			attr := file.Sections[0].Attributes[0]
			if attr.RawValue != input {
				t.Fatalf("source = %q, want %q", attr.RawValue, input)
			}
			if attr.Line != 2 || attr.Column != 3 {
				t.Fatalf("source location = %d:%d", attr.Line, attr.Column)
			}
		})
	}
}

func TestContainerSyntax(t *testing.T) {
	file, err := Parse(strings.NewReader(`[Characters]
kfm, stages/kfm.def, order=2, music="a,b.ogg"
[Begin Action 0]
Clsn2Default: 1
Clsn2[0] = -10,-20,10,0
0, 1, 0, 0, 3,, AS256D128, 1.5, 2.0, 45
LoopStart
[State -1, Two conditions]
trigger1 = 1
trigger1 = 2
`))
	if err != nil {
		t.Fatal(err)
	}
	chars := file.Sections[0].Attributes
	if chars[0].ID.String() != "kfm" {
		t.Fatalf("character name = %q", chars[0].ID.String())
	}
	values, ok := chars[0].Value.(*ValueList)
	if !ok || len(values.Values) != 3 {
		t.Fatalf("character options: %#v", chars[0].Value)
	}
	air := file.Sections[1].Attributes
	if air[0].ID.String() != "Clsn2Default" {
		t.Fatalf("collision count key = %q", air[0].ID.String())
	}
	if air[3].ID.String() != "LoopStart" || air[3].Value != nil {
		t.Fatalf("bare keyword: %#v", air[3])
	}
	if len(file.Sections[2].Attributes) != 2 {
		t.Fatal("repeated conditions were lost")
	}
}

func TestTupleNesting(t *testing.T) {
	parts := SplitList(`ifelse(time, 1, 2), (time = [0, 5]), "a,b", ,3,`)
	if len(parts) != 6 || parts[0] != "ifelse(time, 1, 2)" || parts[4] != "3" || parts[5] != "" {
		t.Fatalf("tuple = %#v", parts)
	}
}

func TestInvalidContainer(t *testing.T) {
	for _, source := range []string{"a = 1", "[Info", "[Info] junk", "[]", "[Info]\n = 1", "[Info]\nname = \"unterminated"} {
		if _, err := Parse(strings.NewReader(source)); err == nil {
			t.Errorf("accepted %q", source)
		}
	}
}

type failingReader struct{}

func (failingReader) Read([]byte) (int, error) { return 0, io.ErrUnexpectedEOF }

func TestContainerReadFailure(t *testing.T) {
	_, err := Parse(failingReader{})
	if !errors.Is(err, io.ErrUnexpectedEOF) {
		t.Fatalf("read error lost: %v", err)
	}
}
