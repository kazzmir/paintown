package paintown

import (
    "github.com/kazzmir/paintown/game/data"
    "github.com/kazzmir/paintown/game/lib/sexp"
)

type Flash struct {
    X, Y float64
    Animation *Animation
}

type FlashFactory struct {
    Animation *Animation
}

func (factory *FlashFactory) MakeFlash(x, y float64) *Flash {
    return &Flash{X: x, Y: y, Animation: factory.Animation.Clone()}
}

func MakeFlashFactory() (*FlashFactory, error) {
    path := data.DataPath("misc/flash/flash.txt")
    raw, err := sexp.ReadSExpression(path)
    if err != nil {
        return nil, err
    }

    animation, err := MakeAnimationFromDefinition("misc/flash", raw)
    if err != nil {
        return nil, err
    }

    return &FlashFactory{Animation: animation}, nil
}
