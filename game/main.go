package main

import (
    "log"
    "os"
    "image"
    "image/png"

    "github.com/kazzmir/paintown/game/lib/coroutine"

    "github.com/hajimehoshi/ebiten/v2"
    "github.com/hajimehoshi/ebiten/v2/inpututil"
    "github.com/hajimehoshi/ebiten/v2/ebitenutil"
)

type DrawFunc func (*ebiten.Image)

type PaintownLevel struct {
}

type Engine struct {
    Coroutine *coroutine.Coroutine
    Drawer func(*ebiten.Image)
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

func runGame(yield coroutine.YieldFunc, setDraw func(drawer DrawFunc) DrawFunc) error {

    drawer := func (screen *ebiten.Image) {
    }

    oldDrawer := setDraw(drawer)
    defer setDraw(oldDrawer)

    for {
        keys := inpututil.AppendJustPressedKeys(nil)
        for _, key := range keys {
            if key == ebiten.KeyTab {
                return nil
            }
        }

        yield()
    }
}

func makeRunMenu(setDraw func(drawer DrawFunc) DrawFunc) (func (yield coroutine.YieldFunc) error, error) {
    backgroundPng, err := loadPng("menu/paintown.png")
    if err != nil {
        return nil, err
    }

    background := ebiten.NewImageFromImage(backgroundPng)

    drawer := func(screen *ebiten.Image) {
        var options ebiten.DrawImageOptions
        screen.DrawImage(background, &options)
        ebitenutil.DebugPrintAt(screen, "Start", 300, 240)
    }

    logic := func(yield coroutine.YieldFunc) error {
        oldDrawer := setDraw(drawer)
        defer setDraw(oldDrawer)

        for {
            keys := inpututil.AppendJustPressedKeys(nil)
            for _, key := range keys {
                if key == ebiten.KeyEnter {
                    err := runGame(yield, setDraw)
                    if err != nil {
                        return err
                    }
                }
            }

            yield()
        }
    }

    return logic, nil
}

func MakeEngine() (*Engine, error) {
    var engine *Engine

    setDraw := func(drawer DrawFunc) DrawFunc {
        old := engine.Drawer
        engine.Drawer = drawer
        return old
    }

    menuLogic, err := makeRunMenu(setDraw)

    if err != nil {
        return nil, err
    }

    engine = &Engine{
        Coroutine: coroutine.MakeCoroutine(menuLogic),
        Drawer: func(screen *ebiten.Image) {
        },
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

    return engine.Coroutine.Run()
}

func (engine *Engine) Draw(screen *ebiten.Image) {
    engine.Drawer(screen)
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
