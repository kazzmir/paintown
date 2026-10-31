package main

import (
    "log"
    "fmt"

    "github.com/kazzmir/paintown/game/lib/coroutine"
    "github.com/kazzmir/paintown/game/paintown"
    "github.com/kazzmir/paintown/game/data"

    "github.com/hajimehoshi/ebiten/v2"
    "github.com/hajimehoshi/ebiten/v2/audio"
    "github.com/hajimehoshi/ebiten/v2/inpututil"
)

type Engine struct {
    Coroutine *coroutine.Coroutine
    Drawer func(*ebiten.Image)
}

func makeRunMenu(setDraw func(drawer data.DrawFunc) data.DrawFunc, audioContext *audio.Context) (func (yield coroutine.YieldFunc) error, error) {
    root, err := paintown.LoadMenu("menu/main.txt", nil)
    if err != nil {
        return nil, err
    }
    backgroundPng, err := data.LoadPng("menu/paintown.png")
    if err != nil {
        return nil, err
    }
    background := ebiten.NewImageFromImage(backgroundPng)
    sounds := paintown.MakeAudioManager(audioContext)
    play := func(path string) {
        if err := sounds.PlaySound(path); err != nil {
            log.Printf("Menu sound: %v", err)
        }
    }
    pressed := func(key ebiten.Key) bool {
        duration := inpututil.KeyPressDuration(key)
        return duration == 1 || (duration > 25 && duration % 6 == 0)
    }

    logic := func(yield coroutine.YieldFunc) error {
        current := root
        var stack []*paintown.Menu
        drawer := func(screen *ebiten.Image) { current.Draw(screen) }
        oldDrawer := setDraw(drawer)
        defer setDraw(oldDrawer)
        backToMenu := fmt.Errorf("back to menu")
        gameYield := func() error {
            if err := yield(); err != nil { return err }
            if inpututil.IsKeyJustPressed(ebiten.KeyEscape) || inpututil.IsKeyJustPressed(ebiten.KeyCapsLock) { return backToMenu }
            return nil
        }
        adventure := func() error {
            // Consume the menu confirmation before character selection.
            if err := gameYield(); err != nil { return err }
            player, err := paintown.ChooseCharacter(gameYield, background, setDraw)
            if err != nil { return err }
            cache := paintown.MakeObjectFactory()
            for i := range 10 {
                if err := paintown.RunLevelWithCache(player, yield, setDraw, fmt.Sprintf("paintown/levels/%v.txt", i+1), audioContext, cache); err != nil { return err }
            }
            return nil
        }
        for {
            current.Update()
            if pressed(ebiten.KeyArrowDown) || pressed(ebiten.KeyS) {
                if current.Move(1) { play(current.SelectSound) }
            }
            if pressed(ebiten.KeyArrowUp) || pressed(ebiten.KeyW) {
                if current.Move(-1) { play(current.SelectSound) }
            }
            if inpututil.IsKeyJustPressed(ebiten.KeyEscape) || inpututil.IsKeyJustPressed(ebiten.KeyCapsLock) || inpututil.IsKeyJustPressed(ebiten.KeyBackspace) {
                play(current.BackSound)
                if len(stack) == 0 { return ebiten.Termination }
                current = stack[len(stack)-1]
                stack = stack[:len(stack)-1]
            } else if (inpututil.IsKeyJustPressed(ebiten.KeyEnter) || inpututil.IsKeyJustPressed(ebiten.KeySpace)) && len(current.Options) > 0 {
                option := current.Options[current.Selected]
                play(current.OKSound)
                switch option.Kind {
                case "menu":
                    next, err := current.OpenSubmenu(option)
                    if err != nil {
                        log.Printf("Menu: %v", err)
                        current.Notice = "Could not open this menu"
                    } else {
                        stack = append(stack, current)
                        current = next
                    }
                case "quit":
                    return ebiten.Termination
                case "adventure":
                    err := adventure()
                    if err == paintown.ErrReturnToMainMenu {
                        current = root
                        stack = nil
                    }
                    setDraw(drawer)
                    if err != nil && err != backToMenu && err != paintown.ErrReturnToMainMenu { return err }
                default:
                    current.Notice = option.Name + " is not implemented yet"
                }
            }
            if err := yield(); err != nil { return err }
        }
    }
    return logic, nil
}

func MakeEngine() (*Engine, error) {
    var engine *Engine

    setDraw := func(drawer data.DrawFunc) data.DrawFunc {
        old := engine.Drawer
        engine.Drawer = drawer
        return old
    }

    audioContext := audio.NewContext(44100)

    menuLogic, err := makeRunMenu(setDraw, audioContext)

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
    return engine.Coroutine.Run()
}

func (engine *Engine) Draw(screen *ebiten.Image) {
    engine.Drawer(screen)
}

func (engine *Engine) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
    return data.ScreenWidth, data.ScreenHeight
}

func main(){
    log.SetFlags(log.Ldate | log.Lmicroseconds | log.Lshortfile)

    ebiten.SetWindowSize(1024, 768)
    ebiten.SetWindowTitle("Paintown")
    ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)
    ebiten.SetTPS(90)

    engine, err := MakeEngine()
    if err != nil {
        log.Printf("Error: %v", err)
        return
    }

    err = ebiten.RunGame(engine)
    if err != nil {
        log.Printf("Error: %v", err)
    }
}
