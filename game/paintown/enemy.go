package paintown

import (
    "log"
    "path/filepath"
    "strings"

    "github.com/kazzmir/paintown/game/data"
)

type Enemy struct {
    Character *CharacterDefinition
    X float64
    Z float64
    Y float64
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

func (enemy *Enemy) Update(level *Level) {
    if enemy.CurrentAnimation != nil {
        enemy.CurrentAnimation.Update()
    }
}
