package paintown

import (
	"log"
	"math"
	"math/rand/v2"
	"os"
	"slices"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/data"
)

// A buddy uses the same animations, hit reactions, grabs, and projectiles as a
// human player, but has its own controller and never spends the human's lives.
type Buddy struct {
	Player            *PlayerState
	cooldown, attacks int
	session           *ArcadeSession
	slot              int
	target            *Enemy
	wanderX, wanderZ  float64
	wanderValid       bool
	wanderPause       int
}

func (buddy *Buddy) GetIcon() *ebiten.Image    { return buddy.Player.GetIcon() }
func (buddy *Buddy) GetName() string           { return buddy.Player.Name + " (AI)" }
func (buddy *Buddy) GetHealthPercent() float64 { return buddy.Player.GetHealthPercent() }

func (session *ArcadeSession) prepareBuddies(human *PaintownCharacter) {
	if session.BuddyCount <= 0 || len(session.buddyCharacters) >= session.BuddyCount {
		return
	}
	entries, _ := os.ReadDir(data.DataPath("players"))
	for _, entry := range entries {
		if !entry.IsDir() || strings.EqualFold(entry.Name(), human.Definition.Name) {
			continue
		}
		character, err := MakePaintownPlayer(entry.Name())
		if err != nil {
			continue
		}
		session.buddyCharacters = append(session.buddyCharacters, character)
		if len(session.buddyCharacters) == session.BuddyCount {
			break
		}
	}
	for len(session.buddyCharacters) < session.BuddyCount {
		session.buddyCharacters = append(session.buddyCharacters, human)
	}
}

func (model *GameModel) createBuddies(session *ArcadeSession, human *PlayerState, level *Level) {
	if session == nil {
		return
	}
	for index, character := range session.buddyCharacters {
		if session.defeatedBuddies[index] {
			continue
		}
		player, err := MakePlayerState(character, level)
		if err != nil {
			log.Printf("Loading AI buddy: %v", err)
			continue
		}
		player.X, player.Z = buddyFollowPosition(human, index, level, model.LevelLimit)
		player.DropIn()
		model.Buddies = append(model.Buddies, &Buddy{Player: player, session: session, slot: index})
	}
}

func buddyFollowPosition(human *PlayerState, index int, level *Level, limit float64) (float64, float64) {
	x := human.X - float64(35+index*30)
	if human.Facing == FacingLeft {
		x = human.X + float64(35+index*30)
	}
	x = max(10, x)
	if limit > 0 {
		x = min(x, max(10, limit-10))
	}
	z := max(float64(level.ZMinimum), min(float64(level.ZMaximum), human.Z+float64((index%2)*6-3)))
	return x, z
}

func (model *GameModel) party(human *PlayerState) []*PlayerState {
	players := []*PlayerState{human}
	for _, buddy := range model.Buddies {
		players = append(players, buddy.Player)
	}
	return players
}

func nearestPartyMember(enemy *Enemy, human *PlayerState, players []*PlayerState) *PlayerState {
	target := human
	best := math.Inf(1)
	for _, player := range players {
		if player.Health <= 0 || player.Status == PlayerStateDead || player.Status == PlayerStateDropping || player.Status == PlayerStateFalling || player.Status == PlayerStateFallen {
			continue
		}
		dx, dz := player.X-enemy.X, player.Z-enemy.Z
		distance := dx*dx + dz*dz*4
		if distance < best {
			target, best = player, distance
		}
	}
	return target
}

func (buddy *Buddy) attack(projectile bool) *Animation {
	player := buddy.Player
	var choices []*Animation
	for _, animation := range player.Animations {
		status := animation.Status
		if status == "" {
			status = "ground"
		}
		if status != player.GetStatus() || len(animation.Keys) == 0 || animation.Sequence != "" {
			continue
		}
		if player.Status == PlayerStateGrab && animation.Name == "throw" && buddy.attacks%3 == 2 {
			return animation
		}
		offensive, isProjectile := false, false
		for _, event := range animation.Events {
			switch event := event.(type) {
			case *AnimationEventAttack:
				offensive = offensive || event.Attack.Damage > 0
			case *AnimationEventProjectile:
				offensive, isProjectile = true, true
			}
		}
		if offensive && isProjectile == projectile {
			choices = append(choices, animation)
		}
	}
	slices.SortFunc(choices, func(a, b *Animation) int {
		if len(a.Keys) != len(b.Keys) {
			return len(a.Keys) - len(b.Keys)
		}
		return strings.Compare(a.Name, b.Name)
	})
	if len(choices) == 0 {
		return nil
	}
	return choices[0]
}

// Sequence names point to the preceding move. Refresh the selected follow-up
// while its predecessor plays so even a long animation cannot expire its input
// buffer before PlayerState performs the normal combo transition.
func (buddy *Buddy) queueComboFollowUp(current *Animation, counter uint64) {
	player := buddy.Player
	var next *Animation
	for _, animation := range player.Animations {
		status := animation.Status
		if status == "" {
			status = "ground"
		}
		if animation == current || animation.Sequence != current.Name || status != player.GetStatus() {
			continue
		}
		if next == nil || len(animation.Keys) < len(next.Keys) || (len(animation.Keys) == len(next.Keys) && animation.Name < next.Name) {
			next = animation
		}
	}
	if next != nil {
		if player.ActivatedAnimations == nil {
			player.ActivatedAnimations = make(map[*Animation]uint64)
		}
		player.ActivatedAnimations[next] = counter
	}
}

// Gameplay positions use the half-resolution world buffer: half of the
// visible 320-unit screen is 160 world units, not 320 UI pixels.
const buddyWanderRadius = float64(data.ScreenWidth) / 4

func steerBuddy(player *PlayerState, x, z, stopX, stopZ float64) InputState {
	return InputState{HeldRight: player.X < x-stopX, HeldLeft: player.X > x+stopX, HeldDown: player.Z < z-stopZ, HeldUp: player.Z > z+stopZ}
}

func (buddy *Buddy) wanderInput(model *GameModel, human *PlayerState, level *Level) InputState {
	player := buddy.Player
	if math.Hypot(player.X-human.X, player.Z-human.Z) > buddyWanderRadius {
		buddy.wanderValid, buddy.wanderPause = false, 0
		return steerBuddy(player, human.X, human.Z, 8, 2)
	}
	if player.ShowAnimation != nil || player.Status == PlayerStateGrab {
		return InputState{}
	}
	if buddy.wanderValid && math.Hypot(buddy.wanderX-human.X, buddy.wanderZ-human.Z) > buddyWanderRadius*.9 {
		buddy.wanderValid, buddy.wanderPause = false, 0
	}
	if buddy.wanderPause > 0 {
		buddy.wanderPause--
		return InputState{}
	}
	if !buddy.wanderValid {
		for range 8 {
			angle := rand.Float64() * 2 * math.Pi
			radius := buddyWanderRadius * (.2 + .65*math.Sqrt(rand.Float64()))
			buddy.wanderX = max(0, human.X+math.Cos(angle)*radius)
			if model.LevelLimit > 0 {
				buddy.wanderX = min(model.LevelLimit, buddy.wanderX)
			}
			buddy.wanderZ = max(float64(level.ZMinimum), min(float64(level.ZMaximum), human.Z+math.Sin(angle)*radius))
			if abs(player.X-buddy.wanderX) > 12 || abs(player.Z-buddy.wanderZ) > 4 {
				break
			}
		}
		buddy.wanderValid = true
	}
	input := steerBuddy(player, buddy.wanderX, buddy.wanderZ, 6, 2)
	if !input.HeldLeft && !input.HeldRight && !input.HeldUp && !input.HeldDown {
		buddy.wanderValid = false
		buddy.wanderPause = max(1, arcadeRespawnDelay()/3) + rand.N(max(1, arcadeRespawnDelay()/2))
		return input
	}
	// Keep the next step within the leash even near the circular boundary.
	x, z := player.X, player.Z
	if input.HeldLeft {
		x--
	}
	if input.HeldRight {
		x++
	}
	if input.HeldUp {
		z--
	}
	if input.HeldDown {
		z++
	}
	if math.Hypot(x-human.X, z-human.Z) > buddyWanderRadius {
		buddy.wanderValid = false
		return steerBuddy(player, human.X, human.Z, 8, 2)
	}
	return input
}

func (model *GameModel) validBuddyTarget(enemy *Enemy) bool {
	return enemy != nil && enemy.Health > 0 && !enemy.IsDead() && slices.Contains(model.Enemies, enemy)
}

// Preserve existing targets, reserve grabbed enemies for their grabbers, and
// randomly distribute remaining buddies over the least-claimed enemies.
func (model *GameModel) assignBuddyTargets() {
	if len(model.Buddies) == 0 {
		return
	}
	if len(model.Enemies) == 0 {
		for _, buddy := range model.Buddies {
			buddy.target = nil
		}
		return
	}
	var enemies []*Enemy
	live := make(map[*Enemy]bool)
	for _, enemy := range model.Enemies {
		if enemy.Health > 0 && !enemy.IsDead() && !live[enemy] {
			live[enemy] = true
			enemies = append(enemies, enemy)
		}
	}
	assigned := make(map[*Buddy]*Enemy)
	claims := make(map[*Enemy]int)
	var active []*Buddy
	for _, buddy := range model.Buddies {
		if buddy.Player.Health <= 0 || buddy.Player.Status == PlayerStateDead {
			continue
		}
		active = append(active, buddy)
		if grabbed, ok := buddy.Player.Grabbed.(*Enemy); ok && live[grabbed] {
			assigned[buddy] = grabbed
			claims[grabbed]++
		}
	}
	for _, buddy := range active {
		if assigned[buddy] == nil && live[buddy.target] && claims[buddy.target] == 0 {
			assigned[buddy] = buddy.target
			claims[buddy.target]++
		}
	}
	for _, buddy := range active {
		if assigned[buddy] != nil || len(enemies) == 0 {
			continue
		}
		minimum := len(model.Buddies) + 1
		var choices []*Enemy
		for _, enemy := range enemies {
			if claims[enemy] < minimum {
				minimum = claims[enemy]
				choices = choices[:0]
			}
			if claims[enemy] == minimum {
				choices = append(choices, enemy)
			}
		}
		target := buddy.target
		if !live[target] || claims[target] != minimum {
			target = choices[rand.N(len(choices))]
		}
		assigned[buddy] = target
		claims[target]++
	}
	for _, buddy := range model.Buddies {
		buddy.target = assigned[buddy]
	}
}

func (buddy *Buddy) input(model *GameModel, human *PlayerState, level *Level) InputState {
	player := buddy.Player
	if buddy.cooldown > 0 {
		buddy.cooldown--
	}
	switch player.Status {
	case PlayerStateDead, PlayerStateDropping, PlayerStatePain, PlayerStateFalling, PlayerStateFallen, PlayerJump:
		return InputState{}
	}
	if current := player.ShowAnimation; current != nil && current != player.Animations["grab"] {
		buddy.queueComboFollowUp(current, model.Counter)
		return InputState{}
	}
	if (buddy.target == nil && len(model.Enemies) > 0) || (buddy.target != nil && !model.validBuddyTarget(buddy.target)) {
		model.assignBuddyTargets()
	}
	target := buddy.target
	if target == nil {
		return buddy.wanderInput(model, human, level)
	}
	buddy.wanderValid, buddy.wanderPause = false, 0
	x, z, stop := target.X, target.Z, 45.0
	ready := player.ShowAnimation == nil || (player.Status == PlayerStateGrab && player.ShowAnimation == player.Animations["grab"])
	if ready && buddy.cooldown == 0 && abs(player.Z-z) < Z_DISTANCE/2 && target.State != EnemyStateFalling && target.State != EnemyStateFallen {
		distance := abs(player.X - x)
		if player.Status != PlayerStateGrab {
			if x < player.X {
				player.Facing = FacingLeft
			} else {
				player.Facing = FacingRight
			}
		}
		var move *Animation
		if distance >= 90 && distance <= 180 {
			move = buddy.attack(true)
		}
		if distance <= 55 || player.Status == PlayerStateGrab {
			move = buddy.attack(false)
		}
		if move != nil {
			if player.ActivatedAnimations == nil {
				player.ActivatedAnimations = make(map[*Animation]uint64)
			}
			player.ActivatedAnimations[move] = model.Counter
			buddy.cooldown = 25
			buddy.attacks++
			return InputState{}
		}
	}
	var input InputState
	if player.Status == PlayerStateGrab {
		return input
	}
	if player.X < x-stop {
		input.HeldRight = true
	}
	if player.X > x+stop {
		input.HeldLeft = true
	}
	if player.Z < z-2 {
		input.HeldDown = true
	}
	if player.Z > z+2 {
		input.HeldUp = true
	}
	return input
}

// Remember the roster slot, not the character: duplicate fighters can die
// independently, and defeated slots must stay empty on subsequent levels.
func (buddy *Buddy) recordDeath() {
	if buddy.session == nil || (buddy.Player.Health > 0 && buddy.Player.Status != PlayerStateDead) {
		return
	}
	if buddy.session.defeatedBuddies == nil {
		buddy.session.defeatedBuddies = make(map[int]bool)
	}
	buddy.session.defeatedBuddies[buddy.slot] = true
}

func (model *GameModel) UpdateBuddies(human *PlayerState, level *Level, system System, flashes *FlashFactory) {
	model.assignBuddyTargets()
	for _, buddy := range model.Buddies {
		buddy.recordDeath()
		if buddy.Player.Status == PlayerStateDead {
			continue
		}
		model.UpdatePlayer(buddy.Player, buddy.input(model, human, level), level, system, flashes)
		buddy.recordDeath()
	}
}

func arcadeModeOptions() []MenuOption {
	return []MenuOption{
		{Kind: "adventure", Name: "Solo", Info: "Play Arcade on your own"},
		{Kind: "adventure-cpu", Name: "With AI Buddies", Info: "Play Arcade with your chosen AI buddies"},
	}
}
