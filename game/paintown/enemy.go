package paintown

import (
    "log"
    "fmt"
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
)

type Enemy struct {
    Character *CharacterDefinition
    X float64
    Z float64
    Y float64

    State EnemyState
    FallenCount int

    HasDestination bool
    DestX float64
    DestZ float64
    CurrentAnimation *Animation
    Animations map[string]*Animation
    Attacks []*Animation
    Facing Facing
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

    return &Enemy{
        Attacks: attacks,
        Character: &definition,
        Animations: animations,
        CurrentAnimation: idle,
        Facing: FacingLeft,
        X: float64(object.Coords.X),
        Z: float64(object.Coords.Y),
    }, nil
}

type PlayerInfo interface {
    GetX() float64
    GetZ() float64
}

func abs(x float64) float64 {
    return max(x, -x)
}

func (enemy *Enemy) GetAttacks() []*Animation {
    return enemy.Attacks
}

func (enemy *Enemy) Move(level *Level, playerInfo PlayerInfo) {

    if enemy.State == EnemyStateAttacking {
        return
    }

    if enemy.State == EnemyStateFallen {
        if enemy.FallenCount > 0 {
            enemy.FallenCount -= 1
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

    if enemy.State == EnemyStateFalling {
        enemy.Y -= 1
        if enemy.Y < 0 {
            enemy.Y = 0
            enemy.State = EnemyStateFallen
            // stay on ground for a while
            enemy.FallenCount = 30
        }
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
    enemy.Move(level, playerInfo)

    if enemy.X < playerInfo.GetX() {
        enemy.Facing = FacingRight
    }
    if enemy.X > playerInfo.GetX() {
        enemy.Facing = FacingLeft
    }

    if enemy.CurrentAnimation != nil {
        if enemy.CurrentAnimation.Update() {
            if enemy.State == EnemyStateAttacking || enemy.State == EnemyStateRise {
                enemy.State = EnemyStateIdle
            }
        }
    }
}
