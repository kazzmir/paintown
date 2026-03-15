package main

import (
    "log"
    "os"

    "github.com/kazzmir/paintown/game/paintown"

    "github.com/hajimehoshi/ebiten/v2"
    "github.com/hajimehoshi/ebiten/v2/inpututil"
)

const ScreenWidth = 320
const ScreenHeight = 240

type Engine struct {
    Character paintown.CharacterDefinition
    Animation *paintown.Animation
}

func MakeEngine(character paintown.CharacterDefinition) *Engine {
    player := paintown.PaintownCharacter{Definition: character}
    idle, err := player.LoadAnimation("idle")
    if err != nil {
        log.Fatal(err)
    }

    idle.Update(true)

    return &Engine{
        Character: character,
        Animation: idle,
    }
}

func (engine *Engine) Update() error {
    keys := inpututil.AppendJustPressedKeys(nil)

    for _, key := range keys {
        switch key {
            case ebiten.KeyEscape, ebiten.KeyCapsLock:
                return ebiten.Termination
        }
    }

    return nil
}

func (engine *Engine) Draw(screen *ebiten.Image) {
    frame := engine.Animation.CurrentFrame()

    if frame == nil {
        return
    }

    var options ebiten.DrawImageOptions
    options.GeoM.Translate(float64((ScreenWidth - frame.Bounds().Dx()) / 2), float64((ScreenHeight - frame.Bounds().Dy()) / 2))

    screen.DrawImage(frame, &options)
}

func (engine *Engine) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
    return ScreenWidth, ScreenHeight
}

func main() {

    if len(os.Args) < 2 {
        log.Fatal("Usage: go run main.go <path_to_rom>")
    }

    path := os.Args[1]

    definition, err := paintown.LoadDefinition(path)
    if err != nil {
        log.Fatal(err)
    }

    err = ebiten.RunGame(MakeEngine(definition))
    if err != nil {
        log.Fatal(err)
    }
}
