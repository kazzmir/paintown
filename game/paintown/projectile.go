package paintown

import (
	"fmt"
	"image"
	"log"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/data"
	"github.com/kazzmir/paintown/game/lib/sexp"
)

// Definitions and sprite resources are immutable and shared by animation
// clones. Each emitted projectile gets independent animation/playback state.
type ProjectileDefinition struct {
	Main, Death *Animation
}

type Projectile struct {
	X, Y, Z     float64
	Vx, Vy      float64
	Facing      Facing
	Life        int
	FromEnemy   bool
	HitSound    string
	Animation   *Animation
	death       *Animation
	Dying, Done bool
	AttackId    uint64
}

type projectileOwner interface {
	GetX() float64
	GetY() float64
	GetZ() float64
	GetFacing() Facing
	QueueProjectile(*Projectile)
}

type AnimationEventProjectile struct {
	Definition   *ProjectileDefinition
	X, Y, Vx, Vy float64
	Life         int
}

func loadProjectileDefinition(path string, frames animationFrames) (*ProjectileDefinition, error) {
	root, err := sexp.ReadSExpression(data.DataPath(path))
	if err != nil {
		return nil, err
	}
	if root.Name != "projectile" {
		return nil, fmt.Errorf("%s: expected projectile definition", path)
	}
	definition := &ProjectileDefinition{}
	for _, raw := range root.FindAll("projectile", "anim") {
		animation, err := makeAnimationFromDefinition(filepath.Dir(path), raw, frames)
		if err != nil {
			return nil, err
		}
		switch animation.Name {
		case "main":
			definition.Main = animation
		case "death":
			definition.Death = animation
		}
	}
	if definition.Main == nil {
		return nil, fmt.Errorf("projectile %s: missing main animation", path)
	}
	return definition, nil
}

func parseProjectileEvent(raw *sexp.SExpr, frames animationFrames) (*AnimationEventProjectile, error) {
	path, _ := sexp.ReadValue[string](raw, "path", 0)
	life, _ := sexp.ReadValue[int](raw, "life", 0)
	if path == "" || life <= 0 {
		return nil, fmt.Errorf("projectile requires a path and positive life")
	}
	definition, err := loadProjectileDefinition(path, frames)
	if err != nil {
		return nil, fmt.Errorf("loading projectile %s: %w", path, err)
	}
	x, _ := sexp.ReadValue[float64](raw, "at", 0)
	y, _ := sexp.ReadValue[float64](raw, "at", 1)
	vx, _ := sexp.ReadValue[float64](raw, "speed", 0)
	vy, _ := sexp.ReadValue[float64](raw, "speed", 1)
	return &AnimationEventProjectile{Definition: definition, X: x, Y: y, Vx: vx, Vy: vy, Life: life}, nil
}

func (event *AnimationEventProjectile) Update(animation *Animation, system System) {
	owner, ok := animation.Owner.(projectileOwner)
	if !ok {
		return
	}
	direction := 1.0
	if owner.GetFacing() == FacingLeft {
		direction = -1
	}
	projectile := &Projectile{
		X: owner.GetX() + direction*(float64(animation.GetOffsetX())+event.X),
		Y: owner.GetY() - float64(animation.GetOffsetY()) - event.Y,
		Z: owner.GetZ(), Facing: owner.GetFacing(), Vx: direction * event.Vx, Vy: event.Vy,
		Life: event.Life, AttackId: 1, Animation: event.Definition.Main.Clone(),
	}
	switch source := animation.Owner.(type) {
	case *PlayerState:
		projectile.HitSound = source.HitSound
	case *Enemy:
		projectile.FromEnemy = true
		projectile.HitSound = source.HitSound
	}
	projectile.Animation.Owner = projectile
	projectile.Animation.Reset()
	projectile.Animation.Update(false, system)
	if event.Definition.Death != nil {
		projectile.death = event.Definition.Death.Clone()
		projectile.death.Owner = projectile
	}
	owner.QueueProjectile(projectile)
}

func (player *PlayerState) QueueProjectile(projectile *Projectile) {
	player.pendingProjectiles = append(player.pendingProjectiles, projectile)
}

func (enemy *Enemy) QueueProjectile(projectile *Projectile) {
	enemy.pendingProjectiles = append(enemy.pendingProjectiles, projectile)
}

func (projectile *Projectile) Move(x, y, z int) {
	projectile.X += float64(x)
	projectile.Y += float64(y)
	projectile.Z += float64(z)
}
func (projectile *Projectile) GetFacing() Facing             { return projectile.Facing }
func (projectile *Projectile) SetFacing(facing Facing)       { projectile.Facing = facing }
func (projectile *Projectile) SetTrail(generate, length int) {}
func (projectile *Projectile) NextAttackId()                 { projectile.AttackId++ }
func (projectile *Projectile) GetAttackId() uint64           { return projectile.AttackId }

func (projectile *Projectile) finish(system System) {
	if projectile.Dying || projectile.Done {
		return
	}
	projectile.Life, projectile.Vx, projectile.Vy = 0, 0, 0
	projectile.Dying = true
	if projectile.death == nil {
		projectile.Done = true
		return
	}
	projectile.Animation = projectile.death
	projectile.Animation.Reset()
	if projectile.Animation.Update(false, system) && projectile.Animation.CurrentDelay == 0 {
		projectile.Done = true
	}
}

func (projectile *Projectile) Update(system System) {
	if projectile.Done {
		return
	}
	if projectile.Dying {
		if projectile.Animation.Update(false, system) && projectile.Animation.CurrentDelay == 0 {
			projectile.Done = true
		}
		return
	}
	projectile.X += projectile.Vx
	projectile.Y += projectile.Vy
	// Hold the final frame for its full delay before looping. Resetting the
	// general-purpose animation early would also clear the projectile's attack.
	if projectile.Animation.Update(false, system) && projectile.Animation.CurrentDelay == 0 {
		projectile.Animation.Reset()
		projectile.Animation.Update(false, system)
	}
	projectile.Life--
	if projectile.Life <= 0 {
		projectile.finish(system)
	}
}

// Drawing and collision use the same centered-bottom sprite transform.
func (projectile *Projectile) transform(cameraX float64) ebiten.GeoM {
	animation := projectile.Animation
	var geom ebiten.GeoM
	frame := animation.CurrentFrame()
	if frame == nil {
		return geom
	}
	geom.Translate(-float64(frame.Bounds().Dx())/2+float64(animation.GetOffsetX()), -float64(frame.Bounds().Dy())+float64(animation.GetOffsetY()))
	if projectile.Facing == FacingLeft {
		geom.Scale(-1, 1)
	}
	geom.Translate(projectile.X-cameraX, projectile.Z-projectile.Y)
	return geom
}

func (projectile *Projectile) GetAttackBox() image.Rectangle {
	if projectile.Animation == nil || projectile.Animation.CurrentFrame() == nil {
		return image.Rectangle{}
	}
	attack := projectile.Animation.Attack
	if attack.IsEmpty() {
		return image.Rectangle{}
	}
	geom := projectile.transform(0)
	x1, y1 := geom.Apply(float64(attack.X1), float64(attack.Y1))
	x2, y2 := geom.Apply(float64(attack.X2), float64(attack.Y2))
	return image.Rect(int(x1), int(y1), int(x2), int(y2)).Canon()
}

func (projectile *Projectile) Draw(screen *ebiten.Image, cameraX float64, shake ebiten.GeoM) {
	if projectile.Done || projectile.Animation == nil || projectile.Animation.CurrentFrame() == nil {
		return
	}
	var options ebiten.DrawImageOptions
	options.GeoM = projectile.transform(cameraX)
	options.GeoM.Concat(shake)
	screen.DrawImage(projectile.Animation.CurrentFrame(), &options)
}

func (model *GameModel) UpdateProjectiles(player *PlayerState, system System, flashes *FlashFactory) {
	model.Projectiles = append(model.Projectiles, player.pendingProjectiles...)
	player.pendingProjectiles = nil
	out := model.Projectiles[:0]
	for _, projectile := range model.Projectiles {
		projectile.Update(system)
		if !projectile.Dying && !projectile.Done && !projectile.Animation.Attack.IsEmpty() {
			attack := projectile.Animation.Attack
			force := attack.Force
			if projectile.Facing == FacingLeft {
				force = -force
			}
			box := projectile.GetAttackBox()
			hit := false
			hitX, hitY, hitZ := 0.0, 0.0, 0.0
			if projectile.FromEnemy {
				if abs(player.Z-projectile.Z) < Z_DISTANCE && player.CanBeHit(projectile, projectile.AttackId) && player.HitBy(box) {
					player.Hurt(projectile, attack.Damage, force)
					hit, hitX, hitY, hitZ = true, player.X, player.Y, player.Z
				}
			} else {
				for _, enemy := range model.Enemies {
					if abs(enemy.Z-projectile.Z) >= Z_DISTANCE || enemy.State == EnemyStateFalling || enemy.State == EnemyStateFallen || enemy.IsDead() {
						continue
					}
					if enemy.HitBy(box) {
						healthBefore := enemy.Health
						// Projectiles are independent attacks, not tickets from
						// the player's melee sequence. Do not overwrite it.
						enemy.Hurt(enemy.LastAttacked, attack.Damage, force)
						model.UpdateHealthMap(enemy, model.Counter)
						if healthBefore > 0 && enemy.Health <= 0 {
							system.PlaySound(enemy.DieSound)
						}
						hit, hitX, hitY, hitZ = true, enemy.X, enemy.Y, enemy.Z
						break
					}
				}
			}
			if hit {
				if flashes != nil {
					model.Flashes = append(model.Flashes, flashes.MakeFlash(hitX, hitY+50, hitZ+.1))
				}
				if err := system.PlaySound(projectile.HitSound); err != nil {
					log.Printf("Projectile hit sound: %v", err)
				}
				projectile.finish(system)
			}
		}
		if !projectile.Done {
			out = append(out, projectile)
		}
	}
	// Clear removed references so dead projectiles do not retain their owner
	// resources through the slice's spare capacity.
	clear(model.Projectiles[len(out):])
	model.Projectiles = out
}
