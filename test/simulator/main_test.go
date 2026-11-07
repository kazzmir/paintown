package main

import (
	"errors"
	"os"
	"testing"

	"github.com/kazzmir/paintown/game/data"
	"github.com/kazzmir/paintown/game/paintown"
)

func simulatorPlayer() *paintown.PlayerState {
	return &paintown.PlayerState{
		X: 100, Z: 200, Health: 100, MaxHealth: 100, PainThreshold: 4,
		Attackers: make(map[paintown.Attacker]uint64),
	}
}

func simulatorEnemy(x float64) *paintown.Enemy {
	return &paintown.Enemy{X: x, Z: 200, Health: 10000000, MaxHealth: 10000000, PainThreshold: 7}
}

func TestDebugExplosionChoosesNearestEnabledEnemy(t *testing.T) {
	engine := &Engine{
		Player:         simulatorPlayer(),
		Enemies:        []*paintown.Enemy{simulatorEnemy(250), simulatorEnemy(120), simulatorEnemy(110)},
		EnemiesEnabled: []bool{true, true, false},
	}
	engine.explodeDebugCharacter(false)
	if engine.Enemies[0].Exploded || !engine.Enemies[1].Exploded || engine.Enemies[2].Exploded || engine.Player.Exploded {
		t.Fatal("G did not explode only the nearest enabled enemy")
	}
	engine.Model.Counter = 100
	engine.scheduleRespawns()
	deadline := engine.enemyRespawnAt[1]
	if deadline != 100+respawnDelayTicks() {
		t.Fatal("explosion did not schedule the two-second respawn")
	}
	engine.Model.Counter++
	engine.scheduleRespawns()
	if engine.enemyRespawnAt[1] != deadline {
		t.Fatal("respawn deadline was restarted every frame")
	}
	engine.explodeDebugCharacter(true)
	if !engine.Player.Exploded || engine.Player.Status != paintown.PlayerStateDead {
		t.Fatal("Shift+G did not explode the player")
	}
	engine.scheduleRespawns()
	if engine.playerRespawnAt != engine.Model.Counter+respawnDelayTicks() {
		t.Fatal("player respawn was not scheduled")
	}
}

func TestSimulatorRespawnRecreatesCharactersWithoutClearingEffects(t *testing.T) {
	oldPlayer, oldEnemy := simulatorPlayer(), simulatorEnemy(230)
	oldPlayer.AttackId = 88
	gore := &paintown.GoreParticle{Y: 10, Vy: 2}
	projectile := &paintown.Projectile{Life: 70}
	engine := &Engine{
		Player: oldPlayer, Enemies: []*paintown.Enemy{oldEnemy}, EnemiesEnabled: []bool{true},
		Model: paintown.GameModel{Counter: 100, Gore: []*paintown.GoreParticle{gore}, Projectiles: []*paintown.Projectile{projectile}},
	}
	playerSpawns, enemySpawns := 0, 0
	engine.makePlayer = func() (*paintown.PlayerState, error) { playerSpawns++; return simulatorPlayer(), nil }
	engine.makeEnemy = func(index int) (*paintown.Enemy, error) {
		enemySpawns++
		return simulatorEnemy(float64(230 + index*40)), nil
	}
	engine.explodeDebugCharacter(false)
	engine.explodeDebugCharacter(true)
	engine.scheduleRespawns()
	engine.Model.Counter = engine.playerRespawnAt - 1
	if err := engine.respawnCharacters(); err != nil {
		t.Fatal(err)
	}
	if playerSpawns != 0 || enemySpawns != 0 {
		t.Fatal("characters respawned before the delay")
	}
	engine.Model.Counter++
	if err := engine.respawnCharacters(); err != nil {
		t.Fatal(err)
	}
	if playerSpawns != 1 || enemySpawns != 1 || engine.Player == oldPlayer || engine.Enemies[0] == oldEnemy {
		t.Fatal("respawn did not create fresh character instances")
	}
	if engine.Player.Exploded || engine.Player.Status != paintown.PlayerIdle || engine.Player.Health != 100 || engine.Enemies[0].Exploded || engine.Enemies[0].Health != 10000000 {
		t.Fatal("respawn retained death state or failed to restore health")
	}
	if engine.Player.AttackId <= oldPlayer.AttackId {
		t.Fatal("respawn reset melee tickets below surviving enemies' last hit")
	}
	if engine.playerRespawnAt != 0 || engine.enemyRespawnAt[0] != 0 {
		t.Fatal("respawn did not clear its deadline")
	}
	if len(engine.Model.Gore) != 1 || engine.Model.Gore[0] != gore || len(engine.Model.Projectiles) != 1 || engine.Model.Projectiles[0] != projectile {
		t.Fatal("respawning cleared the existing effects")
	}
}

func TestDisabledEnemyRespawnAndFactoryFailure(t *testing.T) {
	enemy := simulatorEnemy(230)
	enemy.Hurt(1, 10000020, 2)
	engine := &Engine{Player: simulatorPlayer(), Enemies: []*paintown.Enemy{enemy}, EnemiesEnabled: []bool{false}}
	engine.Model.ShowHealthMap = map[*paintown.Enemy]uint64{enemy: 1}
	engine.Player.Attackers[enemy] = 1
	engine.scheduleRespawns()
	engine.Model.Counter = engine.enemyRespawnAt[0]
	failure := errors.New("failed spawn")
	engine.makeEnemy = func(int) (*paintown.Enemy, error) { return nil, failure }
	if err := engine.respawnCharacters(); err != failure || engine.Enemies[0] != enemy {
		t.Fatal("failed respawn lost the old instance or its error")
	}
	engine.makeEnemy = func(int) (*paintown.Enemy, error) { return simulatorEnemy(230), nil }
	if err := engine.respawnCharacters(); err != nil {
		t.Fatal(err)
	}
	if engine.EnemiesEnabled[0] || engine.Enemies[0] == enemy || engine.Enemies[0].Exploded {
		t.Fatal("respawn changed the enemy enable toggle or retained its corpse")
	}
	if _, retained := engine.Model.ShowHealthMap[enemy]; retained {
		t.Fatal("respawn retained the old enemy's health entry")
	}
	if _, retained := engine.Player.Attackers[enemy]; retained {
		t.Fatal("respawn retained the old enemy's attack entry")
	}
	engine.explodeDebugCharacter(false)
	if engine.Enemies[0].Exploded {
		t.Fatal("debug explosion selected a disabled enemy")
	}
}

func TestSimulatorRespawnReusesCachedSprites(t *testing.T) {
	t.Chdir("../..")
	if _, err := os.Stat("paintown-data/misc/body/arm.png"); os.IsNotExist(err) {
		t.Skip("game assets not installed")
	}
	playerDefinition, err := paintown.LoadDefinition(data.DataPath("players/akuma/akuma.txt"))
	if err != nil {
		t.Fatal(err)
	}
	enemyDefinition, err := paintown.LoadDefinition(data.DataPath("chars/punk/punk.txt"))
	if err != nil {
		t.Fatal(err)
	}
	engine := MakeEngine(playerDefinition, []paintown.CharacterDefinition{enemyDefinition})
	player, err := engine.makePlayer()
	if err != nil {
		t.Fatal(err)
	}
	enemy, err := engine.makeEnemy(0)
	if err != nil {
		t.Fatal(err)
	}
	engine.Player, engine.Enemies, engine.EnemiesEnabled = player, []*paintown.Enemy{enemy}, []bool{true}
	engine.explodeDebugCharacter(false)
	engine.explodeDebugCharacter(true)
	engine.Model.Enemies = engine.Enemies
	engine.Model.UpdateEnemies(&engine.Level, engine.Player, &paintown.DummySystem{}, nil)
	particles := len(engine.Model.Gore)
	if particles == 0 {
		t.Fatal("debug explosion did not produce particles")
	}
	engine.scheduleRespawns()
	engine.Model.Counter = engine.playerRespawnAt
	if err := engine.respawnCharacters(); err != nil {
		t.Fatal(err)
	}
	if engine.Player.Animations["idle"].FirstFrame() != player.Animations["idle"].FirstFrame() || engine.Enemies[0].Animations["idle"].FirstFrame() != enemy.Animations["idle"].FirstFrame() {
		t.Fatal("respawn reloaded sprites instead of reusing the cache")
	}
	if engine.Player.Animations["idle"].Owner != engine.Player || engine.Enemies[0].Animations["idle"].Owner != engine.Enemies[0] || len(engine.Model.Gore) != particles {
		t.Fatal("respawn reused live playback state or cleared particles")
	}
}
