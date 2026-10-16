package main

import (
	"bytes"
	"os"
	"strings"
	"testing"
)

func TestFixtureReplay(t *testing.T) {
	data, err := os.ReadFile("../testdata/expressions.jsonl")
	if err != nil {
		t.Fatal(err)
	}
	var first, second bytes.Buffer
	if err := run(bytes.NewReader(data), &first); err != nil {
		t.Fatal(err)
	}
	if err := run(bytes.NewReader(data), &second); err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(first.Bytes(), second.Bytes()) {
		t.Fatal("same fixture produced different traces")
	}
	if got := bytes.Count(first.Bytes(), []byte("\n")); got != 15 {
		t.Fatalf("trace has %d observations, want 15", got)
	}
}

func TestInvalidFixtures(t *testing.T) {
	for _, data := range []string{
		`{"name":"bad","typo":1,"steps":[]}`,
		`{"name":"bad","steps":[]} {}`,
		`{"name":"bad","steps":[{"expr":"1","want":{"kind":"int","value":2}}]}`,
	} {
		if err := run(strings.NewReader(data), new(bytes.Buffer)); err == nil {
			t.Errorf("accepted %s", data)
		}
	}
}
