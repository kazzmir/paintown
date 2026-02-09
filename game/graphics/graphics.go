package graphics

import (
    "image"
    "image/color"
)

// images use 255,0,255 as the transparent color
func ConvertTransparency(img image.Image) image.Image {
    bounds := img.Bounds()
    newImg := image.NewNRGBA(bounds)

    for x := bounds.Min.X; x < bounds.Max.X; x++ {
        for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
            r, g, b, _ := img.At(x, y).RGBA()
            if r == 0xFFFF && g == 0 && b == 0xFFFF {
                newImg.Set(x, y, image.Transparent)
            } else {
                newImg.Set(x, y, img.At(x, y))
            }
        }
    }

    return newImg
}

func InterpolateColors(start, end color.RGBA, steps int) []color.RGBA {
    var colors []color.RGBA

    for i := 0; i < steps; i++ {
        t := float64(i) / float64(steps-1)
        r := uint8(float64(start.R)*(1-t) + float64(end.R)*t)
        g := uint8(float64(start.G)*(1-t) + float64(end.G)*t)
        b := uint8(float64(start.B)*(1-t) + float64(end.B)*t)
        a := uint8(float64(start.A)*(1-t) + float64(end.A)*t)
        colors = append(colors, color.RGBA{R: r, G: g, B: b, A: a})
    }

    return colors
}

