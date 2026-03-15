package paintown

import (
    "image"
)

// collision takes an image and creates an optimized set of bounding boxes that bound the non-transparent pixels in the image
type Collision struct {
    Boxes []image.Rectangle
}

func MakeCollision(input image.Image) *Collision {
    grid := make(map[image.Point]image.Rectangle)

    // maybe make an in-memory copy of the image first

    for x := input.Bounds().Min.X; x < input.Bounds().Max.X; x += 1 {
        for y := input.Bounds().Min.Y; y < input.Bounds().Max.Y; y += 1 {
            _, _, _, a := input.At(x, y).RGBA()
            if a != 0 {
                grid[image.Point{x, y}] = image.Rect(x, y, x, y)
            }
        }
    }

    quit := false
    for !quit {
        quit = true

        for x := input.Bounds().Min.X; x < input.Bounds().Max.X; x += 1 {
            for y := input.Bounds().Min.Y; y < input.Bounds().Max.Y; y += 1 {
                p := image.Point{x, y}

                if r, ok := grid[p]; ok {

                    // have box at p, try to merge with box to the right
                    // both boxes must have the same height to merge
                    p2 := image.Pt(r.Max.X + 1, y)
                    if r2, ok := grid[p2]; ok {

                        if r.Bounds().Dy() == r2.Bounds().Dy() {
                            delete(grid, p2)
                            grid[p] = r.Union(r2)
                            quit = false
                        }
                    }

                    // try to merge with box below
                    p2 = image.Pt(x, r.Max.Y + 1)
                    if r2, ok := grid[p2]; ok {
                        if r.Bounds().Dx() == r2.Bounds().Dx() {
                            delete(grid, p2)
                            grid[p] = r.Union(r2)
                            quit = false
                        }
                    }
                }
            }
        }
    }

    var boxes []image.Rectangle
    for _, r := range grid {
        boxes = append(boxes, r)
    }
    
    return &Collision{Boxes: boxes}
}
