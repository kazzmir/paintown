package main

import (
    "log"
    "os"
    "fmt"
    "strconv"
    "strings"
    "image"
    "image/png"
    "image/color"
    "path/filepath"

    "github.com/kazzmir/paintown/game/lib/coroutine"
    "github.com/kazzmir/paintown/game/lib/sexp"

    "github.com/hajimehoshi/ebiten/v2"
    "github.com/hajimehoshi/ebiten/v2/vector"
    "github.com/hajimehoshi/ebiten/v2/inpututil"
    "github.com/hajimehoshi/ebiten/v2/ebitenutil"
)

const ScreenWidth = 640
const ScreenHeight = 480

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

// images use 255,0,255 as the transparent color
func convertTransparency(img image.Image) image.Image {
    bounds := img.Bounds()
    newImg := image.NewNRGBA(bounds)

    for x := bounds.Min.X; x < bounds.Max.X; x++ {
        for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
            r, g, b, _ := img.At(x, y).RGBA()
            if r == 0xFFFF && g == 0 && b == 0xFFFF {
                newImg.Set(x, y, image.Transparent)
            } else {
                newImg.Set(x, y, img.At(x, y))
            }
        }
    }

    return newImg
}

type Animation struct {
    Frame *ebiten.Image
    Events []AnimationEvent
    CurrentEvent int

    OffsetX int
    OffsetY int

    Delay int
    CurrentDelay int
}

func MakeAnimation(events []AnimationEvent) *Animation {
    return &Animation{
        Events: events,
        Delay: 1, // set delay to something non-zero to prevent immediately looping through all events
    }
}

func (animation *Animation) CurrentFrame() *ebiten.Image {
    return animation.Frame
}

func (animation *Animation) Update() {
    if animation.CurrentDelay > 0 {
        animation.CurrentDelay -= 1
    } else {
        if len(animation.Events) == 0 {
            return
        }
        now := animation.CurrentEvent
        for animation.CurrentDelay == 0 {
            animation.Events[animation.CurrentEvent].Update(animation)
            animation.CurrentEvent += 1
            if animation.CurrentEvent >= len(animation.Events) {
                animation.CurrentEvent = 0
            }

            // looped without setting delay, this would have been an infinite loop
            if animation.CurrentEvent == now {
                log.Printf("Warning: animation looped without setting delay (probably missing a frame)")
                break
            }
        }
    }
}

type AnimationEvent interface {
    Update(*Animation)
}

type AnimationEventFrame struct {
    Image *ebiten.Image
}

func (frameEvent *AnimationEventFrame) Update(animation *Animation) {
    animation.Frame = frameEvent.Image
    animation.CurrentDelay = animation.Delay
}

type AnimationEventDelay struct {
    Delay float32
}

func (delayEvent *AnimationEventDelay) Update(animation *Animation) {
    animation.Delay = int(delayEvent.Delay)
}

type AnimationEventOffset struct {
    X int
    Y int
}

func (offsetEvent *AnimationEventOffset) Update(animation *Animation) {
    animation.OffsetX = offsetEvent.X
    animation.OffsetY = offsetEvent.Y
}

type AnimationEventStatus struct {
    Status string
}

func (statusEvent *AnimationEventStatus) Update(animation *Animation) {
    // TODO
}

type AnimationEventType struct {
    Type string
}

func (typeEvent *AnimationEventType) Update(animation *Animation) {
    // TODO
}

type AnimationEventKeys struct {
    Keys []string
}

func (keysEvent *AnimationEventKeys) Update(animation *Animation) {
    // TODO
}

type AnimationEventRange struct {
    Range int
}

func (rangeEvent *AnimationEventRange) Update(animation *Animation) {
    // TODO
}

type AnimationEventAttack struct {
    // TODO
    /* (attack (box (x1 ...) (y1 ...) (x2 ...) (y2 ...) (force x y) (damage d)))
     */
}

type AnimationEventSequence struct {
    // TODO
    /* (sequence previous next)
     */
}

type AnimationEventRelativeOffset struct {
    X int
    Y int
}

func (relativeOffsetEvent *AnimationEventRelativeOffset) Update(animation *Animation) {
    animation.OffsetX += relativeOffsetEvent.X
    animation.OffsetY += relativeOffsetEvent.Y
}

func MakeAnimationFromDefinition(baseDirectory string, definition *sexp.SExpr) (*Animation, error) {
    var events []AnimationEvent
    for _, child := range definition.Children {
        switch strings.ToLower(child.Name) {
            case "name":
                // skip the name
            case "basedir":
                if len(child.Children) > 0 {
                    baseDirectory = child.GetValue(0)
                }
            case "delay":
                if len(child.Children) > 0 {
                    value := child.GetValue(0)
                    delay, err := strconv.ParseFloat(value, 32)
                    if err != nil {
                        log.Printf("Error parsing delay value '%v': %v", value, err)
                    } else {
                        events = append(events, &AnimationEventDelay{Delay: float32(delay)})
                    }
                }
            case "offset":
                if len(child.Children) >= 2 {
                    xValue := child.GetValue(0)
                    yValue := child.GetValue(1)
                    x, errX := strconv.Atoi(xValue)
                    y, errY := strconv.Atoi(yValue)
                    if errX != nil || errY != nil {
                        log.Printf("Error parsing offset values '%v', '%v': %v, %v", xValue, yValue, errX, errY)
                    } else {
                        events = append(events, &AnimationEventOffset{X: x, Y: y})
                    }
                }
            case "attack":
                log.Printf("Handle 'attack'")
            case "range":
                log.Printf("Handle 'range'")
            case "status":
                log.Printf("Handle 'status'")
            case "sequence":
                log.Printf("Handle 'sequence'")
            case "type":
                log.Printf("Handle 'type'")
            case "keys":
                log.Printf("Handle 'keys'")
            case "relative-offset":
                if len(child.Children) >= 2 {
                    xValue := child.GetValue(0)
                    yValue := child.GetValue(1)
                    x, errX := strconv.Atoi(xValue)
                    y, errY := strconv.Atoi(yValue)
                    if errX != nil || errY != nil {
                        log.Printf("Error parsing relative offset values '%v', '%v': %v, %v", xValue, yValue, errX, errY)
                    } else {
                        events = append(events, &AnimationEventRelativeOffset{X: x, Y: y})
                    }
                }
            case "frame":
                frame := child.GetValue(0)
                img, err := loadPng(filepath.Join(baseDirectory, frame))
                if err != nil {
                    return nil, err
                }
                events = append(events, &AnimationEventFrame{
                    Image: ebiten.NewImageFromImage(convertTransparency(img)),
                })
            default:
                log.Printf("Unknown animation event type '%v'", child.Name)
        }
    }

    return MakeAnimation(events), nil
}

type PaintownCharacter struct {
    Definition CharacterDefinition
}

func (character *PaintownCharacter) LoadAnimation(name string) (*Animation, error) {
    animations := character.Definition.FindAll("character", "anim")

    // log.Printf("Found %v animations for character %v", len(animations), character.Definition.Name)

    for _, animation := range animations {
        animationName := animation.GetChild("name")
        if animationName != nil && animationName.GetValue(0) == name {
            base := filepath.Join("players", strings.ToLower(character.Definition.Name), name)
            return MakeAnimationFromDefinition(base, animation)
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

    animations := make(map[string]*Animation)
    var allPlayers []*PaintownCharacter
    choices, err := os.ReadDir(dataPath("players"))
    if err == nil {
        for _, choice := range choices {
            if choice.IsDir() {
                player, err := MakePaintownPlayer(choice.Name())
                if err != nil {
                    log.Printf("Error loading player '%v': %v", choice.Name(), err)
                } else {
                    log.Printf("Loaded player '%v'", choice.Name())
                    allPlayers = append(allPlayers, player)

                    idle, err := player.LoadAnimation("idle")
                    if err != nil {
                        log.Printf("Error loading idle animation for player '%v': %v", choice.Name(), err)
                    } else {
                        animations[player.Definition.Name] = idle
                        idle.Update()
                    }
                }
            }
        }
    }

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

        x := 40.0
        y := 130.0

        options.GeoM.Translate(x, y)
        options.GeoM.Translate(0, float64(animation.CurrentFrame().Bounds().Dy() * -1))
        options.GeoM.Scale(2, 2)
        screen.DrawImage(animation.CurrentFrame(), &options)

        options.GeoM.Reset()
        options.GeoM.Translate(x, -y)
        options.GeoM.Translate(0, float64(animation.CurrentFrame().Bounds().Dy() * -1))
        options.GeoM.Scale(2, -2)
        options.ColorScale.ScaleAlpha(0.5)
        screen.DrawImage(animation.CurrentFrame(), &options)

        gridStartX := 280
        gridStartY := 50
        gridX := gridStartX
        gridY := gridStartY
        gridSize := 70
        gap := 18

        options.ColorScale.Reset()

        playerIndex := 0
        for gridX < ScreenWidth && playerIndex < len(allPlayers) {
            player := allPlayers[playerIndex]
            anim, ok := animations[player.Definition.Name]
            if ok && anim.CurrentFrame() != nil {
                area := screen.SubImage(image.Rect(gridX, gridY, gridX + gridSize, gridY + gridSize)).(*ebiten.Image)

                options.GeoM.Reset()
                options.GeoM.Translate(-float64(anim.CurrentFrame().Bounds().Dx()) / 2, float64(anim.CurrentFrame().Bounds().Dy() * -1))
                options.GeoM.Scale(0.6, 0.6)
                options.GeoM.Translate(float64(gridX), float64(gridY))
                options.GeoM.Translate(float64(gridSize) / 2, float64(gridSize))
                vector.FillRect(area, float32(gridX), float32(gridY), float32(gridSize), float32(gridSize), color.RGBA{A:255}, false)
                area.DrawImage(anim.CurrentFrame(), &options)
                vector.StrokeRect(area, float32(gridX) + 1, float32(gridY) + 1, float32(gridSize) - 2, float32(gridSize) - 2, 2, color.RGBA{R:200, G:200, B: 200, A: 255}, false)

                gridX += gridSize + gap
                if gridX + gridSize > ScreenWidth {
                    gridX = gridStartX
                    gridY += gridSize + gap
                }

                playerIndex += 1
            }
        }

    }

    oldDrawer := setDraw(drawer)
    defer setDraw(oldDrawer)

    counter := uint64(0)
    var keys []ebiten.Key
    for {
        counter += 1
        keys = inpututil.AppendJustPressedKeys(keys[:0])
        for _, key := range keys {
            if key == ebiten.KeyTab {
                return nil
            }
        }

        animation.Update()

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
    return ScreenWidth, ScreenHeight
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
    }

    err = ebiten.RunGame(engine)
    if err != nil {
        log.Printf("Error: %v", err)
    }
}
