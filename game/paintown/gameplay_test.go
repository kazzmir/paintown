package paintown

import (
	"encoding/binary"
	"errors"
	"image"
	"image/color"
	"image/png"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/data"
	"github.com/kazzmir/paintown/game/lib/sexp"
	"golang.org/x/image/font/gofont/goregular"
	"golang.org/x/image/font/sfnt"
	"golang.org/x/image/math/fixed"
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

func projectileTestEvent() *AnimationEventProjectile {
	frame := ebiten.NewImage(20, 20)
	main := MakeAnimation("main", "attack", []AnimationEvent{
		&AnimationEventAttack{Attack: AnimationAttack{X2: 20, Y2: 20, Damage: 3, Force: 2}},
		&AnimationEventDelay{Delay: 2}, &AnimationEventFrame{Image: frame}, &AnimationEventNop{},
	}, nil, "", "")
	death := MakeAnimation("death", "", []AnimationEvent{
		&AnimationEventDelay{Delay: 2}, &AnimationEventFrame{Image: frame}, &AnimationEventFrame{Image: frame}, &AnimationEventNop{},
	}, nil, "", "")
	return &AnimationEventProjectile{Definition: &ProjectileDefinition{Main: main, Death: death}, X: 40, Y: -10, Vx: 2, Life: 70}
}

func TestProjectileEventPlacementAndIndependentPlayback(t *testing.T) {
	for _, facing := range []Facing{FacingRight, FacingLeft} {
		player := grabTestPlayer()
		player.X, player.Y, player.Z, player.Facing = 100, 8, 150, facing
		animation := &Animation{Owner: player, OffsetX: 5, OffsetY: 7}
		event := projectileTestEvent()
		event.Update(animation, &dummySystem{})
		event.Update(animation, &dummySystem{})
		first, second := player.pendingProjectiles[0], player.pendingProjectiles[1]
		direction := 1.0
		if facing == FacingLeft {
			direction = -1
		}
		if first.X != 100+direction*45 || first.Y != 11 || first.Z != 150 || first.Vx != direction*2 || first.Facing != facing {
			t.Fatal("projectile did not inherit facing and sprite-relative spawn position")
		}
		first.Animation.CurrentDelay = 99
		if second.Animation.CurrentDelay == 99 || first.Animation == second.Animation || first.Animation.Owner != first || second.Animation.Owner != second {
			t.Fatal("projectiles share mutable animation playback")
		}
		if first.Animation.CurrentFrame() != second.Animation.CurrentFrame() {
			t.Fatal("projectiles did not share sprite resources")
		}
	}
}

func TestProjectileAnimationKeepsLastFrameAndAttack(t *testing.T) {
	player := grabTestPlayer()
	event := projectileTestEvent()
	// A one-frame main animation still needs an active attack throughout
	// its frame delay and subsequent loops.
	event.Definition.Main.Events = event.Definition.Main.Events[:3]
	event.Definition.Death.Events = event.Definition.Death.Events[:2]
	event.Update(&Animation{Owner: player}, &dummySystem{})
	projectile := player.pendingProjectiles[0]
	for range 20 {
		projectile.Update(&dummySystem{})
		if projectile.Animation.Attack.IsEmpty() || projectile.Animation.CurrentFrame() == nil {
			t.Fatal("looping projectile lost its last frame or attack")
		}
	}
	projectile.finish(&dummySystem{})
	if projectile.Done || !projectile.Dying {
		t.Fatal("death animation skipped its only frame")
	}
	for range 2 {
		projectile.Update(&dummySystem{})
		if projectile.Done {
			t.Fatal("death animation did not hold its frame for its full delay")
		}
	}
	projectile.Update(&dummySystem{})
	if !projectile.Done {
		t.Fatal("death animation did not finish")
	}
}

func TestProjectileHitsOneEnemyThenExpires(t *testing.T) {
	for _, facing := range []Facing{FacingRight, FacingLeft} {
		player := grabTestPlayer()
		player.Z, player.Facing = 100, facing
		event := projectileTestEvent()
		event.Update(&Animation{Owner: player}, &dummySystem{})
		projectile := player.pendingProjectiles[0]
		targetX := projectile.X + projectile.Vx
		first, second, otherLane := grabTestEnemy(), grabTestEnemy(), grabTestEnemy()
		for _, enemy := range []*Enemy{first, second, otherLane} {
			enemy.X, enemy.Z, enemy.State = targetX, 100, EnemyStatePain
			enemy.PainThreshold, enemy.LastAttacked = 100, 999
		}
		otherLane.Z += 20
		model := &GameModel{Enemies: []*Enemy{otherLane, first, second}}
		model.UpdateEnemies(&Level{}, player, &dummySystem{}, nil)
		if first.Health != 97 || second.Health != 100 || otherLane.Health != 100 || first.LastAttacked != 999 {
			t.Fatal("projectile failed damage, single-target, lane, or independent ticket checks")
		}
		if len(model.Projectiles) != 1 || !projectile.Dying || projectile.Vx != 0 || len(player.pendingProjectiles) != 0 {
			t.Fatal("hit projectile did not stop and enter its death animation")
		}
		x := projectile.X
		for range 30 {
			model.UpdateProjectiles(player, &dummySystem{}, nil)
		}
		if len(model.Projectiles) != 0 || first.Health != 97 || second.Health != 100 || projectile.X != x {
			t.Fatal("death animation repeated damage, kept moving, or failed to clean up")
		}
	}
}

func TestProjectileLifetimeAndEnemyOwnership(t *testing.T) {
	player := grabTestPlayer()
	player.Z = 100
	event := projectileTestEvent()
	event.Life = 2
	event.Update(&Animation{Owner: player}, &dummySystem{})
	projectile := player.pendingProjectiles[0]
	model := &GameModel{}
	model.UpdateProjectiles(player, &dummySystem{}, nil)
	if projectile.Life != 1 || projectile.X != 42 || projectile.Dying {
		t.Fatal("projectile movement/lifetime incorrect")
	}
	model.UpdateProjectiles(player, &dummySystem{}, nil)
	if !projectile.Dying || projectile.Life != 0 {
		t.Fatal("expired projectile did not enter its death animation")
	}
	for range 30 {
		model.UpdateProjectiles(player, &dummySystem{}, nil)
	}
	if len(model.Projectiles) != 0 {
		t.Fatal("expired projectile was not removed")
	}
	// A projectile without a death animation is removed immediately at expiry.
	event.Definition.Death, event.Life = nil, 1
	event.Update(&Animation{Owner: player}, &dummySystem{})
	model.UpdateProjectiles(player, &dummySystem{}, nil)
	if len(model.Projectiles) != 0 {
		t.Fatal("projectile without death animation was retained")
	}

	// Enemy projectiles hit the player, not their allies, and interrupt grabs.
	source, held := grabTestEnemy(), grabTestEnemy()
	source.Z, source.State = 100, EnemyStatePain
	player.X = 42
	player.Animations["grab"] = grabTestEnemy().Animations["idle"]
	player.DoGrab(held)
	held.WasGrabbed(player)
	event = projectileTestEvent()
	event.Update(&Animation{Owner: source}, &dummySystem{})
	model.Enemies = []*Enemy{source}
	model.UpdateEnemies(&Level{}, player, &dummySystem{}, nil)
	if player.Health != 97 || player.Status != PlayerStatePain || player.Grabbed != nil || held.Grabber != nil || source.Health != 100 {
		t.Fatal("enemy projectile failed to damage the player and release the hold, or damaged its ally")
	}
}

func TestAkumaProjectileMoves(t *testing.T) {
	t.Chdir("../..")
	if _, err := os.Stat("paintown-data/players/akuma/akuma.txt"); os.IsNotExist(err) {
		t.Skip("Akuma assets not installed")
	}
	character, err := MakePaintownPlayer("akuma")
	if err != nil {
		t.Fatal(err)
	}
	for _, move := range []struct {
		name   string
		damage float64
		speed  float64
		super  bool
	}{
		{"haddoken", 8, 2, false}, {"super-haddoken", 17, 2.3, true},
	} {
		for _, facing := range []Facing{FacingRight, FacingLeft} {
			t.Run(move.name+"/"+map[Facing]string{FacingRight: "right", FacingLeft: "left"}[facing], func(t *testing.T) {
				level := &Level{ZMinimum: 100, ZMaximum: 200}
				player, err := MakePlayerState(character, level)
				if err != nil {
					t.Fatal(err)
				}
				player.X, player.Facing = 100, facing
				forward := InputState{Right: true}
				if facing == FacingLeft {
					forward = InputState{Left: true}
				}
				inputs := []InputState{{Down: true}, forward}
				if move.super {
					inputs = append(inputs, InputState{Down: true}, forward)
				}
				inputs = append(inputs, InputState{Attack1: true})
				for index, input := range inputs {
					player.Update(input, level, &dummySystem{}, uint64(100+index))
				}
				if player.ShowAnimation == nil || player.ShowAnimation.Name != move.name {
					t.Fatalf("combo did not select %s", move.name)
				}
				for tick := 200; tick < 500 && len(player.pendingProjectiles) == 0; tick++ {
					player.Update(InputState{}, level, &dummySystem{}, uint64(tick))
				}
				if len(player.pendingProjectiles) != 1 {
					t.Fatal("Akuma attack did not emit its configured projectile")
				}
				projectile := player.pendingProjectiles[0]
				direction := 1.0
				if facing == FacingLeft {
					direction = -1
				}
				if projectile.Life != 70 || projectile.Vx != direction*move.speed || projectile.Animation.Attack.Damage != move.damage {
					t.Fatal("Akuma projectile settings do not match the data")
				}
				box := projectile.GetAttackBox()
				target := grabTestEnemy()
				target.X, target.Z, target.State = float64(box.Min.X+box.Max.X)/2, player.Z, EnemyStatePain
				model := &GameModel{Enemies: []*Enemy{target}}
				model.UpdateEnemies(level, player, &dummySystem{}, nil)
				if target.Health != 100-move.damage || !projectile.Dying {
					t.Fatal("Akuma projectile did not collide and apply its configured damage")
				}
			})
		}
	}
}

// Build a small Unicode cmap over the real test font, allowing individual
// characters to be missing/blank/fallback without making the TTF malformed.
func menuTestFontMappings(t *testing.T, overrides map[rune]sfnt.GlyphIndex) []byte {
	t.Helper()
	raw := append([]byte(nil), goregular.TTF...)
	font, err := sfnt.Parse(raw)
	if err != nil {
		t.Fatal(err)
	}
	const count = 96 // printable ASCII plus the required 0xffff sentinel.
	table := make([]byte, 12+16+8*count)
	put := binary.BigEndian.PutUint16
	put(table[2:], 1)
	put(table[4:], 3)
	put(table[6:], 1)
	binary.BigEndian.PutUint32(table[8:], 12)
	sub := table[12:]
	put(sub, 4)
	put(sub[2:], uint16(len(sub)))
	put(sub[6:], count*2)
	put(sub[8:], 128)
	put(sub[10:], 6)
	put(sub[12:], count*2-128)
	var buffer sfnt.Buffer
	for index := range count {
		character := rune(32 + index)
		glyph := sfnt.GlyphIndex(0)
		if index == count-1 {
			character = 0xffff
		} else {
			glyph, err = font.GlyphIndex(&buffer, character)
			if err != nil {
				t.Fatal(err)
			}
			if replacement, ok := overrides[character]; ok {
				glyph = replacement
			}
		}
		put(sub[14+index*2:], uint16(character))
		put(sub[16+count*2+index*2:], uint16(character))
		put(sub[16+count*4+index*2:], uint16(glyph)-uint16(character))
	}
	for len(raw)%4 != 0 {
		raw = append(raw, 0)
	}
	offset := len(raw)
	raw = append(raw, table...)
	for index := 0; index < int(binary.BigEndian.Uint16(raw[4:])); index++ {
		entry := raw[12+16*index : 12+16*(index+1)]
		if string(entry[:4]) == "cmap" {
			binary.BigEndian.PutUint32(entry[8:], uint32(offset))
			binary.BigEndian.PutUint32(entry[12:], uint32(len(table)))
			return raw
		}
	}
	t.Fatal("test font has no cmap")
	return nil
}

func TestMenuFontRejectsMissingBlankAndFallbackGlyphs(t *testing.T) {
	isolateMenuFontPreferences(t)
	font, err := sfnt.Parse(goregular.TTF)
	if err != nil {
		t.Fatal(err)
	}
	var buffer sfnt.Buffer
	space, _ := font.GlyphIndex(&buffer, ' ')
	placeholder, _ := font.GlyphIndex(&buffer, '?')
	root := t.TempDir()
	good := filepath.Join(root, "good.ttf")
	if err := os.WriteFile(good, goregular.TTF, 0600); err != nil {
		t.Fatal(err)
	}
	for _, item := range []struct {
		name      string
		overrides map[rune]sfnt.GlyphIndex
	}{
		{"missing-a", map[rune]sfnt.GlyphIndex{'a': 0}},
		{"missing-b", map[rune]sfnt.GlyphIndex{'b': 0}},
		{"blank-a", map[rune]sfnt.GlyphIndex{'a': space}},
		{"same-placeholder", map[rune]sfnt.GlyphIndex{'a': placeholder, 'b': placeholder}},
	} {
		path := filepath.Join(root, item.name+".ttf")
		if err := os.WriteFile(path, menuTestFontMappings(t, item.overrides), 0600); err != nil {
			t.Fatal(err)
		}
		if usableMenuFontFile(path) {
			t.Fatalf("accepted unusable font %s", item.name)
		}
		if _, err := loadMenuFontSource(path); err == nil {
			t.Fatalf("direct selection accepted unusable font %s", item.name)
		}
		menu := defaultMenu()
		menu.FontPath = path
		for _, option := range menu.fontFiles() {
			if option.Path == canonicalFontPath(path) {
				t.Fatal("unusable current theme font bypassed picker filtering")
			}
		}
	}
	if err := os.WriteFile(filepath.Join(root, "broken.ttf"), []byte("not a font"), 0600); err != nil {
		t.Fatal(err)
	}
	fonts := discoverMenuFonts([]string{root})
	if len(fonts) != 1 || fonts[0].Path != canonicalFontPath(good) {
		t.Fatalf("font picker did not filter unusable fonts: %+v", fonts)
	}
	menu := defaultMenu()
	menu.FontPath = good
	if err := menu.loadAssets(); err != nil {
		t.Fatal(err)
	}
	before := currentMenuFontConfig()
	menu.ApplySetting(MenuOption{Kind: "set-font", Path: filepath.Join(root, "missing-a.ttf")})
	if currentMenuFontConfig() != before || !strings.Contains(menu.Notice, "Could not load font") {
		t.Fatal("rejected font changed menu preferences or failed silently")
	}
}

func TestMenuFontDetectsHollowReplacementBoxes(t *testing.T) {
	rectangle := func(x, y, size int) sfnt.Segments {
		points := []fixed.Point26_6{{X: fixed.I(x), Y: fixed.I(y)}, {X: fixed.I(x + size), Y: fixed.I(y)}, {X: fixed.I(x + size), Y: fixed.I(y + size)}, {X: fixed.I(x), Y: fixed.I(y + size)}, {X: fixed.I(x), Y: fixed.I(y)}}
		out := make(sfnt.Segments, len(points))
		for index, point := range points {
			out[index].Op = sfnt.SegmentOpLineTo
			out[index].Args[0] = point
		}
		out[0].Op = sfnt.SegmentOpMoveTo
		return out
	}
	box := append(rectangle(0, 0, 20), rectangle(2, 2, 16)...)
	if !hollowMenuGlyphBox(box) {
		t.Fatal("hollow replacement square was not detected")
	}
	if hollowMenuGlyphBox(rectangle(0, 0, 20)) {
		t.Fatal("solid glyph was mistaken for an empty square")
	}
	font, err := sfnt.Parse(goregular.TTF)
	if err != nil {
		t.Fatal(err)
	}
	var buffer sfnt.Buffer
	for _, character := range "ab" {
		index, _ := font.GlyphIndex(&buffer, character)
		outline, err := normalizedMenuGlyph(font, &buffer, index)
		if err != nil || hollowMenuGlyphBox(outline) {
			t.Fatal("normal letter was mistaken for a replacement square")
		}
	}
}

type soundVolumeTestPlayer struct{ values []float64 }

func (player *soundVolumeTestPlayer) SetVolume(value float64) {
	player.values = append(player.values, value)
}

func TestSoundVolumeUpdatesPlayingEffectsAndMute(t *testing.T) {
	previous := SoundVolume()
	t.Cleanup(func() { SetSoundVolume(previous) })
	SetSoundVolume(35)
	first, second := &soundVolumeTestPlayer{}, &soundVolumeTestPlayer{}
	trackSoundVolume(first)
	trackSoundVolume(second)
	defer untrackSoundVolume(first)
	defer untrackSoundVolume(second)
	if !isClose(first.values[0], .35) || !isClose(second.values[0], .35) {
		t.Fatal("new sounds did not inherit shared volume")
	}
	SetSoundVolume(70)
	if !isClose(first.values[len(first.values)-1], .7) || !isClose(second.values[len(second.values)-1], .7) {
		t.Fatal("volume change did not update currently playing sounds")
	}
	untrackSoundVolume(first)
	calls := len(first.values)
	SetSoundVolume(-10)
	if SoundVolume() != 0 || second.values[len(second.values)-1] != 0 || len(first.values) != calls {
		t.Fatal("mute, lower clamp, or completed-player cleanup failed")
	}
	// Muted playback must not decode a file or initialize an audio backend.
	manager := MakeAudioManager(nil)
	if err := manager.PlaySound("does-not-exist.wav"); err != nil || len(manager.sounds) != 0 {
		t.Fatal("muted sound tried to load or play")
	}
	SetSoundVolume(1000)
	if SoundVolume() != 100 || second.values[len(second.values)-1] != 1 {
		t.Fatal("maximum volume was not clamped and normalized")
	}
}

func TestMusicVolumeIsIndependentOfSound(t *testing.T) {
	oldSound, oldMusic := SoundVolume(), MusicVolume()
	t.Cleanup(func() { SetSoundVolume(oldSound); SetMusicVolume(oldMusic) })
	SetSoundVolume(35)
	SetMusicVolume(70)
	sound, music := &soundVolumeTestPlayer{}, &soundVolumeTestPlayer{}
	trackSoundVolume(sound)
	musicSettings.track(music)
	defer untrackSoundVolume(sound)
	defer musicSettings.untrack(music)
	if !isClose(sound.values[0], .35) || !isClose(music.values[0], .7) {
		t.Fatal("audio channels did not retain separate volume levels")
	}
	SetMusicVolume(10)
	if !isClose(music.values[len(music.values)-1], .1) || !isClose(sound.values[len(sound.values)-1], .35) || SoundVolume() != 35 {
		t.Fatal("music adjustment affected sound volume")
	}
	SetSoundVolume(25)
	if !isClose(music.values[len(music.values)-1], .1) || MusicVolume() != 10 {
		t.Fatal("sound adjustment affected music volume")
	}
	SetMusicVolume(-1)
	if MusicVolume() != 0 || music.values[len(music.values)-1] != 0 {
		t.Fatal("music mute did not clamp to zero")
	}
	SetMusicVolume(1000)
	if MusicVolume() != 100 || music.values[len(music.values)-1] != 1 {
		t.Fatal("music maximum did not clamp to 100")
	}
}

func TestMusicVolumeSliderMenuAndPauseControls(t *testing.T) {
	oldSound, oldMusic := SoundVolume(), MusicVolume()
	t.Cleanup(func() { SetSoundVolume(oldSound); SetMusicVolume(oldMusic) })
	SetSoundVolume(23)
	SetMusicVolume(60)
	root := defaultMenu()
	root.Options = []MenuOption{{Kind: "music", Name: "Music"}}
	menu, err := root.OpenSubmenu(root.Options[0])
	if err != nil {
		t.Fatal(err)
	}
	if menu.Name != "Music Volume" || len(menu.Options) != 1 || menu.Options[0].Slider == nil || menu.Options[0].Slider.Value() != 60 {
		t.Fatal("music menu did not use the shared slider component")
	}
	menu.Adjust(-1)
	if MusicVolume() != 59 || SoundVolume() != 23 {
		t.Fatal("music slider adjusted the wrong setting")
	}
	pause := &PauseMenu{Root: root, Current: root, Player: grabTestPlayer()}
	pause.Update(PauseInput{Confirm: true}, &dummySystem{})
	if pause.Current.Name != "Music Volume" {
		t.Fatal("music slider did not open from the pause menu")
	}
	pause.Update(PauseInput{Right: true}, &dummySystem{})
	if MusicVolume() != 60 || SoundVolume() != 23 {
		t.Fatal("pause-menu music adjustment failed")
	}
	pause.Draw(ebiten.NewImage(640, 480))
	pause.Update(PauseInput{Back: true}, &dummySystem{})
	if pause.Current != root {
		t.Fatal("Back did not leave the music slider")
	}
	if menu.Options[0].Slider.Value() != 60 {
		t.Fatal("music slider did not see changes from another menu")
	}
}

func TestLivesSliderMenuAndBounds(t *testing.T) {
	previous := StartingLives()
	t.Cleanup(func() { SetStartingLives(previous) })
	SetStartingLives(4)
	root := defaultMenu()
	root.Options = []MenuOption{{Kind: "lives", Name: "Lives"}}
	menu, err := root.OpenSubmenu(root.Options[0])
	if err != nil {
		t.Fatal(err)
	}
	if menu.Name != "Lives" || len(menu.Options) != 1 || menu.Options[0].Slider == nil {
		t.Fatal("lives menu did not use a slider")
	}
	slider := menu.Options[0].Slider
	if slider.Min != 1 || slider.Max != 100 || slider.Value() != 4 || slider.Label() != "4" {
		t.Fatal("lives range, initial value, or non-percentage label is incorrect")
	}
	menu.Adjust(1)
	if StartingLives() != 5 {
		t.Fatal("Right did not increase lives")
	}
	menu.Adjust(-1)
	if StartingLives() != 4 {
		t.Fatal("Left did not decrease lives")
	}
	SetStartingLives(-10)
	if slider.Value() != 1 || slider.Fraction() != 0 || menu.Adjust(-1) {
		t.Fatal("lives went below one")
	}
	SetStartingLives(1000)
	if slider.Value() != 100 || slider.Fraction() != 1 || menu.Adjust(1) {
		t.Fatal("lives exceeded 100")
	}
	pause := &PauseMenu{Root: root, Current: root, Player: grabTestPlayer()}
	pause.Update(PauseInput{Confirm: true}, &dummySystem{})
	if pause.Current.Name != "Lives" {
		t.Fatal("lives slider did not open from pause menu")
	}
	pause.Update(PauseInput{Left: true}, &dummySystem{})
	if StartingLives() != 99 || slider.Label() != "99" {
		t.Fatal("pause adjustment was not shared with the existing menu")
	}
	pause.Draw(ebiten.NewImage(640, 480))
	pause.Update(PauseInput{Back: true}, &dummySystem{})
	if pause.Current != root {
		t.Fatal("Back did not leave the lives slider")
	}
	volume := makeSoundVolumeMenu(root).Options[0].Slider
	if !strings.HasSuffix(volume.Label(), "%") {
		t.Fatal("volume slider lost its percentage label")
	}
}

func TestSliderBoundsBindingAndDrawing(t *testing.T) {
	value := 50
	slider := &Slider{Min: 0, Max: 100, Step: 1, GetValue: func() int { return value }, SetValue: func(next int) { value = next }}
	if slider.Value() != 50 || slider.Fraction() != .5 || !slider.Adjust(-1) || value != 49 {
		t.Fatal("slider did not bind and adjust its value")
	}
	value = 0
	if slider.Adjust(-1) || slider.Fraction() != 0 {
		t.Fatal("slider moved below its minimum")
	}
	value = 100
	if slider.Adjust(1) || slider.Fraction() != 1 {
		t.Fatal("slider moved above its maximum")
	}
	value = -100
	if slider.Value() != 0 {
		t.Fatal("slider did not clamp an external value")
	}
	value = 1000
	if slider.Value() != 100 {
		t.Fatal("slider did not clamp an excessive external value")
	}
	if slider.Adjust(0) {
		t.Fatal("zero direction changed the slider")
	}
	slider.Draw(ebiten.NewImage(200, 80), image.Rect(10, 30, 190, 50), color.NRGBA{G: 200, B: 200, A: 255})
	slider.Draw(nil, image.Rectangle{}, color.NRGBA{})
	fixed := &Slider{Min: 5, Max: 5}
	if fixed.Fraction() != 0 || fixed.Adjust(1) {
		t.Fatal("zero-range slider was not safe")
	}
}

func TestSoundVolumeSliderMenuAndPauseControls(t *testing.T) {
	previous := SoundVolume()
	t.Cleanup(func() { SetSoundVolume(previous) })
	SetSoundVolume(42)
	root := defaultMenu()
	root.Options = []MenuOption{{Kind: "sound", Name: "Sound"}}
	menu, err := root.OpenSubmenu(root.Options[0])
	if err != nil {
		t.Fatal(err)
	}
	if menu.Name != "Sound Volume" || len(menu.Options) != 1 || menu.Options[0].Slider == nil || menu.Options[0].Kind != "slider" {
		t.Fatal("sound menu was not a single slider")
	}
	if !menu.Adjust(-1) || SoundVolume() != 41 || menu.Options[0].Slider.Value() != 41 {
		t.Fatal("Left did not reduce sound volume")
	}
	if !menu.Adjust(1) || SoundVolume() != 42 {
		t.Fatal("Right did not increase sound volume")
	}
	pause := &PauseMenu{Root: root, Current: root, Player: grabTestPlayer()}
	pause.Update(PauseInput{Confirm: true}, &dummySystem{})
	if pause.Current.Name != "Sound Volume" {
		t.Fatal("pause menu did not open the sound slider")
	}
	pause.Update(PauseInput{Left: true}, &dummySystem{})
	if SoundVolume() != 41 {
		t.Fatal("pause-menu Left did not adjust sound")
	}
	pause.Update(PauseInput{Right: true}, &dummySystem{})
	if SoundVolume() != 42 {
		t.Fatal("pause-menu Right did not adjust sound")
	}
	pause.Update(PauseInput{Confirm: true}, &dummySystem{})
	if pause.Current.Notice != "" || pause.Current.Name != "Sound Volume" {
		t.Fatal("slider confirmation behaved like an unsupported option")
	}
	pause.Draw(ebiten.NewImage(640, 480))
	pause.Update(PauseInput{Back: true}, &dummySystem{})
	if pause.Current != root {
		t.Fatal("Back did not return from the sound slider")
	}
	SetSoundVolume(17)
	if menu.Options[0].Slider.Value() != 17 {
		t.Fatal("another menu did not see the shared sound setting")
	}
}

func TestDiscoverMenuFonts(t *testing.T) {
	root := t.TempDir()
	first, second := filepath.Join(root, "first"), filepath.Join(root, "second")
	for _, dir := range []string{filepath.Join(first, "nested"), second, filepath.Join(first, "directory.ttf")} {
		if err := os.MkdirAll(dir, 0755); err != nil {
			t.Fatal(err)
		}
	}
	for _, path := range []string{filepath.Join(first, "B.ttf"), filepath.Join(first, "nested", "a.TTF"), filepath.Join(second, "B.ttf"), filepath.Join(first, "ignored.otf"), filepath.Join(first, "README")} {
		if err := os.WriteFile(path, goregular.TTF, 0600); err != nil {
			t.Fatal(err)
		}
	}
	// Symlink support is optional on platforms that require extra permissions.
	os.Symlink(filepath.Join(first, "B.ttf"), filepath.Join(second, "duplicate.ttf"))
	os.Symlink(first, filepath.Join(first, "nested", "cycle"))
	fonts := discoverMenuFonts([]string{first, second, filepath.Join(first, "nested"), first, filepath.Join(root, "missing")})
	if len(fonts) != 3 || fonts[0].Name != "a" {
		t.Fatalf("recursive, case-insensitive TTF discovery failed: %+v", fonts)
	}
	seen := make(map[string]bool)
	for _, font := range fonts {
		if !filepath.IsAbs(font.Path) || seen[font.Path] {
			t.Fatal("font discovery returned relative or duplicate paths")
		}
		seen[font.Path] = true
	}
}

func TestCommonMenuFontRoots(t *testing.T) {
	home := t.TempDir()
	t.Setenv("HOME", home)
	t.Setenv("USERPROFILE", home)
	t.Setenv("XDG_DATA_HOME", filepath.Join(home, "xdg"))
	t.Setenv("XDG_DATA_DIRS", filepath.Join(home, "extra"))
	t.Setenv("WINDIR", filepath.Join(home, "windows"))
	t.Setenv("LOCALAPPDATA", filepath.Join(home, "local"))
	roots := make(map[string]bool)
	for _, root := range menuFontRoots() {
		roots[filepath.Clean(root)] = true
	}
	for _, expected := range []string{
		"/usr/share/fonts", "/usr/local/share/fonts", data.DataPath("fonts"), "/Library/Fonts", "/System/Library/Fonts",
		filepath.Join(home, ".fonts"), filepath.Join(home, ".local", "share", "fonts"), filepath.Join(home, "Library", "Fonts"),
		filepath.Join(home, "xdg", "fonts"), filepath.Join(home, "extra", "fonts"), filepath.Join(home, "windows", "Fonts"), filepath.Join(home, "local", "Microsoft", "Windows", "Fonts"),
	} {
		if !roots[filepath.Clean(expected)] {
			t.Fatalf("font search omitted %s", expected)
		}
	}
}

func isolateMenuFontPreferences(t *testing.T) {
	t.Helper()
	previous := currentMenuFontConfig()
	menuFontPreferences.Lock()
	menuFontPreferences.config = menuFontConfig{}
	menuFontPreferences.Unlock()
	t.Cleanup(func() {
		menuFontPreferences.Lock()
		menuFontPreferences.config = previous
		menuFontPreferences.Unlock()
	})
}

func TestAdjustFontMenuAndSharedPreferences(t *testing.T) {
	isolateMenuFontPreferences(t)
	path := filepath.Join(t.TempDir(), "Test-Regular.ttf")
	if err := os.WriteFile(path, goregular.TTF, 0600); err != nil {
		t.Fatal(err)
	}
	path = canonicalFontPath(path)
	parent := defaultMenu()
	parent.FontPath = path
	if err := parent.loadAssets(); err != nil {
		t.Fatal(err)
	}
	originalSource := parent.font.Source
	root, err := sexp.ReadSExpressionString(`(menu (name "Adjust Font") (position 20 200 600 80)
		(option (font-select (adjust name))) (option (font-select (adjust width))) (option (font-select (adjust height))))`)
	if err != nil {
		t.Fatal(err)
	}
	menu, err := ParseMenu(root, parent, nil)
	if err != nil {
		t.Fatal(err)
	}
	if len(menu.Options) != 3 || menu.Options[0].Adjust != "name" || menu.Options[1].Name != "Font Width: 24" || menu.Bounds != image.Rect(20, 200, 620, 280) {
		t.Fatal("Adjust Font entries or legacy placement were not parsed")
	}
	picker, err := menu.OpenSubmenu(menu.Options[0])
	if err != nil {
		t.Fatal(err)
	}
	found := false
	for _, option := range picker.Options {
		if option.Path == canonicalFontPath(path) {
			found = true
			if !picker.ApplySetting(option) {
				t.Fatal("font family option was not handled")
			}
		}
	}
	if !found || picker.font.Source != originalSource {
		t.Fatal("font picker omitted the current font or reloaded its cached source")
	}
	parent.Update()
	if parent.FontPath != path || parent.font.Source != originalSource {
		t.Fatal("font selection did not propagate to an existing parent menu")
	}

	pause := &PauseMenu{Root: menu, Current: menu, Player: grabTestPlayer()}
	menu.Selected = 1
	pause.Update(PauseInput{Right: true}, &dummySystem{})
	if menu.FontWidth != 25 {
		t.Fatal("Right did not adjust font width")
	}
	pause.Update(PauseInput{Confirm: true}, &dummySystem{})
	if pause.Current.Name != "Font Width" || len(pause.Current.Options) != maxMenuFontSize-minMenuFontSize+1 {
		t.Fatal("font width picker did not open")
	}
	pause.Current.Selected = 32 - minMenuFontSize
	pause.Update(PauseInput{Confirm: true}, &dummySystem{})
	pause.Update(PauseInput{Back: true}, &dummySystem{})
	if pause.Current != menu {
		t.Fatal("font picker did not return to Adjust Font")
	}
	menu.Update()
	if menu.FontWidth != 32 || menu.Options[1].Name != "Font Width: 32" {
		t.Fatal("font width did not propagate back into the cached menu")
	}
	menu.Selected = 2
	if !menu.Adjust(-1) || menu.FontSize != 23 {
		t.Fatal("Left did not adjust font height")
	}
	if !menu.ApplySetting(MenuOption{Kind: "set-font-height", Value: 40}) {
		t.Fatal("font height setting was not handled")
	}
	parent.Update()
	if parent.FontSize != 40 || parent.FontWidth != 32 || parent.font.Size != 40 {
		t.Fatal("font dimensions were not shared with other menus")
	}
	newDefinition, _ := sexp.ReadSExpressionString(`(menu (font "missing-theme-font.ttf" 12 12))`)
	fresh, err := ParseMenu(newDefinition, nil, nil)
	if err != nil {
		t.Fatal(err)
	}
	if err := fresh.loadAssets(); err != nil {
		t.Fatal("user font preference did not override the new menu's theme font", err)
	}
	if fresh.FontSize != 40 || fresh.FontPath != path {
		t.Fatal("new menus did not inherit chosen font preferences")
	}
	otherPath := filepath.Join(t.TempDir(), "Alternative.ttf")
	if err := os.WriteFile(otherPath, goregular.TTF, 0600); err != nil {
		t.Fatal(err)
	}
	otherPath = canonicalFontPath(otherPath)
	parent.ApplySetting(MenuOption{Kind: "set-font", Path: otherPath})
	fresh.Update()
	menu.Update()
	if fresh.FontPath != otherPath || menu.font.Source != parent.font.Source || parent.font.Source == originalSource {
		t.Fatal("changing font files did not replace the family in existing menus")
	}
	moves := makeMoveList(grabTestPlayer(), parent)
	if moves.FontSize != 30 || moves.FontWidth != 24 {
		t.Fatal("move list did not retain its relative font scale")
	}
	parent.ApplySetting(MenuOption{Kind: "set-font-height", Value: 32})
	moves.Update()
	if moves.FontSize != 24 {
		t.Fatal("cached move list did not track new font preferences")
	}

	// Font sources own their bytes: later glyph loading and size changes must
	// still work after the original file is closed or removed.
	if err := os.Remove(path); err != nil {
		t.Fatal(err)
	}
	if err := os.Remove(otherPath); err != nil {
		t.Fatal(err)
	}
	if source, err := loadMenuFontSource(path); err != nil || source != originalSource {
		t.Fatal("selected font was not cached")
	}
	parent.ApplySetting(MenuOption{Kind: "set-font-width", Value: -100})
	parent.ApplySetting(MenuOption{Kind: "set-font-height", Value: 1000})
	if parent.FontWidth != minMenuFontSize || parent.FontSize != maxMenuFontSize {
		t.Fatal("font dimensions escaped safe bounds")
	}
	parent.MenuInfo = "Font sample Aa Bb 0123"
	parent.DrawOverlay(ebiten.NewImage(640, 480))
	before := currentMenuFontConfig()
	bad := filepath.Join(t.TempDir(), "bad.ttf")
	if err := os.WriteFile(bad, []byte("not a font"), 0600); err != nil {
		t.Fatal(err)
	}
	if !parent.ApplySetting(MenuOption{Kind: "set-font", Path: bad}) || currentMenuFontConfig() != before || !strings.Contains(parent.Notice, "Could not load font") {
		t.Fatal("invalid font selection changed the working font or failed silently")
	}
}

func TestGibAmountSubmenuAndPauseSelection(t *testing.T) {
	previous := GibAmount()
	t.Cleanup(func() { SetGibAmount(previous) })
	SetGibAmount(3)
	parent := defaultMenu()
	parent.Options = []MenuOption{{Kind: "gibs", Name: "Gib amount"}}
	menu, err := parent.OpenSubmenu(parent.Options[0])
	if err != nil {
		t.Fatal(err)
	}
	if menu.Name != "Gib Amount" || len(menu.Options) != 6 || menu.Selected != 3 || menu.Options[0].Name != "None" || menu.Options[5].Name != "Maximum" {
		t.Fatal("gib submenu did not expose all six levels and focus the current value")
	}
	for amount := range 6 {
		if !menu.ApplySetting(menu.Options[amount]) || GibAmount() != amount {
			t.Fatalf("could not apply gib level %d", amount)
		}
		for _, option := range menu.Options {
			selected := len(option.Name) > 0 && option.Name[0] == '*'
			if selected != (option.Value == amount) {
				t.Fatal("submenu did not mark the applied gib level")
			}
		}
	}
	if len(parent.Options) != 1 || parent.Options[0].Name != "Gib amount" {
		t.Fatal("submenu mutated its parent")
	}
	if menu.ApplySetting(MenuOption{Kind: "quit"}) {
		t.Fatal("unrelated option was treated as a gib setting")
	}

	pause := &PauseMenu{Root: parent, Current: parent, Player: grabTestPlayer()}
	pause.Update(PauseInput{Confirm: true}, &dummySystem{})
	if pause.Current.Name != "Gib Amount" {
		t.Fatal("pause menu did not open the gib submenu")
	}
	pause.Current.Selected = 1
	pause.Update(PauseInput{Confirm: true}, &dummySystem{})
	if GibAmount() != 1 {
		t.Fatal("pause menu did not apply the chosen gib amount")
	}
	pause.Update(PauseInput{Back: true}, &dummySystem{})
	if pause.Current != parent {
		t.Fatal("Back did not leave the gib submenu")
	}
	reopened, err := parent.OpenSubmenu(parent.Options[0])
	if err != nil || reopened.Selected != 1 {
		t.Fatal("gib amount was not shared with the next menu instance")
	}
	SetGibAmount(-100)
	if GibAmount() != 0 {
		t.Fatal("negative gib amount was not clamped")
	}
	SetGibAmount(100)
	if GibAmount() != 5 {
		t.Fatal("excessive gib amount was not clamped")
	}
}

func TestGibAmountScalesPartsAndBlood(t *testing.T) {
	previous := GibAmount()
	t.Cleanup(func() { SetGibAmount(previous) })
	parts := make([]*ebiten.Image, 9)
	for index := range parts {
		parts[index] = ebiten.NewImage(10, 10)
	}
	lastCount := -1
	for amount := range 6 {
		SetGibAmount(amount)
		model := &GameModel{GoreFactory: GoreFactory{parts: parts}}
		model.spawnGore(goreBurst{Y: 32})
		partCount, bloodCount := 0, 0
		unique := make(map[*ebiten.Image]bool)
		for _, particle := range model.Gore {
			if particle.Blood {
				bloodCount++
			} else {
				partCount++
				unique[particle.Image] = true
			}
		}
		if partCount != (9*amount+4)/5 || bloodCount != (64*amount+4)/5 || len(unique) != partCount {
			t.Fatalf("level %d did not scale unique body parts and blood: %d parts, %d blood", amount, partCount, bloodCount)
		}
		if len(model.Gore) <= lastCount {
			t.Fatal("higher gib amounts did not increase particle count")
		}
		lastCount = len(model.Gore)
		if amount == 0 && model.GoreFactory.loaded {
			t.Fatal("disabled gibs still loaded body sprites")
		}
	}
}

func TestGibAmountNoneUsesNormalDeathsAndKeepsExistingEffects(t *testing.T) {
	previous := GibAmount()
	t.Cleanup(func() { SetGibAmount(previous) })
	SetGibAmount(5)
	model := &GameModel{GoreFactory: GoreFactory{parts: []*ebiten.Image{ebiten.NewImage(10, 10)}}}
	model.spawnGore(goreBurst{Y: 32})
	particles := len(model.Gore)
	SetGibAmount(0)
	enemy := grabTestEnemy()
	enemy.Health = 5
	enemy.Hurt(1, 100, 2)
	if enemy.Exploded || enemy.explosion != nil || enemy.State != EnemyStateFalling {
		t.Fatal("disabled gibs did not use normal enemy death")
	}
	player := grabTestPlayer()
	player.Health = 5
	player.Hurt(grabTestEnemy(), 100, 2)
	if player.Exploded || player.explosion != nil || player.Status != PlayerStateFalling {
		t.Fatal("disabled gibs did not use normal player death")
	}
	model.spawnGore(goreBurst{Y: 32})
	model.UpdateGore(player)
	if len(model.Gore) != particles {
		t.Fatal("changing the setting deleted old particles or spawned new gore at zero")
	}
}

func TestExplosiveDamageThresholds(t *testing.T) {
	for _, test := range []struct {
		name                            string
		health, damage, pain, threshold float64
		explode                         bool
	}{
		{"heavy_nonlethal", 100, 20, 0, 7, false},
		{"weak_lethal", 2, 3, 0, 7, false},
		{"exact_threshold", 7, 7, 0, 7, false},
		{"heavy_lethal", 5, 20, 0, 7, true},
		{"accumulated_lethal", 2, 3, 6, 7, true},
	} {
		t.Run(test.name, func(t *testing.T) {
			enemy := grabTestEnemy()
			enemy.Health, enemy.Pain, enemy.PainThreshold = test.health, test.pain, test.threshold
			enemy.Hurt(1, test.damage, 2)
			if enemy.Exploded != test.explode || (enemy.explosion != nil) != test.explode || enemy.IsDead() != test.explode {
				t.Fatal("enemy explosion threshold was incorrect")
			}
			player := grabTestPlayer()
			player.Health, player.Pain, player.PainThreshold = test.health, test.pain, test.threshold
			player.Hurt(grabTestEnemy(), test.damage, 2)
			if player.Exploded != test.explode || (player.explosion != nil) != test.explode || (player.Status == PlayerStateDead) != test.explode {
				t.Fatal("player explosion threshold was incorrect")
			}
		})
	}
}

func TestGibRotationBounceAndFade(t *testing.T) {
	particle := &GoreParticle{Y: 10, Vx: 50, Vy: 160, Vz: 20, Spin: 5}
	for tick := 0; tick < 400 && !particle.Landed; tick++ {
		particle.Update(1.0 / 90)
	}
	if !particle.Landed || particle.Angle == 0 || particle.Spin != 0 || particle.Vy <= 0 || particle.Settled {
		t.Fatal("gib did not rotate in flight and stop rotating on its first bounce")
	}
	angle := particle.Angle
	for tick := 0; tick < 400 && !particle.Settled; tick++ {
		particle.Update(1.0 / 90)
		if particle.Angle != angle {
			t.Fatal("gib resumed rotating after ground impact")
		}
	}
	if !particle.Settled || particle.Y != 0 || particle.Vx != 0 || particle.Vz != 0 || particle.bounces < 1 || particle.bounces > 2 {
		t.Fatal("gib did not settle after small bounces")
	}
	x, z := particle.X, particle.Z
	particle.Update(.9)
	if particle.Alpha() != 1 || particle.Done {
		t.Fatal("gib faded before resting for a second")
	}
	particle.Update(.3)
	if !isClose(float64(particle.Alpha()), .5) || particle.Done {
		t.Fatal("gib did not fade gradually")
	}
	particle.Update(.3)
	if !particle.Done || particle.Alpha() != 0 || particle.X != x || particle.Z != z {
		t.Fatal("gib did not disappear without further motion")
	}

	blood := &GoreParticle{Blood: true, Y: 5, Vy: 80, Vx: 10}
	for tick := 0; tick < 400 && !blood.Settled; tick++ {
		blood.Update(1.0 / 90)
	}
	if !blood.Settled || !blood.Landed || blood.bounces != 0 {
		t.Fatal("blood did not settle on the floor")
	}
}

func TestExplosionReleasesGrabAndSpawnsOnlyOnce(t *testing.T) {
	player, enemy := grabTestPlayer(), grabTestEnemy()
	player.DoGrab(enemy)
	enemy.WasGrabbed(player)
	enemy.Health = 5
	enemy.Animations["idle"].Frame = ebiten.NewImage(30, 60)
	enemy.Hurt(1, 20, 2)
	if !enemy.Exploded || player.Grabbed != nil || enemy.Grabber != nil || enemy.CanBeHit(2) {
		t.Fatal("exploded enemy retained its grab or collision state")
	}
	// This must return before trying to draw the old sprite to a nil target.
	DrawEnemy(enemy, 0, ebiten.GeoM{}, nil)
	model := &GameModel{Enemies: []*Enemy{enemy}, GoreFactory: GoreFactory{parts: []*ebiten.Image{ebiten.NewImage(10, 10)}}}
	model.UpdateEnemies(&Level{}, player, &dummySystem{}, nil)
	if len(model.Enemies) != 0 || len(model.Gore) != 65 || enemy.explosion != nil {
		t.Fatal("enemy removal lost or duplicated the gore burst")
	}
	parts, blood := 0, 0
	for _, particle := range model.Gore {
		if particle.Blood {
			blood++
		} else {
			parts++
			if particle.Spin == 0 || particle.Vy <= 0 {
				t.Fatal("body part did not launch spinning")
			}
		}
	}
	if parts != 1 || blood != 64 {
		t.Fatal("explosion did not emit body parts and a blood spray")
	}
	health := enemy.Health
	enemy.Hurt(2, 100, 2)
	model.UpdateGore(player)
	if enemy.Health != health || enemy.explosion != nil || len(model.Gore) != 65 {
		t.Fatal("dead enemy exploded repeatedly")
	}
	for range 1000 {
		model.UpdateGore(player)
	}
	if len(model.Gore) != 0 {
		t.Fatal("finished gore particles were not removed")
	}
}

func TestExplodedPlayerHasNoSpriteOrLiveAttack(t *testing.T) {
	player, held := grabTestPlayer(), grabTestEnemy()
	player.Health = 5
	player.DoGrab(held)
	held.WasGrabbed(player)
	frame := ebiten.NewImage(40, 60)
	player.ShowAnimation.Frame = frame
	player.ShowAnimation.Attack = AnimationAttack{X2: 40, Y2: 60, Damage: 100}
	player.TrailActive = true
	player.Trails = []*Trail{{Image: frame}}
	player.Hurt(grabTestEnemy(), 20, 2)
	if !player.Exploded || player.Status != PlayerStateDead || held.State != EnemyStateIdle || held.Grabber != nil || player.Grabbed != nil || len(player.Trails) != 0 || player.TrailActive {
		t.Fatal("player explosion retained a grab, trail, or live status")
	}
	DrawPlayer(player, 0, ebiten.GeoM{}, nil)
	enemy := grabTestEnemy()
	model := &GameModel{Enemies: []*Enemy{enemy}, GoreFactory: GoreFactory{parts: []*ebiten.Image{frame}}}
	model.UpdatePlayer(player, InputState{Attack1: true}, &Level{}, &dummySystem{}, nil)
	if enemy.Health != 100 {
		t.Fatal("exploded player retained an invisible attack")
	}
	model.UpdateGore(player)
	if len(model.Gore) != 65 || player.explosion != nil {
		t.Fatal("player explosion did not spawn gore")
	}
}

func TestProjectileExplosionAppearsInSameFrame(t *testing.T) {
	player, enemy := grabTestPlayer(), grabTestEnemy()
	player.Z, enemy.Z, enemy.X, enemy.Health, enemy.PainThreshold = 100, 100, 42, 2, 1
	enemy.State = EnemyStatePain
	projectileTestEvent().Update(&Animation{Owner: player}, &dummySystem{})
	model := &GameModel{Enemies: []*Enemy{enemy}, GoreFactory: GoreFactory{parts: []*ebiten.Image{ebiten.NewImage(10, 10)}}}
	model.UpdateEnemies(&Level{}, player, &dummySystem{}, nil)
	if !enemy.Exploded || len(model.Gore) != 65 || enemy.explosion != nil || len(model.Flashes) != 0 {
		t.Fatal("projectile explosion was delayed, left a flash, or lost its particles")
	}
}

func TestLoadGoreAssets(t *testing.T) {
	t.Chdir("../..")
	if _, err := os.Stat("paintown-data/misc/body/arm.png"); os.IsNotExist(err) {
		t.Skip("body part assets not installed")
	}
	cache := MakeObjectFactory()
	factory := GoreFactory{frames: cache.frames}
	if err := factory.Load(); err != nil {
		t.Fatal(err)
	}
	if len(factory.parts) != 9 {
		t.Fatal("not all body part sprites were loaded")
	}
	first := factory.parts[0]
	if err := factory.Load(); err != nil || factory.parts[0] != first || len(cache.frames) != 9 {
		t.Fatal("body sprites were not cached")
	}
	model := &GameModel{GoreFactory: factory}
	model.spawnGore(goreBurst{X: 100, Y: 30, Z: 150})
	screen := ebiten.NewImage(320, 240)
	for _, particle := range model.Gore {
		particle.Draw(screen, 0, ebiten.GeoM{})
	}
}

func TestRegularKnockdownHasAirborneArc(t *testing.T) {
	for _, lethal := range []bool{false, true} {
		enemy := grabTestEnemy()
		if lethal {
			enemy.Health = 10
			enemy.PainThreshold = 100
		}
		enemy.Hurt(1, 10, -2)
		healthAfterHit := enemy.Health
		if enemy.State != EnemyStateFalling || enemy.Vy <= 0 || enemy.Thrown {
			t.Fatal("regular damage did not start a non-projectile knockdown")
		}
		peak := enemy.Y
		descended := false
		for tick := 0; tick < 200 && enemy.State == EnemyStateFalling; tick++ {
			enemy.UpdateState(&Level{}, grabTestPlayer())
			peak = max(peak, enemy.Y)
			descended = descended || enemy.Vy < 0
			if enemy.Health != healthAfterHit || enemy.Thrown {
				t.Fatal("ordinary knockdown gained landing damage or projectile state")
			}
		}
		if peak < 10 || !descended || enemy.Y != 0 || enemy.State != EnemyStateFallen || enemy.X >= 0 {
			t.Fatalf("missing knockdown arc: peak=%v state=%v position=(%v,%v)", peak, enemy.State, enemy.X, enemy.Y)
		}
		enemy.FallenCount = 0
		enemy.UpdateState(&Level{}, grabTestPlayer())
		if enemy.IsDead() != lethal {
			t.Fatal("knockdown recovery did not respect health from the original hit")
		}
	}
}

func TestThrownLandingDamage(t *testing.T) {
	for _, test := range []struct {
		name               string
		health, wantHealth float64
		thrown             bool
	}{
		{"survives", 10, 7, true},
		{"lethal", 3, 0, true},
		{"overkill", 1, 0, true},
		{"ordinary_knockdown", 1, 1, false},
	} {
		t.Run("enemy/"+test.name, func(t *testing.T) {
			enemy := grabTestEnemy()
			enemy.Health = test.health
			if test.thrown {
				enemy.Throw(3, 5)
			} else {
				enemy.DoFall(3, 5)
			}
			if enemy.Health != test.health {
				t.Fatal("damage applied before landing")
			}
			enemy.Y, enemy.Vy = 0, -1
			enemy.UpdateState(&Level{}, grabTestPlayer())
			if enemy.Health != test.wantHealth || enemy.Thrown || enemy.State != EnemyStateFallen {
				t.Fatalf("incorrect landing: health=%v state=%v thrown=%v", enemy.Health, enemy.State, enemy.Thrown)
			}
			for range 5 {
				enemy.UpdateState(&Level{}, grabTestPlayer())
			}
			if enemy.Health != test.wantHealth {
				t.Fatal("landing damage applied more than once")
			}
			enemy.FallenCount = 0
			enemy.UpdateState(&Level{}, grabTestPlayer())
			if enemy.IsDead() != (test.wantHealth == 0) {
				t.Fatal("lethal landing did not follow the death path")
			}
		})
		t.Run("player/"+test.name, func(t *testing.T) {
			player := grabTestPlayer()
			player.Health = test.health
			if test.thrown {
				player.Throw(3, 5)
			} else {
				player.DoFall(3, 5)
			}
			if player.Health != test.health {
				t.Fatal("damage applied before landing")
			}
			player.Y, player.Dy = 0, -1
			player.Update(InputState{}, &Level{}, &dummySystem{}, 100)
			wantState := PlayerStateFallen
			if test.wantHealth == 0 {
				wantState = PlayerStateDead
			}
			if player.Health != test.wantHealth || player.Thrown || player.Status != wantState {
				t.Fatalf("incorrect landing: health=%v state=%v thrown=%v", player.Health, player.Status, player.Thrown)
			}
			for tick := range 5 {
				player.Update(InputState{Jump: true, Attack1: true}, &Level{}, &dummySystem{}, uint64(101+tick))
			}
			if player.Health != test.wantHealth {
				t.Fatal("landing damage applied more than once")
			}
			if test.wantHealth == 0 && (player.Status != PlayerStateDead || player.CanBeHit(grabTestEnemy(), 1)) {
				t.Fatal("dead player recovered or remained hittable")
			}
		})
	}
}

func TestThrownEnemyLandingDeathEffects(t *testing.T) {
	enemy := grabTestEnemy()
	enemy.Health = 3
	enemy.FallSound, enemy.DieSound = "fall", "die"
	enemy.Throw(3, 5)
	enemy.Y, enemy.Vy = 0, -1
	model := &GameModel{Enemies: []*Enemy{enemy}}
	sound := &catTestSound{}
	model.UpdateEnemies(&Level{}, grabTestPlayer(), sound, nil)
	if len(sound.sounds) != 2 || sound.sounds[0] != "fall" || sound.sounds[1] != "die" {
		t.Fatal("lethal landing did not play fall and death sounds")
	}
	if _, shown := model.ShowHealthMap[enemy]; !shown {
		t.Fatal("landing damage did not update the health display")
	}
	model.UpdateEnemies(&Level{}, grabTestPlayer(), sound, nil)
	if len(sound.sounds) != 2 {
		t.Fatal("landing sounds played more than once")
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

func TestControllerDefaultMappingsAndDeadzone(t *testing.T) {
	for _, item := range []struct {
		name    string
		pad     controllerState
		actions []inputAction
	}{
		{"cross", controllerState{Cross: true}, []inputAction{inputJump, inputConfirm}},
		{"square", controllerState{Square: true}, []inputAction{inputAttack1}},
		{"triangle", controllerState{Triangle: true}, []inputAction{inputAttack2}},
		{"circle", controllerState{Circle: true}, []inputAction{inputAttack2, inputBack}},
		{"start", controllerState{Start: true}, []inputAction{inputPause, inputConfirm}},
		{"select", controllerState{Select: true}, []inputAction{inputBack}},
		{"left stick", controllerState{X: -.8, Y: .9}, []inputAction{inputLeft, inputDown, inputMenuLeft, inputMenuDown}},
		{"dpad", controllerState{Right: true, Up: true}, []inputAction{inputRight, inputUp, inputMenuRight, inputMenuUp}},
		{"neutral drift", controllerState{X: .2, Y: -.2}, nil},
	} {
		t.Run(item.name, func(t *testing.T) {
			want := inputButtons{}
			for _, action := range item.actions {
				want[action] = true
			}
			if got := item.pad.buttons(); got != want {
				t.Fatalf("incorrect controller mapping: got %v want %v", got, want)
			}
		})
	}
}

func TestLogicalInputEdgesMergeRepeatAndDisconnect(t *testing.T) {
	var input logicalInput
	keyboard := inputButtons{inputJump: true}
	keyboard.merge(controllerState{Cross: true, Down: true}.buttons())
	input.advance(keyboard)
	if !input.gameplay().Jump || !input.gameplay().HeldJump || !input.menu().Confirm || !input.menu().Down {
		t.Fatal("first press did not emit logical edges")
	}
	// Releasing one device must not retrigger an action held by another.
	pad := controllerState{Cross: true, Down: true}.buttons()
	for tick := 2; tick <= 32; tick++ {
		input.advance(pad)
		if input.gameplay().Jump || input.menu().Confirm || !input.gameplay().HeldJump {
			t.Fatal("held controller action repeated a press")
		}
		if input.menu().Down != (tick > 25 && tick%6 == 0) {
			t.Fatalf("incorrect menu repeat at tick %d", tick)
		}
	}
	input.advance(inputButtons{})
	if input.gameplay().HeldJump || input.gameplay().HeldDown || input.menu().Down {
		t.Fatal("disconnect left held actions stuck")
	}
	input.advance(pad)
	if !input.gameplay().Jump || !input.menu().Confirm {
		t.Fatal("reconnected/repressed button did not emit a new edge")
	}

	previous := currentInput
	defer func() { currentInput = previous }()
	currentInput = logicalInput{}
	currentInput.advance(controllerState{Start: true}.buttons())
	if !PauseRequested() || !ReadMenuInput().Confirm || ReadMenuInput().Back {
		t.Fatal("Start must pause gameplay or confirm a normal menu, not quit it")
	}
	root := defaultMenu()
	root.Options = []MenuOption{{Kind: "return"}}
	pause := &PauseMenu{Root: root, Current: root, Player: grabTestPlayer()}
	if pause.Update(readPauseInput(), &dummySystem{}) != PauseContinue {
		t.Fatal("Start in the pause menu should resume, not confirm Quit")
	}
}

func TestJoystickPS3EventsUseDriverMaps(t *testing.T) {
	stick := joystickState{axisCount: 6, buttonCount: 17}
	copy(stick.axisCodes[:], []byte{0, 1, 2, 3, 4, 5})
	copy(stick.buttonCodes[:], []uint16{304, 305, 307, 308, 310, 311, 312, 313, 314, 315, 316, 317, 318, 544, 545, 546, 547})
	record := func(kind, index byte, value int16) []byte {
		data := make([]byte, 8)
		binary.LittleEndian.PutUint16(data[4:6], uint16(value))
		data[6], data[7] = kind, index
		return data
	}
	// The connected PS3 reports L2/R2 at -32767 while released. They must
	// not cause phantom left/up movement, including during INIT events.
	stick.feed(append(record(0x82, 2, -32767), record(0x82, 5, -32767)...))
	if got := stick.controller().buttons(); got != (inputButtons{}) {
		t.Fatal("trigger initialization was interpreted as movement")
	}
	press := record(0x81, 3, 1) // Square; INIT is still a state update.
	stick.feed(press[:3])
	if stick.controller().Square {
		t.Fatal("partial joystick event was decoded prematurely")
	}
	stick.feed(press[3:])
	if !stick.controller().Square {
		t.Fatal("fragmented Square event was not decoded")
	}
	stick.feed(record(1, 3, 0))
	stick.feed(record(1, 13, 1))
	stick.feed(record(2, 0, -32768))
	pad := stick.controller()
	if pad.Square || !pad.Up || pad.X != -1 || !pad.buttons()[inputLeft] {
		t.Fatal("button release, dpad, or signed axis event decoded incorrectly")
	}
	// Driver layouts can enumerate the same buttons at different indices.
	stick = joystickState{buttonCount: 2}
	stick.buttonCodes[0], stick.buttonCodes[1] = 308, 304
	stick.feed(record(1, 0, 1))
	if !stick.controller().Square || stick.controller().Cross {
		t.Fatal("joystick mapping depends on hard-coded button indices")
	}
	stick.feed(record(1, 1, 1))
	if !stick.controller().Cross {
		t.Fatal("remapped Cross button did not work")
	}
	stick.feed(record(2, 63, 32767)) // Invalid for this device; ignore safely.
	stick = joystickState{axisCount: 2}
	stick.axisCodes[0], stick.axisCodes[1] = 16, 17
	stick.feed(record(2, 0, 1))
	stick.feed(record(2, 1, -1))
	if !stick.controller().Right || !stick.controller().Up {
		t.Fatal("hat-axis dpad did not work")
	}
}

func TestControllerDirectionalCombo(t *testing.T) {
	player := grabTestPlayer()
	player.ActivatedAnimations = make(map[*Animation]uint64)
	player.Facing, player.Z = FacingRight, 100
	move := MakeAnimation("haddoken", "", []AnimationEvent{&AnimationEventFrame{Image: ebiten.NewImage(10, 10)}, &AnimationEventNop{}}, []InputKey{InputKeyDown, InputKeyForward, InputKeyAttack1}, "", "")
	move.Owner = player
	player.Animations["haddoken"] = move
	var input logicalInput
	for index, pad := range []controllerState{{Down: true}, {Right: true}, {Square: true}} {
		input.advance(pad.buttons())
		player.Update(input.gameplay(), &Level{ZMaximum: 200}, &dummySystem{}, uint64(100+index))
	}
	if player.ShowAnimation != move {
		t.Fatal("D-pad Down, Forward, Square did not activate the projectile combo")
	}
}

func TestPauseMenuNavigationAndMoveList(t *testing.T) {
	t.Chdir("../..")
	if _, err := os.Stat("paintown-data/menu/in-game.txt"); os.IsNotExist(err) {
		t.Skip("menu assets not installed")
	}
	player := grabTestPlayer()
	player.Name = "Akuma"
	player.Animations["haddoken"] = MakeAnimation("haddoken", "", nil, []InputKey{InputKeyDown, InputKeyForward, InputKeyAttack1}, "", "")
	player.Animations["throw"] = MakeAnimation("throw", "", nil, []InputKey{InputKeyBack, InputKeyAttack1}, "", "grab")
	player.Animations["jump-kick"] = MakeAnimation("jump-kick", "", nil, []InputKey{InputKeyAttack2}, "", "jump")
	pause, err := NewPauseMenu(player)
	if err != nil {
		t.Fatal(err)
	}
	if len(pause.Root.Options) != 4 || pause.Root.Options[0].Kind != "continue" || pause.Root.Options[3].Name != "Quit to Main Menu" {
		t.Fatal("pause menu did not expose the four game actions")
	}
	if pause.Update(PauseInput{Confirm: true}, &dummySystem{}) != PauseContinue {
		t.Fatal("Continue did not resume")
	}
	pause.reset()
	pause.Root.Selected = 1
	if pause.Update(PauseInput{Confirm: true}, &dummySystem{}) != PauseStay || pause.Current.Name != "Move List" {
		t.Fatal("move list did not open")
	}
	if len(pause.Current.Options) != 3 || pause.Current.Options[0].Name != "haddoken" || pause.Current.Options[0].Info != "Down > Forward > A\nOn the ground" || pause.Current.Options[2].Info != "Back > A\nWhile grabbing" {
		t.Fatalf("move list did not show the character's commands: %+v", pause.Current.Options)
	}
	pause.Current.DrawOverlay(ebiten.NewImage(640, 480))
	pause.Update(PauseInput{Back: true}, &dummySystem{})
	if pause.Current != pause.Root {
		t.Fatal("move list Back did not return to pause menu")
	}
	pause.Root.Selected = 2
	pause.Update(PauseInput{Confirm: true}, &dummySystem{})
	if pause.Current.Name != "Options" || len(pause.Current.Options) < 10 {
		t.Fatal("game options did not open")
	}
	// options.txt includes inline submenus, not only external menu paths.
	keyboard := -1
	for index, option := range pause.Current.Options {
		if option.Name == "Keyboard Configuration" {
			keyboard = index
		}
	}
	if keyboard < 0 {
		t.Fatal("keyboard submenu missing")
	}
	pause.Current.Selected = keyboard
	pause.Update(PauseInput{Confirm: true}, &dummySystem{})
	if pause.Current.Name != "Keyboard Configuration" || len(pause.Current.Options) < 2 {
		t.Fatal("inline configuration menu did not open")
	}
	pause.Update(PauseInput{Back: true}, &dummySystem{})
	pause.Update(PauseInput{Back: true}, &dummySystem{})
	if pause.Current != pause.Root {
		t.Fatal("options Back did not return to pause menu")
	}
	pause.Root.Selected = 3
	if pause.Update(PauseInput{Confirm: true}, &dummySystem{}) != PauseQuitToMain {
		t.Fatal("Quit did not request the main menu")
	}
	pause.reset()
	if pause.Update(PauseInput{Back: true}, &dummySystem{}) != PauseContinue {
		t.Fatal("Escape at the pause root did not resume")
	}
}

func TestMovePreviewIsolatedFromPausedPlayer(t *testing.T) {
	player := grabTestPlayer()
	player.X, player.Y, player.Z, player.AttackId = 12, 4, 100, 88
	first, second := ebiten.NewImage(20, 30), ebiten.NewImage(24, 32)
	move := MakeAnimation("sandbox", "attack", []AnimationEvent{
		&AnimationEventDelay{Delay: 2},
		&AnimationEventOffset{X: 5, Y: 3},
		&AnimationMoveEvent{X: 4, Y: 2, Z: 1},
		&AnimationFaceEvent{Facing: "left"},
		&AnimationTrailEvent{Generate: 1, Length: 10},
		&AnimationEventNextTicket{},
		&AnimationSoundEvent{Sound: "do-not-play.wav"},
		projectileTestEvent(),
		&AnimationEventFrame{Image: first},
		&AnimationEventFrame{Image: second},
		&AnimationEventNop{},
	}, []InputKey{InputKeyAttack1}, "", "")
	move.Owner, move.Frame = player, second
	move.OffsetX, move.OffsetY, move.Delay, move.CurrentEvent, move.CurrentDelay = 99, 50, 42, 3, 9
	move.KeyPresses[0] = 123
	player.Animations["sandbox"] = move
	root := defaultMenu()
	root.Options = []MenuOption{{Kind: "move-list"}}
	pause := &PauseMenu{Root: root, Current: root, Player: player}
	sounds := &catTestSound{}
	pause.Update(PauseInput{Confirm: true}, sounds)
	preview := pause.preview
	if preview == nil || preview.animation == move || preview.animation.Owner != preview || preview.animation.CurrentFrame() != first {
		t.Fatal("move list did not start an independent preview from the first frame")
	}
	if preview.x != 4 || preview.y != 2 || preview.facing != FacingLeft || len(preview.projectiles) != 1 {
		t.Fatal("animation events did not run in the preview sandbox")
	}
	for range 3 {
		pause.Update(PauseInput{}, sounds)
	}
	if preview.animation.CurrentFrame() != second {
		t.Fatal("preview animation did not advance")
	}
	replayed := false
	for range 100 {
		pause.Update(PauseInput{}, sounds)
		if preview.animation.CurrentFrame() == first {
			replayed = true
		}
	}
	if !replayed {
		t.Fatal("preview did not replay the selected move")
	}
	pause.Draw(ebiten.NewImage(640, 480))
	if player.X != 12 || player.Y != 4 || player.Z != 100 || player.AttackId != 88 || player.TrailActive || len(player.pendingProjectiles) != 0 {
		t.Fatal("preview events changed the paused player's state or emitted live projectiles")
	}
	if move.Owner != player || move.Frame != second || move.OffsetX != 99 || move.OffsetY != 50 || move.Delay != 42 || move.CurrentEvent != 3 || move.CurrentDelay != 9 || move.KeyPresses[0] != 123 {
		t.Fatal("preview changed the live animation's playback state")
	}
	for _, sound := range sounds.sounds {
		if sound == "do-not-play.wav" {
			t.Fatal("preview played live animation audio")
		}
	}
	pause.Update(PauseInput{Back: true}, sounds)
	if pause.Current != root || pause.preview != nil {
		t.Fatal("leaving the move list retained an active preview")
	}
}

func TestMovePreviewRestartsOnSelectionChange(t *testing.T) {
	player := grabTestPlayer()
	first, second := ebiten.NewImage(10, 10), ebiten.NewImage(12, 12)
	for _, item := range []struct {
		name   string
		frame  *ebiten.Image
		status string
	}{
		{"alpha", first, ""}, {"beta", second, "jump"},
	} {
		player.Animations[item.name] = MakeAnimation(item.name, "", []AnimationEvent{
			&AnimationEventDelay{Delay: 4}, &AnimationEventFrame{Image: item.frame}, &AnimationEventNop{},
		}, []InputKey{InputKeyAttack1}, "", item.status)
	}
	root := defaultMenu()
	root.Options = []MenuOption{{Kind: "move-list"}}
	pause := &PauseMenu{Root: root, Current: root, Player: player}
	pause.Update(PauseInput{Confirm: true}, &dummySystem{})
	alpha := pause.preview
	pause.Update(PauseInput{}, &dummySystem{})
	pause.Update(PauseInput{Down: true}, &dummySystem{})
	if pause.preview == alpha || pause.preview.source != player.Animations["beta"] || pause.preview.animation.CurrentFrame() != second || pause.preview.animation.CurrentDelay != 4 || pause.preview.y <= 0 {
		t.Fatal("selecting a jump move did not restart its preview in the air")
	}
	pause.Update(PauseInput{Up: true}, &dummySystem{})
	if pause.preview.source != player.Animations["alpha"] || pause.preview.animation.CurrentFrame() != first || pause.preview.animation.CurrentDelay != 4 || pause.preview.y != 0 {
		t.Fatal("returning to a ground move did not reset its preview")
	}
}

func TestPauseLoopFreezesWorldAndRestoresDrawing(t *testing.T) {
	for _, action := range []string{"continue", "quit", "cancelled"} {
		t.Run(action, func(t *testing.T) {
			player, enemy := grabTestPlayer(), grabTestEnemy()
			player.X, enemy.Y = 12, 20
			projectile := &Projectile{Life: 70}
			model := &GameModel{Counter: 1000, Enemies: []*Enemy{enemy}, Projectiles: []*Projectile{projectile}}
			root := defaultMenu()
			root.Options = []MenuOption{{Kind: "continue"}, {Kind: "return"}}
			pause := &PauseMenu{Root: root, Current: root, Player: player}
			drawCalls, setCalls, yields, inputs := 0, 0, 0, 0
			var drawer data.DrawFunc = func(*ebiten.Image) { drawCalls++ }
			setDraw := func(next data.DrawFunc) data.DrawFunc { previous := drawer; drawer = next; setCalls++; return previous }
			screen := ebiten.NewImage(640, 480)
			cancelled := errors.New("cancelled")
			yield := func() error {
				yields++
				if yields > 20 {
					t.Fatal("pause loop failed to exit")
				}
				drawer(screen)
				if action == "cancelled" {
					return cancelled
				}
				return nil
			}
			read := func() PauseInput {
				inputs++
				if inputs < 10 {
					return PauseInput{}
				}
				return PauseInput{Confirm: true, Down: action == "quit"}
			}
			err := runPauseMenu(pause, yield, setDraw, &dummySystem{}, read)
			if action == "continue" && (err != nil || yields != 11) {
				t.Fatal("Continue failed to consume its menu input before resuming")
			}
			if action == "quit" && err != ErrReturnToMainMenu {
				t.Fatal("Quit did not unwind the level")
			}
			if action == "cancelled" && err != cancelled {
				t.Fatal("pause did not propagate cancellation")
			}
			if model.Counter != 1000 || player.X != 12 || enemy.Y != 20 || projectile.Life != 70 {
				t.Fatal("world advanced while paused")
			}
			if setCalls != 2 || drawCalls != yields {
				t.Fatal("pause did not draw the game or restore its drawer")
			}
			// The restored world drawer tolerates nil; the overlay would not.
			drawer(nil)
		})
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

// Keep the old map-based algorithm here as an oracle: the faster storage
// must preserve collision geometry, including partial cells at image edges.
func referenceCollision(input image.Image) *Collision {
	bounds := input.Bounds()
	grid := make(map[image.Point]image.Rectangle)
	for x := bounds.Min.X; x < bounds.Max.X; x += 3 {
		for y := bounds.Min.Y; y < bounds.Max.Y; y += 3 {
			count := 0
			for dx := range 3 {
				for dy := range 3 {
					if x+dx < bounds.Max.X && y+dy < bounds.Max.Y {
						_, _, _, alpha := input.At(x+dx, y+dy).RGBA()
						if alpha != 0 {
							count++
						}
					}
				}
			}
			if count >= 7 {
				r := image.Rect(x, y, x+3, y+3)
				for dx := range 3 {
					for dy := range 3 {
						if x+dx < bounds.Max.X && y+dy < bounds.Max.Y {
							grid[image.Pt(x+dx, y+dy)] = r
						}
					}
				}
			}
		}
	}
	for changed := true; changed; {
		changed = false
		for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
			for x := bounds.Min.X; x < bounds.Max.X; x++ {
				if r, ok := grid[image.Pt(x, y)]; ok {
					for _, point := range []image.Point{image.Pt(r.Max.X, y), image.Pt(x, r.Max.Y)} {
						if other, ok := grid[point]; ok && r != other && r.Dy() == other.Dy() {
							r = r.Union(other)
							for dx := r.Min.X; dx < other.Max.X; dx++ {
								for dy := r.Min.Y; dy < r.Max.Y; dy++ {
									grid[image.Pt(dx, dy)] = r
								}
							}
							changed = true
						}
					}
				}
			}
		}
	}
	out := &Collision{width: bounds.Dx(), height: bounds.Dy()}
	for _, box := range grid {
		out.Boxes = append(out.Boxes, box)
		out.MainBox = out.MainBox.Union(box)
	}
	return out
}

func TestCollisionOptimizationPreservesGeometry(t *testing.T) {
	for seed := range 40 {
		bounds := image.Rect(seed%3-2, seed%5-2, 17+seed%4, 16+seed%7)
		pixels := image.NewNRGBA(bounds)
		for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
			for x := bounds.Min.X; x < bounds.Max.X; x++ {
				if seed == 0 || (seed != 1 && (x*x+y*7+seed)%11 != 0) {
					pixels.SetNRGBA(x, y, color.NRGBA{A: 255})
				}
			}
		}
		for _, input := range []image.Image{pixels, pixels.SubImage(bounds.Inset(1)), struct{ image.Image }{pixels}} {
			got, want := MakeCollision(input), referenceCollision(input)
			if got.MainBox != want.MainBox || got.width != want.width || got.height != want.height {
				t.Fatalf("seed %d: collision bounds changed", seed)
			}
			unique := make(map[image.Rectangle]bool)
			for _, box := range want.Boxes {
				unique[box] = true
			}
			if len(got.Boxes) != len(unique) {
				t.Fatalf("seed %d: duplicate or missing collision boxes", seed)
			}
			for _, box := range got.Boxes {
				if !unique[box] {
					t.Fatalf("seed %d: unexpected collision box %v", seed, box)
				}
			}
		}
	}
}

func TestAssetCacheReusesResourcesNotPlayback(t *testing.T) {
	t.Chdir(t.TempDir())
	if err := os.MkdirAll("paintown-data/sprites", 0755); err != nil {
		t.Fatal(err)
	}
	for _, name := range []string{"a", "b"} {
		pixels := image.NewNRGBA(image.Rect(0, 0, 9, 9))
		for y := range 9 {
			for x := range 9 {
				if name == "a" || (x >= 3 && x < 6 && y >= 3 && y < 6) {
					pixels.SetNRGBA(x, y, color.NRGBA{A: 255})
				}
			}
		}
		file, err := os.Create("paintown-data/sprites/" + name + ".png")
		if err != nil {
			t.Fatal(err)
		}
		if err := png.Encode(file, pixels); err != nil {
			t.Fatal(err)
		}
		file.Close()
		definition := `(character (name shared) (health 10) (icon sprites/` + name + `.png)
			(anim (name idle) (keys key_attack1) (basedir sprites) (frame ` + name + `.png) (frame ` + name + `.png))
			(anim (name walk) (basedir sprites) (frame ` + name + `.png)))`
		if err := os.WriteFile("paintown-data/"+name+".txt", []byte(definition), 0600); err != nil {
			t.Fatal(err)
		}
	}
	factory := MakeObjectFactory()
	a, b := BlockObject{Path: "a.txt"}, BlockObject{Path: "b.txt"}
	if err := factory.PrecacheEnemy(a); err != nil {
		t.Fatal(err)
	}
	if err := factory.PrecacheEnemy(b); err != nil {
		t.Fatal(err)
	}
	definition, err := factory.LoadDefinition("paintown-data/a.txt")
	if err != nil {
		t.Fatal(err)
	}
	character := &PaintownCharacter{Definition: definition}
	player, err := MakePlayerState(character, &Level{})
	if err != nil {
		t.Fatal(err)
	}
	// No further disk reads are possible after warming the caches.
	for _, path := range []string{"sprites/a.png", "sprites/b.png", "a.txt", "b.txt"} {
		if err := os.Remove("paintown-data/" + path); err != nil {
			t.Fatal(err)
		}
	}
	first, err := MakeEnemy(a, factory, &BehaviorNormal{})
	if err != nil {
		t.Fatal(err)
	}
	second, err := MakeEnemy(a, factory, &BehaviorNormal{})
	if err != nil {
		t.Fatal(err)
	}
	other, err := MakeEnemy(b, factory, &BehaviorNormal{})
	if err != nil {
		t.Fatal(err)
	}
	idle, walk := first.Animations["idle"], first.Animations["walk"]
	frame := idle.FirstFrame()
	if len(factory.frames) != 2 || len(idle.Collision) != 1 || walk.FirstFrame() != frame || idle.Collision[frame] != walk.Collision[frame] || first.Icon != second.Icon {
		t.Fatal("sprite, collision, or icon resources were not shared")
	}
	if frame == other.Animations["idle"].FirstFrame() || idle.Collision[frame].MainBox == other.Animations["idle"].Collision[other.Animations["idle"].FirstFrame()].MainBox {
		t.Fatal("different definition paths with the same character name shared the wrong assets")
	}
	idle.CurrentEvent, idle.KeyPresses[0] = 42, 99
	if second.Animations["idle"].CurrentEvent != 0 || second.Animations["idle"].KeyPresses[0] != 0 || idle.Owner != first || second.Animations["idle"].Owner != second {
		t.Fatal("cached enemies shared mutable playback state")
	}
	player.Animations["idle"].CurrentEvent = 42
	nextPlayer, err := MakePlayerState(character, &Level{})
	if err != nil {
		t.Fatal(err)
	}
	if nextPlayer.Icon != player.Icon || nextPlayer.Animations["idle"].FirstFrame() != player.Animations["idle"].FirstFrame() || nextPlayer.Animations["idle"].CurrentEvent != 0 || nextPlayer.Animations["idle"].Owner != nextPlayer {
		t.Fatal("player level changes did not reuse assets with independent playback")
	}
}

func BenchmarkCollisionGeneration(b *testing.B) {
	pixels := image.NewNRGBA(image.Rect(0, 0, 64, 100))
	for y := range 100 {
		for x := range 64 {
			if x > 10 && x < 55 && y > 5 && (x+y)%13 != 0 {
				pixels.SetNRGBA(x, y, color.NRGBA{A: 255})
			}
		}
	}
	for _, implementation := range []struct {
		name  string
		build func(image.Image) *Collision
	}{
		{"map_reference", referenceCollision}, {"dense", MakeCollision},
	} {
		b.Run(implementation.name, func(b *testing.B) {
			b.ReportAllocs()
			for b.Loop() {
				implementation.build(pixels)
			}
		})
	}
}

func BenchmarkEnemyLoading(b *testing.B) {
	b.Chdir("../..")
	if _, err := os.Stat("paintown-data/chars/punk/punk.txt"); os.IsNotExist(err) {
		b.Skip("game assets not installed")
	}
	object := BlockObject{Type: "enemy", Path: "chars/punk/punk.txt"}
	b.Run("cold", func(b *testing.B) {
		b.ReportAllocs()
		for b.Loop() {
			if _, err := MakeEnemy(object, MakeObjectFactory(), &BehaviorNormal{}); err != nil {
				b.Fatal(err)
			}
		}
	})
	b.Run("cached", func(b *testing.B) {
		factory := MakeObjectFactory()
		if _, err := MakeEnemy(object, factory, &BehaviorNormal{}); err != nil {
			b.Fatal(err)
		}
		b.ReportAllocs()
		for b.Loop() {
			if _, err := MakeEnemy(object, factory, &BehaviorNormal{}); err != nil {
				b.Fatal(err)
			}
		}
	})
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
