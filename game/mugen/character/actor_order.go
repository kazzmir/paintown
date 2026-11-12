package character

import (
	"sort"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/render"
)

// Actor keeps the entity's sprite bank with its world-owned state.
type Actor struct {
	Owner     *Player
	Character *Character
}

// Before the fight call, native roots draw the current sprite at their stage
// starting position while CNS still observes its actual position. Helpers and
// projectiles keep their own positions throughout these initial round phases.
func (c *Character) drawPosition() (float64, float64) {
	if c.Parent == nil && !c.isProjectile && (c.RoundState == 0 || c.RoundState == 1) {
		return c.StartX, c.StartY
	}
	return c.X, c.Y
}

// SpriteLayer adds a renderer-owned sprite to the shared actor priority order.
// Equal-priority sprites are drawn before actors; their supplied order is kept.
type SpriteLayer struct {
	Priority int
	Draw     func()
}

// OrderedActors returns the shared actor layer, from back to front. Priority
// crosses root/helper ownership; equal priorities put the lower world ID in front.
func OrderedActors(players []*Player) []Actor {
	var actors []Actor
	for _, p := range players {
		if p == nil || p.Character == nil {
			continue
		}
		appendActor := func(c *Character) {
			if c != nil && !c.destroyed && !c.DebugDisabled {
				actors = append(actors, Actor{p, c})
			}
		}
		appendActor(p.Character)
		for _, c := range p.helpers() {
			appendActor(c)
		}
		for _, c := range p.Character.Projectiles {
			appendActor(c)
		}
	}
	sort.SliceStable(actors, func(i, j int) bool {
		a, b := actors[i].Character, actors[j].Character
		if a.SprPriority != b.SprPriority {
			return a.SprPriority < b.SprPriority
		}
		return a.EntityID > b.EntityID
	})
	return actors
}

// DrawPlayers projects all root/helper actors into one ordered fight layer.
// floorY includes the stage floor and vertical camera offset in game units.
func DrawPlayers(screen *ebiten.Image, players []*Player, space coord.Space, cameraX, floorY float64) {
	DrawPlayersWithSprites(screen, players, space, cameraX, floorY, nil)
}

func DrawPlayersWithSprites(screen *ebiten.Image, players []*Player, space coord.Space, cameraX, floorY float64, sprites []SpriteLayer) {
	sprites = append([]SpriteLayer(nil), sprites...)
	sort.SliceStable(sprites, func(i, j int) bool { return sprites[i].Priority < sprites[j].Priority })
	next := 0
	for _, p := range players {
		p.pruneSpriteCache()
	}
	for _, actor := range OrderedActors(players) {
		p, c := actor.Owner, actor.Character
		for next < len(sprites) && sprites[next].Priority <= c.SprPriority {
			sprites[next].Draw()
			next++
		}
		scale := float64(space.Width) / float64(c.LocalCoordinateWidth())
		x, y := c.drawPosition()
		p.drawEntity(screen, c, float64(space.Width)/2+x*scale-cameraX, floorY+y*scale, scale, scale)
	}
	for next < len(sprites) {
		sprites[next].Draw()
		next++
	}
	for _, p := range players {
		scale, _ := p.LocalCoord.ScaleTo(space)
		c := p.Character
		p.drawLegacyExplods(screen, float64(space.Width)/2+c.X*scale-cameraX, floorY+c.Y*scale, scale, scale)
	}
}

// DrawPlayerShadows is a separate floor layer; historical AfterImage poses do
// not cast shadows, and an issuer's local flags do not hide its subordinates.
func DrawPlayerShadows(screen *ebiten.Image, players []*Player, space coord.Space, cameraX float64, shadow render.Shadow) {
	for _, actor := range OrderedActors(players) {
		c := actor.Character
		if c.Specials["invisible"] || c.Specials["noshadow"] || (c.isProjectile && !c.projectile.parameters.Shadow) {
			continue
		}
		scale := float64(space.Width) / float64(c.LocalCoordinateWidth())
		x, y := c.drawPosition()
		actor.Owner.drawEntityPose(screen, c, float64(space.Width)/2+x*scale-cameraX, shadow.FloorY+y*scale, scale, scale, nil, &shadow)
	}
}
