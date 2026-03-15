package paintown

import (
    "log"
    "fmt"
    "image"
    "math/rand/v2"
    "path/filepath"
    "strings"

    "github.com/kazzmir/paintown/game/data"
    "github.com/kazzmir/paintown/game/lib/sexp"
)

type EnemyState int

const (
    EnemyStateIdle EnemyState = iota
    EnemyStateWalking
    EnemyStateAttacking
    EnemyStateFalling
    EnemyStateFallen
    EnemyStateRise
    EnemyStatePain
)

type Enemy struct {
    Character *CharacterDefinition
    X float64
    Z float64
    Y float64
    Vy float64
    Vx float64

    Health float64

    // each time the enemy is hurt, add to Pain. When Pain reaches a certain threshold, the enemy will be knocked back and fall down.
    // Pain will go down over time as well
    Pain float64
    PainThreshold float64

    State EnemyState
    FallenCount int

    // each attack that hits the enemy has an id that increments monotonically. the enemy
    // cannot be hit by the same attack twice (unless the attack explicitly enables this)
    LastAttacked uint64

    HasDestination bool
    DestX float64
    DestZ float64
    CurrentAnimation *Animation
    Animations map[string]*Animation
    Attacks []*Animation
    Facing Facing
    Attack AnimationAttack
}

func loadAnimations(definition *CharacterDefinition, factory *ObjectFactory) (map[string]*Animation, error) {
    animations := definition.FindAll("character", "anim")

    // log.Printf("Found %v animations for character %v", len(animations), character.Definition.Name)
    out := make(map[string]*Animation)

    for _, animation := range animations {
        animationName := animation.GetChild("name")
        name := animationName.GetValue(0)
        base := filepath.Join("chars", strings.ToLower(definition.Name), name)
        animation, err := factory.MakeAnimationFromDefinition(strings.ToLower(definition.Name), name, base, animation)
        if err == nil {
            out[name] = animation
        } else {
            log.Printf("Error loading animation '%v' for character '%v': %v", name, definition.Name, err)
        }
    }

    return out, nil
}

type ObjectFactory struct {
    definitions map[string]CharacterDefinition
    animations map[string]map[string]*Animation
}

func MakeObjectFactory() *ObjectFactory {
    return &ObjectFactory{
        definitions: make(map[string]CharacterDefinition),
        animations: make(map[string]map[string]*Animation),
    }
}

func (factory *ObjectFactory) MakeAnimationFromDefinition(player string, animation string, baseDir string, definition *sexp.SExpr) (*Animation, error) {
    chars, ok := factory.animations[player]
    if !ok {
        chars = make(map[string]*Animation)
        factory.animations[player] = chars
    }

    useAnimation, ok := chars[player]
    if !ok {
        dir, ok := sexp.ReadValue[string](definition, "basedir", 0)
        if ok {
            baseDir = dir
        }

        loaded, err := MakeAnimationFromDefinition(baseDir, definition)
        if err != nil {
            return nil, err
        }
        useAnimation = loaded
    }

    if useAnimation != nil {
        out := useAnimation.Clone()
        out.Reset()
        return out, nil
    }

    return nil, fmt.Errorf("Animation '%v' for character '%v' not found", animation, player)
}

func (factory *ObjectFactory) LoadDefinition(path string) (CharacterDefinition, error) {
    definition, ok := factory.definitions[path]
    if !ok {
        var err error
        definition, err = loadDefinition(path)
        if err != nil {
            return CharacterDefinition{}, err
        }

        factory.definitions[path] = definition
    }

    return definition, nil
}

func MakeEnemy(object BlockObject, factory *ObjectFactory) (*Enemy, error) {
    definitionPath := data.DataPath(object.Path)

    definition, err := factory.LoadDefinition(definitionPath)
    if err != nil {
        return nil, err
    }

    animations, err := loadAnimations(&definition, factory)

    idle, ok := animations["idle"]
    if !ok {
        idle = nil
    }

    var attacks []*Animation
    for _, animation := range animations {
        if animation.HasAttack() {
            attacks = append(attacks, animation)
        }
    }

    enemy := &Enemy{
        Attacks: attacks,
        Character: &definition,
        Animations: animations,
        CurrentAnimation: idle,
        Facing: FacingLeft,
        Health: max(1, definition.GetHealth()),
        // FIXME: make this a configuration option in the character definition
        PainThreshold: 7,
        X: float64(object.Coords.X),
        Z: float64(object.Coords.Y),
    }

    for _, animation := range animations {
        animation.Owner = enemy
    }

    return enemy, nil
}

type PlayerInfo interface {
    GetX() float64
    GetZ() float64
}

func abs(x float64) float64 {
    return max(x, -x)
}

func (enemy *Enemy) CanBeHit(attack uint64) bool {
    return attack > enemy.LastAttacked && enemy.State != EnemyStateFallen && enemy.State != EnemyStateFalling
}

func (enemy *Enemy) GetFacing() Facing {
    return enemy.Facing
}

func (enemy *Enemy) SetFacing(facing Facing) {
    enemy.Facing = facing
}

func (enemy *Enemy) GetAttacks() []*Animation {
    return enemy.Attacks
}

func (enemy *Enemy) SetAttack(attack AnimationAttack) {
    enemy.Attack = attack
}

func (enemy *Enemy) SetTrail(generate int, length int) {
    // TODO
}

func (enemy *Enemy) Move(x int, y int, z int) {
    enemy.X += float64(x)
    enemy.Y += float64(y)
    enemy.Z += float64(z)
}

func (enemy *Enemy) DoFall(force float64) {
    enemy.Pain = 0
    enemy.State = EnemyStateFalling
    enemy.Y = 10
    enemy.Vy = 2
    enemy.Vx = force
    fall, ok := enemy.Animations["fall"]
    if ok {
        enemy.CurrentAnimation = fall
        enemy.CurrentAnimation.Reset()
    }
}

func (enemy *Enemy) Hurt(attackId uint64, damage float64, force float64) {
    enemy.LastAttacked = attackId
    enemy.Pain += damage
    log.Printf("Enemy hurt for %v damage, pain is now %v", damage, enemy.Pain)

    enemy.State = EnemyStatePain
    painAnimation, ok := enemy.Animations["pain"]
    if ok {
        enemy.CurrentAnimation = painAnimation
        enemy.CurrentAnimation.Reset()
    }

    if enemy.Pain >= enemy.PainThreshold {
        enemy.DoFall(force)
    }
}

func (enemy *Enemy) HitBy(attack AnimationAttack) bool {
    frame := enemy.CurrentAnimation.CurrentFrame()

    x1 := enemy.X - float64(frame.Bounds().Dx()) / 2
    y1 := enemy.Z - float64(frame.Bounds().Dy())

    x2 := x1 + float64(frame.Bounds().Dx())
    y2 := y1 + float64(frame.Bounds().Dy())

    r1 := image.Rect(int(x1), int(y1), int(x2), int(y2))
    r2 := attack.GetHitBox()

    return r1.Overlaps(r2)
}

func (enemy *Enemy) UpdateState(level *Level, playerInfo PlayerInfo) {

    enemy.Pain = max(0, enemy.Pain - 0.1)

    if enemy.State == EnemyStateAttacking {
        return
    }

    if enemy.State == EnemyStateFallen {
        if enemy.FallenCount > 0 {
            enemy.FallenCount -= 1
            return
        } else {
            enemy.State = EnemyStateRise
            rise, ok := enemy.Animations["rise"]
            if ok {
                enemy.CurrentAnimation = rise
            } else {
                enemy.State = EnemyStateIdle
            }
        }
    }

    if enemy.State == EnemyStateRise {
        return
    }

    if enemy.State == EnemyStatePain {
        return
    }

    if enemy.State == EnemyStateFalling {
        enemy.Y += enemy.Vy
        enemy.X += enemy.Vx
        enemy.Vy -= 0.08
        if enemy.Y < 0 {
            enemy.Y = 0
            enemy.State = EnemyStateFallen
            // stay on ground for a while
            enemy.FallenCount = 90
        }
        return
    }

    // if near the player, then initiate an attack
    if rand.N(50) == 0 && (enemy.State == EnemyStateIdle || enemy.State == EnemyStateWalking) {
        var choices []*Animation
        zRange := 3.0
        for _, attack := range enemy.GetAttacks() {
            if abs(enemy.X - playerInfo.GetX()) < float64(attack.GetRange()) && abs(enemy.Z - playerInfo.GetZ()) < zRange {
                choices = append(choices, attack)
            }
        }

        if len(choices) > 0 {
            enemy.State = EnemyStateAttacking
            enemy.CurrentAnimation = choices[rand.N(len(choices))]
            enemy.CurrentAnimation.Reset()
            return
        }
    }

    if !enemy.HasDestination && rand.N(30) == 0 {

        if rand.N(5) == 0 {
            // head directly towards the player
            side := 1.0
            if rand.N(2) == 0 {
                side = -1.0
            }
            enemy.DestX = playerInfo.GetX() + 30 * side
            enemy.DestZ = playerInfo.GetZ()
        } else {
            // move to a random place
            width := 400
            enemy.DestX = playerInfo.GetX() + float64(rand.N(width)) - float64(width) / 2
            enemy.DestZ = float64(rand.N(level.ZMaximum - level.ZMinimum) + level.ZMinimum) + rand.Float64() - 0.5
        }

        enemy.HasDestination = true
    }

    if enemy.HasDestination {
        walk, ok := enemy.Animations["walk"]
        if ok && enemy.CurrentAnimation != walk {
            enemy.CurrentAnimation = walk
        }

        moved := false
        if enemy.X < enemy.DestX {
            enemy.X += min(1, enemy.DestX - enemy.X)
            moved = true
        } else if enemy.X > enemy.DestX {
            enemy.X -= min(1, enemy.X - enemy.DestX)
            moved = true
        }

        if enemy.Z < enemy.DestZ {
            enemy.Z += min(1, enemy.DestZ - enemy.Z)
            moved = true
        } else if enemy.Z > enemy.DestZ {
            enemy.Z -= min(1, enemy.Z - enemy.DestZ)
            moved = true
        }

        if !moved {
            enemy.HasDestination = false
            enemy.CurrentAnimation = enemy.Animations["idle"]
            enemy.State = EnemyStateIdle
        } else {
            enemy.State = EnemyStateWalking
        }
    }
}

func (enemy *Enemy) Update(level *Level, playerInfo PlayerInfo) {
    enemy.UpdateState(level, playerInfo)

    switch enemy.State {
        case EnemyStateFalling, EnemyStateFallen, EnemyStateRise, EnemyStatePain:
        default:
            if enemy.X < playerInfo.GetX() {
                enemy.Facing = FacingRight
            }
            if enemy.X > playerInfo.GetX() {
                enemy.Facing = FacingLeft
            }
    }

    if enemy.CurrentAnimation != nil {
        loopAnimation := true
        if enemy.State == EnemyStateFallen || enemy.State == EnemyStateFalling {
            loopAnimation = false
        }
        if enemy.CurrentAnimation.Update(loopAnimation) {
            if enemy.State == EnemyStateAttacking || enemy.State == EnemyStateRise || enemy.State == EnemyStatePain {
                enemy.State = EnemyStateIdle
            }
        }
    }
}
