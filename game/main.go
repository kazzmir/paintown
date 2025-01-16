package main

import (
    "log"
    "os"
    "image"
    "image/png"

    "github.com/hajimehoshi/ebiten/v2"
    "github.com/hajimehoshi/ebiten/v2/inpututil"
)

type Engine struct {
    Background *ebiten.Image
}

func dataPath(path string) string {
    return "paintown-data/" + path
}

func loadPng(path string) (image.Image, error) {
    file, err := os.Open(dataPath(path))
    if err != nil {
        return nil, err
    }
    defer file.Close()

    img, err := png.Decode(file)
    if err != nil {
        return nil, err
    }

    return img, nil
}

func MakeEngine() (*Engine, error) {
    backgroundPng, err := loadPng("menu/paintown.png")
    if err != nil {
        return nil, err
    }

    background := ebiten.NewImageFromImage(backgroundPng)

    engine := &Engine{
        Background: background,
    }

    return engine, nil
}

func (engine *Engine) Update() error {
    keys := inpututil.AppendJustPressedKeys(nil)

    for _, key := range keys {
        if key == ebiten.KeyEscape || key == ebiten.KeyCapsLock {
            return ebiten.Termination
        }
    }

    return nil
}

func (engine *Engine) Draw(screen *ebiten.Image) {
    var options ebiten.DrawImageOptions
    screen.DrawImage(engine.Background, &options)
}

func (engine *Engine) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
    return 640, 480
}

func main(){
    log.SetFlags(log.Ldate | log.Lmicroseconds | log.Lshortfile)

    ebiten.SetWindowSize(1024, 768)
    ebiten.SetWindowTitle("Paintown")
    ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)

    engine, err := MakeEngine()
    if err != nil {
        log.Printf("Error: %v", err)
    }

    err = ebiten.RunGame(engine)
    if err != nil {
        log.Printf("Error: %v", err)
    }
}
