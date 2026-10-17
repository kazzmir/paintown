package character

import (
	"fmt"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/parsers"
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
	spriteCache map[paletteCacheKey]cachedSprite

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
	// 1. Draw main character
	p.drawEntity(screen, p.Character, screenX, screenY, scaleX, scaleY)

	// 2. Draw Helpers
	for _, h := range p.Character.Helpers {
		// Helpers are drawn relative to screenX/screenY (which is character axis)
		// But Character.X/Y are absolute world coords.
		// Actually p1.Character.X is -60, h.X is -60 + offX.
		// So h.X - p1.Character.X is real offset.
		hx := screenX + (h.X-p.Character.X)*scaleX
		hy := screenY - (h.Y-p.Character.Y)*scaleY
		p.drawEntity(screen, h, hx, hy, scaleX, scaleY)
	}

	// 3. Draw Projectiles
	for _, pr := range p.Character.Projectiles {
		px := screenX + (pr.X-p.Character.X)*scaleX
		py := screenY - (pr.Y-p.Character.Y)*scaleY
		p.drawEntity(screen, pr, px, py, scaleX, scaleY)
	}

	// 4. Draw Explods
	for _, ex := range p.Character.Explods {
		exX := screenX + (ex.X-p.Character.X)*scaleX
		exY := screenY - (ex.Y-p.Character.Y)*scaleY
		p.drawExplod(screen, ex, exX, exY, scaleX, scaleY)
	}
}

func (p *Player) drawEntity(screen *ebiten.Image, ent *Character, x, y, sx, sy float64) {
	animNo := ent.GetAnim()
	frames := ent.AnimationFile()
	if frames == nil {
		return
	}
	animData, ok := frames.Actions[animNo]
	if !ok {
		return
	}

	var currentElement *air.Element
	if ent.AnimElem < len(animData.Elements) {
		currentElement = &animData.Elements[ent.AnimElem]
	}

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

	opts.GeoM.Translate(float64(currentElement.XOffset), float64(currentElement.YOffset))
	opts.GeoM.Scale(sx, sy)
	opts.GeoM.Translate(x, y)

	screen.DrawImage(ebitenSprite, opts)
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
	screen.DrawImage(ebitenSprite, opts)
}

type paletteCacheKey struct {
	state *palette.State
	index int
}
type cachedSprite struct {
	revision uint64
	image    *ebiten.Image
}

func (p *Player) getEbitenImage(entity *Character, g, i int) *ebiten.Image {
	index := -1
	for n, sprite := range p.SFF.Sprites {
		if int(sprite.GroupNumber) == g && int(sprite.ImageNumber) == i {
			index = n
			break
		}
	}
	if index < 0 {
		return nil
	}
	state := entity.Palettes
	key := paletteCacheKey{state, index}
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
		pixels, err = state.Image(index, true)
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
	active := map[*palette.State]bool{p.Character.Palettes: true}
	for _, entity := range p.Character.Helpers {
		active[entity.Palettes] = true
	}
	for _, entity := range p.Character.Projectiles {
		active[entity.Palettes] = true
	}
	for key, cached := range p.spriteCache {
		if !active[key.state] || (key.state != nil && key.state.Revision != cached.revision) {
			cached.image.Dispose()
			delete(p.spriteCache, key)
		}
	}
}

// DrawSprite uses the entity's remapping and the same cache as its animation.
func (p *Player) DrawSprite(screen *ebiten.Image, entity *Character, group, item int, x, y, sx, sy float64) {
	sprite := p.FindSprite(group, item)
	if sprite == nil {
		return
	}
	img := p.getEbitenImage(entity, group, item)
	if img == nil {
		return
	}
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Translate(-float64(sprite.XAxis), -float64(sprite.YAxis))
	op.GeoM.Scale(sx, sy)
	op.GeoM.Translate(x, y)
	screen.DrawImage(img, op)
}

func (p *Player) FindSprite(group, image int) *sff.Sprite {
	for i := range p.SFF.Sprites {
		s := &p.SFF.Sprites[i]
		if int(s.GroupNumber) == group && int(s.ImageNumber) == image {
			return s
		}
	}
	return nil
}
