package character

import (
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type projectileState struct {
	parameters               engine.ProjectileParameters
	scale                    [2]float64
	pauseMove, superMove     int
	hits, cooldown, priority int
	removing, newAnimation   bool
	prepared                 bool
	preparedTick             int32
}

func (c *Character) CreateProjectile(parameters engine.ProjectileParameters, hit map[string]string) error {
	if err := c.ensureWorld(); err != nil {
		return err
	}
	owner := c.rootPlayer()
	limit := c.World.ProjectileMax
	if limit == 0 {
		limit = 32
	}
	if owner.numProjectiles(0, false) >= max(5, limit) {
		return nil
	}
	p := NewCharacter(c.StateFile, c.AirFile, c.CmdFile)
	p.isProjectile = true
	p.projectile = &projectileState{parameters: parameters, scale: parameters.Scale, pauseMove: parameters.PauseMoveTime, superMove: parameters.SuperMoveTime}
	p.projectile.hits = parameters.Hits
	if p.projectile.hits < 0 {
		p.projectile.hits = 1
	}
	p.projectile.priority = parameters.Priority
	p.projectile.newAnimation = true
	p.AttackMul = c.AttackMul
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
	ratio := float32(1)
	if parameters.CoordinateWidth > 0 {
		ratio = float32(parameters.CoordinateWidth) / float32(c.LocalCoordinateWidth())
		p.LocalCoord.Width = parameters.CoordinateWidth
		p.LocalCoord.Height = max(1, int(float32(c.LocalCoord.Height)*ratio))
	}
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
	// Unlike helpers, a projectile retains the issuing state's coordinate
	// profile. Its AIR/sprite coordinates and HitDef share that same profile.
	x := float64(float32(c.X) * ratio)
	switch parameters.PosType {
	case "p2":
		if len(c.Opponents) > 0 {
			x, _ = c.PositionOf(c.Opponents[0])
			x = float64(float32(x) * ratio)
		} else {
			c.EvaluationWarning("Projectile postype p2 has no opponent; using p1")
		}
	case "left", "right", "front", "back":
		// The beta accepts these four names but anchors all four at the
		// screen center. The isolated native sprite controls distinguish this
		// from the edge placement described in its manual and used by Helper.
		x = float64(float32(c.cameraPositionX()) * ratio)
	}
	p.X = x + parameters.Offset[0]*float64(c.Facing)
	p.Y = float64(float32(c.Y)*ratio) + parameters.Offset[1]
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
	newborn := p.projectile != nil && !p.projectile.prepared
	if p.projectile != nil && p.Timeline != nil {
		if p.projectile.prepared && p.projectile.preparedTick == p.Timeline.GameTime {
			return
		}
		p.projectile.prepared = true
		p.projectile.preparedTick = p.Timeline.GameTime
	}
	p.worldFrozen = p.Timeline != nil && p.Timeline.Paused()
	if p.worldFrozen && p.projectile != nil {
		allowance := &p.projectile.pauseMove
		if p.Timeline.SuperPaused() {
			allowance = &p.projectile.superMove
		}
		if *allowance > 0 {
			if !newborn {
				*allowance--
			}
			p.worldFrozen = false
		}
	}
	wasHitPaused := p.HitPauseTime > 0
	if !p.worldFrozen && !newborn && p.HitPauseTime > 0 {
		p.HitPauseTime--
	}
	p.pausedThisTick = p.HitPauseTime > 0
	if s := p.projectile; s != nil && !s.removing {
		if p.projectileLifetime == 0 {
			p.removeProjectile("expiry")
		} else if !newborn && !p.worldFrozen && !wasHitPaused && p.projectileLifetime > 0 {
			p.projectileLifetime--
		}
	}
	if p.projectile != nil && !p.worldFrozen {
		if !wasHitPaused && p.projectile.cooldown > 0 {
			p.projectile.cooldown--
		}
		if p.projectile.cooldown == 0 && p.HitPauseTime == 0 {
			p.HitGuard = map[int]bool{}
		}
	}
}

func (c *Character) numProjectiles(id int, hasID bool) int {
	count := 0
	for _, p := range c.Projectiles {
		if !p.destroyed && (p.projectile == nil || !p.projectile.removing) && (!hasID || p.projectile != nil && p.projectile.parameters.ID == max(0, id)) {
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

func (p *Character) projectileCanHit() bool {
	s := p.projectile
	return s == nil || !p.destroyed && !s.removing && s.hits > 0 && s.cooldown == 0 && !p.pausedThisTick
}

func (p *Character) projectileContact() {
	s := p.projectile
	if s == nil {
		return
	}
	s.hits--
	s.cooldown = s.parameters.MissTime
	// The beta replaces every measured negative miss interval with two.
	if s.cooldown < 0 {
		s.cooldown = 2
	}
	p.pausedThisTick = p.HitPauseTime > 0
	if s.hits == 0 && s.parameters.Remove {
		p.removeProjectile("hit")
	}
}

func (p *Character) removeProjectile(reason string) {
	s := p.projectile
	if s == nil {
		p.destroyed = true
		return
	}
	if s.removing || p.destroyed {
		return
	}
	s.removing = true
	p.HitDefActive = false
	action := s.parameters.RemoveAnimation
	if reason == "hit" {
		action = s.parameters.HitAnimation
	}
	if reason == "cancel" {
		// Cancellation replaces hit/guard history, but only its wildcard ID
		// matches. ProjContactTime excludes this cancellation event.
		p.rootPlayer().lastProjectileEvent = &projectileEvent{kind: "cancel"}
		action = s.parameters.CancelAnimation
	}
	if action < 0 || p.AirFile == nil || p.AirFile.Actions[action] == nil {
		p.destroyed = true
		return
	}
	p.ChangeAnim(action)
	s.newAnimation = true
	p.VelX, p.VelY = s.parameters.RemoveVelocity[0], s.parameters.RemoveVelocity[1]
}
