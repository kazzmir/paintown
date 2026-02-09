package data

import (
    "os"
    "image"
    "image/png"

    "github.com/hajimehoshi/ebiten/v2"
)

const ScreenWidth = 640
const ScreenHeight = 480

type DrawFunc func (*ebiten.Image)

func DataPath(path string) string {
    return "paintown-data/" + path
}

func LoadPng(path string) (image.Image, error) {
    file, err := os.Open(DataPath(path))
    if err != nil {
        return nil, err
    }
    defer file.Close()

    img, err := png.Decode(file)
    if err != nil {
        return nil, err
    }

    return img, nil
}
