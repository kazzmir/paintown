package paintown

import (
    "log"
    "math/rand/v2"
    "path/filepath"
    "strings"

    "github.com/kazzmir/paintown/game/data"
)

type Enemy struct {
    Character *CharacterDefinition
    X float64
    Z float64
    Y float64

    HasDestination bool
    DestX float64
    DestZ float64
    CurrentAnimation *Animation
    Animations map[string]*Animation
    Facing Facing
}

func loadAnimations(definition *CharacterDefinition) (map[string]*Animation, error) {
    animations := definition.FindAll("character", "anim")

    // log.Printf("Found %v animations for character %v", len(animations), character.Definition.Name)
    out := make(map[string]*Animation)

    for _, animation := range animations {
        animationName := animation.GetChild("name")
        name := animationName.GetValue(0)
        base := filepath.Join("chars", strings.ToLower(definition.Name), name)
        animation, err := MakeAnimationFromDefinition(base, animation)
        if err == nil {
            out[name] = animation
        } else {
            log.Printf("Error loading animation '%v' for character '%v': %v", name, definition.Name, err)
        }
    }

    return out, nil
}

func MakeEnemy(object BlockObject) (*Enemy, error) {
    definitionPath := data.DataPath(object.Path)
    definition, err := loadDefinition(definitionPath)
    if err != nil {
        return nil, err
    }

    animations, err := loadAnimations(&definition)

    idle, ok := animations["idle"]
    if !ok {
        idle = nil
    }

    return &Enemy{
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

func (enemy *Enemy) Move(level *Level, playerInfo PlayerInfo) {
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
        enemy.CurrentAnimation.Update()
    }
}
