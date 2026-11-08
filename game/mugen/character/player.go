package character

import (
	"fmt"
	"image"
	"math"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/render"
	"github.com/kazzmir/paintown/game/mugen/sff"
	"github.com/kazzmir/paintown/game/mugen/snd"
)

// Player is a high-level container that combines state, sprites, animations, and commands.
type Player struct {
	Character  *Character
	SFF        *sff.SFF
	AIR        *air.Data
	CMD        *cmd.Data
	SND        *snd.SoundBank
	DefPath    string
	Definition *parsers.DefData
	LocalCoord coord.Space

	// Path to the character's directory
	Dir string

	// Sprite cache for Ebiten
	spriteCache  map[paletteCacheKey]cachedSprite
	rgbSampler   render.Linear
	indexSampler render.Nearest

	// Command buffering
	Commands *CommandBuffer

	// Player ID (1 or 2)
	InputSlot int
}

// Update advances the player's state.
func (p *Player) Update() error {
	/*
		if p.Character.Time == 0 {
			fmt.Printf("Player.Update: Starting Tick 0\n")
		}
	*/
	// 1. (Removed redundant CommandBuffer.Add as Character.Update now handles it)

	// 2. (Removed redundant AnimDuration sync as it's now handled by ChangeAnim)
	// 3. Tick character state machine
	p.Character.Update()
	return p.Character.RuntimeError
}

// Draw renders the player to the screen at the specified screen coordinates.
func (p *Player) Draw(screen *ebiten.Image, screenX, screenY float64) {
	p.DrawScaled(screen, screenX, screenY, 1.0, 1.0)
}

// DrawScaled renders the player and its subordinates.
func (p *Player) DrawScaled(screen *ebiten.Image, screenX, screenY, scaleX, scaleY float64) {
	p.pruneSpriteCache()
	for _, actor := range OrderedActors([]*Player{p}) {
		c := actor.Character
		units := p.Character.ScaleFrom(c)
		x := screenX + (c.X*units-p.Character.X)*scaleX
		y := screenY + (c.Y*units-p.Character.Y)*scaleY
		p.drawEntity(screen, c, x, y, scaleX*units, scaleY*units)
	}

	p.drawLegacyExplods(screen, screenX, screenY, scaleX, scaleY)
}

func (p *Player) drawLegacyExplods(screen *ebiten.Image, screenX, screenY, scaleX, scaleY float64) {
	for _, ex := range p.Character.Explods {
		if ex.Visual != nil {
			continue // shared fight effect scene owns the rendered instance
		}
		exX := screenX + (ex.X-p.Character.X)*scaleX
		exY := screenY - (ex.Y-p.Character.Y)*scaleY
		p.drawExplod(screen, ex, exX, exY, scaleX, scaleY)
	}
}

func (p *Player) drawEntity(screen *ebiten.Image, ent *Character, x, y, sx, sy float64) {
	p.drawAfterImages(screen, ent, x, y, sx, sy)
	if !ent.Specials["invisible"] {
		p.drawEntityWithPalette(screen, ent, x, y, sx, sy, nil)
	}
}

func (p *Player) drawEntityWithPalette(screen *ebiten.Image, ent *Character, x, y, sx, sy float64, override *palette.Transform) {
	p.drawEntityPose(screen, ent, x, y, sx, sy, override, nil)
}

func (p *Player) drawEntityPose(screen *ebiten.Image, ent *Character, x, y, sx, sy float64, override *palette.Transform, shadow *render.Shadow) {
	animNo := ent.GetAnim()
	frames := ent.AnimationFile()
	if frames == nil {
		return
	}
	animData, ok := frames.Actions[animNo]
	if !ok {
		return
	}

	visual := animData.Sample(ent.AnimElem, ent.AnimElemTime)
	currentElement := visual.Element

	if currentElement == nil {
		return
	}

	sprite := p.FindSprite(currentElement.Group, currentElement.Image)
	if sprite == nil || sprite.Image == nil {
		return
	}

	ebitenSprite := p.getEbitenImage(ent, currentElement.Group, currentElement.Image)
	if ebitenSprite == nil {
		return
	}
	opts := &ebiten.DrawImageOptions{}
	if ent.Timeline != nil {
		tint := ent.Timeline.ActorTint(ent.pauseActor)
		opts.ColorScale.Scale(tint[0], tint[1], tint[2], 1)
	}
	opts.GeoM.Translate(-float64(sprite.XAxis), -float64(sprite.YAxis))

	// Flip logic
	elementFlipX := currentElement.FlipX
	elementFlipY := currentElement.FlipY
	if ent.Facing == -1 {
		elementFlipX = !elementFlipX
	}

	if elementFlipX {
		opts.GeoM.Scale(-1, 1)
	}
	if elementFlipY {
		opts.GeoM.Scale(1, -1)
	}

	opts.GeoM.Scale(visual.ScaleX, visual.ScaleY)
	opts.GeoM.Rotate(-visual.Angle * float64(ent.Facing) * math.Pi / 180)
	opts.GeoM.Translate(visual.X*float64(ent.Facing), visual.Y)
	characterScaleX, characterScaleY := ent.spriteScale()
	if ent.AngleDrawingEnabled {
		characterScaleX = float64(float32(characterScaleX) * ent.DrawScale[0])
		characterScaleY = float64(float32(characterScaleY) * ent.DrawScale[1])
	}
	opts.GeoM.Scale(sx*characterScaleX, sy*characterScaleY)
	if ent.AngleDrawingEnabled {
		opts.GeoM.Rotate(-ent.DrawAngle * float64(ent.Facing) * math.Pi / 180)
	}
	// Native flipped-axis bias remains in screen coordinates after rotation.
	if elementFlipX != (characterScaleX < 0) {
		opts.GeoM.Translate(1, 0)
	}
	if elementFlipY != (characterScaleY < 0) {
		opts.GeoM.Translate(0, 1)
	}
	// Constant draw.offset follows facing; frame Offset uses screen-oriented X.
	// Both translate the axis after rotation/scaling and leave collision unchanged.
	if ent.StateFile != nil {
		if d := ent.StateFile.Size.DrawOffset; len(d) > 0 {
			x += float64(d[0]*ent.Facing) * sx
			if len(d) > 1 {
				y += float64(d[1]) * sy
			}
		}
	}
	if len(ent.DrawOffset) > 1 {
		x += ent.DrawOffset[0] * sx
		y += ent.DrawOffset[1] * sy
	}
	// Indexed sampling uses integer game pixel centers. RGB textures use the
	// usual half-pixel centers and retain fractional axes.
	_, indexed := sprite.Image.(*image.Paletted)
	if indexed {
		x, y = x+0.5, y+0.5
	}
	opts.GeoM.Translate(x, y)

	blend, alphaSrc, alphaDst := currentElement.Blend, visual.AlphaSrc, visual.AlphaDst
	if ent.DrawTransparency.Enabled {
		blend, alphaSrc, alphaDst = ent.DrawTransparency.Mode, ent.DrawTransparency.Source, ent.DrawTransparency.Destination
	}
	var style render.DrawStyle
	if shadow == nil || shadow.Reflection {
		destination := render.ApplyBlend(opts, blend, alphaSrc, alphaDst)
		style = render.DrawStyle{DestinationFactor: destination, Palette: ent.Palettes.Transform()}
		if override != nil {
			style.Palette = *override
		}
		if ent.Timeline != nil {
			style.Palette = palette.Combine(style.Palette, ent.Timeline.AllPalette.Transform())
			if override != nil {
				// AfterImage's integer palette coefficients are quantized again
				// after merging the global multiplier, before the first draw pass.
				for i, coefficient := range style.Palette.Mul {
					style.Palette.Mul[i] = float32(math.Trunc(float64(coefficient*256))) / 256
				}
			}
		}
	}

	if shadow != nil {
		opacity := shadow.Opacity(ent.Y*sy, blend, alphaDst)
		if opacity <= 0 {
			return
		}
		offset := 0.0
		if ent.StateFile != nil && !ent.isProjectile && !shadow.Reflection {
			offset = float64(ent.StateFile.Size.ShadowOffset) * sy
		}
		reverse := shadow.SamplingReversal(opts.GeoM, offset, indexed, elementFlipY != (characterScaleY < 0))
		style.SourceReversal = &reverse
		opts.GeoM = shadow.SpriteGeometry(opts.GeoM, offset, indexed, elementFlipY != (characterScaleY < 0))
		if shadow.Reflection {
			opts.ColorScale.ScaleAlpha(opacity)
		} else {
			opts.ColorScale.Reset()
			opts.ColorScale.Scale(0, 0, 0, opacity)
			style.DestinationFactor = -1
		}

	}

	zoom := ent.Viewport.ZoomTransform()
	opts.GeoM.Scale(zoom.ScaleX, zoom.ScaleY)
	opts.GeoM.Translate(zoom.X, zoom.Y)
	var err error
	if indexed {
		err = p.indexSampler.DrawStyledImage(screen, ebitenSprite, opts, style)
	} else {
		err = p.rgbSampler.DrawStyledImage(screen, ebitenSprite, opts, style)
	}
	if err != nil {
		ent.RuntimeError = fmt.Errorf("%s sprite %d,%d shader: %w", p.DefPath, currentElement.Group, currentElement.Image, err)
	}
}

func (p *Player) drawExplod(screen *ebiten.Image, ex *ExplodData, x, y, sx, sy float64) {
	animData, ok := p.AIR.Actions[ex.Anim]
	if !ok || len(animData.Elements) == 0 {
		return
	}
	// Simplified: just draw first frame or loop through?
	// Explods are complex, for now just first frame.
	el := animData.Elements[0]
	sprite := p.FindSprite(el.Group, el.Image)
	if sprite == nil {
		return
	}
	ebitenSprite := p.getEbitenImage(p.Character, el.Group, el.Image)
	if ebitenSprite == nil {
		return
	}
	opts := &ebiten.DrawImageOptions{}
	opts.GeoM.Translate(-float64(sprite.XAxis), -float64(sprite.YAxis))
	if ex.Facing == -1 {
		opts.GeoM.Scale(-1, 1)
	}
	opts.GeoM.Scale(sx, sy)
	opts.GeoM.Translate(x, y)
	zoom := p.Character.Viewport.ZoomTransform()
	opts.GeoM.Scale(zoom.ScaleX, zoom.ScaleY)
	opts.GeoM.Translate(zoom.X, zoom.Y)
	screen.DrawImage(ebitenSprite, opts)
}

type paletteCacheKey struct {
	state     *palette.State
	index     int
	hitEffect bool
}
type cachedSprite struct {
	revision uint64
	image    *ebiten.Image
}

func (p *Player) getEbitenImage(entity *Character, g, i int) *ebiten.Image {
	return p.getMappedSpriteImage(entity, g, i, true)
}

func (p *Player) getMappedSpriteImage(entity *Character, g, i int, hitEffect bool) *ebiten.Image {
	index, found := p.SFF.Index(g, i)
	if !found {
		return nil
	}
	state := entity.Palettes
	key := paletteCacheKey{state, index, hitEffect}
	revision := uint64(0)
	if state != nil {
		revision = state.Revision
	}
	if p.spriteCache == nil {
		p.spriteCache = map[paletteCacheKey]cachedSprite{}
	}
	if cached, found := p.spriteCache[key]; found {
		if cached.revision == revision {
			return cached.image
		}
		cached.image.Dispose()
		delete(p.spriteCache, key)
	}
	pixels := p.SFF.Sprites[index].Image
	if state != nil {
		var err error
		pixels, err = state.MappedImage(index, true)
		if err != nil {
			entity.RuntimeError = fmt.Errorf("%s sprite %d,%d: %w", p.DefPath, g, i, err)
			return nil
		}
	}
	if pixels == nil {
		return nil
	}
	img := ebiten.NewImageFromImage(pixels)
	p.spriteCache[key] = cachedSprite{revision, img}
	return img
}

func (p *Player) pruneSpriteCache() {
	active := map[*palette.State]bool{}
	retain := func(entity *Character) {
		active[entity.Palettes] = true
		if entity.afterImages != nil {
			for _, frame := range entity.afterImages.frames {
				active[frame.palettes] = true
			}
		}
	}
	retain(p.Character)
	for _, entity := range p.helpers() {
		retain(entity)
	}
	for _, entity := range p.Character.Projectiles {
		retain(entity)
	}
	for key, cached := range p.spriteCache {
		if !active[key.state] || (key.state != nil && key.state.Revision != cached.revision) {
			cached.image.Dispose()
			delete(p.spriteCache, key)
		}
	}
}

func (p *Player) helpers() []*Character {
	if p.Character.World != nil {
		return p.Character.helperFamily(0, false)
	}
	return p.Character.Helpers
}

// HUD sprites retain palette remapping without temporary combat effects.
func (p *Player) DrawSprite(screen *ebiten.Image, entity *Character, group, item int, x, y, sx, sy float64) {
	sprite := p.FindSprite(group, item)
	if sprite == nil {
		return
	}
	img := p.getMappedSpriteImage(entity, group, item, false)
	if img == nil {
		return
	}
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Translate(-float64(sprite.XAxis), -float64(sprite.YAxis))
	op.GeoM.Scale(sx, sy)
	op.GeoM.Translate(x, y)
	transform := palette.Transform{}
	if entity.Timeline != nil {
		transform = entity.Timeline.AllPalette.Transform()
	}
	if !transform.Enabled {
		screen.DrawImage(img, op)
		return
	}
	style := render.DrawStyle{DestinationFactor: -1, Palette: transform}
	var err error
	if _, indexed := sprite.Image.(*image.Paletted); indexed {
		err = p.indexSampler.DrawStyledImage(screen, img, op, style)
	} else {
		err = p.rgbSampler.DrawStyledImage(screen, img, op, style)
	}
	if err != nil {
		entity.RuntimeError = fmt.Errorf("HUD portrait palette: %w", err)
	}
}

func (p *Player) FindSprite(group, image int) *sff.Sprite {
	if i, ok := p.SFF.Index(group, image); ok {
		return &p.SFF.Sprites[i]
	}
	return nil
}
