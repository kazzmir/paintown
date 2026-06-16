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
	"github.com/kazzmir/paintown/game/mugen/logger"
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
	Controllers  []*BGCtrlDef
}

// Update advances all background elements by one tick (velocity, animation frames).
func (b *Background) Update() {
	for _, ctrl := range b.Controllers {
		ctrl.Update(b.Elements)
	}
	for _, el := range b.Elements {
		el.Update()
	}
}

type Element interface {
	Update()
	Draw(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages)
	GetLayerNo() int
	GetID() int
	SetVisible(v bool)
	SetEnabled(v bool)
	SetVelocityX(x float64)
	SetVelocityY(y float64)
	AddVelocityX(x float64)
	AddVelocityY(y float64)
	SetPositionX(x float64)
	SetPositionY(y float64)
	AddPositionX(x float64)
	AddPositionY(y float64)
	SetAction(action int)
	SetSinX(v [3]float64)
	SetSinY(v [3]float64)
	GetPosition() (float64, float64)
	GetStart() (float64, float64)
	GetOffset() (float64, float64)
	GetVelocity() (float64, float64)
	GetDelta() (float64, float64)
	GetLink() Element
	GetName() string // Added GetName to the Element interface
	DrawReflection(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages, intensity int)
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
	Visible  bool
	Enabled  bool
	Flip     string // "H", "V", "VH"

	// Sinusoidal movement
	SinX [3]float64 // amplitude, period, phase
	SinY [3]float64 // amplitude, period, phase

	// Internal state
	CurrentPos   [2]float64
	Time         int // ticks since start
	PositionLink bool
	Link         Element
}

func (e *CommonElement) Update() {
	if !e.Enabled {
		return
	}
	e.CurrentPos[0] += e.Velocity[0]
	e.CurrentPos[1] += e.Velocity[1]
	e.Time++
}

func (e *CommonElement) GetOffset() (float64, float64) {
	offX, offY := 0.0, 0.0
	if e.SinX[1] > 0 {
		// MUGEN phase is in degrees
		phase := e.SinX[2] * math.Pi / 180.0
		offX = e.SinX[0] * math.Sin(2.0*math.Pi*float64(e.Time)/e.SinX[1]+phase)
	}
	if e.SinY[1] > 0 {
		phase := e.SinY[2] * math.Pi / 180.0
		offY = e.SinY[0] * math.Sin(2.0*math.Pi*float64(e.Time)/e.SinY[1]+phase)
	}

	if e.PositionLink && e.Link != nil {
		lx, ly := e.Link.GetPosition()
		plx, ply := e.Link.GetOffset()
		offX += lx + plx
		offY += ly + ply
	}

	return offX, offY
}
func (e *CommonElement) GetPosition() (float64, float64) {
	return e.CurrentPos[0], e.CurrentPos[1]
}

func (e *CommonElement) GetStart() (float64, float64) {
	return e.Start[0], e.Start[1]
}

func (e *CommonElement) GetLayerNo() int {
	return e.LayerNo
}

func (e *CommonElement) GetVelocity() (float64, float64) {
	return e.Velocity[0], e.Velocity[1]
}

func (e *CommonElement) GetName() string {
	return e.Name
}

func (e *CommonElement) GetDelta() (float64, float64) {
	return e.Delta[0], e.Delta[1]
}

func (e *CommonElement) GetLink() Element {
	return e.Link
}

func (e *CommonElement) GetID() int {
	return e.ID
}

func (e *CommonElement) SetVisible(v bool) {
	e.Visible = v
}

func (e *CommonElement) SetEnabled(v bool) {
	e.Enabled = v
}

func (e *CommonElement) SetVelocityX(x float64) {
	e.Velocity[0] = x
}

func (e *CommonElement) SetVelocityY(y float64) {
	e.Velocity[1] = y
}

func (e *CommonElement) AddVelocityX(x float64) {
	e.Velocity[0] += x
}

func (e *CommonElement) AddVelocityY(y float64) {
	e.Velocity[1] += y
}

func (e *CommonElement) SetPositionX(x float64) {
	e.CurrentPos[0] = x
}

func (e *CommonElement) SetPositionY(y float64) {
	e.CurrentPos[1] = y
}

func (e *CommonElement) AddPositionX(x float64) {
	e.CurrentPos[0] += x
}

func (e *CommonElement) AddPositionY(y float64) {
	e.CurrentPos[1] += y
}

func (e *CommonElement) SetAction(action int) {
	// Base element has no action
}

func (e *CommonElement) SetSinX(v [3]float64) {
	e.SinX = v
}

func (e *CommonElement) SetSinY(v [3]float64) {
	e.SinY = v
}

func (e *CommonElement) Draw(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages) {
	// Base common element does nothing by itself
}

func (e *CommonElement) applyBlending(op *ebiten.DrawImageOptions, trans string, alpha [2]int) {
	t := strings.ToLower(trans)
	if t == "default" {
		if e.Type == Animation {
			// For animation, 'default' means use AIR settings.
			t = "none"
		} else {
			t = "none"
		}
	}

	switch t {
	case "add":
		op.Blend = ebiten.BlendLighter
	case "add1":
		op.ColorScale.Scale(0.5, 0.5, 0.5, 1.0)
		op.Blend = ebiten.BlendLighter
	case "sub":
		op.Blend = ebiten.Blend{
			BlendOperationRGB:           ebiten.BlendOperationReverseSubtract,
			BlendOperationAlpha:         ebiten.BlendOperationAdd,
			BlendFactorSourceRGB:        ebiten.BlendFactorOne,
			BlendFactorDestinationRGB:   ebiten.BlendFactorOne,
			BlendFactorSourceAlpha:      ebiten.BlendFactorZero,
			BlendFactorDestinationAlpha: ebiten.BlendFactorOne,
		}
	case "addalpha":
		if alpha[0] > 0 || alpha[1] > 0 {
			s := float32(alpha[0]) / 256.0
			op.ColorScale.Scale(s, s, s, s)
			d := float32(alpha[1]) / 256.0
			if d == 1.0 {
				op.Blend = ebiten.BlendLighter
			} else {
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

func (e *NormalElement) DrawReflection(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages, intensity int) {
	// Stub
}

func (e *NormalElement) Draw(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages) {
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

	e.draw(screen, img, int(e.Sprite.XAxis), int(e.Sprite.YAxis), cameraX, cameraY, zOffset)
}

type AnimationElement struct {
	CommonElement
	Player  *air.Player
	Sprites map[string]*sff.Sprite
	AirData *air.Data
	SffData *sff.SFF
}

func (e *AnimationElement) Update() {
	e.CommonElement.Update()
	if e.Player != nil {
		e.Player.Update()
	}
}

func (e *AnimationElement) SetAction(actionNo int) {
	if e.AirData == nil {
		return
	}
	if action, ok := e.AirData.Actions[actionNo]; ok {
		e.Player = air.NewPlayer(action)
		if e.SffData != nil {
			for _, el := range action.Elements {
				key := fmt.Sprintf("%d-%d", el.Group, el.Image)
				if _, exists := e.Sprites[key]; !exists {
					for j := range e.SffData.Sprites {
						spr := &e.SffData.Sprites[j]
						if spr.GroupNumber == uint16(el.Group) && spr.ImageNumber == uint16(el.Image) {
							e.Sprites[key] = spr
							break
						}
					}
				}
			}
		}
	}
}

func (e *AnimationElement) DrawReflection(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages, intensity int) {
	// Stub
}

func (e *AnimationElement) Draw(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages) {
	if e.Player == nil {
		return
	}

	el := e.Player.CurrentElement()
	if el == nil || el.Group == -1 {
		return
	}

	key := fmt.Sprintf("%d-%d", el.Group, el.Image)
	si, ok := sprites[key]
	if !ok {
		return
	}

	pivotX, pivotY := 0, 0
	if e.Sprites != nil {
		if spr, ok := e.Sprites[key]; ok {
			pivotX = int(spr.XAxis)
			pivotY = int(spr.YAxis)
		}
	}

	trans := e.Trans
	alpha := e.Alpha

	if el.Flags != "" {
		f := strings.ToLower(el.Flags)
		if strings.Contains(f, "a1") {
			trans = "add1"
		} else if strings.Contains(f, "a") {
			if strings.Contains(f, "as") {
				trans = "addalpha"
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

	totalX := pivotX - el.XOffset
	totalY := pivotY - el.YOffset

	// Interpolation
	factor := e.Player.GetInterpolationFactor()
	if factor > 0 {
		next := e.Player.NextElement()
		if next != nil {
			// Interpolate position relative to pivots
			totalX = int(float64(totalX) + float64(el.XOffset-next.XOffset)*factor)
			totalY = int(float64(totalY) + float64(el.YOffset-next.YOffset)*factor)
		}
	}

	// Combine flips
	oldFlip := e.Flip
	if el.FlipX {
		if strings.Contains(e.Flip, "H") {
			e.Flip = strings.ReplaceAll(e.Flip, "H", "")
		} else {
			e.Flip += "H"
		}
	}
	if el.FlipY {
		if strings.Contains(e.Flip, "V") {
			e.Flip = strings.ReplaceAll(e.Flip, "V", "")
		} else {
			e.Flip += "V"
		}
	}

	e.drawWithBlending(screen, img, totalX, totalY, cameraX, cameraY, zOffset, trans, alpha, nil)
	e.Flip = oldFlip
}

func (e *CommonElement) drawWithBlending(screen *ebiten.Image, img *ebiten.Image, xAxis int, yAxis int, cameraX, cameraY, zOffset float64, trans string, alpha [2]int, parallax *ParallaxElement) {
	if img == nil || !e.Visible {
		return
	}

	w, h := img.Size()
	offX, offY := e.GetOffset()

	// Coordinate alignment
	baseDrawX := 160.0 + e.Start[0] + e.CurrentPos[0] + offX - cameraX*e.Delta[0]
	baseDrawY := e.Start[1] + e.CurrentPos[1] + offY - cameraY*e.Delta[1]

	flipX, flipY := 1.0, 1.0
	if strings.Contains(e.Flip, "H") {
		flipX = -1.0
	}
	if strings.Contains(e.Flip, "V") {
		flipY = -1.0
	}

	tileX, tileY := e.Tile[0], e.Tile[1]
	if parallax != nil {
		tileY = 0
	}

	tileWidth := float64(w) + float64(e.Spacing[0])
	tileHeight := float64(h) + float64(e.Spacing[1])

	winX, winY := 0.0, 0.0
	target := screen
	if e.Window[0] != 0 || e.Window[1] != 0 || e.Window[2] != 0 || e.Window[3] != 0 {
		rect := image.Rect(e.Window[0], e.Window[1], e.Window[2]+1, e.Window[3]+1)
		target = screen.SubImage(rect).(*ebiten.Image)
		winX = float64(e.Window[0])
		winY = float64(e.Window[1])
	}

	minTX, maxTX := 0, 0
	if tileX == 1 {
		minTX, maxTX = -20, 20
	} else if tileX > 1 {
		maxTX = int(tileX) - 1
	}

	minTY, maxTY := 0, 0
	if tileY == 1 {
		minTY, maxTY = -10, 10
	} else if tileY > 1 {
		maxTY = int(tileY) - 1
	}

	for tx := minTX; tx <= maxTX; tx++ {
		for ty := minTY; ty <= maxTY; ty++ {
			tilePosX := float64(tx) * tileWidth
			tilePosY := float64(ty) * tileHeight

			sX, sY := 1.0, 1.0
			if parallax != nil {
				if parallax.ScaleStart[1] != 0 {
					sY = parallax.ScaleStart[1] + parallax.ScaleDelta[1]*cameraY
				} else if parallax.YScaleStart != 0 {
					sY = 1.0 / (parallax.YScaleStart/100.0 + parallax.YScaleDelta/100.0*cameraY)
				}
				if parallax.Width[0] != 0 && w > 0 {
					sX = parallax.Width[0] / float64(w)
				} else if parallax.XScale[0] != 0 {
					sX = parallax.XScale[0]
				}
			}

			// Determine if we need scanline parallax
			isScanlineParallax := false
			topScale := sX
			bottomScale := sX
			if parallax != nil {
				if parallax.XScale[0] != 0 && parallax.XScale[1] != 0 && parallax.XScale[0] != parallax.XScale[1] {
					isScanlineParallax = true
					topScale = parallax.XScale[0]
					bottomScale = parallax.XScale[1]
				} else if parallax.Width[0] != 0 && parallax.Width[1] != 0 && w > 0 && parallax.Width[0] != parallax.Width[1] {
					isScanlineParallax = true
					topScale = parallax.Width[0] / float64(w)
					bottomScale = parallax.Width[1] / float64(w)
				}
			}

			if isScanlineParallax {
				for y := 0; y < h; y++ {
					var op ebiten.DrawImageOptions
					op.Filter = ebiten.FilterNearest

					sX_line := topScale + (bottomScale-topScale)*(float64(y)/float64(h-1))
					lineDrawX := 160.0 + e.Start[0] + e.CurrentPos[0] + offX - cameraX*(e.Delta[0]*sX_line)
					lineDrawY := e.Start[1] + e.CurrentPos[1] + offY - cameraY*e.Delta[1]

					op.GeoM.Translate(-float64(xAxis), -float64(yAxis)+float64(y))
					op.GeoM.Scale(sX_line*flipX, sY*flipY)
					op.GeoM.Translate(tilePosX, tilePosY)
					op.GeoM.Translate(lineDrawX-winX, lineDrawY-winY)

					e.applyBlending(&op, trans, alpha)
					line := img.SubImage(image.Rect(0, y, w, y+1)).(*ebiten.Image)
					target.DrawImage(line, &op)
				}
			} else {
				var op ebiten.DrawImageOptions
				op.Filter = ebiten.FilterNearest

				op.GeoM.Translate(-float64(xAxis), -float64(yAxis))
				op.GeoM.Scale(sX*flipX, sY*flipY)
				op.GeoM.Translate(tilePosX, tilePosY)
				op.GeoM.Translate(baseDrawX-winX, baseDrawY-winY)

				e.applyBlending(&op, trans, alpha)
				target.DrawImage(img, &op)
			}
		}
	}
}

func (e *CommonElement) draw(screen *ebiten.Image, img *ebiten.Image, xAxis int, yAxis int, cameraX, cameraY, zOffset float64) {
	e.drawWithBlending(screen, img, xAxis, yAxis, cameraX, cameraY, zOffset, e.Trans, e.Alpha, nil)
}

type ParallaxElement struct {
	CommonElement
	Sprite      *sff.Sprite
	XScale      [2]float64
	Width       [2]float64
	ScaleStart  [2]float64
	ScaleDelta  [2]float64
	YScaleStart float64
	YScaleDelta float64
}

func (e *ParallaxElement) Update() {
	e.CommonElement.Update()
}

func (e *ParallaxElement) DrawReflection(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages, intensity int) {
	// Stub
}

func (e *ParallaxElement) Draw(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages) {
	if e.Sprite != nil {
		si, ok := sprites[fmt.Sprintf("%d-%d", e.Sprite.GroupNumber, e.Sprite.ImageNumber)]
		if ok {
			img := si.Masked
			if !e.Mask {
				img = si.Opaque
			}
			e.drawWithBlending(screen, img, int(e.Sprite.XAxis), int(e.Sprite.YAxis), cameraX, cameraY, zOffset, e.Trans, e.Alpha, e)
		}
	}
}

func (b *Background) LoadFromAST(ast *parsers.File, prefix string, sffData *sff.SFF, airData *air.Data) {
	prefix = strings.ToLower(prefix)
	for _, sec := range ast.Sections {
		name := strings.ToLower(sec.Name)
		// Skip known top-level sections
		if name == "info" || name == "camera" || name == "playerinfo" || name == "bound" || name == "stageinfo" || name == "shadow" || name == "music" || name == "scaling" || name == "reflection" || strings.HasPrefix(name, "bgdef") || strings.HasPrefix(name, "bgctrl") {
			continue
		}
		if !strings.HasPrefix(name, prefix) {
			logger.LogUnresolved(fmt.Sprintf("Unhandled Section: [%s]", sec.Name))
		}
	}

	for _, sec := range ast.Sections {
		lowerName := strings.ToLower(sec.Name)
		if !strings.HasPrefix(lowerName, prefix) || strings.HasSuffix(lowerName, "def") {
			continue
		}

		// Guard against empty sections
		if len(sec.Attributes) == 0 {
			continue
		}

		common := CommonElement{
			Name:    sec.Name,
			Mask:    false,
			Delta:   [2]float64{1, 1},
			Spacing: [2]int{0, 0},
			Trans:   "none",
			Window:  [4]int{0, 0, 319, 239},
			Visible: true,
			Enabled: true,
		}
		maskSet := false
		var elemType string
		var spriteNo [2]int
		var actionNo int
		hasSprite := false
		hasAction := false
		consumed := make(map[*parsers.Attribute]bool)

		for _, attr := range sec.Attributes {
			id := strings.ToLower(attr.ID.String())
			switch id {
			case "id":
				common.ID = int(GetFloat(attr.Value))
				consumed[attr] = true
			case "positionlink":
				common.PositionLink = GetBool(attr.Value)
				consumed[attr] = true
			case "type":
				elemType = strings.ToLower(GetString(attr.Value))
				consumed[attr] = true
			case "spriteno":
				v := GetVector2(attr.Value)
				spriteNo[0] = int(v[0])
				spriteNo[1] = int(v[1])
				hasSprite = true
				consumed[attr] = true
			case "actionno":
				actionNo = int(GetFloat(attr.Value))
				hasAction = true
				consumed[attr] = true
			case "start":
				common.Start = GetVector2(attr.Value)
				consumed[attr] = true
			case "delta":
				common.Delta = GetVector2(attr.Value)
				consumed[attr] = true
			case "velocity":
				common.Velocity = GetVector2(attr.Value)
				consumed[attr] = true
			case "mask":
				common.Mask = GetBool(attr.Value)
				maskSet = true
				consumed[attr] = true
			case "tile":
				v := GetVector2(attr.Value)
				common.Tile[0] = int(v[0])
				common.Tile[1] = int(v[1])
				consumed[attr] = true
			case "tilespacing":
				v := GetVector2(attr.Value)
				common.Spacing[0] = int(v[0])
				common.Spacing[1] = int(v[1])
				consumed[attr] = true
			case "trans":
				common.Trans = strings.ToLower(GetString(attr.Value))
				consumed[attr] = true
			case "alpha":
				v := GetVector2(attr.Value)
				common.Alpha[0] = int(v[0])
				common.Alpha[1] = int(v[1])
				consumed[attr] = true
			case "window":
				common.Window = GetVector4(attr.Value)
				consumed[attr] = true
			case "layerno":
				common.LayerNo = int(GetFloat(attr.Value))
				consumed[attr] = true
			case "sin.x":
				v := GetVector3(attr.Value)
				common.SinX = [3]float64{v[0], v[1], v[2]}
				consumed[attr] = true
			case "sin.y":
				v := GetVector3(attr.Value)
				common.SinY = [3]float64{v[0], v[1], v[2]}
				consumed[attr] = true
			case "flip":
				common.Flip = strings.ToUpper(GetString(attr.Value))
				consumed[attr] = true
			}
		}

		var rootElement Element
		for i := len(b.Elements) - 1; i >= 0; i-- {
			if b.Elements[i].GetLink() == nil {
				rootElement = b.Elements[i]
				break
			}
		}

		if common.PositionLink && rootElement != nil {
			rsx, rsy := rootElement.GetStart()
			common.Link = rootElement
			common.Start[0] += rsx
			common.Start[1] += rsy
			rvx, rvy := rootElement.GetVelocity()
			common.Velocity[0] += rvx
			common.Velocity[1] += rvy
		}

		if !hasSprite && !hasAction && elemType != "dummy" {
			continue
		}

		var xscale [2]float64
		var yscale, yscalestart, yscaledelta float64
		var width, scalestart, scaledelta [2]float64

		for _, attr := range sec.Attributes {
			id := strings.ToLower(attr.ID.String())
			switch id {
			case "xscale":
				xscale = GetVector2(attr.Value)
				consumed[attr] = true
			case "yscale":
				yscale = GetFloat(attr.Value)
				consumed[attr] = true
			case "width":
				width = GetVector2(attr.Value)
				consumed[attr] = true
			case "scalestart":
				scalestart = GetVector2(attr.Value)
				consumed[attr] = true
			case "scaledelta":
				scaledelta = GetVector2(attr.Value)
				consumed[attr] = true
			case "yscalestart":
				yscalestart = GetFloat(attr.Value)
				consumed[attr] = true
			case "yscaledelta":
				yscaledelta = GetFloat(attr.Value)
				consumed[attr] = true
			}
		}

		for _, attr := range sec.Attributes {
			if !consumed[attr] {
				logger.LogUnresolved(fmt.Sprintf("[%s] %s = %v", sec.Name, attr.ID.String(), attr.Value))
			}
		}

		switch elemType {
		case "normal", "":
			common.Type = Normal
			ne := &NormalElement{CommonElement: common}
			if sffData != nil {
				for i := range sffData.Sprites {
					spr := &sffData.Sprites[i]
					if int(spr.GroupNumber) == spriteNo[0] && int(spr.ImageNumber) == spriteNo[1] {
						ne.Sprite = spr
						break
					}
				}
			}
			fmt.Printf("Loaded Normal BG: spr=%v, start=%v, delta=%v, mask=%v\n", spriteNo, ne.Start, ne.Delta, ne.Mask)
			b.Elements = append(b.Elements, ne)
		case "anim", "animation":
			common.Type = Animation
			if !maskSet {
				common.Mask = true
			}
			ae := &AnimationElement{CommonElement: common, Sprites: make(map[string]*sff.Sprite), AirData: airData, SffData: sffData}
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
			fmt.Printf("Loaded Anim BG: action=%v, start=%v, delta=%v, mask=%v\n", actionNo, ae.Start, ae.Delta, ae.Mask)
			b.Elements = append(b.Elements, ae)
		case "parallax":
			common.Type = Parallax
			pe := &ParallaxElement{CommonElement: common, XScale: xscale, Width: width, ScaleStart: scalestart, ScaleDelta: scaledelta, YScaleStart: yscale, YScaleDelta: yscaledelta}
			if yscalestart != 0 {
				pe.YScaleStart = yscalestart
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
			fmt.Printf("Loaded Parallax BG: spr=%v, start=%v, delta=%v, mask=%v\n", spriteNo, pe.Start, pe.Delta, pe.Mask)
			b.Elements = append(b.Elements, pe)
		}
	}

	ctrlDefs := make(map[string]*BGCtrlDef)
	for _, sec := range ast.Sections {
		name := strings.ToLower(sec.Name)
		if strings.HasPrefix(name, "bgctrldef") {
			parts := strings.Fields(name)
			if len(parts) > 1 {
				defName := parts[1]
				def := &BGCtrlDef{}
				for _, attr := range sec.Attributes {
					id := strings.ToLower(attr.ID.String())
					switch id {
					case "looptime":
						def.Looptime = int(GetFloat(attr.Value))
					case "ctrlid", "eventid":
						if list, ok := attr.Value.(*parsers.ValueList); ok {
							for _, v := range list.Values {
								def.CtrlIDs = append(def.CtrlIDs, int(GetFloat(v)))
							}
						} else {
							def.CtrlIDs = append(def.CtrlIDs, int(GetFloat(attr.Value)))
						}
					}
				}
				ctrlDefs[defName] = def
				b.Controllers = append(b.Controllers, def)
			}
		} else if strings.HasPrefix(name, "bgctrl") {
			parts := strings.Fields(name)
			if len(parts) > 1 {
				defName := parts[1]
				if def, ok := ctrlDefs[defName]; ok {
					ctrl := &BGCtrl{Params: make(map[string]parsers.Value)}
					for _, attr := range sec.Attributes {
						id := strings.ToLower(attr.ID.String())
						switch id {
						case "type":
							ctrl.Type = GetString(attr.Value)
						case "time":
							if list, ok := attr.Value.(*parsers.ValueList); ok {
								if len(list.Values) >= 1 {
									ctrl.Time[0] = int(GetFloat(list.Values[0]))
									ctrl.Time[1] = ctrl.Time[0]
								}
								if len(list.Values) >= 2 {
									ctrl.Time[1] = int(GetFloat(list.Values[1]))
								}
								if len(list.Values) >= 3 {
									ctrl.Looptime = int(GetFloat(list.Values[2]))
								}
							} else {
								ctrl.Time[0] = int(GetFloat(attr.Value))
								ctrl.Time[1] = ctrl.Time[0]
							}
						case "ctrlid":
							if list, ok := attr.Value.(*parsers.ValueList); ok {
								for _, v := range list.Values {
									ctrl.CtrlIDs = append(ctrl.CtrlIDs, int(GetFloat(v)))
								}
							} else {
								ctrl.CtrlIDs = append(ctrl.CtrlIDs, int(GetFloat(attr.Value)))
							}
						default:
							ctrl.Params[id] = attr.Value
						}
					}
					def.Ctrls = append(def.Ctrls, ctrl)
				}
			}
		}
	}
}

func ResolvePath(path string, motifDir string, dataDir string) string {
	if path == "" {
		return ""
	}
	p := filepath.Join(motifDir, path)
	if _, err := os.Stat(p); err == nil {
		return p
	}
	p = filepath.Join(dataDir, "data", path)
	if _, err := os.Stat(p); err == nil {
		return p
	}
	p = filepath.Join(dataDir, path)
	if _, err := os.Stat(p); err == nil {
		return p
	}
	return path
}

func GetFloat(v parsers.Value) float64 {
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

func GetString(v parsers.Value) string {
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

func GetBool(v parsers.Value) bool { return GetFloat(v) != 0 }

func GetVector3(v parsers.Value) [3]float64 {
	if list, ok := v.(*parsers.ValueList); ok {
		var res [3]float64
		for i := 0; i < 3 && i < len(list.Values); i++ {
			res[i] = GetFloat(list.Values[i])
		}
		return res
	}
	if v != nil {
		s := GetString(v)
		if strings.Contains(s, " ") {
			fields := strings.Fields(s)
			var res [3]float64
			for i := 0; i < 3 && i < len(fields); i++ {
				res[i], _ = strconv.ParseFloat(fields[i], 64)
			}
			return res
		}
		return [3]float64{GetFloat(v), 0, 0}
	}
	return [3]float64{0, 0, 0}
}

func GetVector2(v parsers.Value) [2]float64 {
	if list, ok := v.(*parsers.ValueList); ok {
		if len(list.Values) >= 2 {
			return [2]float64{GetFloat(list.Values[0]), GetFloat(list.Values[1])}
		}
		if len(list.Values) == 1 {
			return [2]float64{GetFloat(list.Values[0]), 0}
		}
	}
	if v != nil {
		s := GetString(v)
		if strings.Contains(s, " ") {
			fields := strings.Fields(s)
			if len(fields) >= 2 {
				f1, _ := strconv.ParseFloat(fields[0], 64)
				f2, _ := strconv.ParseFloat(fields[1], 64)
				return [2]float64{f1, f2}
			}
		}
		return [2]float64{GetFloat(v), 0}
	}
	return [2]float64{0, 0}
}

func GetVector4(v parsers.Value) [4]int {
	if list, ok := v.(*parsers.ValueList); ok {
		var res [4]int
		for i := 0; i < 4 && i < len(list.Values); i++ {
			res[i] = int(GetFloat(list.Values[i]))
		}
		return res
	}
	if v != nil {
		s := GetString(v)
		if strings.Contains(s, " ") {
			fields := strings.Fields(s)
			var res [4]int
			for i := 0; i < 4 && i < len(fields); i++ {
				f, _ := strconv.ParseFloat(fields[i], 64)
				res[i] = int(f)
			}
			return res
		}
		return [4]int{int(GetFloat(v)), 0, 0, 0}
	}
	return [4]int{0, 0, 0, 0}
}

type BGCtrlDef struct {
	Looptime int
	CtrlIDs  []int
	Ctrls    []*BGCtrl
	Time     int
}

func (d *BGCtrlDef) Update(elements []Element) {
	// MUGEN: looptime = 1 is often used to mean no loop or it's a legacy quirk.
	// We only loop if looptime > 1.
	if d.Looptime > 1 && d.Time >= d.Looptime {
		d.Time = 0
	}
	for _, ctrl := range d.Ctrls {
		ctrl.Update(d.Time, d.CtrlIDs, elements)
	}
	d.Time++
}

type BGCtrl struct {
	Type     string
	Time     [2]int
	Looptime int
	CtrlIDs  []int
	Params   map[string]parsers.Value
}

func (c *BGCtrl) Update(globalTime int, defaultIDs []int, elements []Element) {
	t := globalTime
	if c.Looptime > 0 {
		// MUGEN: if looptime is specified, time is relative to that loop.
		// Usually formatted as time = start, end, looptime
		t = globalTime % c.Looptime
	}

	if t >= c.Time[0] && (c.Time[1] == -1 || t <= c.Time[1]) {
		ids := c.CtrlIDs
		if len(ids) == 0 {
			ids = defaultIDs
		}

		for _, el := range elements {
			match := false
			if len(ids) == 0 {
				match = true
			} else {
				for _, id := range ids {
					if el.GetID() == id {
						match = true
						break
					}
				}
			}

			if match {
				c.Apply(el)
			}
		}
	}
}
func (c *BGCtrl) Apply(el Element) {
	switch strings.ToLower(c.Type) {
	case "visible":
		if v, ok := c.Params["value"]; ok {
			el.SetVisible(GetFloat(v) != 0)
		}
	case "enable", "enabled":
		if v, ok := c.Params["value"]; ok {
			el.SetEnabled(GetFloat(v) != 0)
		}
	case "velset":
		if v, ok := c.Params["x"]; ok {
			el.SetVelocityX(GetFloat(v))
		}
		if v, ok := c.Params["y"]; ok {
			el.SetVelocityY(GetFloat(v))
		}
	case "veladd":
		if v, ok := c.Params["x"]; ok {
			el.AddVelocityX(GetFloat(v))
		}
		if v, ok := c.Params["y"]; ok {
			el.AddVelocityY(GetFloat(v))
		}
	case "posset":
		if v, ok := c.Params["x"]; ok {
			el.SetPositionX(GetFloat(v))
		}
		if v, ok := c.Params["y"]; ok {
			el.SetPositionY(GetFloat(v))
		}
	case "posadd":
		if v, ok := c.Params["x"]; ok {
			el.AddPositionX(GetFloat(v))
		}
		if v, ok := c.Params["y"]; ok {
			el.AddPositionY(GetFloat(v))
		}
	case "anim":
		if v, ok := c.Params["value"]; ok {
			el.SetAction(int(GetFloat(v)))
		}
	case "sinxset":
		if v, ok := c.Params["value"]; ok {
			el.SetSinX(GetVector3(v))
		}
	case "sinyset":
		if v, ok := c.Params["value"]; ok {
			el.SetSinY(GetVector3(v))
		}
	}
}
