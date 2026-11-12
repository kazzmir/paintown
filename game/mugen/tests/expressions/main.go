package main

import (
	"bufio"
	"bytes"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"os"

	"github.com/kazzmir/paintown/game/mugen/internal/exprfixture"
)

func run(r io.Reader, w io.Writer) error {
	scanner := bufio.NewScanner(r)
	scanner.Buffer(make([]byte, 4096), 1024*1024)
	encoder := json.NewEncoder(w)
	line := 0
	for scanner.Scan() {
		line++
		if len(bytes.TrimSpace(scanner.Bytes())) == 0 {
			continue
		}
		var fixture exprfixture.Fixture
		decoder := json.NewDecoder(bytes.NewReader(scanner.Bytes()))
		decoder.DisallowUnknownFields()
		if err := decoder.Decode(&fixture); err != nil {
			return fmt.Errorf("line %d: %w", line, err)
		}
		if err := decoder.Decode(new(any)); err != io.EOF {
			return fmt.Errorf("line %d: trailing JSON data", line)
		}
		observations, err := exprfixture.Run(fixture)
		for _, obs := range observations {
			if outErr := encoder.Encode(obs); outErr != nil {
				return outErr
			}
		}
		if err != nil {
			return fmt.Errorf("line %d: %w", line, err)
		}
	}
	return scanner.Err()
}

func main() {
	input := flag.String("input", "-", "JSONL fixtures, or - for stdin")
	flag.Parse()
	r := os.Stdin
	if *input != "-" {
		var err error
		r, err = os.Open(*input)
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
		defer r.Close()
	}
	if err := run(r, os.Stdout); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
