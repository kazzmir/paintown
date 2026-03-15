package main

import (
    "log"

    "github.com/hajimehoshi/ebiten/v2"
)

const ScreenWidth = 320
const ScreenHeight = 240

type Engine struct {
}

func (engine *Engine) Update() error {
    return nil
}

func (engine *Engine) Draw(screen *ebiten.Image) {
}

func (engine *Engine) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
    return ScreenWidth, ScreenHeight
}

func main() {

    err := ebiten.RunGame(&Engine{})
    if err != nil {
        log.Fatal(err)
    }
}
