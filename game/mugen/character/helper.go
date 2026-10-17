package character

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/engine"
)

func (c *Character) CreateHelper(p engine.HelperParameters) error {
	if err := c.ensureWorld(); err != nil {
		return err
	}
	if len(c.World.Helpers) >= c.World.HelperMax {
		c.EvaluationWarning("Helper limit reached")
		return nil
	}
	h := NewCharacter(c.StateFile, c.AirFile, c.CmdFile)
	h.Name, h.AuthorName = c.Name, c.AuthorName
	h.Parent, h.Root = c, c.rootPlayer()
	h.HelperID, h.HelperKeyControl, h.PlayerHelper = max(0, p.ID), p.KeyControl, p.Type == "player"
	h.LocalCoord, h.GameCoord, h.Viewport = c.LocalCoord, c.GameCoord, c.Viewport
	if h.Viewport != nil {
		h.ScreenWidth = h.LocalCoordinateWidth()
	}
	h.LeftBound, h.RightBound = c.LeftBound, c.RightBound
	h.ScreenLeftMargin, h.ScreenRightMargin = c.ScreenLeftMargin, c.ScreenRightMargin
	h.screenBoundsDisabled = !h.PlayerHelper
	// New helpers enter camera tracking at their first own state pass.
	h.cameraDisabled = [2]bool{true, true}
	h.InputSlot, h.InputSource = c.InputSlot, c.rootPlayer().InputSource
	h.Commands = NewCommandBuffer(120)
	h.RoundState, h.RoundNo, h.RoundsExisted = c.RoundState, c.RoundNo, c.RoundsExisted
	h.Timeline, h.SuperTargetDefenceMul = c.Timeline, c.SuperTargetDefenceMul
	h.helperPauseMoveTime, h.helperSuperMoveTime = p.PauseMoveTime, p.SuperMoveTime
	h.BindSound(c.SoundQueue, c.OwnSounds, c.CommonSounds)
	h.BindEffects(c.EffectQueue, c.CommonEffects)
	h.PaletteNo, h.Palettes = c.PaletteNo, c.Palettes
	if p.OwnPalette && c.Palettes != nil {
		h.Palettes = c.Palettes.Clone()
		if p.RemapPalette[0] != -1 {
			if err := h.SetPaletteMapping([2]int{1, 1}, p.RemapPalette); err != nil {
				return err
			}
		}
	}
	h.HelperName = p.Name
	if !p.Named {
		h.HelperName = c.Name + "'s helper"
	}
	h.applyHelperSize(p.Size)
	x, y, face := c.X, c.Y, c.Facing
	switch p.PosType {
	case "p2":
		if len(c.Opponents) > 0 {
			x, y = c.PositionOf(c.Opponents[0])
			face = c.Opponents[0].Facing
		} else {
			c.EvaluationWarning("Helper postype p2 has no opponent; using p1")
		}
	case "front", "back", "left", "right":
		width := float64(c.LocalCoordinateWidth())
		gameWidth := c.GameCoord.Width
		if gameWidth <= 0 {
			gameWidth = 320
		}
		left := c.cameraPositionX() - width/2
		right := left + width - width/float64(gameWidth)
		x, y = left, 0
		if p.PosType == "right" || p.PosType == "front" && c.Facing > 0 || p.PosType == "back" && c.Facing < 0 {
			x = right
		}
		if p.PosType == "left" || p.PosType == "right" {
			face = 1
		}
	}
	h.X, h.Y, h.Facing = x+p.X*float64(face), y+p.Y, face
	if p.Facing == -1 {
		h.Facing = -h.Facing
	}
	// Player helpers are confined at creation, before their first CNS pass.
	h.constrainPosition()
	h.ChangeAnim(c.Anim)
	if h.RuntimeError != nil {
		return h.RuntimeError
	}
	if err := c.World.addHelper(h); err != nil {
		return err
	}
	c.Helpers = append(c.Helpers, h)
	number := max(0, p.StateNo)
	_, found := h.StateFile.States[number]
	h.changeStateWithEntry(number, engine.EntryOverrides{}, !found)
	if h.RuntimeError != nil {
		return fmt.Errorf("helper %d initial state: %w", h.EntityID, h.RuntimeError)
	}
	return nil
}

// A helper owns its constant overrides without copying immutable state programs.
func (c *Character) applyHelperSize(fields map[string][]float64) {
	if len(fields) == 0 {
		return
	}
	copy := *c.StateFile
	copy.ConstantFields = make(map[string][]float64, len(c.StateFile.ConstantFields)+len(fields))
	for key, values := range c.StateFile.ConstantFields {
		copy.ConstantFields[key] = values
	}
	for key, values := range fields {
		copy.ConstantFields[key] = append([]float64(nil), values...)
		n := values[0]
		switch key {
		case "size.xscale":
			copy.Size.XScale = n
		case "size.yscale":
			copy.Size.YScale = n
		case "size.ground.back":
			copy.Size.GroundBack = int(n)
		case "size.ground.front":
			copy.Size.GroundFront = int(n)
		case "size.air.back":
			copy.Size.AirBack = int(n)
		case "size.air.front":
			copy.Size.AirFront = int(n)
		case "size.height":
			copy.Size.Height = int(n)
		case "size.proj.doscale":
			copy.Size.ProjDoScale = int(n)
		case "size.head.pos":
			copy.Size.HeadPos = []int{int(n), int(values[1])}
		case "size.mid.pos":
			copy.Size.MidPos = []int{int(n), int(values[1])}
		case "size.shadowoffset":
			copy.Size.ShadowOffset = int(n)
		}
	}
	c.StateFile = &copy
}
