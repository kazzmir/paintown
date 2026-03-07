package background

import (
	"fmt"
	"image"
	"image/color"
	"math"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/parsers"
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

// Update advances all background elements by one tick (velocity, animation frames).
func (b *Background) Update() {
	for _, el := range b.Elements {
		el.Update()
	}
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
func (b *Background) LoadFromAST(ast *parsers.File, prefix string, sffData *sff.SFF, airData *air.Data) {
	prefix = strings.ToLower(prefix)

	for _, sec := range ast.Sections {
		lowerName := strings.ToLower(sec.Name)
		if !strings.HasPrefix(lowerName, prefix) || strings.HasSuffix(lowerName, "def") {
			continue
		}

		// Individual element
		common := CommonElement{
			Name:    sec.Name,
			Mask:    true,
			Delta:   [2]float64{1, 1},
			Spacing: [2]int{0, 0},
			Trans:   "none",
			Window:  [4]int{0, 0, 319, 239},
		}
		var elemType string
		var spriteNo [2]int
		var actionNo int

		for _, attr := range sec.Attributes {
			id := strings.ToLower(attr.ID.String())
			switch id {
			case "type":
				elemType = strings.ToLower(getString(attr.Value))
			case "spriteno":
				v := getVector2(attr.Value)
				spriteNo[0] = int(v[0])
				spriteNo[1] = int(v[1])
			case "actionno":
				actionNo = int(getFloat(attr.Value))
			case "start":
				common.Start = getVector2(attr.Value)
			case "delta":
				common.Delta = getVector2(attr.Value)
			case "velocity":
				common.Velocity = getVector2(attr.Value)
			case "mask":
				common.Mask = getBool(attr.Value)
			case "tile":
				v := getVector2(attr.Value)
				common.Tile[0] = int(v[0])
				common.Tile[1] = int(v[1])
			case "tilespacing":
				v := getVector2(attr.Value)
				common.Spacing[0] = int(v[0])
				common.Spacing[1] = int(v[1])
			case "trans":
				common.Trans = strings.ToLower(getString(attr.Value))
			case "alpha":
				v := getVector2(attr.Value)
				common.Alpha[0] = int(v[0])
				common.Alpha[1] = int(v[1])
			case "window":
				common.Window = getVector4(attr.Value)
			case "layerno":
				common.LayerNo = int(getFloat(attr.Value))
			}
		}

		var xscale [2]float64
		var yscale float64
		for _, attr := range sec.Attributes {
			id := strings.ToLower(attr.ID.String())
			if id == "xscale" {
				xscale = getVector2(attr.Value)
			} else if id == "yscale" {
				yscale = getFloat(attr.Value)
			}
		}

		switch elemType {
		case "normal", "":
			common.Type = Normal
			ne := &NormalElement{CommonElement: common}
			if sffData != nil {
				// Find sprite in SFF
				for i := range sffData.Sprites {
					spr := &sffData.Sprites[i]
					if int(spr.GroupNumber) == spriteNo[0] && int(spr.ImageNumber) == spriteNo[1] {
						ne.Sprite = spr
						break
					}
				}
			}
			b.Elements = append(b.Elements, ne)
		case "animation":
			common.Type = Animation
			ae := &AnimationElement{
				CommonElement: common,
				Sprites:       make(map[string]*sff.Sprite),
			}
			if airData != nil {
				if action, ok := airData.Actions[actionNo]; ok {
					ae.Player = air.NewPlayer(action)
					if sffData != nil {
						for _, el := range action.Elements {
							key := fmt.Sprintf("%d-%d", el.Group, el.Image)
							for j := range sffData.Sprites {
								spr := &sffData.Sprites[j]
								if spr.GroupNumber == uint16(el.Group) && spr.ImageNumber == uint16(el.Image) {
									ae.Sprites[key] = spr
									break
								}
							}
						}
					}
				}
			}
			b.Elements = append(b.Elements, ae)
		case "parallax":
			common.Type = Parallax
			pe := &ParallaxElement{
				CommonElement: common,
				XScale:        xscale,
				YScale:        yscale,
			}
			if sffData != nil {
				for i := range sffData.Sprites {
					spr := &sffData.Sprites[i]
					if int(spr.GroupNumber) == spriteNo[0] && int(spr.ImageNumber) == spriteNo[1] {
						pe.Sprite = spr
						break
					}
				}
			}
			b.Elements = append(b.Elements, pe)
		}
	}
}

func ResolvePath(path string, motifDir string, dataDir string) string {
	if path == "" {
		return ""
	}
	// 1. Check motif directory
	p := filepath.Join(motifDir, path)
	if _, err := os.Stat(p); err == nil {
		return p
	}
	// 2. Check data/ directory
	p = filepath.Join(dataDir, "data", path)
	if _, err := os.Stat(p); err == nil {
		return p
	}
	// 3. Check MUGEN root (dataDir)
	p = filepath.Join(dataDir, path)
	if _, err := os.Stat(p); err == nil {
		return p
	}
	return path
}

func getFloat(v parsers.Value) float64 {
	if n, ok := v.(parsers.NumberValue); ok {
		return n.Val
	}
	if s, ok := v.(parsers.StringValue); ok {
		val, _ := strconv.ParseFloat(strings.TrimSpace(s.Val), 64)
		return val
	}
	if k, ok := v.(parsers.KeywordValue); ok {
		val, _ := strconv.ParseFloat(strings.TrimSpace(k.Val), 64)
		return val
	}
	return 0
}

func getString(v parsers.Value) string {
	if v == nil {
		return ""
	}
	if s, ok := v.(parsers.StringValue); ok {
		return s.Val
	}
	if k, ok := v.(parsers.KeywordValue); ok {
		return k.Val
	}
	return ""
}

func getBool(v parsers.Value) bool {
	return getFloat(v) != 0
}

func getVector2(v parsers.Value) [2]float64 {
	if list, ok := v.(*parsers.ValueList); ok {
		if len(list.Values) >= 2 {
			return [2]float64{getFloat(list.Values[0]), getFloat(list.Values[1])}
		} else if len(list.Values) == 1 {
			return [2]float64{getFloat(list.Values[0]), 0}
		}
	}
	if v != nil {
		return [2]float64{getFloat(v), 0}
	}
	return [2]float64{0, 0}
}

func getVector4(v parsers.Value) [4]int {
	if list, ok := v.(*parsers.ValueList); ok {
		var res [4]int
		for i := 0; i < 4 && i < len(list.Values); i++ {
			res[i] = int(getFloat(list.Values[i]))
		}
		return res
	}
	if v != nil {
		return [4]int{int(getFloat(v)), 0, 0, 0}
	}
	return [4]int{0, 0, 0, 0}
}
