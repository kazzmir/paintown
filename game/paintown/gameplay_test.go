package paintown

import (
	"image"
	"os"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/lib/sexp"
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

func TestGrabPoseStaysStatic(t *testing.T) {
	player := grabTestPlayer()
	grab := MakeAnimation("grab", "", []AnimationEvent{
		&AnimationEventDelay{Delay: 2},
		&AnimationEventFrame{},
		&AnimationEventOffset{X: 20},
		&AnimationEventFrame{},
	}, nil, "", "")
	player.Animations["grab"] = grab
	enemy := grabTestEnemy()
	player.DoGrab(enemy)
	enemy.WasGrabbed(player)

	checkStatic := func(counter uint64) {
		t.Helper()
		event, delay := grab.CurrentEvent, grab.CurrentDelay
		for i := uint64(0); i < 20; i++ {
			player.Update(InputState{HeldRight: true}, &Level{}, &dummySystem{}, counter+i)
		}
		if player.ShowAnimation != grab || grab.CurrentEvent != event || grab.CurrentDelay != delay || grab.OffsetX != 0 {
			t.Fatal("grab hold advanced its animation instead of keeping a static pose")
		}
		if player.Status != PlayerStateGrab || player.Grabbed != enemy || player.X != 0 {
			t.Fatal("static grab pose did not preserve the hold")
		}
	}
	checkStatic(100)

	// Grab attacks still advance and return to the same frozen hold pose.
	punch := MakeAnimation("grab-punch", "attack", nil, nil, "", "grab")
	punch.CurrentDelay = 2
	player.ShowAnimation = punch
	player.Update(InputState{}, &Level{}, &dummySystem{}, 200)
	if punch.CurrentDelay != 1 || player.ShowAnimation != punch {
		t.Fatal("grab attack did not advance")
	}
	player.Update(InputState{}, &Level{}, &dummySystem{}, 201)
	player.Update(InputState{}, &Level{}, &dummySystem{}, 202)
	if player.ShowAnimation != grab {
		t.Fatal("grab attack did not return to the hold pose")
	}
	checkStatic(203)
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

func catTestAnimations() map[string]*Animation {
	out := make(map[string]*Animation)
	for _, name := range []string{"idle1", "idle2", "yawn", "walk", "run", "turn", "sit"} {
		out[name] = &Animation{Name: name}
	}
	return out
}

type catTestSound struct{ sounds []string }

func (sound *catTestSound) PlaySound(path string) error {
	sound.sounds = append(sound.sounds, path)
	return nil
}

func TestCatMovementAndBounds(t *testing.T) {
	level := &Level{ZMinimum: 100, ZMaximum: 200}
	for _, state := range []string{"walk", "run"} {
		cat := &Cat{X: 50, Z: 150, Facing: FacingRight, State: state, Animations: catTestAnimations()}
		cat.Animations[state].CurrentDelay = 100
		cat.update(level, 100, &dummySystem{}, func(int) int { return 1 })
		speed := 0.8
		if state == "run" {
			speed = 2.2
		}
		if !isClose(cat.X, 50-speed) {
			t.Fatalf("%s: wrong movement %v", state, cat.X)
		}
		cat.X, cat.Z = 0, 99
		cat.update(level, 100, &dummySystem{}, func(int) int { return 1 })
		if cat.X != 0 || cat.Facing != FacingLeft || cat.Z != 100 {
			t.Fatal("cat did not stay inside the map and turn around")
		}
		cat.X, cat.Z = 100, 201
		cat.update(level, 100, &dummySystem{}, func(int) int { return 1 })
		if cat.X != 100 || cat.Facing != FacingRight || cat.Z != 200 {
			t.Fatal("cat escaped right/depth boundary")
		}
	}
}

func TestCatTransitionsAndMeow(t *testing.T) {
	cat := &Cat{X: 50, Z: 150, Facing: FacingRight, State: "idle1", Animations: catTestAnimations()}
	sound := &catTestSound{}
	level := &Level{ZMinimum: 100, ZMaximum: 200}
	// Select yawn, then walk, then run when each animation finishes.
	chooseRun := func(n int) int {
		if n == 2000 {
			return 0
		}
		if n == 12 {
			return 4
		}
		return n - 1
	}
	for _, expected := range []string{"yawn", "walk", "run"} {
		cat.update(level, 100, sound, chooseRun)
		if cat.State != expected {
			t.Fatalf("expected %s, got %s", expected, cat.State)
		}
	}
	if len(sound.sounds) != 3 || sound.sounds[0] != catMeowSound {
		t.Fatal("cat did not meow during updates")
	}
	cat.State = "turn"
	cat.update(level, 100, &dummySystem{}, func(int) int { return 1 })
	if cat.State != "walk" || cat.Facing != FacingLeft {
		t.Fatal("turn animation did not reverse direction and resume walking")
	}
}

func TestCatSpawningAndIndependentAnimations(t *testing.T) {
	model := &GameModel{CatFactory: CatFactory{animations: map[string]map[string]*Animation{"cat.txt": catTestAnimations()}}}
	model.CreateCats([]BlockObject{
		{Type: "enemy", Path: "ignored"},
		{Type: "cat", Path: "cat.txt", Coords: image.Pt(20, 5)},
		{Type: "cat", Path: "cat.txt", Coords: image.Pt(40, 10)},
	}, &Level{ZMinimum: 100}, 500)
	if len(model.Cats) != 2 || len(model.Enemies) != 0 || model.Cats[0].X != 520 || model.Cats[0].Z != 105 {
		t.Fatal("incorrect cat spawning or block coordinates")
	}
	model.Cats[0].Animations["walk"].CurrentDelay = 123
	if model.Cats[1].Animations["walk"].CurrentDelay == 123 {
		t.Fatal("cats share mutable animation playback")
	}
}

func TestMenuParsingAndNavigation(t *testing.T) {
	root, err := sexp.ReadSExpressionString(`(menu
		(name main)
		(coordinate (relative -.6 -.2 .6 .7))
		(transforms (radius 15))
		(info-position 0, -.5)
		(font fonts/Neuropol.ttf 24 24)
		(display-list (wrap 1) (items (other-color-alpha 180) (distance-fade-multiplier 40)))
		(option (menu menu/arcade.txt) (info (language English Arcade)))
		(option (quit (name (language Deutsch Beenden) (language English Quit)) (info "Exit the game")))
		;@ (option (platformer (name Disabled)))
	)`)
	if err != nil {
		t.Fatal(err)
	}
	read := func(path string) (*sexp.SExpr, error) {
		if path != "menu/arcade.txt" {
			t.Fatalf("unexpected submenu %s", path)
		}
		return sexp.ReadSExpressionString(`(menu (name Arcade) (info "Play an adventure"))`)
	}
	menu, err := ParseMenu(root, nil, read)
	if err != nil {
		t.Fatal(err)
	}
	if menu.Bounds != image.Rect(128, 192, 512, 408) || menu.Radius != 15 || menu.InfoX != 320 || menu.InfoY != 120 {
		t.Fatalf("incorrect menu positioning: %+v", menu.Bounds)
	}
	if len(menu.Options) != 2 || menu.Options[0].Name != "Arcade" || menu.Options[0].Info != "Arcade" || menu.Options[1].Name != "Quit" {
		t.Fatalf("incorrect localized options: %+v", menu.Options)
	}
	menu.Move(-1)
	if menu.Selected != 1 {
		t.Fatal("up did not wrap to the last option")
	}
	menu.Move(1)
	if menu.Selected != 0 {
		t.Fatal("down did not wrap to the first option")
	}
	menu.Wrap = false
	if menu.Move(-1) || menu.Selected != 0 {
		t.Fatal("non-wrapping selection escaped bounds")
	}
	if menu.itemColor(1).A != 140 || menu.itemColor(5).A != 0 {
		t.Fatal("distance alpha did not fade")
	}
	before := menu.itemColor(0)
	for range 25 {
		menu.Update()
	}
	if menu.itemColor(0) == before {
		t.Fatal("selected option color did not pulse")
	}
	child, _ := sexp.ReadSExpressionString(`(menu (name Arcade) (coordinate (absolute 100 200 450 250)) (display-list (type normal)) (option (adventure (name "Single Player"))))`)
	sub, err := ParseMenu(child, menu, nil)
	if err != nil {
		t.Fatal(err)
	}
	if sub.Bounds != image.Rect(100, 200, 550, 450) || sub.FontPath != menu.FontPath || sub.Scroll || sub.Selected != 0 || len(sub.Options) != 1 {
		t.Fatal("submenu did not apply its layout and inherit its font")
	}
	if menu.Bounds != image.Rect(128, 192, 512, 408) || len(menu.Options) != 2 {
		t.Fatal("submenu mutated parent")
	}
}

func TestMenuAnimationTiming(t *testing.T) {
	root, err := sexp.ReadSExpressionString(`(anim
		(depth foreground top) (basedir menu/bg) (image 0 first.png) (image 1 second.png)
		(velocity -.001 0) (axis .06 .7) (window -.325 -.89 .3 .18)
		(frame (image 0) (time 2) (alpha 15)) (frame (image 1) (time -1) (hflip 1)))`)
	if err != nil {
		t.Fatal(err)
	}
	animation := parseMenuAnimation(root)
	if animation.Depth != 5 || animation.Paths[0] != "menu/bg/first.png" || animation.Frames[0].Alpha != 15 {
		t.Fatal("animation images, depth, or alpha parsed incorrectly")
	}
	if animation.Clip.Min.X != 104 || animation.Clip.Max.X != 544 || !isClose(animation.AxisX, 19.2) {
		t.Fatalf("animation coordinates are wrong: %v", animation.Clip)
	}
	animation.Update()
	if animation.current != 0 {
		t.Fatal("animation advanced too early")
	}
	animation.Update()
	if animation.current != 1 || !animation.Frames[1].HFlip {
		t.Fatal("animation did not reach second frame")
	}
	for range 10 {
		animation.Update()
	}
	if animation.current != 1 || !isClose(animation.scrollX, -3.84) {
		t.Fatal("infinite frame stopped scrolling or advanced")
	}
	animation.Frames[1].Time = 1
	animation.Update()
	if animation.current != 0 {
		t.Fatal("animation did not loop")
	}
}

func TestLoadMainMenuAssets(t *testing.T) {
	t.Chdir("../..")
	if _, err := os.Stat("paintown-data/menu/main.txt"); os.IsNotExist(err) {
		t.Skip("menu assets not installed")
	}
	menu, err := LoadMenu("menu/main.txt", nil)
	if err != nil {
		t.Fatal(err)
	}
	if menu.FontPath != "fonts/Neuropol.ttf" || len(menu.Options) != 8 || menu.Options[0].Name != "Arcade" || menu.Options[4].Name != "Options" {
		t.Fatalf("main menu does not match its definition: %+v", menu.Options)
	}
	if len(menu.Animations) != 1 {
		t.Fatalf("disabled menu effects were loaded: %d", len(menu.Animations))
	}
	if menu.Options[2].Info != "Host a game that can be\nplayed over the internet" {
		t.Fatal("menu info did not decode its line break")
	}
	menu.Update()
	menu.Draw(ebiten.NewImage(640, 480))
	sub, err := LoadMenu("menu/arcade.txt", menu)
	if err != nil {
		t.Fatal(err)
	}
	if len(sub.Options) != 3 || sub.Options[0].Kind != "adventure" || sub.FontPath != menu.FontPath || sub.Scroll {
		t.Fatal("arcade submenu did not load correctly")
	}
	sub.Update()
	sub.Draw(ebiten.NewImage(640, 480))
}

func TestLoadCatAssets(t *testing.T) {
	// Game assets are distributed separately; exercise the real definition
	// when available, while keeping unit tests usable without them.
	t.Chdir("../..")
	if _, err := os.Stat("paintown-data/misc/cat/cat.txt"); os.IsNotExist(err) {
		t.Skip("cat assets not installed")
	}
	var factory CatFactory
	cat, err := factory.Make(BlockObject{Path: "misc/cat/cat.txt"})
	if err != nil {
		t.Fatal(err)
	}
	if cat.Animations["idle1"].CurrentFrame() == nil {
		t.Fatal("cat has no initial frame")
	}
}
