package main

import (
    "log"
    "os"
    "fmt"
    "image"
    "image/png"
    "path/filepath"

    "github.com/kazzmir/paintown/game/lib/coroutine"
    "github.com/kazzmir/paintown/game/lib/sexp"

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

type Animation struct {
    Frames []*ebiten.Image
    Current int
}

func MakeAnimation(frames []*ebiten.Image) *Animation {
    return &Animation{
        Frames: frames,
        Current: 0,
    }
}

func (animation *Animation) CurrentFrame() *ebiten.Image {
    if animation.Current < len(animation.Frames) {
        return animation.Frames[animation.Current]
    }

    return nil
}

func (animation *Animation) NextFrame() {
    animation.Current = (animation.Current + 1) % len(animation.Frames)
}

type PaintownCharacter struct {
    Definition CharacterDefinition
}

func (character *PaintownCharacter) LoadAnimation(name string) (*Animation, error) {
    animations := character.Definition.FindAll("character", "anim")

    log.Printf("Found %v animations for character %v", len(animations), character.Definition.Name)

    for _, animation := range animations {
        animationName := animation.GetChild("name")
        if animationName != nil && animationName.GetValue(0) == name {
            frames := animation.GetChild("frames")
            if frames != nil {
                var images []*ebiten.Image
                for i := range len(frames.Children) {
                    frame := frames.GetValue(i)
                    img, err := loadPng(frame)
                    if err != nil {
                        return nil, err
                    }
                    images = append(images, ebiten.NewImageFromImage(img))
                }
                return MakeAnimation(images), nil
            }
        }
    }

    return nil, fmt.Errorf("Animation %v not found for character %v", name, character.Definition.Name)
}

type CharacterDefinition struct {
    Name string
    SExpr *sexp.SExpr
}

func (definition *CharacterDefinition) FindAll(names ...string) []*sexp.SExpr {
    var results []*sexp.SExpr

    current := definition.SExpr

    if current.Name != names[0] {
        return results
    }

    names = names[1:]

    for check := range len(names) - 1 {
        for _, child := range current.Children {
            if current.Name == names[check] {
                current = child
            }
        }
    }

    last := names[len(names) - 1]
    for _, child := range current.Children {
        if child.Name == last {
            results = append(results, child)
        }
    }

    return results
}

// a definition file is a parentheses delimited set of values
func loadDefinition(path string) (CharacterDefinition, error) {
    raw, err := sexp.ReadSExpression(path)
    if err != nil {
        return CharacterDefinition{}, err
    }

    value := raw.GetChild("name")
    return CharacterDefinition{
        Name: value.GetValue(0),
        SExpr: raw,
    }, nil
}

func MakePaintownPlayer(name string) (*PaintownCharacter, error) {
    path := dataPath("players/" + name)

    definitionPath := filepath.Join(path, name + ".txt")
    definition, err := loadDefinition(definitionPath)
    if err != nil {
        return nil, err
    }

    player := &PaintownCharacter{
        Definition: definition,
    }

    return player, nil
}

func chooseCharacter(yield coroutine.YieldFunc, background *ebiten.Image, setDraw func(drawer DrawFunc) DrawFunc) error {

    player, err := MakePaintownPlayer("akuma")
    if err != nil {
        return err
    }

    animation, err := player.LoadAnimation("idle")
    if err != nil {
        return err
    }

    drawer := func (screen *ebiten.Image) {
        var options ebiten.DrawImageOptions
        screen.DrawImage(background, &options)

        options.GeoM.Translate(10, 10)
        screen.DrawImage(animation.CurrentFrame(), &options)
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
                    err := chooseCharacter(yield, background, setDraw)
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
