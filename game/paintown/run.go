package paintown

import (
    "image"
    "fmt"
    "log"
    "os"
    "math"
    "image/color"

    "github.com/kazzmir/paintown/game/lib/coroutine"
    "github.com/kazzmir/paintown/game/graphics"
    "github.com/kazzmir/paintown/game/data"

    "github.com/hajimehoshi/ebiten/v2"
    "github.com/hajimehoshi/ebiten/v2/vector"
    "github.com/hajimehoshi/ebiten/v2/inpututil"
)

func RunLevel(player *PaintownCharacter, yield coroutine.YieldFunc, setDraw func(drawer data.DrawFunc) data.DrawFunc, levelPath string) error {
    level, err := LoadLevel(levelPath)
    if err != nil {
        return err
    }

    cameraX := float64(0)

    animations, err := player.LoadAnimations()
    if err != nil {
        return err
    }

    playerState := PlayerState{
        X: 60,
        Y: 0,
        Z: float64(level.ZMinimum + level.ZMaximum) / 2,
        Status: PlayerIdle,
        Animations: animations,
    }

    for _, animation := range playerState.Animations {
        animation.Owner = &playerState
    }

    drawBackground := func(screen *ebiten.Image) {
        var options ebiten.DrawImageOptions
        options.GeoM.Translate(-cameraX * 1/float64(level.BackgroundParallax), 0)
        if level.BackgroundImage != nil {
            for {
                x, _ := options.GeoM.Apply(0, 0)
                if int(x) > screen.Bounds().Dx() {
                    break
                }
                screen.DrawImage(level.BackgroundImage, &options)
                options.GeoM.Translate(float64(level.BackgroundImage.Bounds().Dx()), 0)
            }
        }
    }

    drawBackPanels := func(screen *ebiten.Image) {
        var orderOptions ebiten.DrawImageOptions
        orderOptions.GeoM.Translate(-cameraX, 0)
        for _, index := range level.PanelOrder {
            x, _ := orderOptions.GeoM.Apply(0, 0)
            if int(x) > screen.Bounds().Dx() {
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
            panelOptions.GeoM.Translate(-cameraX * float64(level.ForegroundParallax), float64(screen.Bounds().Dy()))
            panelI := 0
            for {
                x, _ := panelOptions.GeoM.Apply(0, 0)
                if int(x) > screen.Bounds().Dx() {
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

        for _, trail := range playerState.Trails {
            if trail.Facing == FacingLeft {
                options.GeoM.Scale(-1, 1)
            }

            if playerState.TrailLength > 0 {
                options.ColorScale.ScaleAlpha(0.7 * float32(trail.Time) / float32(playerState.TrailLength))
            }

            options.GeoM.Translate(trail.X - cameraX, trail.Z - playerState.Y)
            bounds := trail.Image.Bounds()
            if playerState.Facing == FacingLeft {
                options.GeoM.Translate(+float64(bounds.Dx()) / 2, float64(-bounds.Dy()))
            } else {
                options.GeoM.Translate(-float64(bounds.Dx()) / 2, float64(-bounds.Dy()))
            }
            screen.DrawImage(trail.Image, &options)

            options.ColorScale.Reset()
            options.GeoM.Reset()
        }

        animation := playerState.CurrentAnimation()

        if animation != nil && animation.CurrentFrame() != nil {
            if playerState.Facing == FacingLeft {
                options.GeoM.Scale(-1, 1)
            }

            options.GeoM.Translate(playerState.X - cameraX, playerState.Z - playerState.Y)
            bounds := animation.CurrentFrame().Bounds()
            if playerState.Facing == FacingLeft {
                options.GeoM.Translate(+float64(bounds.Dx()) / 2 - float64(animation.GetOffsetX()), float64(-bounds.Dy()) + float64(animation.GetOffsetY()))
            } else {
                options.GeoM.Translate(-float64(bounds.Dx()) / 2 + float64(animation.GetOffsetX()), float64(-bounds.Dy()) + float64(animation.GetOffsetY()))
            }
            screen.DrawImage(animation.CurrentFrame(), &options)
        }
    }

    buffer := ebiten.NewImage(data.ScreenWidth / 2, data.ScreenHeight / 2)
    drawer := func(screen *ebiten.Image) {
        drawBackground(buffer)
        drawBackPanels(buffer)

        drawPlayer(buffer)

        drawFrontPanels(buffer)
        var options ebiten.DrawImageOptions
        options.GeoM.Scale(2, 2)
        screen.DrawImage(buffer, &options)
    }

    var keys []ebiten.Key
    readInputState := func() InputState {
        keys = inpututil.AppendPressedKeys(keys[:0])
        var inputState InputState
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

        return inputState
    }

    oldDrawer := setDraw(drawer)
    defer setDraw(oldDrawer)

    blocks := level.Blocks
    if len(blocks) == 0 {
        return fmt.Errorf("No blocks in level")
    }

    currentBlock := -1
    levelLimit := float64(0)

    var enemies []*Enemy

    cameraSpeed := float64(1)

    // enemies = append(enemies, &Enemy{})

    // avoid triggering moves immediately
    counter := uint64(1000)
    for currentBlock < len(blocks) {
        counter += 1

        if playerState.X > float64(levelLimit) - 50 && len(enemies) == 0 {
            currentBlock += 1
            if currentBlock < len(blocks) {
                levelLimit += float64(blocks[currentBlock].Length)
                log.Printf("Entering block %v, limit %v", currentBlock, levelLimit)

                enemies = nil
                for _, object := range blocks[currentBlock].Objects {
                    if object.Type == "enemy" {
                        enemies = append(enemies, &Enemy{})
                    }
                }
            }
        }

        inputState := readInputState()

        playerState.Update(inputState, level, counter)

        if playerState.X > levelLimit {
            playerState.X = levelLimit
        }

        if playerState.X - cameraX < (data.ScreenWidth/2) / 4 {
            cameraX = max(0, cameraX - cameraSpeed)
        }

        if playerState.X - cameraX > data.ScreenWidth / 2 * 3 / 4 {
            cameraX = cameraX + cameraSpeed
            // int(playerState.X) - data.ScreenWidth / 2 * 3 / 4
        }

        if data.ScreenWidth / 2 + cameraX > levelLimit {
            cameraX = max(0, levelLimit - data.ScreenWidth / 2)
        }

        err := yield()
        if err != nil {
            return err
        }
    }

    // goto next level
    return nil
}

func ChooseCharacter(yield coroutine.YieldFunc, background *ebiten.Image, setDraw func(drawer data.DrawFunc) data.DrawFunc) (*PaintownCharacter, error) {

    animations := make(map[string]*Animation)
    var allPlayers []*PaintownCharacter
    choices, err := os.ReadDir(data.DataPath("players"))
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

    selectedColors := append(graphics.InterpolateColors(color.RGBA{R:255, A:255}, color.RGBA{A: 255}, 20), graphics.InterpolateColors(color.RGBA{A:255}, color.RGBA{R:255, A:255}, 20)...)

    drawer := func (screen *ebiten.Image) {
        animation := animations[allPlayers[currentChoice].Definition.Name]
        var options ebiten.DrawImageOptions
        screen.DrawImage(background, &options)

        x := 60.0
        y := 130.0

        currentBounds := animation.CurrentFrame().Bounds()

        options.GeoM.Translate(x, y)
        options.GeoM.Translate(float64(-currentBounds.Dx()) / 2 + float64(animation.GetOffsetX()), float64(currentBounds.Dy() * -1))
        options.GeoM.Scale(2, 2)
        screen.DrawImage(animation.CurrentFrame(), &options)

        options.GeoM.Reset()
        options.GeoM.Translate(x, -y)
        options.GeoM.Translate(float64(-currentBounds.Dx()) / 2 + float64(animation.GetOffsetX()), float64(currentBounds.Dy() * -1))
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
        for gridX < data.ScreenWidth && playerIndex < len(allPlayers) {
            player := allPlayers[playerIndex]
            anim, ok := animations[player.Definition.Name]
            if ok && anim.CurrentFrame() != nil {
                area := screen.SubImage(image.Rect(gridX, gridY, gridX + gridSize, gridY + gridSize)).(*ebiten.Image)

                bounds := anim.CurrentFrame().Bounds()

                options.GeoM.Reset()
                options.GeoM.Translate(float64(anim.GetOffsetX()) + -float64(bounds.Dx()) / 2, float64(bounds.Dy() * -1))
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
                if gridX + gridSize > data.ScreenWidth {
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

        err := yield()
        if err != nil {
            return nil, err
        }
    }
}
