package paintown

import (
	"image"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
)

func grabTestEnemy() *Enemy {
	collision := &Collision{width: 40, height: 60, MainBox: image.Rect(0, 0, 40, 60), Boxes: []image.Rectangle{image.Rect(0, 0, 40, 60)}}
	animations := make(map[string]*Animation)
	for _, name := range []string{"idle", "pain", "fall", "rise"} {
		animations[name] = &Animation{Name: name, Collision: map[*ebiten.Image]*Collision{nil: collision}}
	}
	return &Enemy{Health: 100, PainThreshold: 7, Animations: animations, CurrentAnimationValue: animations["idle"]}
}

func grabTestPlayer() *PlayerState {
	return &PlayerState{Health: 100, PainThreshold: 10, Animations: map[string]*Animation{
		"grab": {Name: "grab"}, "pain": {Name: "pain"}, "fall": {Name: "fall"},
		"idle": {Name: "idle"}, "walk": {Name: "walk"},
	}, Attackers: make(map[Attacker]uint64)}
}

func TestGrabReleasedByPlayerHurt(t *testing.T) {
	player := grabTestPlayer()
	enemy := grabTestEnemy()
	player.DoGrab(enemy)
	enemy.WasGrabbed(player)
	enemy.Hurt(1, 1, 1)
	if enemy.State != EnemyStateGrabbed || player.Grabbed != enemy {
		t.Fatal("a light grab attack broke the hold")
	}
	player.Hurt(grabTestEnemy(), 1, 1)
	if player.Status != PlayerStatePain || player.Grabbed != nil || enemy.Grabber != nil || enemy.State != EnemyStateIdle {
		t.Fatal("player hurt did not release both sides of the grab")
	}
}

func TestGrabTimeoutAndKnockdown(t *testing.T) {
	for _, timeout := range []bool{true, false} {
		player := grabTestPlayer()
		enemy := grabTestEnemy()
		player.DoGrab(enemy)
		enemy.WasGrabbed(player)
		if timeout {
			enemy.GrabTime = 1
			enemy.UpdateState(nil, player)
		} else {
			enemy.Hurt(1, 10, 2)
		}
		if player.Grabbed != nil || enemy.Grabber != nil || player.ShowAnimation != nil || player.Status != PlayerIdle {
			t.Fatal("release left stale grab state")
		}
	}
}

func TestThrownEnemyDoesNotChainProjectiles(t *testing.T) {
	player := grabTestPlayer()
	thrown := grabTestEnemy()
	target := grabTestEnemy()
	target.PainThreshold = 1
	player.DoGrab(thrown)
	thrown.WasGrabbed(player)
	thrown.Throw(3, 5)
	if player.Grabbed != nil || thrown.Grabber != nil || !thrown.Thrown {
		t.Fatal("throw did not detach the hold and enable projectile state")
	}
	model := &GameModel{Enemies: []*Enemy{thrown, target}}
	model.UpdateThrownEnemies()
	if target.Health != 95 || target.State != EnemyStateFalling || target.Thrown {
		t.Fatal("thrown body must knock down its target without making it a projectile")
	}
	target.State = EnemyStateIdle
	model.UpdateThrownEnemies()
	if target.Health != 95 {
		t.Fatal("thrown body hit the same target twice")
	}
	bystander := grabTestEnemy()
	model.Enemies = []*Enemy{target, bystander}
	target.State = EnemyStateFalling
	model.UpdateThrownEnemies()
	if bystander.Health != 100 {
		t.Fatal("ordinary falling enemy caused chain damage")
	}
	thrown.Y, thrown.Vy = 0, -1
	thrown.UpdateState(nil, player)
	if thrown.Thrown || thrown.State != EnemyStateFallen {
		t.Fatal("landing did not disable projectile state")
	}
}

func TestGrabMoveSelectionAndThrow(t *testing.T) {
	player := grabTestPlayer()
	enemy := grabTestEnemy()
	punch := MakeAnimation("grab-punch", "attack", nil, []InputKey{InputKeyAttack1}, "", "grab")
	throw := MakeAnimation("throw", "attack", nil, []InputKey{InputKeyAttack2}, "", "grab")
	ground := MakeAnimation("ground-punch", "attack", nil, []InputKey{InputKeyAttack1}, "", "ground")
	player.Animations["grab-punch"] = punch
	player.Animations["throw"] = throw
	player.DoGrab(enemy)
	enemy.WasGrabbed(player)
	player.ActivatedAnimations[ground] = 100
	player.Update(InputState{Attack1: true}, &Level{}, &dummySystem{}, 100)
	if player.ShowAnimation != punch || player.Status != PlayerStateGrab {
		t.Fatal("grab input did not select a grab-only attack")
	}
	player.Update(InputState{Attack2: true}, &Level{}, &dummySystem{}, 101)
	if player.ShowAnimation != throw || player.Status != PlayerIdle || player.Grabbed != nil || !enemy.Thrown {
		t.Fatal("throw input did not throw and release the grabbed enemy")
	}
}

// Unlike the stationary simulator targets, normal enemies can already be
// attacking and stand a few pixels off the player's lane.
func TestGrabGroundedEnemyInNormalGameLoop(t *testing.T) {
	for _, state := range []EnemyState{EnemyStateIdle, EnemyStateWalking, EnemyStateAttacking, EnemyStatePain} {
		t.Run(state.String(), func(t *testing.T) {
			player := grabTestPlayer()
			player.Facing = FacingRight
			enemy := grabTestEnemy()
			enemy.State = state
			enemy.X = 40
			enemy.Z = 8
			enemy.CurrentAnimationValue = &Animation{CurrentDelay: 20, Attack: AnimationAttack{
				X1: 1, Y1: 1, X2: 40, Y2: 60, Damage: 5,
			}}
			model := &GameModel{Enemies: []*Enemy{enemy}, LevelLimit: 1000}
			level := &Level{ZMaximum: 100}
			model.UpdatePlayer(player, InputState{HeldRight: true}, level, &dummySystem{}, nil)
			model.UpdateEnemies(level, player, &dummySystem{}, nil)
			if player.Status != PlayerStateGrab || player.Grabbed != enemy || enemy.State != EnemyStateGrabbed || enemy.Grabber != player {
				t.Fatal("normal game updates did not establish and preserve the grab")
			}
			if player.Health != 100 {
				t.Fatal("interrupted enemy attack still hit the player")
			}
		})
	}
}

func TestGrabRequiresMovingTowardEnemy(t *testing.T) {
	for _, toward := range []bool{false, true} {
		player := grabTestPlayer()
		player.Facing = FacingRight
		enemy := grabTestEnemy()
		enemy.X = 40
		model := &GameModel{Enemies: []*Enemy{enemy}, LevelLimit: 1000}
		model.UpdatePlayer(player, InputState{HeldRight: toward, HeldDown: !toward}, &Level{ZMaximum: 100}, &dummySystem{}, nil)
		if (player.Status == PlayerStateGrab) != toward {
			t.Fatalf("moving toward=%v: got %v", toward, player.Status)
		}
		if toward && (enemy.State != EnemyStateGrabbed || enemy.X != player.X+36) {
			t.Fatal("grab did not position and hold enemy")
		}
	}
}
