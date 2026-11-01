package graphics

import (
    "image"
    "image/color"

    "github.com/hajimehoshi/ebiten/v2"
)

var source *ebiten.Image

func init() {
    source = ebiten.NewImage(1, 1)
    source.Fill(color.NRGBA{R: 255, G: 255, B: 255, A: 255})
}

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

// returns alpha pre-multiplied float32 values in the range 0-1
func frgba(col color.Color) (float32, float32, float32, float32) {
    r, g, b, a := col.RGBA()
    rf := float32(r) / 0xffff
    gf := float32(g) / 0xffff
    bf := float32(b) / 0xffff
    af := float32(a) / 0xffff

    return rf, gf, bf, af
}

func DrawHorizontalGradient(buffer *ebiten.Image, x float32, y float32, width float32, height float32, leftColor color.Color, rightColor color.Color) {
    leftR, leftG, leftB, leftA := frgba(leftColor)
    rightR, rightG, rightB, rightA := frgba(rightColor)

    vertices := [4]ebiten.Vertex{
        ebiten.Vertex{
            DstX: x,
            DstY: y,
            SrcX: 0,
            SrcY: 0,
            ColorR: leftR,
            ColorG: leftG,
            ColorB: leftB,
            ColorA: leftA,

        },
        ebiten.Vertex{
            DstX: x,
            DstY: y + height,
            SrcX: 0,
            SrcY: 1,
            ColorR: leftR,
            ColorG: leftG,
            ColorB: leftB,
            ColorA: leftA,
        },
        ebiten.Vertex{
            DstX: x + width,
            DstY: y,
            SrcX: 1,
            SrcY: 0,
            ColorR: rightR,
            ColorG: rightG,
            ColorB: rightB,
            ColorA: rightA,
        },
        ebiten.Vertex{
            DstX: x + width,
            DstY: y + height,
            SrcX: 1,
            SrcY: 1,
            ColorR: rightR,
            ColorG: rightG,
            ColorB: rightB,
            ColorA: rightA,
        },
    }

    buffer.DrawTriangles(vertices[:], []uint16{0, 1, 2, 2, 3, 1}, source, &ebiten.DrawTrianglesOptions{
        ColorScaleMode: ebiten.ColorScaleModePremultipliedAlpha,
    })
}
