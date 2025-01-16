package main

import (
    "log"

    "github.com/hajimehoshi/ebiten/v2"
)

type Engine struct {
}

func (engine *Engine) Update() error {
    return nil
}

func (engine *Engine) Draw(screen *ebiten.Image) {
}

func (engine *Engine) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
    return 1024, 768
}

func main(){
    log.SetFlags(log.Ldate | log.Lmicroseconds | log.Lshortfile)

    ebiten.SetWindowSize(1024, 768)
    ebiten.SetWindowTitle("Paintown")
    ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)

    engine := &Engine{}

    err := ebiten.RunGame(engine)
    if err != nil {
        log.Printf("Error: %v", err)
    }
}
