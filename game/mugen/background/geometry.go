package background

import (
	"image"
	"math"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
)

// tileRange intersects the repeated source footprint with the visible surface.
// Infinite tiling follows the camera instead of stopping after a fixed count.
func tileRange(count int, origin, low, high, stride, visibleLow, visibleHigh float64) (int, int) {
	if visibleHigh <= origin+low && count == 0 || visibleLow >= origin+high && count == 0 {
		return 0, -1
	}
	if count == 0 || stride == 0 || math.IsNaN(stride) || math.IsInf(stride, 0) {
		return 0, 0
	}
	a, b := (visibleLow-origin-high)/stride, (visibleHigh-origin-low)/stride
	first, last := int(math.Floor(min(a, b))), int(math.Ceil(max(a, b)))
	if count > 1 {
		first, last = max(0, first), min(count-1, last)
	}
	return first, last
}

func geometryBounds(g ebiten.GeoM, w, h float64) (left, top, right, bottom float64) {
	left, top, right, bottom = math.Inf(1), math.Inf(1), math.Inf(-1), math.Inf(-1)
	for _, point := range [4][2]float64{{0, 0}, {w, 0}, {0, h}, {w, h}} {
		x, y := g.Apply(point[0], point[1])
		left, top, right, bottom = min(left, x), min(top, y), max(right, x), max(bottom, y)
	}
	return
}

func (e *CommonElement) clipTarget(screen *ebiten.Image, cameraX, cameraY, floor float64) *ebiten.Image {
	if !e.windowSet && e.Window == [4]int{} {
		return screen
	}
	left, top, right, bottom := float64(e.Window[0]), float64(e.Window[1]), float64(e.Window[2]+1), float64(e.Window[3]+1)
	var g ebiten.GeoM
	if e.maskWindow {
		e.stageGeometry(&g, float64(screen.Bounds().Dx())/2)
	}
	g.Translate(-cameraX*e.WindowDelta[0], -cameraY*e.WindowDelta[1])
	if e.maskWindow {
		e.zoomGeometry(&g, float64(screen.Bounds().Dx())/2, floor)
	}
	left, top = g.Apply(left, top)
	right, bottom = g.Apply(right, bottom)
	rect := image.Rect(int(min(left, right)), int(min(top, bottom)), int(max(left, right)), int(max(top, bottom))).Intersect(screen.Bounds())
	if rect.Empty() {
		return nil
	}
	return screen.SubImage(rect).(*ebiten.Image)
}

func (e *CommonElement) drawWithBlending(screen *ebiten.Image, img *ebiten.Image, xAxis, yAxis int, cameraX, cameraY, floor float64, trans string, alpha [2]int, parallax *ParallaxElement, visuals ...air.Visual) {
	if img == nil || !e.Visible || !e.Enabled {
		return
	}
	target := e.clipTarget(screen, cameraX, cameraY, floor)
	if target == nil {
		return
	}
	center := float64(screen.Bounds().Dx()) / 2
	var view ebiten.GeoM
	e.stageGeometry(&view, center)
	e.zoomGeometry(&view, center, floor)
	if !view.IsInvertible() {
		return
	}
	inverse := view
	inverse.Invert()
	bounds := target.Bounds()
	left, top := inverse.Apply(float64(bounds.Min.X), float64(bounds.Min.Y))
	right, bottom := inverse.Apply(float64(bounds.Max.X), float64(bounds.Max.Y))
	left, top, right, bottom = min(left, right), min(top, bottom), max(left, right), max(top, bottom)
	if e.stageScale != [2]float64{} {
		cameraX, cameraY = cameraX/e.stageScale[0], cameraY/e.stageScale[1]
	}
	startX, startY := e.GetStart()
	offX, offY := e.GetOffset()
	deltaX, deltaY := e.GetDelta()
	baseX, baseY := center+startX+e.CurrentPos[0]+offX-cameraX*deltaX, startY+e.CurrentPos[1]+offY-cameraY*deltaY
	sX, sY := e.ScaleStart[0]+e.ScaleDelta[0]*cameraX, e.ScaleStart[1]+e.ScaleDelta[1]*cameraY
	if !e.scaleSet && e.ScaleStart == [2]float64{} && e.ScaleDelta == [2]float64{} {
		sX, sY = 1, 1
	}
	if parallax != nil && parallax.legacyYScale {
		sY = 1 / (parallax.YScaleStart/100 + parallax.YScaleDelta/100*cameraY)
	}
	if sX == 0 || sY == 0 || math.IsNaN(sX) || math.IsNaN(sY) || math.IsInf(sX, 0) || math.IsInf(sY, 0) {
		return
	}
	flipX, flipY := 1.0, 1.0
	if strings.Contains(e.Flip, "H") {
		flipX = -1
	}
	if strings.Contains(e.Flip, "V") {
		flipY = -1
	}
	visual := air.Visual{ScaleX: 1, ScaleY: 1}
	if len(visuals) > 0 {
		visual = visuals[0]
	}
	w, h := img.Size()
	frameGeometry := func(pivotX, row float64) ebiten.GeoM {
		var g ebiten.GeoM
		g.Translate(-pivotX, -float64(yAxis)+row)
		g.Scale(visual.ScaleX, visual.ScaleY)
		// The beta leaves angles on animated parallax undefined.
		if parallax == nil {
			g.Rotate(-visual.Angle * math.Pi / 180)
		}
		g.Translate(visual.X, visual.Y)
		return g
	}
	drawTiles := func(source *ebiten.Image, g ebiten.GeoM, originX, originY, strideX, strideY float64, tileX, tileY int) {
		w, h := source.Size()
		x0, y0, x1, y1 := geometryBounds(g, float64(w), float64(h))
		minX, maxX := tileRange(tileX, originX, x0, x1, strideX, left, right)
		minY, maxY := tileRange(tileY, originY, y0, y1, strideY, top, bottom)
		for tx := minX; tx <= maxX; tx++ {
			for ty := minY; ty <= maxY; ty++ {
				op := &ebiten.DrawImageOptions{GeoM: g, Filter: ebiten.FilterNearest}
				op.GeoM.Translate(originX+float64(tx)*strideX, originY+float64(ty)*strideY)
				op.GeoM.Concat(view)
				e.drawPaletteImage(target, source, op, trans, alpha)
			}
		}
	}
	if parallax == nil {
		g := frameGeometry(float64(xAxis), 0)
		g.Scale(sX*flipX, sY*flipY)
		strideX, strideY := float64(w+e.Spacing[0]), float64(h+e.Spacing[1])
		if e.Type == Animation {
			// Animated spacing measures between tile origins, not sprite edges.
			strideX, strideY = float64(e.Spacing[0]), float64(e.Spacing[1])
		}
		drawTiles(img, g, baseX, baseY, strideX*sX, strideY*sY, e.Tile[0], e.Tile[1])
		return
	}
	widthMode := parallax.XScale == [2]float64{} && parallax.Width[0] > 0 && parallax.Width[1] > 0
	for row := 0; row < h; row++ {
		fraction := (float64(row) + .5) / float64(h)
		rowScale, cameraFactor, pivotX := sX, 1.0, float64(xAxis)
		if widthMode {
			width := parallax.Width[0] + (parallax.Width[1]-parallax.Width[0])*fraction
			rowScale *= width / float64(w)
			cameraFactor = width / parallax.Width[0]
			pivotX = float64(w) / 2 // width ignores the authored SFF x-axis.
		} else if parallax.XScale != [2]float64{} {
			// Legacy xscale shears a pre-distorted sprite; it never stretches it.
			cameraFactor = parallax.XScale[0] + (parallax.XScale[1]-parallax.XScale[0])*fraction
		}
		g := frameGeometry(pivotX, float64(row))
		g.Scale(rowScale*flipX, sY*flipY)
		rowX := baseX - cameraX*deltaX*(cameraFactor-1)
		line := img.SubImage(image.Rect(0, row, w, row+1)).(*ebiten.Image)
		// Parallax ignores tilespacing and vertical tiling. Each row repeats
		// by its own width so adjacent trapezoids remain joined as they shear.
		drawTiles(line, g, rowX, baseY, float64(w)*rowScale*visual.ScaleX, 0, e.Tile[0], 0)
	}
}
