package character

import (
	"image"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cmd"
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

	// Path to the character's directory
	Dir string

	// Sprite cache for Ebiten
	spriteCache map[int]map[int]*ebiten.Image

	// Command buffering
	Commands *CommandBuffer

	// Player ID (1 or 2)
	PlayerID int
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
	animData, ok := p.AIR.Actions[animNo]
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

	ebitenSprite := p.getEbitenImage(currentElement.Group, currentElement.Image, sprite.Image)
	opts := &ebiten.DrawImageOptions{}
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
	ebitenSprite := p.getEbitenImage(el.Group, el.Image, sprite.Image)
	opts := &ebiten.DrawImageOptions{}
	opts.GeoM.Translate(-float64(sprite.XAxis), -float64(sprite.YAxis))
	if ex.Facing == -1 {
		opts.GeoM.Scale(-1, 1)
	}
	opts.GeoM.Scale(sx, sy)
	opts.GeoM.Translate(x, y)
	screen.DrawImage(ebitenSprite, opts)
}

func (p *Player) getEbitenImage(g, i int, src image.Image) *ebiten.Image {
	if p.spriteCache[g] == nil {
		p.spriteCache[g] = make(map[int]*ebiten.Image)
	}
	img, ok := p.spriteCache[g][i]
	if !ok {
		img = ebiten.NewImageFromImage(src)
		p.spriteCache[g][i] = img
	}
	return img
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
