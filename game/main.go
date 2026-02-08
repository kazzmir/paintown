package main

import (
    "log"
    "os"
    "fmt"
    "strconv"
    "strings"
    "math"
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

func (animation *Animation) Reset() {
    animation.CurrentEvent = 0
    animation.CurrentDelay = 0
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

func (character *PaintownCharacter) LoadAnimations() (map[string]*Animation, error) {
    animations := character.Definition.FindAll("character", "anim")

    // log.Printf("Found %v animations for character %v", len(animations), character.Definition.Name)
    out := make(map[string]*Animation)

    for _, animation := range animations {
        animationName := animation.GetChild("name")
        name := animationName.GetValue(0)
        base := filepath.Join("players", strings.ToLower(character.Definition.Name), name)
        animation, err := MakeAnimationFromDefinition(base, animation)
        if err == nil {
            out[name] = animation
        } else {
            log.Printf("Error loading animation '%v' for character '%v': %v", name, character.Definition.Name, err)
        }
    }

    return out, nil
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
    return definition.SExpr.FindAll(names...)
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

func interpolateColors(start, end color.RGBA, steps int) []color.RGBA {
    var colors []color.RGBA

    for i := 0; i < steps; i++ {
        t := float64(i) / float64(steps-1)
        r := uint8(float64(start.R)*(1-t) + float64(end.R)*t)
        g := uint8(float64(start.G)*(1-t) + float64(end.G)*t)
        b := uint8(float64(start.B)*(1-t) + float64(end.B)*t)
        a := uint8(float64(start.A)*(1-t) + float64(end.A)*t)
        colors = append(colors, color.RGBA{R: r, G: g, B: b, A: a})
    }

    return colors
}

func chooseCharacter(yield coroutine.YieldFunc, background *ebiten.Image, setDraw func(drawer DrawFunc) DrawFunc) (*PaintownCharacter, error) {

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

    if len(allPlayers) == 0 {
        return nil, fmt.Errorf("No players found")
    }

    counter := uint64(0)
    currentChoice := 0

    selectedColors := append(interpolateColors(color.RGBA{R:255, A:255}, color.RGBA{A: 255}, 20), interpolateColors(color.RGBA{A:255}, color.RGBA{R:255, A:255}, 20)...)

    drawer := func (screen *ebiten.Image) {
        animation := animations[allPlayers[currentChoice].Definition.Name]
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

                if currentChoice == playerIndex {
                    N := float64(len(selectedColors))
                    colorIndex := math.Sin(float64((counter / 3) % uint64(len(selectedColors))) * math.Pi * 2 / N) * N / 2 + N / 2
                    v := int(colorIndex) % len(selectedColors)
                    if v < 0 {
                        v += len(selectedColors)
                    }
                    vector.StrokeRect(area, float32(gridX) + 1, float32(gridY) + 1, float32(gridSize) - 2, float32(gridSize) - 2, 2, selectedColors[v], false)
                } else {
                    vector.StrokeRect(area, float32(gridX) + 1, float32(gridY) + 1, float32(gridSize) - 2, float32(gridSize) - 2, 2, color.RGBA{R:200, G:200, B: 200, A: 255}, false)
                }

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

    perRow := 4

    var keys []ebiten.Key
    for {
        counter += 1
        keys = inpututil.AppendJustPressedKeys(keys[:0])
        for _, key := range keys {
            switch key {
                case ebiten.KeyEnter:
                    return allPlayers[currentChoice], nil
                case ebiten.KeyArrowRight:
                    currentChoice = (currentChoice + 1) % len(allPlayers)
                case ebiten.KeyArrowLeft:
                    currentChoice = (currentChoice - 1 + len(allPlayers)) % len(allPlayers)
                case ebiten.KeyArrowDown:
                    currentChoice = (currentChoice + perRow) % len(allPlayers)
                case ebiten.KeyArrowUp:
                    currentChoice = (currentChoice - perRow + len(allPlayers)) % len(allPlayers)
            }
        }

        animation := animations[allPlayers[currentChoice].Definition.Name]

        animation.Update()

        yield()
    }
}

type Panel struct {
    Index int
    Image *ebiten.Image
}

type Level struct {
    ZMinimum int
    ZMaximum int
    Atmosphere string

    BackgroundParallax float32
    ForegroundParallax float32

    BackgroundImage *ebiten.Image
    FrontPanels []*ebiten.Image
    Panels map[int]*ebiten.Image

    PanelOrder []int
    Description string
}

func LoadLevel(path string) (*Level, error) {
    raw, err := sexp.ReadSExpression(dataPath(path))
    if err != nil {
        return nil, err
    }

    if strings.ToLower(raw.Name) != "level" {
        return nil, fmt.Errorf("Expected 'level' as root element, got '%v'", raw.Name)
    }

    zMinimum, _ := sexp.ReadValue[int](raw, "z/minimum", 0)
    zMaximum, _ := sexp.ReadValue[int](raw, "z/maximum", 0)

    backgroundParallax, _ := sexp.ReadValue[float32](raw, "background-parallax", 0)
    foregroundParallax, _ := sexp.ReadValue[float32](raw, "foreground-parallax", 0)

    if backgroundParallax <= 0 {
        backgroundParallax = 1
    }

    if foregroundParallax <= 0 {
        foregroundParallax = 1
    }

    var backgroundImage *ebiten.Image
    backgroundPath, _ := sexp.ReadValue[string](raw, "background", 0)
    if backgroundPath != "" {
        backgroundImagePng, err := loadPng(backgroundPath)
        if err != nil {
            log.Printf("Error loading background image '%v': %v", backgroundPath, err)
        } else {
            backgroundImage = ebiten.NewImageFromImage(convertTransparency(backgroundImagePng))
        }
    }

    var frontPanels []*ebiten.Image
    frontPanelsElements := raw.FindAll("level", "frontpanel")
    for _, panel := range frontPanelsElements {
        frontPanelPng, err := loadPng(panel.GetValue(0))
        if err != nil {
            log.Printf("Error loading front panel image '%v': %v", panel.GetValue(0), err)
        } else {
            frontPanels = append(frontPanels, ebiten.NewImageFromImage(convertTransparency(frontPanelPng)))
        }
    }

    panels := make(map[int]*ebiten.Image)
    panelElements := raw.FindAll("level", "panel")
    for _, panelElement := range panelElements {
        index, _ := panelElement.GetInt(0)
        imagePath := panelElement.GetValue(1)
        panelPng, err := loadPng(imagePath)
        if err != nil {
            log.Printf("Error loading panel image '%v': %v", imagePath, err)
        } else {
            panels[index] = ebiten.NewImageFromImage(convertTransparency(panelPng))
        }
    }

    var order []int
    orderElement := raw.GetChild("order")
    if orderElement != nil {
        for _, child := range orderElement.Children {
            index, err := strconv.Atoi(child.Name)
            if err != nil {
                log.Printf("Error parsing panel order index '%v': %v", child.Name, err)
            } else {
                order = append(order, index)
            }
        }
    }

    return &Level{
        ZMinimum: zMinimum,
        ZMaximum: zMaximum,
        BackgroundParallax: backgroundParallax,
        ForegroundParallax: foregroundParallax,
        BackgroundImage: backgroundImage,
        FrontPanels: frontPanels,
        Panels: panels,
        PanelOrder: order,
    }, nil
}

type PlayerStatus int
const (
    PlayerIdle PlayerStatus = iota
    PlayerMove
    PlayerJump
)

type Facing int
const (
    FacingRight Facing = iota
    FacingLeft
)

type InputState struct {
    Right bool
    Left bool
    Up bool
    Down bool
    Jump bool
}

type PlayerState struct {
    X float64
    Y float64
    Z float64
    Dy float64
    Animations map[string]*Animation
    Status PlayerStatus
    Facing Facing
}

func (playerState *PlayerState) CurrentAnimation() *Animation {
    switch playerState.Status {
        case PlayerIdle:
            idle, ok := playerState.Animations["idle"]
            if ok {
                return idle
            }
        case PlayerMove:
            move, ok := playerState.Animations["walk"]
            if ok {
                return move
            }
        case PlayerJump:
            jump, ok := playerState.Animations["jump"]
            if ok {
                return jump
            }
    }

    return nil
}

func (playerState *PlayerState) Update(input InputState, level *Level) {
    doJump := false
    move := false

    if input.Right {
        if playerState.Status != PlayerJump {
            playerState.X += 1
            playerState.Facing = FacingRight
            move = true
        }
    }

    if input.Left {
        if playerState.Status != PlayerJump {
            playerState.X -= 1
            playerState.Facing = FacingLeft
            move = true
        }
    }

    if input.Down {
        if playerState.Status != PlayerJump {
            playerState.Z = min(float64(level.ZMaximum), playerState.Z + 1)
            move = true
        }
    }

    if input.Up {
        if playerState.Status != PlayerJump {
            playerState.Z = max(float64(level.ZMinimum), playerState.Z - 1)
            move = true
        }
    }

    if input.Jump {
        if playerState.Status != PlayerJump {
            doJump = true
        }
    }

    if doJump {
        playerState.Status = PlayerJump
        playerState.Dy = 6
        playerState.Y = 0.001

        jumpAnimation, ok := playerState.Animations["jump"]
        if ok {
            jumpAnimation.Reset()
        }

    } else if playerState.Status != PlayerJump {
        if move {
            playerState.Status = PlayerMove
        } else {
            playerState.Status = PlayerIdle
        }
    }

    if playerState.Status == PlayerJump {
        playerState.Dy -= 0.2
        playerState.Y += playerState.Dy
        if playerState.Y <= 0 {
            playerState.Y = 0
            playerState.Dy = 0
            playerState.Status = PlayerIdle
        }
    }

    animation := playerState.CurrentAnimation()

    if animation != nil {
        animation.Update()
    }
}

func runGame(player *PaintownCharacter, yield coroutine.YieldFunc, setDraw func(drawer DrawFunc) DrawFunc) error {
    levelPath := "paintown/levels/1.txt"

    level, err := LoadLevel(levelPath)
    if err != nil {
        return err
    }

    cameraX := 0

    animations, err := player.LoadAnimations()
    if err != nil {
        return err
    }

    playerState := PlayerState{
        X: 40,
        Y: 0,
        Z: float64(level.ZMinimum + level.ZMaximum) / 2,
        Status: PlayerIdle,
        Animations: animations,
    }

    drawBackground := func(screen *ebiten.Image) {
        var options ebiten.DrawImageOptions
        options.GeoM.Translate(float64(-cameraX) * 1/float64(level.BackgroundParallax), 0)
        if level.BackgroundImage != nil {
            for {
                x, _ := options.GeoM.Apply(0, 0)
                if x > ScreenWidth {
                    break
                }
                screen.DrawImage(level.BackgroundImage, &options)
                options.GeoM.Translate(float64(level.BackgroundImage.Bounds().Dx()), 0)
            }
        }
    }

    drawBackPanels := func(screen *ebiten.Image) {
        var orderOptions ebiten.DrawImageOptions
        orderOptions.GeoM.Translate(float64(-cameraX), 0)
        for _, index := range level.PanelOrder {
            x, _ := orderOptions.GeoM.Apply(0, 0)
            if x > ScreenWidth {
                break
            }

            panel, ok := level.Panels[index]
            if ok {
                screen.DrawImage(panel, &orderOptions)
                orderOptions.GeoM.Translate(float64(panel.Bounds().Dx()), 0)
            }
        }
    }

    drawFrontPanels := func(screen *ebiten.Image) {
        if len(level.FrontPanels) > 0 {
            var panelOptions ebiten.DrawImageOptions
            panelOptions.GeoM.Translate(float64(-cameraX) * float64(level.ForegroundParallax), ScreenHeight)
            panelI := 0
            for {
                x, _ := panelOptions.GeoM.Apply(0, 0)
                if x > ScreenWidth {
                    break
                }

                panel := level.FrontPanels[panelI]
                panelOptions.GeoM.Translate(0, float64(-panel.Bounds().Dy()))
                screen.DrawImage(panel, &panelOptions)
                panelOptions.GeoM.Translate(float64(panel.Bounds().Dx()), float64(panel.Bounds().Dy()))
                panelI = (panelI + 1) % len(level.FrontPanels)
            }
        }
    }

    drawPlayer := func(screen *ebiten.Image) {
        var options ebiten.DrawImageOptions

        animation := playerState.CurrentAnimation()

        if animation != nil && animation.CurrentFrame() != nil {
            if playerState.Facing == FacingLeft {
                options.GeoM.Scale(-1, 1)
            }

            options.GeoM.Translate(playerState.X - float64(cameraX), playerState.Z - playerState.Y)
            bounds := animation.CurrentFrame().Bounds()
            if playerState.Facing == FacingLeft {
                options.GeoM.Translate(+float64(bounds.Dx()) / 2, float64(-bounds.Dy()))
            } else {
                options.GeoM.Translate(-float64(bounds.Dx()) / 2, float64(-bounds.Dy()))
            }
            screen.DrawImage(animation.CurrentFrame(), &options)
        }
    }

    buffer := ebiten.NewImage(ScreenWidth / 2, ScreenHeight / 2)
    drawer := func(screen *ebiten.Image) {
        drawBackground(buffer)
        drawBackPanels(buffer)

        drawPlayer(buffer)

        drawFrontPanels(buffer)
        var options ebiten.DrawImageOptions
        options.GeoM.Scale(2, 2)
        screen.DrawImage(buffer, &options)
    }

    oldDrawer := setDraw(drawer)
    defer setDraw(oldDrawer)

    counter := uint64(0)
    var keys []ebiten.Key
    for {
        counter += 1

        keys = inpututil.AppendPressedKeys(keys[:0])
        var inputState InputState
        for _, key := range keys {
            switch key {
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
            }
        }

        playerState.Update(inputState, level)

        if int(playerState.X) - cameraX < (ScreenWidth/2) / 4 {
            cameraX = max(0, cameraX - 1)
        }

        if int(playerState.X) - cameraX > (ScreenWidth/2) * 3 / 4 {
            // FIXME: add limit based on level width
            cameraX = int(playerState.X) - (ScreenWidth/2) * 3 / 4
        }

        err := yield()
        if err != nil {
            return err
        }
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
                    yield()

                    choosePlayer, err := chooseCharacter(yield, background, setDraw)
                    if err != nil {
                        return err
                    }

                    err = runGame(choosePlayer, yield, setDraw)
                    if err != nil {
                        return err
                    }
                }
            }

            err := yield()
            if err != nil {
                return err
            }
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
