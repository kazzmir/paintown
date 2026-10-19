package character

import (
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type projectileState struct {
	parameters           engine.ProjectileParameters
	scale                [2]float64
	pauseMove, superMove int
}

func (c *Character) CreateProjectile(parameters engine.ProjectileParameters, hit map[string]string) error {
	if err := c.ensureWorld(); err != nil {
		return err
	}
	owner := c.rootPlayer()
	p := NewCharacter(c.StateFile, c.AirFile, c.CmdFile)
	p.isProjectile = true
	p.projectile = &projectileState{parameters: parameters, scale: parameters.Scale, pauseMove: parameters.PauseMoveTime, superMove: parameters.SuperMoveTime}
	if c.StateFile != nil && c.StateFile.Size.ProjDoScale != 0 {
		x, y := c.spriteScale()
		p.projectile.scale[0] *= x
		p.projectile.scale[1] *= y
	}
	p.World = c.World
	p.Parent, p.Root = owner, owner
	p.TeamSide, p.InputSlot = owner.TeamSide, owner.InputSlot
	p.BindSound(c.SoundQueue, c.OwnSounds, c.CommonSounds)
	p.BindEffects(c.EffectQueue, c.CommonEffects)
	p.LocalCoord, p.GameCoord, p.Viewport = c.LocalCoord, c.GameCoord, c.Viewport
	p.Timeline, p.SuperTargetDefenceMul = c.Timeline, c.SuperTargetDefenceMul
	p.PowerRules = c.PowerRules
	p.Palettes, p.PaletteNo = c.Palettes, c.PaletteNo
	if parameters.OwnPalette && p.Palettes != nil {
		p.Palettes = p.Palettes.Clone()
		if parameters.RemapPalette[0] != -1 {
			if err := p.SetPaletteMapping([2]int{1, 1}, parameters.RemapPalette); err != nil {
				return err
			}
		}
	}
	p.Facing = c.Facing
	p.Anim = parameters.Action
	p.ChangeAnim(parameters.Action)
	x := c.X
	switch parameters.PosType {
	case "p2":
		if len(c.Opponents) > 0 {
			x, _ = c.PositionOf(c.Opponents[0])
		} else {
			c.EvaluationWarning("Projectile postype p2 has no opponent; using p1")
		}
	case "left", "right", "front", "back":
		// The beta accepts these four names but anchors all four at the
		// screen center. The isolated native sprite controls distinguish this
		// from the edge placement described in its manual and used by Helper.
		x = c.cameraPositionX()
	}
	p.X = x + parameters.Offset[0]*float64(c.Facing)
	p.Y = c.Y + parameters.Offset[1]
	p.VelX, p.VelY = parameters.Velocity[0], parameters.Velocity[1]
	p.SprPriority = parameters.SpritePriority
	p.Opponents = owner.Opponents
	p.projectileLifetime = parameters.RemoveTime
	if p.projectileLifetime < -1 {
		p.projectileLifetime = 1
	}
	p.HitDef(hit)
	owner.Projectiles = append(owner.Projectiles, p)
	return p.RuntimeError
}

func (p *Character) prepareProjectileTick() {
	p.worldFrozen = p.Timeline != nil && p.Timeline.Paused()
	if p.worldFrozen && p.projectile != nil {
		allowance := &p.projectile.pauseMove
		if p.Timeline.SuperPaused() {
			allowance = &p.projectile.superMove
		}
		if *allowance > 0 {
			*allowance--
			p.worldFrozen = false
		}
	}
	if !p.worldFrozen && p.HitPauseTime > 0 {
		p.HitPauseTime--
	}
	p.pausedThisTick = p.HitPauseTime > 0
}

func (c *Character) numProjectiles(id int, hasID bool) int {
	count := 0
	for _, p := range c.Projectiles {
		if !p.destroyed && (!hasID || p.projectile != nil && p.projectile.parameters.ID == max(0, id)) {
			count++
		}
	}
	return count
}

func (c *Character) lookupProjectileCount(name string, args []evaluator.Value) (evaluator.Value, bool) {
	if name != "numprojid" {
		return evaluator.Value{}, false
	}
	if len(args) != 1 {
		c.EvaluationWarning("NumProjID requires one ID")
		return evaluator.Value{}, true
	}
	id, err := evaluator.CoerceInt(args[0], c, "NumProjID ID")
	if err != nil {
		c.EvaluationWarning(err.Error())
		return evaluator.Value{}, true
	}
	return evaluator.Int(int32(c.numProjectiles(int(id), true))), true
}
