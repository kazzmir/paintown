package paintown

import (
    "github.com/kazzmir/paintown/game/lib/sexp"
    "github.com/kazzmir/paintown/game/data"

    "github.com/hajimehoshi/ebiten/v2"
)

type Item struct {
    X int
    Y int
    Stimulation *Stimulation
    Sound string
    Frame *ebiten.Image
}

type ItemFactory struct {
    Items map[string]*Item
}

func (factory *ItemFactory) Load(path string) (*Item, error) {
    if factory.Items == nil {
        factory.Items = make(map[string]*Item)
    }

    item, ok := factory.Items[path]
    if ok {
        return item, nil
    }

    fullPath := data.DataPath(path)
    raw, err := sexp.ReadSExpression(fullPath)
    if err != nil {
        return nil, err
    }

    soundExp := raw.GetChild("sound")
    sound := ""
    if soundExp != nil {
        sound = soundExp.GetValue(0)
    }

    frameExp := raw.GetChild("frame")
    var frame *ebiten.Image
    if frameExp != nil {
        framePath := frameExp.GetValue(0)
        originalFrame, err := data.LoadPng(data.DataPath(framePath))
        if err != nil {
            return nil, err
        }

        frame = ebiten.NewImageFromImage(originalFrame)
    }

    item = &Item{
        Sound: sound,
        Frame: frame,
    }

    factory.Items[path] = item
    return item, nil
}

func MakeItem(object BlockObject, itemFactory *ItemFactory) (*Item, error) {
    original, err := itemFactory.Load(object.Path)
    if err != nil {
        return nil, err
    }
    stimulation := object.Stimulation

    return &Item{
        X: object.Coords.X,
        Y: object.Coords.Y,
        Stimulation: stimulation,
        Sound: original.Sound,
        Frame: original.Frame,
    }, nil
}
