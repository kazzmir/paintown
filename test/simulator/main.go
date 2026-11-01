package main

import (
    "log"
    "sync"
    "fmt"
    "image"
    "image/color"
    "flag"

    "github.com/kazzmir/paintown/game/data"
    "github.com/kazzmir/paintown/game/paintown"

    "github.com/hajimehoshi/ebiten/v2"
    "github.com/hajimehoshi/ebiten/v2/inpututil"
    "github.com/hajimehoshi/ebiten/v2/vector"
    "github.com/hajimehoshi/ebiten/v2/ebitenutil"
)

const ScreenWidth = 320
const ScreenHeight = 240

type Engine struct {
    Player *paintown.PlayerState
    Enemies []*paintown.Enemy
    EnemiesEnabled []bool
    Level paintown.Level
    FlashFactory *paintown.FlashFactory
    // Counter uint64

    Model paintown.GameModel

    Load func()
    Init sync.Once

    makePlayer func() (*paintown.PlayerState, error)
    makeEnemy func(int) (*paintown.Enemy, error)
    playerRespawnAt uint64
    enemyRespawnAt []uint64
}

// always stands at X
type WaitAtBehavior struct {
    X int
}

func (wait *WaitAtBehavior) Update(enemy *paintown.Enemy, level *paintown.Level, playerInfo paintown.PlayerInfo) {
    enemy.DestX = float64(wait.X)

    moved := false
    if enemy.X < enemy.DestX {
        enemy.X += min(1, enemy.DestX - enemy.X)
        moved = true
    } else if enemy.X > enemy.DestX {
        enemy.X -= min(1, enemy.X - enemy.DestX)
        moved = true
    }

    if moved {
        enemy.State = paintown.EnemyStateWalking
    } else {
        enemy.State = paintown.EnemyStateIdle
        enemy.CurrentAnimationValue = enemy.Animations["idle"]
    }
}

func MakeEngine(playerDefinition paintown.CharacterDefinition, enemyDefinitions []paintown.CharacterDefinition) *Engine {
    engine := &Engine{
        Level: paintown.Level{ZMinimum: 200, ZMaximum: 201},
        Model: paintown.GameModel{
            Counter: 1000,
            LevelLimit: 1000,
        },
    }

    // Reuse immutable assets, but create fresh character/playback state on
    // respawn rather than trying to repair a dead or grabbed instance.
    player := paintown.PaintownCharacter{Definition: playerDefinition}
    factory := paintown.MakeObjectFactory()
    engine.makePlayer = func() (*paintown.PlayerState, error) {
        state, err := paintown.MakePlayerState(&player, &engine.Level)
        if err != nil { return nil, err }
        state.X, state.Y, state.Z = 100, 0, float64(engine.Level.ZMinimum)
        return state, nil
    }
    engine.makeEnemy = func(index int) (*paintown.Enemy, error) {
        x := 230 + index*40
        enemy, err := paintown.MakeEnemyFromDefinition(paintown.BlockObject{
            Coords: image.Pt(x, engine.Level.ZMinimum),
        }, enemyDefinitions[index], factory, &WaitAtBehavior{X:x})
        if err != nil { return nil, err }
        enemy.Facing = paintown.FacingLeft
        enemy.Health, enemy.MaxHealth = 10000000, 10000000
        return enemy, nil
    }

    engine.Load = func() {
        flashFactory, err := paintown.MakeFlashFactory()
        if err != nil {
            log.Fatal(err)
        }
        engine.FlashFactory = flashFactory
        playerState, err := engine.makePlayer()
        if err != nil { log.Fatal(err) }

        var enemies []*paintown.Enemy
        for index := range enemyDefinitions {
            enemy, err := engine.makeEnemy(index)
            if err != nil { log.Fatal(err) }
            enemies = append(enemies, enemy)
            engine.EnemiesEnabled = append(engine.EnemiesEnabled, true)
            engine.enemyRespawnAt = append(engine.enemyRespawnAt, 0)
        }

        engine.Player = playerState
        engine.Enemies = enemies
        engine.Model.Enemies = enemies
    }

    return engine
}

func (engine *Engine) explodeDebugCharacter(player bool) {
    if engine.Player == nil { return }
    if player {
        if engine.Player.Status == paintown.PlayerStateDead { return }
        damage := max(1, engine.Player.Health) + max(1, engine.Player.PainThreshold) + 1
        engine.Player.Hurt(&paintown.Enemy{AttackId:engine.Model.Counter}, damage, 2)
        return
    }
    nearest := -1
    distance := 0.0
    for index, enemy := range engine.Enemies {
        if index >= len(engine.EnemiesEnabled) || !engine.EnemiesEnabled[index] || enemy.IsDead() { continue }
        dx,dz := enemy.X - engine.Player.X,enemy.Z - engine.Player.Z
        squared := dx*dx + dz*dz
        if nearest < 0 || squared < distance { nearest,distance = index,squared }
    }
    if nearest >= 0 {
        enemy := engine.Enemies[nearest]
        damage := max(1, enemy.Health) + max(1, enemy.PainThreshold) + 1
        enemy.Hurt(enemy.LastAttacked+1,damage,2)
    }
}

func respawnDelayTicks() uint64 {
    tps := ebiten.TPS()
    if tps <= 0 { tps = 60 }
    return uint64(tps)*2
}

func (engine *Engine) scheduleRespawns() {
    if engine.Player != nil && engine.Player.Status == paintown.PlayerStateDead && engine.playerRespawnAt == 0 {
        engine.playerRespawnAt = engine.Model.Counter + respawnDelayTicks()
    }
    for len(engine.enemyRespawnAt) < len(engine.Enemies) { engine.enemyRespawnAt = append(engine.enemyRespawnAt,0) }
    for index, enemy := range engine.Enemies {
        if enemy.IsDead() && engine.enemyRespawnAt[index] == 0 {
            engine.enemyRespawnAt[index] = engine.Model.Counter + respawnDelayTicks()
        }
    }
}

func (engine *Engine) respawnCharacters() error {
    if engine.playerRespawnAt != 0 && engine.Model.Counter >= engine.playerRespawnAt {
        if engine.makePlayer == nil { return fmt.Errorf("missing player respawn factory") }
        player,err := engine.makePlayer()
        if err != nil { return err }
        // Surviving enemies must still accept this player's next melee ticket.
        player.AttackId = engine.Player.AttackId + 1
        engine.Player = player
        engine.playerRespawnAt = 0
    }
    for index, deadline := range engine.enemyRespawnAt {
        if deadline == 0 || engine.Model.Counter < deadline { continue }
        if engine.makeEnemy == nil { return fmt.Errorf("missing enemy respawn factory") }
        enemy,err := engine.makeEnemy(index)
        if err != nil { return err }
        previous := engine.Enemies[index]
        delete(engine.Model.ShowHealthMap, previous)
        if engine.Player != nil { delete(engine.Player.Attackers, previous) }
        engine.Enemies[index] = enemy
        engine.enemyRespawnAt[index] = 0
    }
    return nil
}

func (engine *Engine) Update() error {
    engine.Init.Do(engine.Load)
    engine.Model.Counter += 1
    if err := engine.respawnCharacters(); err != nil { return err }

    paintown.UpdateInput()
    inputState := paintown.ReadGameInput()
    keys := inpututil.AppendJustPressedKeys(nil)
    for _, key := range keys {
        switch key {
            case ebiten.KeyEscape, ebiten.KeyCapsLock:
                return ebiten.Termination
            case ebiten.KeyG:
                engine.explodeDebugCharacter(ebiten.IsKeyPressed(ebiten.KeyShiftLeft) || ebiten.IsKeyPressed(ebiten.KeyShiftRight))
            case ebiten.Key1:
                if len(engine.EnemiesEnabled) > 0 {
                    engine.EnemiesEnabled[0] = !engine.EnemiesEnabled[0]
                }
            case ebiten.Key2:
                if len(engine.EnemiesEnabled) > 1 {
                    engine.EnemiesEnabled[1] = !engine.EnemiesEnabled[1]
                }
            case ebiten.Key3:
                if len(engine.EnemiesEnabled) > 2 {
                    engine.EnemiesEnabled[2] = !engine.EnemiesEnabled[2]
                }
        }
    }

    var enemies []*paintown.Enemy
    for i, enemy := range engine.Enemies {
        if engine.EnemiesEnabled[i] || enemy.Exploded {
            // Drain an explosion even if its enemy was disabled this frame.
            enemies = append(enemies, enemy)
        }
    }
    engine.Model.Enemies = enemies

    engine.Model.UpdatePlayer(engine.Player, inputState, &engine.Level, &paintown.DummySystem{}, engine.FlashFactory)
    engine.Model.UpdateEnemies(&engine.Level, engine.Player, &paintown.DummySystem{}, engine.FlashFactory)
    engine.scheduleRespawns()

    return nil
}

func (engine *Engine) Draw(screen *ebiten.Image) {
    screen.Fill(color.RGBA{R: 64, G: 64, B: 64, A: 255})

    vector.StrokeLine(screen, 0, float32(engine.Level.ZMinimum), ScreenWidth, float32(engine.Level.ZMinimum), 2, color.NRGBA{R: 255, G: 255, B: 255, A: 100}, false)

    if engine.Player != nil {
        paintown.DrawPlayer(engine.Player, 0, ebiten.GeoM{}, screen)
    }

    for i, enemy := range engine.Enemies {
        if engine.EnemiesEnabled[i] {
             paintown.DrawEnemy(enemy, 0, ebiten.GeoM{}, screen)
        }
    }

    for _, projectile := range engine.Model.Projectiles {
        projectile.Draw(screen, 0, ebiten.GeoM{})
    }
    for _, particle := range engine.Model.Gore {
        particle.Draw(screen, 0, ebiten.GeoM{})
    }

    if engine.Player != nil {
        ebitenutil.DebugPrintAt(screen, fmt.Sprintf("Player state: %v", engine.Player.Status), 0, 0)
    }
    if len(engine.Enemies) > 0 {
        ebitenutil.DebugPrintAt(screen, fmt.Sprintf("Enemy state: %v", engine.Enemies[0].State), 0, 15)
    }
    ebitenutil.DebugPrintAt(screen, "G: gib nearest enemy | Shift+G: gib player\nExploded characters respawn after 2 seconds", 0, 30)
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

    enemyDefinitions := []paintown.CharacterDefinition{enemyDefinition}
    for _, path := range []string{"chars/angel/angel.txt", "chars/billy/billy.txt"} {
        log.Printf("Loading enemy definition from %s", path)
        definition, err := paintown.LoadDefinition(data.DataPath(path))
        if err != nil {
            log.Fatal(err)
        }
        enemyDefinitions = append(enemyDefinitions, definition)
    }

    err = ebiten.RunGame(MakeEngine(playerDefinition, enemyDefinitions))
    if err != nil {
        log.Fatal(err)
    }
}
