package paintown

import (
    "github.com/kazzmir/paintown/game/data"
    "github.com/kazzmir/paintown/game/lib/sexp"
)

type Flash struct {
    X, Y, Z float64
    Animation *Animation
}

type FlashFactory struct {
    Animation *Animation
}

func (factory *FlashFactory) MakeFlash(x, y, z float64) *Flash {
    return &Flash{X: x, Y: y, Z: z, Animation: factory.Animation.Clone()}
}

func MakeFlashFactory() (*FlashFactory, error) {
    path := data.DataPath("misc/flash/flash.txt")
    raw, err := sexp.ReadSExpression(path)
    if err != nil {
        return nil, err
    }

    // sexp should be (effect (anim ...))

    animSexp := raw.GetChild("anim")

    animation, err := MakeAnimationFromDefinition("misc/flash", animSexp)
    if err != nil {
        return nil, err
    }

    return &FlashFactory{Animation: animation}, nil
}
