package paintown

import (
    // "fmt"
    "image"
)

// collision takes an image and creates an optimized set of bounding boxes that bound the non-transparent pixels in the image
type Collision struct {
    Boxes []image.Rectangle
    width int
    height int
}

func MakeCollision(input image.Image) *Collision {
    grid := make(map[image.Point]image.Rectangle)

    // maybe make an in-memory copy of the image first

    boxSize := 3
    pixelRatio := 0.8

    for x := input.Bounds().Min.X; x < input.Bounds().Max.X; x += boxSize {
        for y := input.Bounds().Min.Y; y < input.Bounds().Max.Y; y += boxSize {

            pixelCount := 0

            for x2 := range boxSize {
                for y2 := range boxSize {
                    cx := x + x2
                    cy := y + y2

                    if cx < input.Bounds().Max.X && cy < input.Bounds().Max.Y {
                        _, _, _, a := input.At(cx, cy).RGBA()
                        if a != 0 {
                            pixelCount += 1
                        }
                    }
                }
            }

            if pixelCount >= int(float64(boxSize * boxSize) * pixelRatio) {
                r := image.Rect(x, y, x + boxSize, y + boxSize)

                for x2 := range boxSize {
                    for y2 := range boxSize {
                        cx := x + x2
                        cy := y + y2
                        if cx < input.Bounds().Max.X && cy < input.Bounds().Max.Y {
                            grid[image.Pt(cx, cy)] = r
                        }
                    }
                }
            }

            /*
            _, _, _, a := input.At(x, y).RGBA()
            if a != 0 {
                grid[image.Point{x, y}] = image.Rect(x, y, x + 1, y + 1)
            }
            */
        }
    }

    quit := false
    count := uint64(0)
    for !quit {
        quit = true
        count += 1

        /*
        count -= 1
        if count <= 0 {
            fmt.Printf("too many iterations, quitting\n")
            break
        }
        */

        /*
        fmt.Printf("grid has %v boxes\n", len(grid))
        for p, r := range grid {
            fmt.Printf("box at %v: %v\n", p, r)
        }
        */

        for y := input.Bounds().Min.Y; y < input.Bounds().Max.Y; y += 1 {
            for x := input.Bounds().Min.X; x < input.Bounds().Max.X; x += 1 {
                p := image.Point{x, y}

                if r, ok := grid[p]; ok {

                    p1 := image.Pt(r.Max.X, y)
                    p2 := image.Pt(x, r.Max.Y)

                    for _, pMerge := range []image.Point{p1, p2} {

                        // have box at p, try to merge with box to the right
                        // both boxes must have the same height to merge
                        if r2, ok := grid[pMerge]; ok {

                            if r != r2 && r.Bounds().Dy() == r2.Bounds().Dy() {

                                // fmt.Printf("merging %v and %v\n", r, r2)

                                r = r.Union(r2)

                                // fmt.Printf("  merged box is %v\n", r)

                                for x2 := r.Min.X; x2 < r2.Max.X; x2 += 1 {
                                    for y2 := r.Min.Y; y2 < r.Max.Y; y2 += 1 {
                                        grid[image.Pt(x2, y2)] = r
                                    }
                                }

                                quit = false
                            }
                        }
                    }
                }
            }
        }
    }

    // fmt.Printf("merged boxes in %v iterations\n", count)

    var boxes []image.Rectangle
    for _, r := range grid {
        boxes = append(boxes, r)
    }
    
    return &Collision{
        Boxes: boxes,
        width: input.Bounds().Dx(),
        height: input.Bounds().Dy(),
    }
}

func (collision *Collision) Intersect(x float64, y float64, rect image.Rectangle, flip bool) bool {

    point := image.Pt(int(x) - collision.width / 2, int(y) - collision.height)

    for _, box := range collision.Boxes {
        if box.Add(point).Overlaps(rect) {
            return true
        }
    }

    return false
}
