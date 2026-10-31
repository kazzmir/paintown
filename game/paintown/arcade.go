package paintown

import (
	"github.com/hajimehoshi/ebiten/v2"
	audiolib "github.com/hajimehoshi/ebiten/v2/audio"
	"github.com/kazzmir/paintown/game/data"
	"github.com/kazzmir/paintown/game/lib/coroutine"
)

// An adventure shares its remaining lives and immutable enemy assets across
// levels. Changing StartingLives only affects the next adventure.
type ArcadeSession struct {
	Lives int
	cache *ObjectFactory
}

func NewArcadeSession() *ArcadeSession {
	return &ArcadeSession{Lives: StartingLives(), cache: MakeObjectFactory()}
}

func (session *ArcadeSession) RunLevel(player *PaintownCharacter, yield coroutine.YieldFunc, setDraw func(data.DrawFunc) data.DrawFunc, path string, audio *audiolib.Context) error {
	if session.Lives <= 0 {
		return ErrReturnToMainMenu
	}
	if session.cache == nil {
		session.cache = MakeObjectFactory()
	}
	return runLevelWithSession(player, yield, setDraw, path, audio, session.cache, session)
}

func arcadeRespawnDelay() int {
	ticks := ebiten.TPS()
	if ticks <= 0 {
		ticks = 60
	}
	return ticks
}

// DropIn is a separate state from combat knockdowns/throws. It cannot attack,
// be interrupted by input, or incur impact damage.
func (player *PlayerState) DropIn() {
	if player.Grabbed != nil {
		player.Grabbed.Ungrab()
	}
	player.Grabbed = nil
	player.Thrown, player.Exploded = false, false
	player.explosion = nil
	player.pendingProjectiles = nil
	player.TrailActive = false
	player.Trails = nil
	player.spawnProtection = 0
	player.Dx, player.Dy = 0, 0
	player.Pain = 0
	player.ActivatedAnimations = make(map[*Animation]uint64)
	for _, animation := range player.Animations {
		animation.Reset()
		animation.OffsetX, animation.OffsetY = 0, 0
		animation.Frame = nil
		clear(animation.KeyPresses)
	}
	player.Status = PlayerStateDropping
	player.Y = max(float64(data.ScreenHeight/2+64), player.Z+64)
	player.ShowAnimation = nil
	for _, name := range []string{"jump", "fall", "idle"} {
		if animation := player.Animations[name]; animation != nil && animation.FirstFrame() != nil {
			animation.Frame = animation.FirstFrame()
			player.ShowAnimation = animation
			break
		}
	}
	if player.ShowAnimation == nil {
		player.ShowAnimation = player.Animations["idle"]
	}
}

func (player *PlayerState) RespawnDrop() {
	player.Health = player.MaxHealth
	player.VisualEffect = VisualEffect{}
	player.Attackers = make(map[Attacker]uint64)
	player.AttackId++ // Surviving enemies must accept the next melee attack.
	player.DropIn()
}

func (model *GameModel) updateArcadeRespawn(player *PlayerState) {
	if !model.Arcade || model.GameOver {
		return
	}
	if player.Status != PlayerStateDead {
		model.awaitingRespawn = false
		model.playerRespawnTicks = 0
		return
	}
	if !model.awaitingRespawn {
		player.Lives = max(0, player.Lives-1)
		model.awaitingRespawn = true
		model.playerRespawnTicks = arcadeRespawnDelay()
		return
	}
	if model.playerRespawnTicks > 0 {
		model.playerRespawnTicks--
		return
	}
	if player.Lives > 0 {
		// Knockback can leave a corpse beyond the stage edges. Bring its
		// respawn back into view before starting the drop.
		if model.LevelLimit > 0 {
			margin := min(40.0, model.LevelLimit/2)
			player.X = max(margin, min(model.LevelLimit-margin, player.X))
		} else {
			player.X = max(0, player.X)
		}
		player.RespawnDrop()
		model.awaitingRespawn = false
	} else if len(model.Gore) == 0 {
		// Let an explosive final death finish before returning to the menu.
		model.GameOver = true
	}
}
