package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/sff"
	"github.com/kazzmir/paintown/game/mugen/stage"
)

func main() {
	dataDir := flag.String("data-dir", "data-new/mugen", "Path to MUGEN data directory")
	flag.Parse()

	fmt.Printf("Starting comprehensive MUGEN data audit in: %s\n", *dataDir)
	fmt.Println("Logs will be written to game/mugen/unresolved.log")

	// 1. Audit Stages
	stagesDir := filepath.Join(*dataDir, "stages")
	filepath.Walk(stagesDir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return nil
		}
		if !info.IsDir() && strings.HasSuffix(strings.ToLower(info.Name()), ".def") {
			fmt.Printf("Auditing Stage: %s\n", path)
			_, _ = stage.LoadStage(path, *dataDir)
		}
		return nil
	})

	// 2. Audit AIR files (they might be standalone or in char dirs)
	filepath.Walk(*dataDir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return nil
		}
		if !info.IsDir() && strings.HasSuffix(strings.ToLower(info.Name()), ".air") {
			fmt.Printf("Auditing AIR: %s\n", path)
			f, err := os.Open(path)
			if err == nil {
				_, _ = air.Parse(f)
				f.Close()
			}
		}
		return nil
	})

	// 3. Audit SFF files
	filepath.Walk(*dataDir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return nil
		}
		if !info.IsDir() && strings.HasSuffix(strings.ToLower(info.Name()), ".sff") {
			fmt.Printf("Auditing SFF: %s\n", path)
			f, err := os.Open(path)
			if err == nil {
				_, _ = sff.ParseWithPalette(f, nil)
				f.Close()
			}
		}
		return nil
	})

	fmt.Println("\nAudit complete. Please check game/mugen/unresolved.log for details.")
}
