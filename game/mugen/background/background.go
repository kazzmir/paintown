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
	Draw(screen *ebiten.Image, cameraX, cameraY float64, sprites map[string]*SpriteImages)
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

	// Sinusoidal movement
	SinX [3]float64 // amplitude, period, phase
	SinY [3]float64 // amplitude, period, phase

	// Internal state
	CurrentPos [2]float64
	Time       int // ticks since start
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
		offX = e.SinX[0] * math.Sin(2*math.Pi*float64(e.Time)/e.SinX[1]+e.SinX[2])
	}
	if e.SinY[1] > 0 {
		offY = e.SinY[0] * math.Sin(2*math.Pi*float64(e.Time)/e.SinY[1]+e.SinY[2])
	}
	return offX, offY
}

func (e *CommonElement) GetLayerNo() int {
	return e.LayerNo
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

	offX, offY := e.GetOffset()
	e.CommonElement.drawWithBlending(screen, img, int(e.Sprite.XAxis), int(e.Sprite.YAxis), cameraX+offX, cameraY+offY, e.Trans, e.Alpha, nil)
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

	offX, offY := e.GetOffset()
	e.CommonElement.drawWithBlending(screen, img, pivotX-el.XOffset, pivotY-el.YOffset, cameraX+offX, cameraY+offY, trans, alpha, nil)
}

func (e *CommonElement) drawWithBlending(screen *ebiten.Image, img *ebiten.Image, xAxis, yAxis int, cameraX, cameraY float64, trans string, alpha [2]int, parallax *ParallaxElement) {
	if img == nil || !e.Visible {
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

	// Determine scale and shearing for parallax
	scaleX, scaleY := 1.0, 1.0
	shearX := 0.0

	if parallax != nil {
		tileY = 0 // Parallax ignores Y tiling

		// Scalestart/delta logic
		if parallax.ScaleStart[0] != 0 {
			scaleX = parallax.ScaleStart[0] + parallax.ScaleDelta[0]*cameraY
		}
		if parallax.ScaleStart[1] != 0 {
			scaleY = parallax.ScaleStart[1] + parallax.ScaleDelta[1]*cameraY
		} else if parallax.YScaleStart != 0 {
			// Deprecated formula
			scaleY = 1.0 / (parallax.YScaleStart/100.0 + parallax.YScaleDelta/100.0*cameraY)
		}

		// Shearing logic: (delta_bottom - delta_top) / height
		// We'll simplify for now: just apply xscale if provided
		if parallax.XScale[0] != 0 && parallax.XScale[1] != 0 {
			// Approximate shear
			shearX = (parallax.XScale[1] - parallax.XScale[0]) * cameraX / float64(h)
		}
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

			// Apply Scale and Shearing
			op.GeoM.Scale(scaleX, scaleY)
			if shearX != 0 {
				op.GeoM.Skew(shearX, 0)
			}
			op.GeoM.Translate(xPos, yPos)

			e.applyBlending(op, trans, alpha)
			target.DrawImage(img, op)
		}
	}
}

func (e *CommonElement) draw(screen *ebiten.Image, img *ebiten.Image, xAxis, yAxis int, cameraX, cameraY float64) {
	e.drawWithBlending(screen, img, xAxis, yAxis, cameraX, cameraY, e.Trans, e.Alpha, nil)
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

	offX, offY := e.GetOffset()
	e.CommonElement.drawWithBlending(screen, img, int(e.Sprite.XAxis), int(e.Sprite.YAxis), cameraX+offX, cameraY+offY, e.Trans, e.Alpha, e)
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
			Visible: true,
			Enabled: true,
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
			case "sin.x":
				v := getVector3(attr.Value)
				common.SinX = [3]float64{v[0], v[1], v[2]}
			case "sin.y":
				v := getVector3(attr.Value)
				common.SinY = [3]float64{v[0], v[1], v[2]}
			}
		}

		var xscale [2]float64
		var yscale float64
		var width [2]float64
		var scalestart [2]float64
		var scaledelta [2]float64

		for _, attr := range sec.Attributes {
			id := strings.ToLower(attr.ID.String())
			switch id {
			case "xscale":
				xscale = getVector2(attr.Value)
			case "yscale":
				yscale = getFloat(attr.Value)
			case "width":
				width = getVector2(attr.Value)
			case "scalestart":
				scalestart = getVector2(attr.Value)
			case "scaledelta":
				scaledelta = getVector2(attr.Value)
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
				Width:         width,
				ScaleStart:    scalestart,
				ScaleDelta:    scaledelta,
				YScaleStart:   yscale,
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

func getVector3(v parsers.Value) [3]float64 {
	if list, ok := v.(*parsers.ValueList); ok {
		var res [3]float64
		for i := 0; i < 3 && i < len(list.Values); i++ {
			res[i] = getFloat(list.Values[i])
		}
		return res
	}
	if v != nil {
		return [3]float64{getFloat(v), 0, 0}
	}
	return [3]float64{0, 0, 0}
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

type BGCtrlDef struct {
	Looptime int
	CtrlIDs  []int
	Ctrls    []*BGCtrl
	Time     int // internal timer
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
		t = globalTime % c.Looptime
	}

	if t >= c.Time[0] && t <= c.Time[1] {
		ids := c.CtrlIDs
		if len(ids) == 0 {
			ids = defaultIDs
		}

		for _, el := range elements {
			match := false
			if len(ids) == 0 {
				match = true // affect all
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
			el.SetVisible(getFloat(v) != 0)
		}
	case "enabled":
		if v, ok := c.Params["value"]; ok {
			el.SetEnabled(getFloat(v) != 0)
		}
	case "velset":
		if v, ok := c.Params["x"]; ok {
			el.SetVelocityX(getFloat(v))
		}
		if v, ok := c.Params["y"]; ok {
			el.SetVelocityY(getFloat(v))
		}
	case "veladd":
		if v, ok := c.Params["x"]; ok {
			el.AddVelocityX(getFloat(v))
		}
		if v, ok := c.Params["y"]; ok {
			el.AddVelocityY(getFloat(v))
		}
	case "posset":
		if v, ok := c.Params["x"]; ok {
			el.SetPositionX(getFloat(v))
		}
		if v, ok := c.Params["y"]; ok {
			el.SetPositionY(getFloat(v))
		}
	case "posadd":
		if v, ok := c.Params["x"]; ok {
			el.AddPositionX(getFloat(v))
		}
		if v, ok := c.Params["y"]; ok {
			el.AddPositionY(getFloat(v))
		}
	}
}
