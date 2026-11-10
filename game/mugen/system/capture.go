package main

import (
	"fmt"
	"image"
	"image/png"
	"log"
	"os"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
)

// Capture follows the final composited draw, so HUD/debug/training overlays and
// the fitted viewport are included. Each requested press saves one lossless PNG;
// existing user files are never replaced.
func (e *Engine) captureFrame(screen *ebiten.Image) {
	if !e.capturePending {
		return
	}
	e.capturePending = false
	directory := e.screenshotDir
	if directory == "" {
		directory = filepath.Join(e.dataDir, "screenshots")
	}
	if err := os.MkdirAll(directory, 0755); err != nil {
		log.Printf("Screenshot warning: %v", err)
		return
	}
	var file *os.File
	var path string
	for number := 0; number < 1000000; number++ {
		path = filepath.Join(directory, fmt.Sprintf("mugen%06d.png", number))
		var err error
		file, err = os.OpenFile(path, os.O_WRONLY|os.O_CREATE|os.O_EXCL, 0600)
		if os.IsExist(err) {
			continue
		}
		if err != nil {
			log.Printf("Screenshot warning: %v", err)
			return
		}
		break
	}
	if file == nil {
		log.Print("Screenshot warning: screenshot directory is full")
		return
	}
	pixels := image.NewRGBA(image.Rect(0, 0, screen.Bounds().Dx(), screen.Bounds().Dy()))
	screen.ReadPixels(pixels.Pix)
	err := png.Encode(file, pixels)
	closed := file.Close()
	if err == nil {
		err = closed
	}
	if err != nil {
		os.Remove(path)
		log.Printf("Screenshot warning: %v", err)
		return
	}
	log.Printf("Saved screenshot: %s", path)
}
