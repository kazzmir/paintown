package paintown

import (
    "image"
    "fmt"
    "log"
    "os"
    "math"
    "math/rand/v2"
    "image/color"
    "slices"
    "cmp"

    "github.com/kazzmir/paintown/game/lib/coroutine"
    "github.com/kazzmir/paintown/game/graphics"
    "github.com/kazzmir/paintown/game/data"

    "github.com/hajimehoshi/ebiten/v2"
    audiolib "github.com/hajimehoshi/ebiten/v2/audio"
    "github.com/hajimehoshi/ebiten/v2/colorm"
	"github.com/hajimehoshi/ebiten/v2/text/v2"
    "github.com/hajimehoshi/ebiten/v2/vector"
    "github.com/hajimehoshi/ebiten/v2/inpututil"
)

const Z_DISTANCE = 6

func loadArrowImage() (*ebiten.Image, error) {
    path := "sprites/arrow.png"
    img, err := data.LoadPng(path)
    if err != nil {
        return nil, err
    }

    return ebiten.NewImageFromImage(graphics.ConvertTransparency(img)), nil
}

func loadFont() (*text.GoTextFace, error) {
    file, err := data.LoadFile("fonts/arial.ttf")
    if err != nil {
        return nil, err
    }

    source, err := text.NewGoTextFaceSource(file)
    if err != nil {
        return nil, err
    }

    return &text.GoTextFace{
        Source: source,
        Size:   15,
    }, nil
}

func DrawPlayer(playerState *PlayerState, cameraX float64, screenShake ebiten.GeoM, screen *ebiten.Image) {
    var options ebiten.DrawImageOptions

    for _, trail := range playerState.Trails {
        bounds := trail.Image.Bounds()

        // FIXME: take offset x/y into account
        options.GeoM.Translate(-float64(bounds.Dx()) / 2, float64(-bounds.Dy()))

        if trail.Facing == FacingLeft {
            options.GeoM.Scale(-1, 1)
        }

        if playerState.TrailLength > 0 {
            options.ColorScale.ScaleAlpha(0.7 * float32(trail.Time) / float32(playerState.TrailLength))
        }

        options.GeoM.Translate(trail.X - cameraX, trail.Z - playerState.Y)
        options.GeoM.Concat(screenShake)
        screen.DrawImage(trail.Image, &options)

        options.ColorScale.Reset()
        options.GeoM.Reset()
    }

    animation := playerState.CurrentAnimation()

    if animation != nil && animation.CurrentFrame() != nil {

        bounds := animation.CurrentFrame().Bounds()
        options.GeoM.Translate(-float64(bounds.Dx()) / 2 + float64(animation.GetOffsetX()), float64(-bounds.Dy()) + float64(animation.GetOffsetY()))

        if playerState.Facing == FacingLeft {
            options.GeoM.Scale(-1, 1)
        }

        options.GeoM.Translate(playerState.X - cameraX, playerState.Z - playerState.Y)
        options.GeoM.Concat(screenShake)
        screen.DrawImage(animation.CurrentFrame(), &options)
    }

    attack := playerState.CurrentAnimation().Attack
    if !attack.IsEmpty() {
        x1, y1 := options.GeoM.Apply(float64(attack.X1), float64(attack.Y1))
        x2, y2 := options.GeoM.Apply(float64(attack.X2), float64(attack.Y2))

        x1, x2 = min(x1, x2), max(x1, x2)
        y1, y2 = min(y1, y2), max(y1, y2)

        // log.Printf("Draw attack box from (%v, %v) to (%v, %v)", x1, y1, x2, y2)

        vector.StrokeRect(screen, float32(x1), float32(y1), float32(x2 - x1), float32(y2 - y1), 3, color.RGBA{R:255, A:255}, false)
    }
}

func DrawEnemy(enemy *Enemy, cameraX float64, screenShake ebiten.GeoM, screen *ebiten.Image) {
    var options ebiten.DrawImageOptions

    // for _, enemy := range enemies {
    animation := enemy.CurrentAnimation()

    if animation != nil && animation.CurrentFrame() != nil {
        if enemy.Facing == FacingLeft {
            options.GeoM.Scale(-1, 1)
        }

        options.GeoM.Translate(enemy.X - cameraX, enemy.Z - enemy.Y)
        options.GeoM.Concat(screenShake)
        bounds := animation.CurrentFrame().Bounds()
        if enemy.Facing == FacingLeft {
            options.GeoM.Translate(+float64(bounds.Dx()) / 2 - float64(animation.GetOffsetX()), float64(-bounds.Dy()) + float64(animation.GetOffsetY()))
        } else {
            options.GeoM.Translate(-float64(bounds.Dx()) / 2 + float64(animation.GetOffsetX()), float64(-bounds.Dy()) + float64(animation.GetOffsetY()))
        }
        screen.DrawImage(animation.CurrentFrame(), &options)

        // FIXME: ugly to pull this attack out
        attack := enemy.CurrentAnimationValue.Attack
        if !attack.IsEmpty() {
            x1, y1 := options.GeoM.Apply(float64(attack.X1), float64(attack.Y1))
            x2, y2 := options.GeoM.Apply(float64(attack.X2), float64(attack.Y2))

            x1, x2 = min(x1, x2), max(x1, x2)
            y1, y2 = min(y1, y2), max(y1, y2)

            // log.Printf("Draw attack box from (%v, %v) to (%v, %v)", x1, y1, x2, y2)

            vector.StrokeRect(screen, float32(x1), float32(y1), float32(x2 - x1), float32(y2 - y1), 3, color.RGBA{B:255, A:255}, false)
        }

    }
    // }
}

func RunLevel(player *PaintownCharacter, yield coroutine.YieldFunc, setDraw func(drawer data.DrawFunc) data.DrawFunc, levelPath string, audioContext *audiolib.Context) error {
    level, err := LoadLevel(levelPath)
    if err != nil {
        return err
    }

    font, err := loadFont()
	if err != nil {
		return err
	}

    fontWidth, fontHeight := text.Measure("A", font, 1)
    _ = fontWidth

    cameraX := float64(0)

    playerState, err := MakePlayerState(player, level)
    if err != nil {
        return err
    }

    var enemies []*Enemy

    var screenShake ebiten.GeoM

    drawBackground := func(screen *ebiten.Image) {
        var options ebiten.DrawImageOptions
        options.GeoM.Translate(-cameraX * 1/float64(level.BackgroundParallax), 0)
        options.GeoM.Concat(screenShake)
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
        orderOptions.GeoM.Concat(screenShake)
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
            panelOptions.GeoM.Concat(screenShake)
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

    type ShadowObject interface {
        CurrentAnimation() *Animation
        GetX() float64
        GetY() float64
        GetZ() float64
        GetFacing() Facing
    }

    drawShadow := func(screen *ebiten.Image, object ShadowObject) {
        var options colorm.DrawImageOptions

        animation := object.CurrentAnimation()

        if animation != nil && animation.CurrentFrame() != nil {
            // options.GeoM.Skew(0.5, 0)

            bounds := animation.CurrentFrame().Bounds()
            options.GeoM.Translate(-float64(bounds.Dx()) / 2 + float64(animation.GetOffsetX()), float64(-bounds.Dy()) + float64(animation.GetOffsetY()))

            if object.GetFacing() == FacingLeft {
                options.GeoM.Scale(-1, 1)
            }

            options.GeoM.Scale(0.9, 0.7)
            options.GeoM.Skew(0.6, 0)

            options.GeoM.Translate(object.GetX() - cameraX - object.GetY() / 4, object.GetZ())
            options.GeoM.Concat(screenShake)

            options.GeoM.Translate(0, 0)
            var m colorm.ColorM

            m.ChangeHSV(0, 1, 0)

            alpha := min(0.4, object.GetY() / 300)

            m.Scale(1, 1, 1, 0.4 - alpha)
            colorm.DrawImage(screen, animation.CurrentFrame(), m, &options)
        }
    }

    type HealthObject interface {
        GetIcon() *ebiten.Image
        GetHealthPercent() float64
        GetName() string
    }

    drawHealthBar := func(screen *ebiten.Image, health HealthObject, slot int) {
        icon := health.GetIcon()

        maxHealthBars := 5

        totalWidth := 100
        gap := 5

        x := 5.0 + (slot % maxHealthBars) * (totalWidth + gap)
        y := float64(2.0 + (slot / maxHealthBars) * 30)
        var options ebiten.DrawImageOptions
        options.GeoM.Translate(float64(x), y)
        screen.DrawImage(icon, &options)

        var textOptions text.DrawOptions
        textOptions.GeoM.Translate(float64(x+icon.Bounds().Dx()+2), float64(y))
        text.Draw(screen, health.GetName(), font, &textOptions)

        y += fontHeight + 1

        x1 := x + icon.Bounds().Dx() + 2
        length := totalWidth - icon.Bounds().Dx() - 2
        vector.FillRect(screen, float32(x1), float32(y), float32(length), float32(10), color.NRGBA{R: 32, G: 32, B: 32, A: 200}, false)
        healthLength := float64(length) * health.GetHealthPercent()

		sub := screen.SubImage(image.Rect(x1, int(y), x1 + int(healthLength), int(y) + 10)).(*ebiten.Image)
		graphics.DrawHorizontalGradient(sub, float32(x1), float32(y), float32(length), float32(10), color.NRGBA{R: 200, G: 0, B: 0, A: 255}, color.NRGBA{R: 0, G: 200, B: 0, A: 255})
        // vector.FillRect(screen, float32(x1), float32(y), float32(healthLength), float32(10), color.NRGBA{R: 200, G: 0, B: 0, A: 255}, false)
    }

    type Drawable struct {
        Z float64
        Draw func()
        DrawFirst func()
    }

    showForwardArrow := false
    var flashes []*Flash

    arrowImage, err := loadArrowImage()
    if err != nil {
        arrowImage = ebiten.NewImage(1, 1)
    }

    // avoid triggering moves immediately
    counter := uint64(1000)

    // tracks whether to show the health for an enemy. the value is the last logical time
    // that the enemy did something worth displaying the health bar for.
    // if the value is less than X, then display the health bar
    showHealthMap := make(map[*Enemy]uint64)

    var objects []Drawable
    buffer := ebiten.NewImage(data.ScreenWidth / 2, data.ScreenHeight / 2)
    drawer := func(screen *ebiten.Image) {
        drawBackground(buffer)
        drawBackPanels(buffer)

        healthObjects := []HealthObject{
            playerState,
        }

        objects = objects[:0]
        for _, enemy := range enemies {
            if !enemy.Blinking() {
                objects = append(objects, Drawable{
                    DrawFirst: func() {
                        drawShadow(buffer, enemy)
                    },
                    Draw: func() {
                        DrawEnemy(enemy, cameraX, screenShake, buffer)
                    },
                    Z: enemy.Z,
                })
            }

            when, ok := showHealthMap[enemy]
            if enemy.Health > 0 && ok && counter - when < 600 {
                healthObjects = append(healthObjects, enemy)
            }
        }

        for _, flash := range flashes {
            objects = append(objects, Drawable{
                Draw: func() {
                    frame := flash.Animation.CurrentFrame()
                    if frame != nil {
                        var options ebiten.DrawImageOptions
                        options.GeoM.Translate(float64(flash.X) - cameraX, float64(flash.Z) - float64(flash.Y))
                        bounds := frame.Bounds()
                        options.GeoM.Translate(-float64(bounds.Dx()) / 2, float64(-bounds.Dy()))
                        buffer.DrawImage(frame, &options)
                    }
                },
                Z: float64(flash.Z),
            })
        }

        objects = append(objects, Drawable{
            DrawFirst: func() {
                drawShadow(buffer, playerState)
            },
            Draw: func(){
                DrawPlayer(playerState, cameraX, screenShake, buffer)
            },
            Z: playerState.Z,
        })

        slices.SortStableFunc(objects, func (a, b Drawable) int {
            return cmp.Compare(a.Z, b.Z)
        })

        for _, object := range objects {
            if object.DrawFirst != nil {
                object.DrawFirst()
            }
        }

        for _, object := range objects {
            object.Draw()
        }

        drawFrontPanels(buffer)

        if showForwardArrow && (counter / 20) % 2 == 0 {
            var arrowOptions ebiten.DrawImageOptions
            arrowOptions.GeoM.Translate(float64(buffer.Bounds().Dx() - arrowImage.Bounds().Dx() - 10), float64(buffer.Bounds().Dy() / 2 - arrowImage.Bounds().Dy() / 2))
            buffer.DrawImage(arrowImage, &arrowOptions)
        }

        var options ebiten.DrawImageOptions
        options.GeoM.Scale(2, 2)
        screen.DrawImage(buffer, &options)

        for i, health := range healthObjects {
            drawHealthBar(screen, health, i)
        }
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

    factory := MakeObjectFactory()

    createEnemies := func(objects []BlockObject) []*Enemy {
        var out []*Enemy
        for _, object := range objects {
            if object.Type == "enemy" {
                enemy, err := MakeEnemy(object, factory, &BehaviorNormal{})
                if err != nil {
                    log.Printf("Error creating enemy from object '%v': %v", object.Name, err)
                } else {
                    out = append(out, enemy)
                }
            }
        }

        return out
    }

    draw1 := setDraw(func(screen *ebiten.Image) {
        drawBackground(buffer)
        drawBackPanels(buffer)

        var options ebiten.DrawImageOptions
        options.GeoM.Scale(2, 2)
        screen.DrawImage(buffer, &options)
    })

    preload := make(chan struct{}, 1)
    go func(){
        // preload all enemies
        log.Printf("Preloading enemies...")
        for _, block := range level.Blocks {
            // create enemies, and throw them away
            createEnemies(block.Objects)
        }
        log.Printf("Done preloading enemies")

        preload <- struct{}{}
    }()

    preloaded := false
    for !preloaded {
        select {
            case <-preload:
                // preload is done, we can start the game
                preloaded = true
            default:
        }

        err := yield()
        if err != nil {
            return err
        }
    }

    setDraw(draw1)

    audio := MakeAudioManager(audioContext)

    oldDrawer := setDraw(drawer)
    defer setDraw(oldDrawer)

    blocks := level.Blocks
    if len(blocks) == 0 {
        return fmt.Errorf("No blocks in level")
    }

    currentBlock := -1
    levelLimit := float64(0)

    cameraSpeed := float64(1)

    flashFactory, err := MakeFlashFactory()
    if err != nil {
        return fmt.Errorf("Error creating flash factory: %v", err)
    }

    // enemies = append(enemies, &Enemy{})

    shake := 0
    for currentBlock < len(blocks) {
        counter += 1

        if shake > 0 {
            dx := (rand.Float64() - 0.5) * float64(shake) / 3
            dy := (rand.Float64() - 0.5) * float64(shake) / 3

            screenShake.Reset()
            screenShake.Translate(dx, dy)

            shake -= 1
        } else {
            screenShake.Reset()
        }

        if playerState.X > float64(levelLimit) - 50 && len(enemies) == 0 {
            currentBlock += 1
            if currentBlock < len(blocks) {
                levelLimit += float64(blocks[currentBlock].Length)
                log.Printf("Entering block %v, limit %v", currentBlock, levelLimit)

                showHealthMap = make(map[*Enemy]uint64)

                playerState.ResetAttackers()

                enemies = createEnemies(blocks[currentBlock].Objects)
                for _, enemy := range enemies {
                    enemy.X += levelLimit - float64(blocks[currentBlock].Length)
                    enemy.Z += float64(level.ZMinimum)
                }
            }
        } else if len(enemies) == 0 {
            // no enemies left in the current block, so show the arrow indicating the way forward
            showForwardArrow = true
        }

        if len(enemies) > 0 {
            showForwardArrow = false
        }

        inputState := readInputState()

        playerState.Update(inputState, level, audio, counter)

        if playerState.X > levelLimit {
            playerState.X = levelLimit
        }

        if !playerState.CurrentAnimation().Attack.IsEmpty() {
            attack := playerState.CurrentAnimation().Attack
            attackBox := playerState.GetAttackBox()
            for _, enemy := range enemies {
                if abs(enemy.Z - playerState.Z) < Z_DISTANCE && enemy.CanBeHit(playerState.AttackId) {
                    if enemy.HitBy(attackBox) {
                        force := float64(attack.Force)
                        if playerState.Facing == FacingLeft {
                            force = -force
                        }
                        enemy.Hurt(playerState.AttackId, attack.Damage, force)
                        playerState.IgnoreHit(enemy)

                        if playerState.Grabbed == enemy {
                            if enemy.State == EnemyStateFalling || enemy.State == EnemyStateDead {
                                playerState.Grabbed = nil
                                playerState.Status = PlayerIdle
                            } else {
                                enemy.State = EnemyStateGrabbed
                            }
                        }

                        // log.Printf("Enemy hit! Enemy at (%v, %v), attack from (%v, %v) to (%v, %v)", enemy.X, enemy.Z, playerState.Attack.X1, playerState.Attack.Y1, playerState.Attack.X2, playerState.Attack.Y2)
                        // create hit projectile, flash

                        flashes = append(flashes, flashFactory.MakeFlash(enemy.X, enemy.Y + 50, enemy.Z + 0.1))

                        showHealthMap[enemy] = counter

                        err := audio.PlaySound(playerState.HitSound)
                        if err != nil {
                            log.Printf("Error playing hit sound: %v", err)
                        }

                        if enemy.Health <= 0 {
                            err := audio.PlaySound(enemy.DieSound)
                            if err != nil {
                                log.Printf("Error playing enemy die sound: %v", err)
                            }
                        }
                    }
                }
            }
        } else if playerState.Status == PlayerMove {
            for _, enemy := range enemies {
                if abs(enemy.Z - playerState.Z) < Z_DISTANCE {
                    grabbed := false

                    if playerState.Facing == FacingRight && enemy.X > playerState.X && enemy.X - playerState.X < 50 {
                        grabbed = true
                    }

                    if playerState.Facing == FacingLeft && enemy.X < playerState.X && playerState.X - enemy.X < 50 {
                        grabbed = true
                    }

                    if grabbed {
                        showHealthMap[enemy] = counter
                        playerState.DoGrab(enemy)
                        enemy.WasGrabbed()
                        enemy.Z = playerState.Z + 0.1
                        break
                    }
                }
            }
        }

        if len(flashes) > 0 {
            flashesOut := make([]*Flash, 0, len(flashes))
            for _, flash := range flashes {
                if !flash.Animation.Update(false, audio) {
                    flashesOut = append(flashesOut, flash)
                }
            }
            flashes = flashesOut
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

        outEnemies := make([]*Enemy, 0, len(enemies))
        for _, enemy := range enemies {
            enemy.Update(level, playerState, func(state EnemyState){
                switch state {
                    case EnemyStateFallen:
                        shake = 30
                        audio.PlaySound(enemy.FallSound)
                }
            }, audio)
            if !enemy.IsDead() {

                attack := enemy.CurrentAnimationValue.Attack
                if !attack.IsEmpty() && abs(enemy.Z - playerState.Z) < Z_DISTANCE && playerState.CanBeHit(enemy, enemy.AttackId) {
                    if playerState.HitBy(enemy.GetAttackBox()) {
                        force := attack.Force
                        if enemy.GetFacing() == FacingLeft {
                            force = -force
                        }

                        playerState.Hurt(enemy, attack.Damage, force)
                        showHealthMap[enemy] = counter

                        flashes = append(flashes, flashFactory.MakeFlash(playerState.X, playerState.Y + 50, playerState.Z + 0.1))

                        showHealthMap[enemy] = counter

                        err := audio.PlaySound(enemy.HitSound)
                        if err != nil {
                            log.Printf("Error playing hit sound: %v", err)
                        }
                    }
                }

                outEnemies = append(outEnemies, enemy)
            }
        }

        enemies = outEnemies

        err := yield()
        if err != nil {
            return err
        }
    }

    // goto next level
    return nil
}

type DummySystem struct {}
func (dummy *DummySystem) PlaySound(path string) error {
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
                        idle.Update(true, &DummySystem{})
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

    bigStart := float32(-500)
    bigX := bigStart

    drawer := func (screen *ebiten.Image) {
        animation := animations[allPlayers[currentChoice].Definition.Name]
        var options ebiten.DrawImageOptions
        screen.DrawImage(background, &options)

        x := 60.0
        y := 150.0

        frame := animation.FirstFrame()
        frame = frame.SubImage(image.Rect(0, 0, frame.Bounds().Dx(), frame.Bounds().Dy() / 2)).(*ebiten.Image)

        bigY := float32(0.0)
        bigWidth := float32(frame.Bounds().Dx() * 4)
        bigHeight := float32(frame.Bounds().Dy() * 4)

        vertices := []ebiten.Vertex{
            ebiten.Vertex{
                DstX: bigX,
                DstY: bigY,
                SrcX: 0,
                SrcY: 0,
                ColorR: 1,
                ColorG: 1,
                ColorB: 1,
                ColorA: 0.8,
            },
            ebiten.Vertex{
                DstX: bigX + bigWidth,
                DstY: bigY,
                SrcX: float32(frame.Bounds().Dx()),
                SrcY: 0,
                ColorR: 1,
                ColorG: 1,
                ColorB: 1,
                ColorA: 0.8,
            },
            ebiten.Vertex{
                DstX: bigX,
                DstY: bigY + bigHeight,
                SrcX: 0,
                SrcY: float32(frame.Bounds().Dy()),
                ColorR: 1,
                ColorG: 1,
                ColorB: 1,
                ColorA: 0,
            },
            ebiten.Vertex{
                DstX: bigX + bigWidth,
                DstY: bigY + bigHeight,
                SrcX: float32(frame.Bounds().Dx()),
                SrcY: float32(frame.Bounds().Dy()),
                ColorR: 1,
                ColorG: 1,
                ColorB: 1,
                ColorA: 0,
            },
        }

        screen.DrawTriangles(vertices, []uint16{0, 1, 2, 1, 2, 3}, frame, nil)

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
        change := false
        for _, key := range keys {
            switch key {
                case ebiten.KeyEnter:
                    return allPlayers[currentChoice], nil
                case ebiten.KeyArrowRight:
                    currentChoice = (currentChoice + 1) % len(allPlayers)
                    change = true
                case ebiten.KeyArrowLeft:
                    currentChoice = (currentChoice - 1 + len(allPlayers)) % len(allPlayers)
                    change = true
                case ebiten.KeyArrowDown:
                    currentChoice = (currentChoice + perRow) % len(allPlayers)
                    change = true
                case ebiten.KeyArrowUp:
                    currentChoice = (currentChoice - perRow + len(allPlayers)) % len(allPlayers)
                    change = true
            }
        }

        if change {
            bigX = bigStart
        }

        bigPos := float32(-60)
        if bigX < bigPos {
            speed := math.Sqrt(float64(bigPos - bigX))
            bigX += max(float32(speed), 1)
        }

        animation := animations[allPlayers[currentChoice].Definition.Name]

        animation.Update(true, &DummySystem{})

        err := yield()
        if err != nil {
            return nil, err
        }
    }
}
