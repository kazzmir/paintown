package paintown

import (
    "log"
    "strings"
    "image"
    "path/filepath"
    "fmt"
    "cmp"
    "slices"

    "github.com/kazzmir/paintown/game/lib/sexp"
    "github.com/kazzmir/paintown/game/graphics"
    "github.com/kazzmir/paintown/game/data"

    "github.com/hajimehoshi/ebiten/v2"
)

type PlayerStatus int
const (
    PlayerIdle PlayerStatus = iota
    PlayerMove
    PlayerJump
    PlayerStateFallen
    PlayerStateFalling
    PlayerStatePain
)

type Facing int
const (
    FacingRight Facing = iota
    FacingLeft
)

func (facing Facing) Reverse() Facing {
    if facing == FacingRight {
        return FacingLeft
    } else {
        return FacingRight
    }
}

type InputState struct {
    HeldRight bool
    HeldLeft bool
    HeldUp bool
    HeldDown bool
    HeldJump bool
    HeldAttack1 bool
    HeldAttack2 bool

    Right bool
    Left bool
    Up bool
    Down bool
    Jump bool
    Attack1 bool
    Attack2 bool
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
            animation.InitializeCollision()
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

func (definition *CharacterDefinition) GetHealth() float64 {
    v, _ := sexp.ReadValue[float64](definition.SExpr, "health", 0)
    return v
}

func (definition *CharacterDefinition) FindAll(names ...string) []*sexp.SExpr {
    return definition.SExpr.FindAll(names...)
}

func (definition *CharacterDefinition) GetIcon() string {
    icon := definition.SExpr.GetChild("icon")
    if icon != nil {
        return icon.GetValue(0)
    }

    return ""
}

func (definition *CharacterDefinition) GetHitSound() string {
    hit := definition.SExpr.GetChild("hit-sound")
    if hit != nil {
        return hit.GetValue(0)
    }

    return ""
}

func (definition *CharacterDefinition) GetDieSound() string {
    die := definition.SExpr.GetChild("die-sound")
    if die != nil {
        return die.GetValue(0)
    }

    return ""
}

func (definition *CharacterDefinition) GetName() string {
    name := definition.SExpr.GetChild("name")
    if name != nil {
        return name.GetValue(0)
    }

    return ""
}

func (definition *CharacterDefinition) GetFallSound() string {
    die := definition.SExpr.GetChild("landed")
    if die != nil {
        return die.GetValue(0)
    }

    return ""
}

// a definition file is a parentheses delimited set of values
func LoadDefinition(path string) (CharacterDefinition, error) {
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
    path := data.DataPath("players/" + name)

    definitionPath := filepath.Join(path, name + ".txt")
    definition, err := LoadDefinition(definitionPath)
    if err != nil {
        return nil, err
    }

    player := &PaintownCharacter{
        Definition: definition,
    }

    return player, nil
}


type Trail struct {
    X float64
    Y float64
    Z float64
    Facing Facing
    Image *ebiten.Image
    Time int
}

type Attacker interface {
}

type PlayerState struct {
    X float64
    Y float64
    Z float64
    Dy float64
    Dx float64
    Animations map[string]*Animation
    // if doing some kind of move
    ShowAnimation *Animation
    Status PlayerStatus
    Facing Facing

    Icon *ebiten.Image

    AttackId uint64

    HitSound string

    Health float64
    MaxHealth float64
    Pain float64
    PainThreshold float64

    Attackers map[Attacker]uint64

    NextAnimation *Animation
    NextAnimationTime uint64

    Name string

    Attack AnimationAttack

    TrailActive bool
    TrailGenerate int
    TrailLength int

    Trails []*Trail

    // FIXME: this needs to be per enemy
    LastAttacked uint64
}

func MakePlayerState(player *PaintownCharacter, level *Level) (*PlayerState, error) {
    animations, err := player.LoadAnimations()
    if err != nil {
        return nil, err
    }

    icon := player.Definition.GetIcon()

    var iconImage *ebiten.Image
    if icon != "" {
        img, err := data.LoadPng(icon)
        if err == nil {
            iconImage = ebiten.NewImageFromImage(graphics.ConvertTransparency(img))
        } else {
            log.Printf("Unable to load icon %v: %v", icon, err)
        }
    }

    name := player.Definition.GetName()

    playerState := PlayerState{
        X: 60,
        Y: 0,
        Z: float64(level.ZMinimum + level.ZMaximum) / 2,
        Status: PlayerIdle,
        Animations: animations,
        HitSound: player.Definition.GetHitSound(),
        // TODO: make configurable in the definition file
        PainThreshold: 4,
        Icon: iconImage,
        Health: max(1, player.Definition.GetHealth()),
        MaxHealth: max(1, player.Definition.GetHealth()),
        Attackers: make(map[Attacker]uint64),
        Name: name,
    }

    for _, animation := range playerState.Animations {
        animation.Owner = &playerState
    }

    return &playerState, nil
}

// return a rectangle representing the attack hitbox in world coordinates, taking into account the player's position, facing direction, and current animation frame
func (playerState *PlayerState) GetAttackBox() image.Rectangle {
    var geom ebiten.GeoM

    if playerState.Facing == FacingLeft {
        geom.Scale(-1, 1)
    }

    geom.Translate(playerState.X, playerState.Z - playerState.Y)

    animation := playerState.CurrentAnimation()

    bounds := animation.CurrentFrame().Bounds()
    if playerState.Facing == FacingLeft {
        geom.Translate(+float64(bounds.Dx()) / 2 - float64(animation.GetOffsetX()), float64(-bounds.Dy()) + float64(animation.GetOffsetY()))
    } else {
        geom.Translate(-float64(bounds.Dx()) / 2 + float64(animation.GetOffsetX()), float64(-bounds.Dy()) + float64(animation.GetOffsetY()))
    }

    x1, y1 := geom.Apply(float64(playerState.Attack.X1), float64(playerState.Attack.Y1))
    x2, y2 := geom.Apply(float64(playerState.Attack.X2), float64(playerState.Attack.Y2))

    return image.Rect(int(x1), int(y1), int(x2), int(y2)).Canon()
}

func (playerState *PlayerState) ResetAttackers() {
    playerState.Attackers = make(map[Attacker]uint64)
}

func (playerState *PlayerState) SetAttack(attack AnimationAttack) {
    playerState.Attack = attack
}

func (playerState *PlayerState) GetIcon() *ebiten.Image {
    return playerState.Icon
}

func (playerState *PlayerState) GetHealthPercent() float64 {
    return playerState.Health / playerState.MaxHealth
}

func (playerState *PlayerState) GetX() float64 {
    return playerState.X
}

func (playerState *PlayerState) GetY() float64 {
    return playerState.Y
}

func (playerState *PlayerState) GetZ() float64 {
    return playerState.Z
}

func (playerState *PlayerState) GetFacing() Facing {
    return playerState.Facing
}

func (playerState *PlayerState) SetFacing(facing Facing) {
    playerState.Facing = facing
}

func (playerState *PlayerState) Move(x int, y int, z int) {
    playerState.X += float64(x)
    playerState.Y += float64(y)
    playerState.Z += float64(z)
}

func (playerState *PlayerState) SetTrail(generate int, length int) {
    playerState.TrailActive = true
    playerState.TrailGenerate = generate
    playerState.TrailLength = length
}

func (playerState *PlayerState) GetAnimation(name string) *Animation {
    animation, ok := playerState.Animations[name]
    if ok {
        return animation
    }
    return nil
}

func (playerState *PlayerState) UpdateTrails(counter uint64) {
    if playerState.TrailActive {
        if playerState.TrailGenerate > 0 && counter % uint64(playerState.TrailGenerate) == 0 {
            playerState.Trails = append(playerState.Trails, &Trail{
                X: playerState.X + float64(playerState.CurrentAnimation().GetOffsetX()),
                Y: playerState.Y + float64(playerState.CurrentAnimation().GetOffsetY()),
                Z: playerState.Z,
                Facing: playerState.Facing,
                Image: playerState.CurrentAnimation().CurrentFrame(),
                Time: playerState.TrailLength,
            })
        }
    }

    var trails []*Trail
    for _, trail := range playerState.Trails {
        trail.Time -= 1
        if trail.Time > 0 {
            trails = append(trails, trail)
        }
    }
    playerState.Trails = trails
}

func (playerState *PlayerState) CanBeHit(hitter Attacker, attack uint64) bool {
    lastAttack, has := playerState.Attackers[hitter]

    if has && attack <= lastAttack {
        return false
    }

    return playerState.Status != PlayerStateFallen && playerState.Status != PlayerStateFalling
}

func (playerState *PlayerState) Hurt(hitter Attacker, attackId uint64, damage float64, force float64) {
    playerState.Attackers[hitter] = attackId
    playerState.Health -= damage
    playerState.Pain += damage

    playerState.Status = PlayerStatePain
    painAnimation, ok := playerState.Animations["pain"]
    if ok {
        playerState.ShowAnimation = painAnimation
        playerState.ShowAnimation.Reset()
    }

    /*
    // TODO
    if playerState.Pain >= playerState.PainThreshold || playerState.Health <= 0 {
        enemy.DoFall(force)
    }
    */
}

func (playerState *PlayerState) HitBy(attackBox image.Rectangle) bool {
    collision := playerState.CurrentAnimation().CurrentCollision()
    if collision == nil {
        return false
    }

    x := playerState.X + float64(playerState.CurrentAnimation().OffsetX)
    y := playerState.Z + playerState.Y + float64(playerState.CurrentAnimation().OffsetY)

    return collision.Intersect(x, y, attackBox, playerState.Facing == FacingLeft)
}

func (playerState *PlayerState) CurrentAnimation() *Animation {
    if playerState.ShowAnimation != nil {
        return playerState.ShowAnimation
    }

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

func (playerState *PlayerState) GetName() string {
    return playerState.Name
}

// FIXME: replace string with an enum
func (playerState *PlayerState) GetStatus() string {
    switch playerState.Status {
        case PlayerIdle: return "ground"
        case PlayerMove: return "ground"
        case PlayerJump: return "jump"
    }

    return "ground"
}

func (playerState *PlayerState) Update(input InputState, level *Level, system System, counter uint64) {
    doJump := false
    move := false

    playerState.Pain = max(0, playerState.Pain - 0.1)

    var nextAnimation *Animation

    playerState.UpdateTrails(counter)

    // if playerState.Status != PlayerJump {
        /*
        if input.Attack1 {
            playerState.ShowAnimation = playerState.GetAnimation("strong-punch")
        }
        */

        var possibleNextAnimations []*Animation

        for _, animation := range playerState.Animations {
            if animation.Name == "idle" || animation.Name == "walk" || animation.Name == "grab" || animation.Name == "get" || animation.Name == "jump" {
                continue
            }

            inSequence := false

            if animation.Sequence != "" {
                ok := true

                /*
                if playerState.ShowAnimation != nil {
                    log.Printf("Check animation %v with sequence %v against %v", animation.Name, animation.Sequence, playerState.ShowAnimation.Name)
                }
                */

                if playerState.ShowAnimation != nil && animation.Sequence == playerState.ShowAnimation.Name {
                    // log.Printf("Animation %v is ok", animation.Name)
                    inSequence = true
                } else {
                    ok = false
                }
                if !ok {
                    continue
                }
            }

            requiredStatus := "ground"
            if animation.Status != "" {
                requiredStatus = animation.Status
            }

            if playerState.GetStatus() != requiredStatus {
                continue
            }

            /*
            if requiredStatus == "jump" {
                log.Printf("Consider jumping animation %v", animation.Name)
            }
            */

            /*
            if animation.Status != "" && animation.Status != "ground" {
                continue
            }
            */

            pressedAll := len(animation.Keys) > 0
            for i, key := range animation.Keys {

                if counter - animation.KeyPresses[i] < uint64(len(animation.Keys) - i) * 20 {
                } else {
                    switch key {
                        case InputKeyJump:
                            if input.Jump {
                                animation.KeyPresses[i] = counter
                            }
                        case InputKeyAttack1:
                            if input.Attack1 {
                                animation.KeyPresses[i] = counter
                            }
                        case InputKeyAttack2:
                            if input.Attack2 {
                                animation.KeyPresses[i] = counter
                            }
                        case InputKeyForward:
                            if (playerState.Facing == FacingRight && input.Right) || (playerState.Facing == FacingLeft && input.Left) {
                                animation.KeyPresses[i] = counter
                            }
                        case InputKeyDown:
                            if input.Down {
                                animation.KeyPresses[i] = counter
                            }
                        case InputKeyBack:
                            if ( playerState.Facing == FacingRight && input.Left) || (playerState.Facing == FacingLeft && input.Right) {
                                animation.KeyPresses[i] = counter
                            }
                    }

                    pressedAll = false
                    break
                }
            }

            if pressedAll {
                // prefer animation with a sequence
                if nextAnimation == nil || inSequence {
                    // log.Printf("Possible next animation: '%v' presses: %v", animation.Name, animation.KeyPresses)
                    possibleNextAnimations = append(possibleNextAnimations, animation)
                    // log.Printf("Set next animation to '%v' at %v", animation.Name, counter)
                    // nextAnimation = animation
                    // playerState.NextAnimationTime = counter
                }
                // break
            }
        }

        // prioritize moves that are in sequence with the current animation, and
        // have the longest set of keys to activate them
        if len(possibleNextAnimations) > 0 {
            isInSequence := func(animation *Animation) bool {
                return playerState.ShowAnimation != nil && animation.Sequence == playerState.ShowAnimation.Name
            }

            slices.SortFunc(possibleNextAnimations, func(a, b *Animation) int {
                scoreA := 0
                scoreB := 0

                if isInSequence(a) {
                    scoreA += 1000
                }

                if isInSequence(b) {
                    scoreB += 1000
                }

                scoreA += len(a.Keys)
                scoreB += len(b.Keys)

                return cmp.Compare(scoreA, scoreB)
            })

            /*
            for _, animation := range possibleNextAnimations {
                log.Printf("Possible next animation: %v", animation.Name)
            }
            */

            // last element should be the one with the highest score
            nextAnimation = possibleNextAnimations[len(possibleNextAnimations) - 1]
            playerState.NextAnimationTime = counter

            // log.Printf("Choose next animation: %v", nextAnimation.Name)
        }

        if playerState.Status != PlayerJump && playerState.ShowAnimation == nil {
            if input.HeldRight {
                playerState.X += 1
                playerState.Facing = FacingRight
                move = true
            } else if input.HeldLeft {
                playerState.X -= 1
                playerState.Facing = FacingLeft
                move = true
            }

            if input.HeldDown {
                playerState.Z = min(float64(level.ZMaximum), playerState.Z + 1)
                move = true
            } else if input.HeldUp {
                playerState.Z = max(float64(level.ZMinimum), playerState.Z - 1)
                move = true
            }

            if input.Jump {
                doJump = true
            }
        }
    // }

    if doJump {
        playerState.Status = PlayerJump
        playerState.Dy = 6.5
        playerState.Y = 0.001

        if input.HeldRight {
            playerState.Dx = 1
        }
        if input.HeldLeft {
            playerState.Dx = -1
        }

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

    if playerState.Status == PlayerMove || playerState.Status == PlayerIdle || playerState.Status == PlayerJump {
        if nextAnimation != nil && playerState.ShowAnimation == nil {
            playerState.AttackId += 1
            playerState.ShowAnimation = nextAnimation
            nextAnimation.Reset()
            playerState.TrailActive = false
        }
    }

    if playerState.Status == PlayerJump {
        playerState.Dy -= 0.2
        playerState.Y += playerState.Dy
        playerState.X += playerState.Dx
        if playerState.Y <= 0 {
            playerState.Y = 0
            playerState.Dy = 0
            playerState.Dx = 0
            playerState.Status = PlayerIdle
            playerState.ShowAnimation = nil
            playerState.Attack = AnimationAttack{}
        }
    }

    animation := playerState.CurrentAnimation()

    if animation != nil {
        if animation.Update(true, system) {
            if playerState.Status == PlayerJump {
                playerState.ShowAnimation = nil
            } else if /* playerState.Status != PlayerJump && */ playerState.Status != PlayerMove {
                playerState.Status = PlayerIdle
                if playerState.ShowAnimation != nil {
                    playerState.ShowAnimation = nil

                    if counter - playerState.NextAnimationTime < 180 {
                        playerState.AttackId += 1
                        playerState.ShowAnimation = nextAnimation
                        playerState.TrailActive = true
                        playerState.NextAnimation = nil
                        if playerState.ShowAnimation != nil {
                            playerState.ShowAnimation.Reset()
                            playerState.ShowAnimation.Update(true, system)
                        }
                    }
                }
            }
        }
    }
}

