package background

import (
	"fmt"
	"image"
	"image/color"
	"math"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

type SpriteImages struct {
	Masked *ebiten.Image
	Opaque *ebiten.Image
}

type ElementType int

const (
	Normal ElementType = iota
	Animation
	Parallax
)

type Background struct {
	Elements     []Element
	BGClearColor color.Color
}

type Element interface {
	Update()
	Draw(screen *ebiten.Image, cameraX, cameraY float64, sprites map[string]*SpriteImages)
	GetLayerNo() int
}

type CommonElement struct {
	Name     string
	Type     ElementType
	ID       int
	LayerNo  int
	Start    [2]float64
	Delta    [2]float64
	Mask     bool
	Tile     [2]int
	Spacing  [2]int
	Window   [4]int // x1, y1, x2, y2
	Velocity [2]float64
	Trans    string // "none", "add", "add1", "sub", "addalpha"
	Alpha    [2]int // s, d

	// Internal state
	CurrentPos [2]float64
}

func (e *CommonElement) Update() {
	e.CurrentPos[0] += e.Velocity[0]
	e.CurrentPos[1] += e.Velocity[1]
}

func (e *CommonElement) GetLayerNo() int {
	return e.LayerNo
}

func (e *CommonElement) Draw(screen *ebiten.Image, cameraX, cameraY float64, sprites map[string]*SpriteImages) {
	// Base common element does nothing by itself
}

func (e *CommonElement) applyBlending(op *ebiten.DrawImageOptions, trans string, alpha [2]int) {
	switch trans {
	case "add":
		op.Blend = ebiten.BlendLighter
	case "add1":
		// Src + Dst * 0.5
		// Approximation: scale source down and use lighter
		op.ColorScale.Scale(0.5, 0.5, 0.5, 1.0)
		op.Blend = ebiten.BlendLighter
	case "sub":
		op.Blend = ebiten.Blend{
			BlendOperationRGB:           ebiten.BlendOperationReverseSubtract,
			BlendOperationAlpha:         ebiten.BlendOperationAdd,
			BlendFactorSourceRGB:        ebiten.BlendFactorSourceAlpha,
			BlendFactorDestinationRGB:   ebiten.BlendFactorOne,
			BlendFactorSourceAlpha:      ebiten.BlendFactorOne,
			BlendFactorDestinationAlpha: ebiten.BlendFactorOne,
		}
	case "addalpha":
		if alpha[0] > 0 || alpha[1] > 0 {
			s := float32(alpha[0]) / 256.0
			// MUGEN addalpha: Src * s + Dst * d
			// Ebiten Blend can do Src * SrcFactor + Dst * DstFactor
			// We can use ColorScale for s, and Blend for factors.
			op.ColorScale.Scale(s, s, s, s)

			// If we want Dst * d, we need something that multiplies Dst.
			// Standard ebiten.BlendFactor for Dst usually doesn't have a constant 'd'.
			// But if d is 1-s, we can use BlendFactorOneMinusSourceAlpha.
			// For generic d, it's hard. We'll approximate with Lighter if d=1, or just standard blending.
			d := float32(alpha[1]) / 256.0
			if d == 1.0 {
				op.Blend = ebiten.BlendLighter
			} else {
				// Approximation: just use source-over but with our scaled source.
				// This isn't perfect for additive alpha but better than nothing.
				op.Blend = ebiten.BlendSourceOver
			}
		}
	}
}

type NormalElement struct {
	CommonElement
	Sprite *sff.Sprite
}

func (e *NormalElement) Update() {
	e.CommonElement.Update()
}

func (e *NormalElement) Draw(screen *ebiten.Image, cameraX, cameraY float64, sprites map[string]*SpriteImages) {
	if e.Sprite == nil {
		return
	}

	key := fmt.Sprintf("%d-%d", e.Sprite.GroupNumber, e.Sprite.ImageNumber)
	si, ok := sprites[key]
	if !ok {
		return
	}

	img := si.Masked
	if !e.Mask {
		img = si.Opaque
	}

	e.CommonElement.draw(screen, img, int(e.Sprite.XAxis), int(e.Sprite.YAxis), cameraX, cameraY)
}

type AnimationElement struct {
	CommonElement
	Player  *air.Player
	Sprites map[string]*sff.Sprite
}

func (e *AnimationElement) Update() {
	e.CommonElement.Update()
	if e.Player != nil {
		e.Player.Update()
	}
}

func (e *AnimationElement) Draw(screen *ebiten.Image, cameraX, cameraY float64, sprites map[string]*SpriteImages) {
	if e.Player == nil {
		return
	}

	el := e.Player.CurrentElement()
	if el == nil {
		return
	}

	key := fmt.Sprintf("%d-%d", el.Group, el.Image)
	si, ok := sprites[key]
	if !ok {
		return
	}

	// Calculate pivot point
	pivotX, pivotY := 0, 0
	if e.Sprites != nil {
		if spr, ok := e.Sprites[key]; ok {
			pivotX = int(spr.XAxis)
			pivotY = int(spr.YAxis)
		}
	}

	// Determine blending for this frame
	trans := e.Trans
	alpha := e.Alpha

	if el.Flags != "" {
		f := strings.ToLower(el.Flags)
		if strings.Contains(f, "a1") {
			trans = "add1"
		} else if strings.Contains(f, "a") {
			if strings.Contains(f, "as") {
				// Custom alpha: ASxxxDyyy
				trans = "addalpha"
				// MUGEN format can be AS256D128
				// We need a regex or simple parser
				s, d := 256, 256
				fmt.Sscanf(f, "as%dd%d", &s, &d)
				alpha = [2]int{s, d}
			} else {
				trans = "add"
			}
		} else if strings.Contains(f, "s") {
			trans = "sub"
		}
	}

	img := si.Masked
	if !e.Mask {
		img = si.Opaque
	}

	e.CommonElement.drawWithBlending(screen, img, pivotX-el.XOffset, pivotY-el.YOffset, cameraX, cameraY, trans, alpha)
}

func (e *CommonElement) drawWithBlending(screen *ebiten.Image, img *ebiten.Image, xAxis, yAxis int, cameraX, cameraY float64, trans string, alpha [2]int) {
	if img == nil {
		return
	}

	w, h := img.Size()
	drawX := 160.0 + e.Start[0] + e.CurrentPos[0] - cameraX*e.Delta[0]
	drawY := e.Start[1] + e.CurrentPos[1] - cameraY*e.Delta[1]

	originX := drawX - float64(xAxis)
	originY := drawY - float64(yAxis)

	// Determine tiling range
	screenW := float64(screen.Bounds().Dx())
	screenH := float64(screen.Bounds().Dy())

	tileWidth := float64(w) + float64(e.Spacing[0])
	tileHeight := float64(h) + float64(e.Spacing[1])

	tileX, tileY := e.Tile[0], e.Tile[1]
	if e.Type == Parallax {
		tileY = 0
	}

	minX, maxX := 0, 0
	minY, maxY := 0, 0

	if tileX == 1 {
		leftLimit := -originX
		rightLimit := screenW - originX
		minX = int(math.Floor(leftLimit / tileWidth))
		maxX = int(math.Ceil(rightLimit / tileWidth))
	} else if tileX > 1 {
		minX = 0
		maxX = int(tileX) - 1
	}

	if tileY == 1 {
		topLimit := -originY
		bottomLimit := screenH - originY
		minY = int(math.Floor(topLimit / tileHeight))
		maxY = int(math.Ceil(bottomLimit / tileHeight))
	} else if tileY > 1 {
		minY = 0
		maxY = int(tileY) - 1
	}

	target := screen
	if e.Window[0] != 0 || e.Window[1] != 0 || e.Window[2] != 0 || e.Window[3] != 0 {
		// MUGEN window clipping
		rect := image.Rect(e.Window[0], e.Window[1], e.Window[2]+1, e.Window[3]+1)
		target = screen.SubImage(rect).(*ebiten.Image)
	}

	for ty := minY; ty <= maxY; ty++ {
		for tx := minX; tx <= maxX; tx++ {
			xPos := originX + float64(tx)*tileWidth
			yPos := originY + float64(ty)*tileHeight

			op := &ebiten.DrawImageOptions{}
			op.Filter = ebiten.FilterNearest
			op.GeoM.Translate(xPos, yPos)
			e.applyBlending(op, trans, alpha)
			target.DrawImage(img, op)
		}
	}
}

func (e *CommonElement) draw(screen *ebiten.Image, img *ebiten.Image, xAxis, yAxis int, cameraX, cameraY float64) {
	e.drawWithBlending(screen, img, xAxis, yAxis, cameraX, cameraY, e.Trans, e.Alpha)
}

type ParallaxElement struct {
	CommonElement
	Sprite *sff.Sprite
	XScale [2]float64
	YScale float64
}

func (e *ParallaxElement) Update() {
	e.CommonElement.Update()
}

func (e *ParallaxElement) Draw(screen *ebiten.Image, cameraX, cameraY float64, sprites map[string]*SpriteImages) {
	if e.Sprite == nil {
		return
	}

	key := fmt.Sprintf("%d-%d", e.Sprite.GroupNumber, e.Sprite.ImageNumber)
	si, ok := sprites[key]
	if !ok {
		return
	}

	img := si.Masked
	if !e.Mask {
		img = si.Opaque
	}

	// Parallax is similar to normal but ignores Y tiling usually and applies X scaling based on vertical position
	// For now, let's treat it as a normal element until we implement the complex XScale logic.
	e.CommonElement.draw(screen, img, int(e.Sprite.XAxis), int(e.Sprite.YAxis), cameraX, cameraY)
}
