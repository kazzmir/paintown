package paintown

import (
	"image/color"
	"log"
	"math"
	"math/rand/v2"
	"sync/atomic"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/vector"
)

const (
	goreGravity      = 500.0
	goreRestSeconds  = 1.0
	goreFadeSeconds  = 0.4
	maxGoreParticles = 2048
)

var configuredGibAmount atomic.Int32

func init() { configuredGibAmount.Store(5) }

// GibAmount is shared by all levels and the simulator for this session.
func GibAmount() int { return int(configuredGibAmount.Load()) }

// SetGibAmount affects future deaths; existing particles finish normally.
func SetGibAmount(amount int) { configuredGibAmount.Store(int32(max(0, min(5, amount)))) }

// Match the C++ explosive-death rule: a lethal hit with accumulated damage
// above the character's knockdown threshold. Small lethal hits die normally.
func explosiveDamage(health, pain, threshold float64) bool {
	return GibAmount() > 0 && health <= 0 && pain > threshold
}

type goreBurst struct{ X, Y, Z, Force float64 }

type GoreFactory struct {
	frames animationFrames
	parts  []*ebiten.Image
	loaded bool
	err    error
}

func (factory *GoreFactory) Load() error {
	if factory.loaded {
		return factory.err
	}
	factory.loaded = true
	if len(factory.parts) > 0 {
		return nil
	}
	if factory.frames == nil {
		factory.frames = make(animationFrames)
	}
	for _, name := range []string{"arm", "bone", "hand", "intestines", "leg", "ribcage", "skull", "spine", "torso"} {
		frame, err := factory.frames.load("misc/body/" + name + ".png")
		if err != nil {
			factory.err = err
			factory.parts = nil
			return err
		}
		factory.parts = append(factory.parts, frame.Image)
	}
	return nil
}

// Gore has no combat state or attack boxes; falling body parts never hurt
// other characters. Landed stops rotation on the first impact, even if the
// piece bounces again before settling.
type GoreParticle struct {
	X, Y, Z, Vx, Vy, Vz   float64
	Angle, Spin           float64
	Image                 *ebiten.Image
	Blood                 bool
	Radius                float32
	Color                 color.NRGBA
	Landed, Settled, Done bool
	bounces               int
	restTime              float64
}

func (particle *GoreParticle) Update(seconds float64) {
	if particle.Done || seconds <= 0 {
		return
	}
	if particle.Settled {
		particle.restTime += seconds
		if particle.restTime >= goreRestSeconds+goreFadeSeconds {
			particle.Done = true
		}
		return
	}
	if !particle.Landed {
		particle.Angle += particle.Spin * seconds
	}
	particle.X += particle.Vx * seconds
	particle.Z += particle.Vz * seconds
	particle.Y += particle.Vy * seconds
	particle.Vy -= goreGravity * seconds
	if particle.Y <= 0 {
		particle.Y = 0
		particle.Landed, particle.Spin = true, 0
		if !particle.Blood && -particle.Vy > 45 && particle.bounces < 2 {
			particle.Vy = -particle.Vy * .25
			particle.Vx *= .4
			particle.Vz *= .4
			particle.bounces++
		} else {
			particle.Settled = true
			particle.Vx, particle.Vy, particle.Vz = 0, 0, 0
		}
	}
}

func (particle *GoreParticle) Alpha() float32 {
	if particle.Done {
		return 0
	}
	return float32(max(0, min(1, 1-(particle.restTime-goreRestSeconds)/goreFadeSeconds)))
}

func (particle *GoreParticle) Draw(screen *ebiten.Image, cameraX float64, shake ebiten.GeoM) {
	if particle.Done {
		return
	}
	if particle.Blood {
		x, y := shake.Apply(particle.X-cameraX, particle.Z-particle.Y)
		c := particle.Color
		c.A = uint8(float32(c.A) * particle.Alpha())
		vector.FillCircle(screen, float32(x), float32(y), particle.Radius, c, false)
		return
	}
	if particle.Image == nil {
		return
	}
	var options ebiten.DrawImageOptions
	options.GeoM.Translate(-float64(particle.Image.Bounds().Dx())/2, -float64(particle.Image.Bounds().Dy())/2)
	options.GeoM.Rotate(particle.Angle)
	options.GeoM.Translate(particle.X-cameraX, particle.Z-particle.Y)
	options.GeoM.Concat(shake)
	options.ColorScale.ScaleAlpha(particle.Alpha())
	screen.DrawImage(particle.Image, &options)
}

func (enemy *Enemy) explode(force float64) {
	enemy.Ungrab()
	enemy.Exploded = true
	enemy.Health = 0
	enemy.Vx, enemy.Vy = 0, 0
	enemy.State = EnemyStateDead
	enemy.Thrown = false
	enemy.ThrowHits = nil
	enemy.pendingProjectiles = nil
	enemy.explosion = &goreBurst{X: enemy.X, Y: enemy.Y + 32, Z: enemy.Z, Force: force}
}

func (player *PlayerState) explode(force float64) {
	if player.Grabbed != nil {
		player.Grabbed.Ungrab()
	}
	player.Grabbed = nil
	player.Exploded = true
	player.Health = 0
	player.Dx, player.Dy = 0, 0
	player.Status = PlayerStateDead
	player.Thrown = false
	player.TrailActive = false
	player.Trails = nil
	player.pendingProjectiles = nil
	player.ActivatedAnimations = make(map[*Animation]uint64)
	player.explosion = &goreBurst{X: player.X, Y: player.Y + 32, Z: player.Z, Force: force}
}

func (model *GameModel) collectEnemyGore(enemy *Enemy) {
	if enemy.explosion != nil {
		model.goreBursts = append(model.goreBursts, *enemy.explosion)
		enemy.explosion = nil
	}
}

func (model *GameModel) spawnGore(burst goreBurst) {
	amount := GibAmount()
	if amount == 0 {
		return
	}
	if err := model.GoreFactory.Load(); err != nil && !model.goreLoadErrorLogged {
		log.Printf("Loading body parts: %v", err)
		model.goreLoadErrorLogged = true
	}
	parts := model.GoreFactory.parts
	count := (len(parts)*amount + 4) / 5
	// Lower settings show a smaller, varied subset, without duplicating parts.
	order := rand.Perm(len(parts))
	for _, index := range order[:count] {
		image := parts[index]
		if len(model.Gore) >= maxGoreParticles {
			break
		}
		spin := (3 + rand.Float64()*7)
		if rand.N(2) == 0 {
			spin = -spin
		}
		model.Gore = append(model.Gore, &GoreParticle{
			X: burst.X + rand.Float64()*20 - 10, Y: max(5, burst.Y+rand.Float64()*24-12), Z: burst.Z,
			Vx: rand.Float64()*520 - 260 + burst.Force*24, Vy: 240 + rand.Float64()*240, Vz: rand.Float64()*140 - 70,
			Image: image, Angle: rand.Float64() * 2 * math.Pi, Spin: spin,
		})
	}
	for range (64*amount + 4) / 5 {
		if len(model.Gore) >= maxGoreParticles {
			break
		}
		model.Gore = append(model.Gore, &GoreParticle{
			X: burst.X, Y: max(3, burst.Y+rand.Float64()*20-10), Z: burst.Z,
			Vx: rand.Float64()*400 - 200 + burst.Force*15, Vy: 60 + rand.Float64()*240, Vz: rand.Float64()*100 - 50,
			Blood: true, Radius: float32(1 + rand.Float64()*1.5), Color: color.NRGBA{R: uint8(150 + rand.N(90)), A: 220},
		})
	}
}

func (model *GameModel) UpdateGore(player *PlayerState) {
	if player.explosion != nil {
		model.goreBursts = append(model.goreBursts, *player.explosion)
		player.explosion = nil
	}
	// Projectile kills happen after the normal enemy-update pass.
	for _, enemy := range model.Enemies {
		model.collectEnemyGore(enemy)
	}
	tps := ebiten.TPS()
	if tps <= 0 {
		tps = 60
	}
	out := model.Gore[:0]
	for _, particle := range model.Gore {
		particle.Update(1 / float64(tps))
		if !particle.Done {
			out = append(out, particle)
		}
	}
	clear(model.Gore[len(out):])
	model.Gore = out
	for _, burst := range model.goreBursts {
		model.spawnGore(burst)
	}
	model.goreBursts = nil
}
