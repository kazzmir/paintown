package background

import (
	"fmt"
	"image/color"
	"math"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/logger"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/render"
	"github.com/kazzmir/paintown/game/mugen/resource"
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
	// SpriteData overrides the caller's shared archive for this background.
	SpriteData *sff.SFF
	LocalCoord coord.Space
	CameraZoom float64    // zero retains the unzoomed motif/storyboard default
	StageScale [2]float64 // zero retains unit scale outside stage rendering
	sprites    map[string]*SpriteImages
}

// DrawLayer selects this background's optional archive before drawing. GPU
// images are created lazily on the drawing thread, independently of file loading.
func (b *Background) DrawLayer(screen *ebiten.Image, layer int, cameraX, cameraY, zOffset float64, shared map[string]*SpriteImages) {
	_ = b.DrawLayerWithPalette(screen, layer, cameraX, cameraY, zOffset, shared, palette.Transform{})
}

func (b *Background) DrawLayerWithPalette(screen *ebiten.Image, layer int, cameraX, cameraY, zOffset float64, shared map[string]*SpriteImages, transform palette.Transform) error {
	return b.DrawLayerWithPaletteTint(screen, layer, cameraX, cameraY, zOffset, shared, transform, [3]float32{1, 1, 1})
}

// Scene tint applies to drawn sprites. The caller's clear color stays separate.
func (b *Background) DrawLayerWithPaletteTint(screen *ebiten.Image, layer int, cameraX, cameraY, zOffset float64, shared map[string]*SpriteImages, transform palette.Transform, tint [3]float32) error {
	images := shared
	if b.SpriteData != nil {
		if b.sprites == nil {
			b.sprites = make(map[string]*SpriteImages)
			for _, sprite := range b.SpriteData.Sprites {
				if sprite.Image == nil {
					continue
				}
				images := &SpriteImages{Masked: ebiten.NewImageFromImage(sprite.Image)}
				if sprite.ImageOpaque != nil {
					images.Opaque = ebiten.NewImageFromImage(sprite.ImageOpaque)
				} else {
					images.Opaque = images.Masked
				}
				b.sprites[fmt.Sprintf("%d-%d", sprite.GroupNumber, sprite.ImageNumber)] = images
			}
		}
		images = b.sprites
	}
	for _, element := range b.Elements {
		if element.GetLayerNo() == layer {
			if scaled, ok := element.(interface{ setStageScale([2]float64) }); ok {
				scaled.setStageScale(b.StageScale)
			}
			if zoomed, ok := element.(interface{ setCameraZoom(float64) }); ok {
				zoomed.setCameraZoom(b.CameraZoom)
			}
			tinted, hasTint := element.(interface{ setSceneTint([3]float32) })
			if hasTint {
				tinted.setSceneTint(tint)
			} else if tint != [3]float32{1, 1, 1} {
				return fmt.Errorf("background %s has no scene tint renderer", element.GetName())
			}
			styled, ok := element.(interface {
				setPalette(palette.Transform)
				paletteError() error
			})
			if ok {
				styled.setPalette(transform)
			} else if transform.Enabled {
				return fmt.Errorf("background %s has no palette renderer", element.GetName())
			}
			element.Draw(screen, cameraX, cameraY, zOffset, images)
			if hasTint {
				tinted.setSceneTint([3]float32{1, 1, 1})
			}
			if ok {
				err := styled.paletteError()
				styled.setPalette(palette.Transform{})
				if err != nil {
					return fmt.Errorf("background %s palette: %w", element.GetName(), err)
				}
			}
		}
	}
	return nil
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
	paletteTransform palette.Transform
	sceneTint        [3]float32
	sceneTintActive  bool
	paletteSampler   render.Nearest
	renderError      error
	Name             string
	Type             ElementType
	ID               int
	LayerNo          int
	Start            [2]float64
	Delta            [2]float64
	ZoomDelta        float64
	cameraZoom       float64
	stageScale       [2]float64
	Mask             bool
	Tile             [2]int
	Spacing          [2]int
	Window           [4]int // x1, y1, x2, y2
	windowSet        bool
	maskWindow       bool
	WindowDelta      [2]float64
	ScaleStart       [2]float64
	ScaleDelta       [2]float64
	scaleSet         bool
	Velocity         [2]float64
	Trans            string // "none", "add", "add1", "sub", "addalpha"
	Alpha            [2]int // s, d
	legacyTrans      bool   // pre-1.0 anim trans=none uses the AIR blend
	Visible          bool
	Enabled          bool
	Flip             string // "H", "V", "VH"

	// Sinusoidal movement
	SinX [3]float64 // amplitude, period, phase
	SinY [3]float64 // amplitude, period, phase

	// Internal state
	CurrentPos   [2]float64
	Time         int // ticks since start
	PositionLink bool
	Link         Element
}

func (e *CommonElement) setPalette(transform palette.Transform) {
	e.paletteTransform, e.renderError = transform, nil
}

func (e *CommonElement) setCameraZoom(zoom float64) { e.cameraZoom = zoom }

func (e *CommonElement) setStageScale(scale [2]float64) {
	for axis := range scale {
		if scale[axis] == 0 {
			scale[axis] = 1
		}
	}
	e.stageScale = scale
}

func (e *CommonElement) stageGeometry(geometry *ebiten.GeoM, center float64) {
	if e.stageScale == [2]float64{} || e.stageScale == [2]float64{1, 1} {
		return
	}
	geometry.Translate(-center, 0)
	geometry.Scale(e.stageScale[0], e.stageScale[1])
	geometry.Translate(center, 0)
}

func (e *CommonElement) zoomGeometry(geometry *ebiten.GeoM, center, floor float64) {
	if e.cameraZoom <= 0 || e.cameraZoom == 1 {
		return
	}
	z := 1 + (e.cameraZoom-1)*e.ZoomDelta
	geometry.Translate(-center, -floor)
	geometry.Scale(z, z)
	geometry.Translate(center, floor)
}

func (e *CommonElement) paletteError() error { return e.renderError }

func (e *CommonElement) setSceneTint(tint [3]float32) {
	e.sceneTint, e.sceneTintActive = tint, tint != [3]float32{1, 1, 1}
}

func (e *CommonElement) drawPaletteImage(target, source *ebiten.Image, options *ebiten.DrawImageOptions, trans string, alpha [2]int) {
	if e.sceneTintActive {
		options.ColorScale.Scale(e.sceneTint[0], e.sceneTint[1], e.sceneTint[2], 1)
	}
	destination := float32(-1)
	switch strings.ToLower(trans) {
	case "add", "addalpha":
		destination = render.ApplyAlpha(options, alpha[0], alpha[1])
	case "add1":
		destination = render.ApplyAlpha(options, 256, 128)
	case "sub":
		destination = render.ApplyBlend(options, "sub", 256, 256)
	}
	if !e.paletteTransform.Enabled && destination < 0 {
		target.DrawImage(source, options)
		return
	}
	if err := e.paletteSampler.DrawStyledImage(target, source, options, render.DrawStyle{DestinationFactor: destination, Palette: e.paletteTransform}); err != nil {
		e.renderError = err
	}
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
	x, y := e.Start[0], e.Start[1]
	if e.PositionLink && e.Link != nil {
		px, py := e.Link.GetStart()
		x, y = x+px, y+py
	}
	return x, y
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
	if e.PositionLink && e.Link != nil {
		return e.Link.GetDelta()
	}
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

func (e *CommonElement) DrawReflection(_ *ebiten.Image, _, _, _ float64, _ map[string]*SpriteImages, _ int) {
	// Dummy elements move/control linked objects without drawing a surface.
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
	if !e.Enabled {
		return
	}
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
	e.drawFrame(screen, cameraX, cameraY, zOffset, sprites, nil)
}

func (e *AnimationElement) drawFrame(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages, parallax *ParallaxElement) {
	if e.Player == nil {
		return
	}

	visual := e.Player.Visual()
	el := visual.Element
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

	trans, alpha := e.Trans, e.Alpha
	if trans == "default" || (trans == "none" && e.legacyTrans) {
		switch el.Blend {
		case "alpha":
			trans = "addalpha"
			alpha = [2]int{visual.AlphaSrc, visual.AlphaDst}
		case "sub":
			trans = "sub"
		}
	}

	img := si.Masked // Anim and animated parallax are always masked.

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

	e.drawWithBlending(screen, img, pivotX, pivotY, cameraX, cameraY, zOffset, trans, alpha, parallax, visual)
	e.Flip = oldFlip
}

func (e *CommonElement) draw(screen *ebiten.Image, img *ebiten.Image, xAxis int, yAxis int, cameraX, cameraY, zOffset float64) {
	e.drawWithBlending(screen, img, xAxis, yAxis, cameraX, cameraY, zOffset, e.Trans, e.Alpha, nil)
}

type ParallaxElement struct {
	AnimationElement
	Sprite       *sff.Sprite
	XScale       [2]float64
	Width        [2]float64
	YScaleStart  float64
	YScaleDelta  float64
	legacyYScale bool
}

func (e *ParallaxElement) Update() {
	e.AnimationElement.Update()
}

func (e *ParallaxElement) SetAction(action int) {
	if e.XScale != [2]float64{} {
		return // The beta supports animated parallax only with width.
	}
	e.AnimationElement.SetAction(action)
	if e.Player != nil {
		e.Mask = true
	}
}

func (e *ParallaxElement) DrawReflection(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages, intensity int) {
	// Stub
}

func (e *ParallaxElement) Draw(screen *ebiten.Image, cameraX, cameraY, zOffset float64, sprites map[string]*SpriteImages) {
	if e.Player != nil {
		e.drawFrame(screen, cameraX, cameraY, zOffset, sprites, e)
		return
	}
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
	viewport := b.LocalCoord
	if viewport.Width == 0 && viewport.Height == 0 {
		viewport = coord.Default()
	}
	legacyTrans := true
	for _, sec := range ast.Sections {
		if strings.EqualFold(sec.Name, "Info") {
			for _, attr := range sec.Attributes {
				if strings.EqualFold(attr.ID.String(), "mugenversion") {
					value := attr.Value
					if list, ok := value.(*parsers.ValueList); ok && len(list.Values) > 0 {
						value = list.Values[0]
					}
					version := GetFloat(value)
					legacyTrans = version < 1 || version >= 1000 // date-form WinMUGEN versions
				}
			}
		}
	}
	for _, sec := range ast.Sections {
		lowerName := strings.ToLower(sec.Name)
		if !strings.HasPrefix(lowerName, prefix) || strings.HasSuffix(lowerName, "def") || strings.HasPrefix(lowerName, prefix+"ctrl") {
			continue
		}

		// Guard against empty sections
		if len(sec.Attributes) == 0 {
			continue
		}

		common := CommonElement{
			Name:        sec.Name,
			Mask:        false,
			Delta:       [2]float64{1, 1},
			ZoomDelta:   1,
			ScaleStart:  [2]float64{1, 1},
			scaleSet:    true,
			Spacing:     [2]int{0, 0},
			Trans:       "default",
			Alpha:       [2]int{256, 256},
			legacyTrans: legacyTrans,
			Window:      [4]int{0, 0, viewport.Width - 1, viewport.Height - 1},
			windowSet:   true,
			Visible:     true,
			Enabled:     true,
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
			case "zoomdelta":
				common.ZoomDelta = GetFloat(attr.Value)
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
			case "maskwindow":
				common.Window = GetVector4(attr.Value)
				common.maskWindow = true
				// 1.1 uses a top-middle origin and exclusive bottom/right;
				// the shared internal rectangle uses inclusive endpoints.
				common.Window[0] += viewport.Width / 2
				common.Window[2] += viewport.Width/2 - 1
				common.Window[3]--
				consumed[attr] = true
			case "windowdelta":
				common.WindowDelta = GetVector2(attr.Value)
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

		if common.PositionLink && len(b.Elements) > 0 {
			common.Link = b.Elements[len(b.Elements)-1]
		}

		if !hasSprite && !hasAction && elemType != "dummy" {
			continue
		}

		var xscale [2]float64
		var yscale, yscalestart, yscaledelta float64
		var width, scalestart, scaledelta [2]float64
		modernScale, legacyScale := false, false

		for _, attr := range sec.Attributes {
			id := strings.ToLower(attr.ID.String())
			switch id {
			case "xscale":
				xscale = GetVector2(attr.Value)
				consumed[attr] = true
			case "yscale":
				yscale = GetFloat(attr.Value)
				legacyScale = true
				consumed[attr] = true
			case "width":
				width = GetVector2(attr.Value)
				consumed[attr] = true
			case "scalestart":
				modernScale = true
				scalestart = GetVector2(attr.Value)
				common.ScaleStart = scalestart
				consumed[attr] = true
			case "scaledelta":
				modernScale = true
				scaledelta = GetVector2(attr.Value)
				common.ScaleDelta = scaledelta
				consumed[attr] = true
			case "yscalestart":
				yscalestart = GetFloat(attr.Value)
				legacyScale = true
				consumed[attr] = true
			case "yscaledelta":
				yscaledelta = GetFloat(attr.Value)
				legacyScale = true
				consumed[attr] = true
			}
		}
		if !maskSet && (common.Trans == "add" || common.Trans == "sub") {
			common.Mask = true
		}

		for _, attr := range sec.Attributes {
			if !consumed[attr] {
				logger.LogUnresolved(fmt.Sprintf("[%s] %s = %v", sec.Name, attr.ID.String(), attr.Value))
			}
		}

		switch elemType {
		case "dummy":
			b.Elements = append(b.Elements, &common)
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
			b.Elements = append(b.Elements, ne)
		case "anim", "animation":
			common.Type = Animation
			common.Mask = true
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
			b.Elements = append(b.Elements, ae)
		case "parallax":
			common.Type = Parallax
			pe := &ParallaxElement{AnimationElement: AnimationElement{CommonElement: common, Sprites: make(map[string]*sff.Sprite), AirData: airData, SffData: sffData}, XScale: xscale, Width: width, YScaleStart: 100, YScaleDelta: yscaledelta, legacyYScale: legacyScale && !modernScale}
			if yscale != 0 {
				pe.YScaleStart = yscale
			}
			if yscalestart != 0 {
				pe.YScaleStart = yscalestart
			}
			if hasAction && xscale == [2]float64{} {
				pe.Mask = true
				pe.SetAction(actionNo)
			}
			if !hasAction && sffData != nil {
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

	b.loadControllers(ast, prefix)
}

func ResolvePath(path string, motifDir string, dataDir string) string {
	if path == "" {
		return ""
	}
	if resolved, err := resource.Resolve(path, motifDir, filepath.Join(dataDir, "data"), dataDir); err == nil {
		return resolved
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
	if d.Looptime > 0 && d.Time >= d.Looptime {
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
