package character

import (
	"sort"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/coord"
)

// Actor keeps the entity's sprite bank with its world-owned state.
type Actor struct {
	Owner     *Player
	Character *Character
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
			if c != nil && !c.destroyed {
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
	for _, p := range players {
		p.pruneSpriteCache()
	}
	for _, actor := range OrderedActors(players) {
		p, c := actor.Owner, actor.Character
		scale := float64(space.Width) / float64(c.LocalCoordinateWidth())
		p.drawEntity(screen, c, float64(space.Width)/2+c.X*scale-cameraX, floorY+c.Y*scale, scale, scale)
	}
	for _, p := range players {
		scale, _ := p.LocalCoord.ScaleTo(space)
		c := p.Character
		p.drawLegacyExplods(screen, float64(space.Width)/2+c.X*scale-cameraX, floorY+c.Y*scale, scale, scale)
	}
}
