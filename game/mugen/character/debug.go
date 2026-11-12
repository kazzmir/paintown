package character

import "github.com/kazzmir/paintown/game/mugen/combat"

// CollisionDebugBox reports the same unrotated, character-local world geometry
// used by collision detection. Sprite draw offsets/AngleDraw do not move it.
type CollisionDebugBox struct {
	X1, Y1, X2, Y2 float64
	Attack         bool
}

func (c *Character) CollisionDebugBoxes() []CollisionDebugBox {
	frame := c.CurrentElement()
	if frame == nil {
		return nil
	}
	sx, sy := c.spriteScale()
	var boxes []CollisionDebugBox
	for _, b := range frame.Clsn2 {
		x1, y1, x2, y2 := BoxToWorld(b, c.X, c.Y, c.Facing, sx, sy)
		boxes = append(boxes, CollisionDebugBox{x1, y1, x2, y2, false})
	}
	for _, b := range frame.Clsn1 {
		x1, y1, x2, y2 := BoxToWorld(b, c.X, c.Y, c.Facing, sx, sy)
		boxes = append(boxes, CollisionDebugBox{x1, y1, x2, y2, true})
	}
	return boxes
}

type CombatDebugState struct {
	Juggle     int
	Targets    []int32
	Protection [2]combat.HitFilter
}

func (c *Character) CombatDebugState() CombatDebugState {
	state := CombatDebugState{Juggle: c.juggleBudget(), Protection: c.HitFilters}
	for _, target := range c.acquiredTargets(-1, true) {
		state.Targets = append(state.Targets, target.EntityID)
	}
	return state
}
