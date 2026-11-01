package main

import (
    "sync"
    "context"
    "log"
    "os"
    "runtime/pprof"

    "github.com/kazzmir/paintown/game/paintown"

    "github.com/hajimehoshi/ebiten/v2"
)

type Engine struct {
    run sync.Once
    done context.Context
    cancel context.CancelFunc
}

func MakeEngine() *Engine {
    quit, cancel := context.WithCancel(context.Background())
    return &Engine{
        done: quit,
        cancel: cancel,
    }
}

func (engine *Engine) Update() error {
    engine.run.Do(func() {
        go func(){
            defer engine.cancel()
            log.Printf("Start load")

            factory := paintown.MakeObjectFactory()

            paths := []string{
                "chars/yashiro/yashiro.txt",
                "chars/angel/angel.txt",
                "chars/billy/billy.txt",
                "chars/heavy/heavy.txt",
                "chars/punk/punk.txt",
                "chars/rugal/rugal.txt",
            }

            for _, path := range paths {
                object := paintown.BlockObject{
                    Type: "enemy",
                    Path: path,
                }

                _, err := paintown.MakeEnemy(object, factory, &paintown.BehaviorNormal{})
                if err != nil {
                    log.Printf("Error creating enemy %v: %v", path, err)
                }
            }

            log.Printf("End load")
        }()
    })

    return engine.done.Err()
}

func (engine *Engine) Draw(screen *ebiten.Image) {
}

func (engine *Engine) Layout(outsideWidth int, outsideHeight int) (int, int) {
    return 320, 240
}

func main() {
    profile, err := os.Create("level-profile.cpu")
    if err != nil {
        log.Printf("Unable to make level-profile.cpu")
    } else {
        defer profile.Close()
        pprof.StartCPUProfile(profile)
        defer pprof.StopCPUProfile()
    }

    log.SetFlags(log.Ldate | log.Lshortfile | log.Lmicroseconds)
    ebiten.RunGame(MakeEngine())
}
