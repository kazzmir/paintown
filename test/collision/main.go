package main

import (
    "log"
    "os"
    "sync"
    "image"
    "image/color"

    "github.com/kazzmir/paintown/game/paintown"

    "github.com/hajimehoshi/ebiten/v2"
    "github.com/hajimehoshi/ebiten/v2/ebitenutil"
    "github.com/hajimehoshi/ebiten/v2/vector"
    "github.com/hajimehoshi/ebiten/v2/inpututil"
)

const ScreenWidth = 320
const ScreenHeight = 240

type Engine struct {
    Character paintown.CharacterDefinition
    Animation *paintown.Animation

    Init sync.Once

    Box image.Rectangle
    Flip bool
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
        Box: image.Rect(10, 10, 30, 30).Add(image.Pt(50, 50)),
    }
}

func (engine *Engine) Update() error {
    engine.Init.Do(func() {
        log.Printf("Initialize collision")
        engine.Animation.InitializeCollision()
        log.Printf("Done")
    })

    keys := inpututil.AppendJustPressedKeys(nil)

    for _, key := range keys {
        switch key {
            case ebiten.KeyEscape, ebiten.KeyCapsLock:
                return ebiten.Termination
            case ebiten.KeySpace:
                frame := engine.Animation.CurrentFrame()
                for {
                    if engine.Animation.Update(true) {
                        break
                    }
                    if frame != engine.Animation.CurrentFrame() {
                        break
                    }
                }
            case ebiten.KeyTab:
                engine.Flip = !engine.Flip
        }
    }

    if ebiten.IsKeyPressed(ebiten.KeyArrowLeft) {
        engine.Box = engine.Box.Add(image.Pt(-1, 0))
    }
    if ebiten.IsKeyPressed(ebiten.KeyArrowRight) {
        engine.Box = engine.Box.Add(image.Pt(1, 0))
    }
    if ebiten.IsKeyPressed(ebiten.KeyArrowUp) {
        engine.Box = engine.Box.Add(image.Pt(0, -1))
    }
    if ebiten.IsKeyPressed(ebiten.KeyArrowDown) {
        engine.Box = engine.Box.Add(image.Pt(0, 1))
    }

    return nil
}

func (engine *Engine) Draw(screen *ebiten.Image) {

    frame := engine.Animation.CurrentFrame()

    if frame == nil {
        return
    }

    var options ebiten.DrawImageOptions
    options.GeoM.Translate(-float64(frame.Bounds().Dx() / 2), -float64(frame.Bounds().Dy()))
    if engine.Flip {
        options.GeoM.Scale(-1, 1)
    }
    x := ScreenWidth / 2
    y := ScreenHeight - 50
    options.GeoM.Translate(float64(x), float64(y))

    screen.DrawImage(frame, &options)

    collision := engine.Animation.CurrentCollision()
    if collision != nil {
        // mx, my := options.GeoM.Apply(0, 0)

        for _, box := range collision.Boxes {
            x1, y1 := options.GeoM.Apply(float64(box.Min.X), float64(box.Min.Y))
            x2, y2 := options.GeoM.Apply(float64(box.Max.X), float64(box.Max.Y))
            /*
            x1 := float32(box.Min.X) + float32(mx)
            y1 := float32(box.Min.Y) + float32(my)
            x2 := float32(box.Max.X) + float32(mx)
            y2 := float32(box.Max.Y) + float32(my)
            */
            x1, x2 = min(x1, x2), max(x1, x2)
            y1, y2 = min(y1, y2), max(y1, y2)
            vector.StrokeRect(screen, float32(x1), float32(y1), float32(x2-x1), float32(y2-y1), 1, color.RGBA{B: 255, A: 200}, false)
        }

        bx, by := options.GeoM.Apply(float64(frame.Bounds().Dx() / 2), float64(frame.Bounds().Dy()))
        if collision.Intersect(bx, by, engine.Box, engine.Flip) {
            ebitenutil.DebugPrint(screen, "Collision!")
        }
    }

    vector.StrokeRect(screen, float32(engine.Box.Min.X), float32(engine.Box.Min.Y), float32(engine.Box.Dx()), float32(engine.Box.Dy()), 1, color.RGBA{G: 255, A: 255}, false)
}

func (engine *Engine) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
    return ScreenWidth, ScreenHeight
}

func main() {
    log.SetFlags(log.Lshortfile | log.Ldate | log.Lmicroseconds)

    if len(os.Args) < 2 {
        log.Fatal("Usage: go run main.go <path_to_rom>")
    }

    ebiten.SetWindowSize(ScreenWidth*3, ScreenHeight*3)
    ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)

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
