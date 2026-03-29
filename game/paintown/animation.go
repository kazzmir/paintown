package paintown

import (
    "log"
    "image"
    "sync"
    "strconv"
    "strings"
    "path/filepath"

    "github.com/kazzmir/paintown/game/lib/sexp"
    "github.com/kazzmir/paintown/game/graphics"
    "github.com/kazzmir/paintown/game/data"

    "github.com/hajimehoshi/ebiten/v2"
)

type AnimationOwner interface {
    Move(x int, y int, z int)
    SetAttack(attack AnimationAttack)
    GetFacing() Facing
    SetFacing(facing Facing)
    SetTrail(generate int, length int)
    NextAttackId()
}

type AnimationAttack struct {
    X1 int
    Y1 int
    X2 int
    Y2 int
    Force float64
    Damage float64
}

func (attack *AnimationAttack) GetHitBox() image.Rectangle {
    return image.Rect(attack.X1, attack.Y1, attack.X2, attack.Y2)
}

func (attack *AnimationAttack) IsEmpty() bool {
    return attack.X1 == 0 && attack.Y1 == 0 && attack.X2 == 0 && attack.Y2 == 0
}

type Animation struct {
    Name string
    Type string
    Frame *ebiten.Image

    Collision map[*ebiten.Image]*Collision

    Events []AnimationEvent
    CurrentEvent int
    Keys []InputKey
    // times when the keys were pressed
    KeyPresses []uint64

    Range int

    Sequence string
    Status string

    OffsetX int
    OffsetY int

    Delay int
    CurrentDelay int

    Owner AnimationOwner
}

func MakeAnimation(name string, animationType string, events []AnimationEvent, keys []InputKey, sequence string, status string) *Animation {
    return &Animation{
        Name: name,
        Type: animationType,
        Keys: keys,
        KeyPresses: make([]uint64, len(keys)),
        Collision: make(map[*ebiten.Image]*Collision),
        Events: events,
        Sequence: sequence,
        Status: status,
        Delay: 1, // set delay to something non-zero to prevent immediately looping through all events
    }
}

// return a duplicate animation but without an owner
// the caller should invoke Reset() on the returned animation to reset the event state
func (animation *Animation) Clone() *Animation {
    out := *animation
    out.Owner = nil
    out.KeyPresses = make([]uint64, len(animation.Keys))
    out.CurrentEvent = 0
    return &out
}

func (animation *Animation) HasAttack() bool {
    return animation.Type == "attack"
}

func (animation *Animation) GetRange() int {
    return animation.Range
}

func (animation *Animation) CurrentFrame() *ebiten.Image {
    return animation.Frame
}

func (animation *Animation) CurrentCollision() *Collision {
    return animation.Collision[animation.Frame]
}

func (animation *Animation) GetOffsetX() int {
    return animation.OffsetX
}

func (animation *Animation) GetOffsetY() int {
    return animation.OffsetY
}

func (animation *Animation) Reset() {
    animation.CurrentEvent = 0
    animation.CurrentDelay = 0
}

func (animation *Animation) InitializeCollision() {
    collisionMap := make(map[*ebiten.Image]*Collision)

    var group sync.WaitGroup
    var lock sync.Mutex

    for _, event := range animation.Events {
        if frameEvent, ok := event.(*AnimationEventFrame); ok {
            group.Go(func() {
                collision := MakeCollision(frameEvent.Image)

                lock.Lock()
                collisionMap[frameEvent.Image] = collision
                lock.Unlock()
            })
        }
    }

    group.Wait()

    animation.Collision = collisionMap
}

// returns true if the animation reaches the end of its events and loops back to the beginning
func (animation *Animation) Update(loop bool, system System) bool {
    if animation.CurrentDelay > 0 {
        animation.CurrentDelay -= 1
        return false
    } else {
        if len(animation.Events) == 0 || animation.CurrentEvent >= len(animation.Events) {
            return true
        }
        now := animation.CurrentEvent
        finished := false
        for animation.CurrentDelay == 0 {
            animation.Events[animation.CurrentEvent].Update(animation, system)
            animation.CurrentEvent += 1
            if animation.CurrentEvent >= len(animation.Events) {
                if loop {
                    animation.CurrentEvent = 0
                    finished = true

                    // always reset attack
                    if animation.Owner != nil {
                        animation.Owner.SetAttack(AnimationAttack{})
                    }
                } else {
                    return true
                }
            }

            // looped without setting delay, this would have been an infinite loop
            if animation.CurrentEvent == now {
                log.Printf("Warning: animation looped without setting delay (probably missing a frame)")
                break
            }
        }

        return finished
    }
}

type System interface {
    PlaySound(sound string) error
}

type AnimationEvent interface {
    Update(*Animation, System)
}

type AnimationEventFrame struct {
    Image *ebiten.Image
}

func (frameEvent *AnimationEventFrame) Update(animation *Animation, system System) {
    animation.Frame = frameEvent.Image
    animation.CurrentDelay = animation.Delay
}

// reset the delay timer
type AnimationEventNop struct {
}

func (nopEvent *AnimationEventNop) Update(animation *Animation, system System) {
    animation.CurrentDelay = animation.Delay
}

type AnimationEventNextTicket struct {
}

func (nextTicketEvent *AnimationEventNextTicket) Update(animation *Animation, system System) {
    animation.Owner.NextAttackId()
}

type AnimationEventDelay struct {
    Delay float32
}

func (delayEvent *AnimationEventDelay) Update(animation *Animation, system System) {
    animation.Delay = int(delayEvent.Delay)
}

type AnimationEventAttack struct {
    Attack AnimationAttack
}

func (attackEvent *AnimationEventAttack) Update(animation *Animation, system System) {
    // log.Printf("set attack to %+v", attackEvent.Attack)
    owner := animation.Owner
    owner.SetAttack(attackEvent.Attack)
}

type AnimationEventOffset struct {
    X int
    Y int
}

func (offsetEvent *AnimationEventOffset) Update(animation *Animation, system System) {
    animation.OffsetX = offsetEvent.X
    animation.OffsetY = offsetEvent.Y
}

type AnimationEventType struct {
    Type string
}

func (typeEvent *AnimationEventType) Update(animation *Animation, system System) {
    // TODO
}

type AnimationMoveEvent struct {
    X int
    Y int
    Z int
}

func (moveEvent *AnimationMoveEvent) Update(animation *Animation, system System) {
    x := moveEvent.X

    if animation.Owner.GetFacing() == FacingLeft {
        x = -x
    }

    animation.Owner.Move(x, moveEvent.Y, moveEvent.Z)
}

type AnimationFaceEvent struct {
    Facing string
}

func (faceEvent *AnimationFaceEvent) Update(animation *Animation, system System) {
    switch faceEvent.Facing {
        case "reverse": animation.Owner.SetFacing(animation.Owner.GetFacing().Reverse())
        case "left": animation.Owner.SetFacing(FacingLeft)
        case "right": animation.Owner.SetFacing(FacingRight)
    }
}

type AnimationEventRelativeOffset struct {
    X int
    Y int
}

func (relativeOffsetEvent *AnimationEventRelativeOffset) Update(animation *Animation, system System) {
    animation.OffsetX += relativeOffsetEvent.X
    animation.OffsetY += relativeOffsetEvent.Y
}

type AnimationTrailEvent struct {
    Generate int
    Length int
}

func (trailEvent *AnimationTrailEvent) Update(animation *Animation, system System) {
    animation.Owner.SetTrail(trailEvent.Generate, trailEvent.Length)
}

type AnimationSoundEvent struct {
    // path to sound file
    Sound string
}

func (soundEvent *AnimationSoundEvent) Update(animation *Animation, system System) {
    system.PlaySound(soundEvent.Sound)
}

type InputKey int
const (
    InputKeyNone InputKey = iota
    InputKeyJump
    InputKeyAttack1
    InputKeyAttack2
    InputKeyForward
    InputKeyDown
    InputKeyBack
)

func keyFromString(key string) InputKey {
    switch strings.ToLower(key) {
        case "key_attack1": return InputKeyAttack1
        case "key_attack2": return InputKeyAttack2
        case "key_jump": return InputKeyJump
        case "key_forward": return InputKeyForward
        case "key_down": return InputKeyDown
        case "key_back": return InputKeyBack
    }

    return InputKeyNone
}

func parseAttack(definition *sexp.SExpr) AnimationAttack {
    var x1, y1, x2, y2 int
    var force, damage float64

    x1, _ = sexp.ReadValue[int](definition, "x1", 0)
    y1, _ = sexp.ReadValue[int](definition, "y1", 0)
    x2, _ = sexp.ReadValue[int](definition, "x2", 0)
    y2, _ = sexp.ReadValue[int](definition, "y2", 0)
    force, _ = sexp.ReadValue[float64](definition, "force", 0)
    damage, _ = sexp.ReadValue[float64](definition, "damage", 0)

    return AnimationAttack{
        X1: x1,
        Y1: y1,
        X2: x2,
        Y2: y2,
        Force: force,
        Damage: damage,
    }
}

func MakeAnimationFromDefinition(baseDirectory string, definition *sexp.SExpr) (*Animation, error) {
    var events []AnimationEvent
    var name string
    var sequence string
    var status string
    var rangeValue int = 0
    var keys []InputKey
    var type_ string
    for _, child := range definition.Children {
        switch strings.ToLower(child.Name) {
            case "name":
                name = strings.ToLower(child.GetValue(0))
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
                box := child.GetChild("box")
                if box != nil {
                    events = append(events, &AnimationEventAttack{Attack: parseAttack(box)})
                } else {
                    events = append(events, &AnimationEventAttack{Attack: parseAttack(child)})
                }

            case "range":
                v, ok := child.GetInt(0)
                if ok {
                    rangeValue = v
                }
            case "status":
                status = child.GetValue(0)
            case "sequence":
                sequence = child.GetValue(0)
            case "type":
                type_ = child.GetValue(0)
            case "keys":
                for _, key := range child.Children {
                    input := keyFromString(key.Name)
                    if input != InputKeyNone {
                        keys = append(keys, input)
                    } else {
                        log.Printf("Unknown input key '%v'", key.Name)
                    }
                }
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
                img, err := data.LoadPng(filepath.Join(baseDirectory, frame))
                if err != nil {
                    return nil, err
                }
                events = append(events, &AnimationEventFrame{
                    Image: ebiten.NewImageFromImage(graphics.ConvertTransparency(img)),
                })
            case "move":
                x := 0
                y := 0
                z := 0
                for i := range len(child.Children) {
                    value, _ := child.GetInt(i)
                    switch i {
                        case 0: x = value
                        case 1: y = value
                        case 2: z = value
                    }
                }
                events = append(events, &AnimationMoveEvent{X: x, Y: y, Z: z})
            case "face":
                value := child.GetValue(0)
                events = append(events, &AnimationFaceEvent{Facing: value})
            case "trail":
                generate, _ := sexp.ReadValue[int](child, "generate", 0)
                length, _ := sexp.ReadValue[int](child, "length", 0)
                // log.Printf("Trail generate=%v length=%v", generate, length)
                events = append(events, &AnimationTrailEvent{Generate: generate, Length: length})
            case "sound":
                value := child.GetValue(0)
                events = append(events, &AnimationSoundEvent{Sound: value})
            case "bbox":
                // ignore
            case "loop":
                // used to determine if the animation should loop
            case "shadow":
                // ignore, shadow is dynamically computed now
            case "nop":
                events = append(events, &AnimationEventNop{})
            case "next-ticket":
                events = append(events, &AnimationEventNextTicket{})
            default:
                log.Printf("'%s': Unknown animation event type '%v'", name, child.Name)
        }
    }

    out := MakeAnimation(name, type_, events, keys, sequence, status)
    out.Range = rangeValue

    return out, nil
}
