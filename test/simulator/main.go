package main

import (
    "log"
    "sync"
    "image"
    "image/color"
    "flag"

    "github.com/kazzmir/paintown/game/data"
    "github.com/kazzmir/paintown/game/paintown"

    "github.com/hajimehoshi/ebiten/v2"
    "github.com/hajimehoshi/ebiten/v2/inpututil"
    "github.com/hajimehoshi/ebiten/v2/vector"
    /*
    "github.com/hajimehoshi/ebiten/v2/ebitenutil"
    */
)

const ScreenWidth = 320
const ScreenHeight = 240

type Engine struct {
    Player *paintown.PlayerState
    Enemy *paintown.Enemy
    Level paintown.Level
    Counter uint64

    Load func()
    Init sync.Once
}

type DoNothing struct {}
func (nothing *DoNothing) Update(enemy *paintown.Enemy, level *paintown.Level, playerInfo paintown.PlayerInfo) {
}

func MakeEngine(playerDefinition paintown.CharacterDefinition, enemyDefinition paintown.CharacterDefinition) *Engine {
    engine := &Engine{
        Level: paintown.Level{ZMinimum: 200, ZMaximum: 201},
    }

    engine.Load = func() {
        player := paintown.PaintownCharacter{Definition: playerDefinition}

        playerState, err := paintown.MakePlayerState(&player, &engine.Level)

        if err != nil {
            log.Fatal(err)
        }

        playerState.X = 100
        playerState.Y = 0
        playerState.Z = float64(engine.Level.ZMinimum)

        enemy, err := paintown.MakeEnemyFromDefinition(paintown.BlockObject{
            Coords: image.Pt(250, 200),
        }, enemyDefinition, paintown.MakeObjectFactory(), &DoNothing{})
        if err != nil {
            log.Fatal(err)
        }

        enemy.Facing = paintown.FacingLeft

        engine.Player = playerState
        engine.Enemy = enemy
    }

    return engine
}

func (engine *Engine) Update() error {
    engine.Init.Do(engine.Load)
    engine.Counter += 1

    var inputState paintown.InputState
    keys := inpututil.AppendPressedKeys(nil)
    for _, key := range keys {
        switch key {
            case ebiten.KeyArrowRight:
                inputState.HeldRight = true
            case ebiten.KeyArrowLeft:
                inputState.HeldLeft = true
            case ebiten.KeyArrowDown:
                inputState.HeldDown = true
            case ebiten.KeyArrowUp:
                inputState.HeldUp = true
            case ebiten.KeySpace:
                inputState.HeldJump = true
            case ebiten.KeyA:
                inputState.HeldAttack1 = true
            case ebiten.KeyS:
                inputState.HeldAttack2 = true
        }
    }

    keys = inpututil.AppendJustPressedKeys(keys[:0])
    for _, key := range keys {
        switch key {
            case ebiten.KeyEscape, ebiten.KeyCapsLock:
                return ebiten.Termination
            case ebiten.KeyArrowRight:
                inputState.Right = true
            case ebiten.KeyArrowLeft:
                inputState.Left = true
            case ebiten.KeyArrowDown:
                inputState.Down = true
            case ebiten.KeyArrowUp:
                inputState.Up = true
            case ebiten.KeySpace:
                inputState.Jump = true
            case ebiten.KeyA:
                inputState.Attack1 = true
            case ebiten.KeyS:
                inputState.Attack2 = true
        }
    }

    if engine.Player != nil {
        engine.Player.Update(inputState, &engine.Level, &paintown.DummySystem{}, engine.Counter)
    }

    if engine.Enemy != nil {
        engine.Enemy.Update(&engine.Level, engine.Player, func(state paintown.EnemyState) {}, &paintown.DummySystem{})
    }

    return nil
}

func (engine *Engine) Draw(screen *ebiten.Image) {
    screen.Fill(color.RGBA{R: 64, G: 64, B: 64, A: 255})

    vector.StrokeLine(screen, 0, float32(engine.Level.ZMinimum), ScreenWidth, float32(engine.Level.ZMinimum), 2, color.NRGBA{R: 255, G: 255, B: 255, A: 100}, false)

    if engine.Player != nil {
        paintown.DrawPlayer(engine.Player, 0, ebiten.GeoM{}, screen)
    }

    if engine.Enemy != nil {
        paintown.DrawEnemy(engine.Enemy, 0, ebiten.GeoM{}, screen)
    }
}

func (engine *Engine) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
    return ScreenWidth, ScreenHeight
}

func main() {
    log.SetFlags(log.Lshortfile | log.Ldate | log.Lmicroseconds)

    playerPath := flag.String("player", "", "Path to player character definition")
    enemyPath := flag.String("enemy", "", "Path to enemy character definition")
    flag.Parse()

    ebiten.SetWindowSize(ScreenWidth*3, ScreenHeight*3)
    ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)
    ebiten.SetTPS(90)

    playerArg := *playerPath
    if playerArg == "" {
        playerArg = "players/akuma/akuma.txt"
    }

    enemyArg := *enemyPath
    if enemyArg == "" {
        enemyArg = "chars/yashiro/yashiro.txt"
    }

    log.Printf("Loading player definition from %s", playerArg)
    playerDefinition, err := paintown.LoadDefinition(data.DataPath(playerArg))
    if err != nil {
        log.Fatal(err)
    }

    log.Printf("Loading enemy definition from %s", enemyArg)
    enemyDefinition, err := paintown.LoadDefinition(data.DataPath(enemyArg))
    if err != nil {
        log.Fatal(err)
    }

    err = ebiten.RunGame(MakeEngine(playerDefinition, enemyDefinition))
    if err != nil {
        log.Fatal(err)
    }
}
