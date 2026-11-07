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

    MainBox image.Rectangle
}

func MakeCollision(input image.Image) *Collision {
    bounds := input.Bounds()
    boxSize := 3
    pixelRatio := 0.8
    opaque := func(x, y int) bool {
        _, _, _, a := input.At(x, y).RGBA()
        return a != 0
    }
    if pixels, ok := input.(*image.NRGBA); ok {
        opaque = func(x, y int) bool { return pixels.Pix[pixels.PixOffset(x, y) + 3] != 0 }
    } else if pixels, ok := input.(*image.RGBA); ok {
        opaque = func(x, y int) bool { return pixels.Pix[pixels.PixOffset(x, y) + 3] != 0 }
    }

    // Dense storage avoids hashing a point for every pixel on every merge
    // pass. Pad to whole cells because the original boxes include edge cells
    // extending up to two pixels beyond the image bounds.
    width := (bounds.Dx() + boxSize - 1) / boxSize * boxSize
    height := (bounds.Dy() + boxSize - 1) / boxSize * boxSize
    grid := make([]image.Rectangle, width * height)
    get := func(p image.Point) (image.Rectangle, bool) {
        x, y := p.X - bounds.Min.X, p.Y - bounds.Min.Y
        if x < 0 || y < 0 || x >= width || y >= height {
            return image.Rectangle{}, false
        }
        r := grid[y * width + x]
        return r, !r.Empty()
    }
    set := func(x, y int, r image.Rectangle) {
        grid[(y - bounds.Min.Y) * width + x - bounds.Min.X] = r
    }

    for x := input.Bounds().Min.X; x < input.Bounds().Max.X; x += boxSize {
        for y := input.Bounds().Min.Y; y < input.Bounds().Max.Y; y += boxSize {

            pixelCount := 0

            for x2 := range boxSize {
                for y2 := range boxSize {
                    cx := x + x2
                    cy := y + y2

                    if cx < input.Bounds().Max.X && cy < input.Bounds().Max.Y {
                        if opaque(cx, cy) {
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
                            set(cx, cy, r)
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

                if r, ok := get(p); ok {

                    p1 := image.Pt(r.Max.X, y)
                    p2 := image.Pt(x, r.Max.Y)

                    for _, pMerge := range []image.Point{p1, p2} {

                        // have box at p, try to merge with box to the right
                        // both boxes must have the same height to merge
                        if r2, ok := get(pMerge); ok {

                            if r != r2 && r.Bounds().Dy() == r2.Bounds().Dy() {

                                // fmt.Printf("merging %v and %v\n", r, r2)

                                r = r.Union(r2)

                                // fmt.Printf("  merged box is %v\n", r)

                                for x2 := r.Min.X; x2 < r2.Max.X; x2 += 1 {
                                    for y2 := r.Min.Y; y2 < r.Max.Y; y2 += 1 {
                                        set(x2, y2, r)
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

    mainBox := image.Rectangle{}

    // A merged rectangle is stored at every pixel it covers. Keep just one
    // copy for collision checks instead of thousands of duplicate boxes.
    seen := make(map[image.Rectangle]bool)
    var boxes []image.Rectangle
    for _, r := range grid {
        if r.Empty() || seen[r] {
            continue
        }
        seen[r] = true
        boxes = append(boxes, r)
        mainBox = mainBox.Union(r)
    }
    
    return &Collision{
        Boxes: boxes,
        width: input.Bounds().Dx(),
        height: input.Bounds().Dy(),
        MainBox: mainBox,
    }
}

func flipRect(r image.Rectangle) image.Rectangle {
    return image.Rect(-r.Max.X, r.Min.Y, -r.Min.X, r.Max.Y).Canon()
}

// x and y are the world coordinates of the center of the bottom of the collision box
// rect is the rectangle to check for intersection with the collision boxes
// flip is whether to flip the collision boxes horizontally
func (collision *Collision) Intersect(x float64, y float64, rect image.Rectangle, flip bool) bool {
    center := image.Pt(-collision.width / 2, -collision.height)

    worldPoint := image.Pt(int(x), int(y))

    main := collision.MainBox.Add(center)

    if flip {
        main = flipRect(main)
    }

    // fmt.Printf("check collision between main %v and rect %v\n", main.Add(worldPoint), rect)

    if !main.Add(worldPoint).Overlaps(rect) {
        return false
    }

    for _, box := range collision.Boxes {
        b2 := box.Add(center)
        if flip {
            b2 = flipRect(b2)
        }

        b2 = b2.Add(worldPoint)

        if b2.Overlaps(rect) {
            return true
        }
    }

    return false
}
