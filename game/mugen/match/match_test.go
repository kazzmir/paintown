package match

import (
	"bytes"
	"fmt"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/sound"
)

func stockMatch(t *testing.T, time int) *Match {
	t.Helper()
	root, err := filepath.Abs("../temp/mugen-1.1b1")
	if err != nil {
		t.Fatal(err)
	}
	m, err := New(Request{Players: [2]Selection{{DefPath: filepath.Join(root, "chars/kfm/kfm.def"), Palette: 1}, {DefPath: filepath.Join(root, "chars/kfm720/kfm720.def"), Palette: 2}}, StagePath: "stages/kfm.def", Mode: "versus"}, Options{RandomSeed: 1, RootDir: root, FightPath: "data/fight.def", GameSpace: coord.Space{Width: 1280, Height: 720}, RoundTime: time, LifePercent: 100, Input: func(int, int) []string { return nil }})
	if err != nil {
		t.Fatal(err)
	}
	return m
}

func advanceUntil(t *testing.T, m *Match, phase Phase, limit int) {
	t.Helper()
	for i := 0; i < limit && m.Phase != phase; i++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
	}
	if m.Phase != phase {
		t.Fatalf("phase=%d want=%d at tick %d, states=%d/%d", m.Phase, phase, m.Tick, m.Players[0].Character.StateNo, m.Players[1].Character.StateNo)
	}
}

func TestStockMixedMatchIntroKOAndReset(t *testing.T) {
	m := stockMatch(t, 99)
	if m.Players[0].Character.X != -70 || m.Players[1].Character.X != 280 {
		t.Fatal("mixed spawn", m.Players[0].Character.X, m.Players[1].Character.X)
	}
	advanceUntil(t, m, Active, 600)
	if m.Players[1].Character.Palettes.Destination(palette.ID{1, 1}) != (palette.ID{1, 2}) {
		t.Fatal("selected palette did not reach common RemapPal")
	}
	for slot, p := range m.Players {
		if !p.Character.Ctrl || p.Character.RoundState != 2 || p.Character.InputSlot != slot+1 {
			t.Fatal("control/identity", slot, p.Character)
		}
	}
	m.Players[1].Character.SetLife(0)
	// Isolate lifecycle with a valid dead pose; a standing self-KO can hold
	// native pre-over until its 900-pass readiness timeout.
	m.Players[1].Character.ChangeState(5150, 0, -1)
	advanceUntil(t, m, PreOver, 2)
	if m.Winner != 0 || m.Wins != [2]int{} || m.TimeOver {
		t.Fatal("KO result", m.Winner, m.Wins, m.TimeOver)
	}
	advanceUntil(t, m, Over, 300)
	if m.Wins != [2]int{1, 0} {
		t.Fatal("round-state 4 did not commit win", m.Wins)
	}
	advanceUntil(t, m, PreIntro, 500)
	if m.Round != 2 || m.TimeLeft != 99 {
		t.Fatal("round reset", m.Round, m.TimeLeft)
	}
	for _, p := range m.Players {
		if p.Character.Life != p.Character.MaxLife || p.Character.RoundNo != 2 || p.Character.RoundsExisted != 1 || len(p.Character.Helpers) != 0 {
			t.Fatal("fresh round state")
		}
	}
	advanceUntil(t, m, Active, 600)
	if m.Players[1].Character.Palettes.Destination(palette.ID{1, 1}) != (palette.ID{1, 2}) {
		t.Fatal("round reset lost selected palette")
	}
	m.Players[1].Character.SetLife(0)
	m.Players[1].Character.ChangeState(5150, 0, -1)
	advanceUntil(t, m, Finished, 500)
	if m.Wins != [2]int{2, 0} || !m.Players[0].Character.IsMatchOver {
		t.Fatal("match result", m.Wins)
	}
}

func TestStockStageCropSurvivesCameraUpdatesAndRoundReset(t *testing.T) {
	base := stockMatch(t, 99)
	options := base.Options
	options.StageCrop, options.GameSpace = true, coord.Space{Width: 1280, Height: 720}
	m, err := New(base.Request, options)
	if err != nil {
		t.Fatal(err)
	}
	check := func() {
		t.Helper()
		if m.Options.GameSpace != (coord.Space{1280, 720}) || m.CameraY != 140 || m.Viewport.CropY != 140 || m.Viewport.CameraY != 140 {
			t.Fatal("native StageFit=0 crop lost", m.Options.GameSpace, m.CameraY, m.Viewport)
		}
	}
	check()
	advanceUntil(t, m, Active, 600)
	check()
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	check()
}

func TestStockMatchTimeOverUsesLifeFraction(t *testing.T) {
	m := stockMatch(t, 1)
	advanceUntil(t, m, Active, 600)
	m.Players[0].Character.MaxLife = 2000
	m.Players[0].Character.Life = 1000
	m.Players[1].Character.Life = 750
	// Native counts through the displayed zero interval before time-over.
	advanceUntil(t, m, PreOver, 121)
	// Time-over result flags become observable one CNS pass after state 3.
	if err := m.Update(); err != nil {
		t.Fatal(err)
	}
	if m.Winner != 1 || !m.TimeOver || m.TimeLeft != 0 {
		t.Fatal("time over must compare fractions", m.Winner, m.TimeOver, m.TimeLeft)
	}
}

func TestStockMatchBasicAttackFromLogicalInput(t *testing.T) {
	m := stockMatch(t, 99)
	advanceUntil(t, m, Active, 600)
	m.SoundQueue.Drain()
	mixer, err := sound.NewMixer(sound.Options{SampleRate: 44100, MaxVoices: 32, Volume: .8, Stereo: true, PanningWidth: 240})
	if err != nil {
		t.Fatal(err)
	}
	source := func(slot, facing int) []string {
		if slot != 1 {
			return nil
		}
		distance, _ := m.Players[0].Character.GetOpponentBodyDistance()
		if distance > 16 {
			return []string{"DirForward"}
		}
		if m.Tick%40 < 2 {
			return []string{"ButtonX"}
		}
		return nil
	}
	for _, p := range m.Players {
		p.Character.InputSource = source
	}
	sawAttack, sawHit, sawContactCue, audible, sawSpark := false, false, false, false, false
	for tick := 0; tick < 600; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
		for _, command := range m.SoundQueue.Drain() {
			if command.Operation == sound.Play && command.Bank == m.Fight.CommonSounds && command.Group == 5 {
				sawContactCue = true
			}
			if err := mixer.Apply(command); err != nil {
				t.Fatal(err)
			}
		}
		pcm := make([]byte, 735*4)
		if _, err := mixer.Read(pcm); err != nil {
			t.Fatal(err)
		}
		audible = audible || !bytes.Equal(pcm, make([]byte, len(pcm)))
		for _, instance := range m.Effects.Active {
			sawSpark = true
			if instance.Source.Sprites != m.Fight.EffectSprites || instance.Source.Animations != m.Fight.EffectAnimations {
				t.Fatal("KFM contact did not use common effect resources")
			}
		}
		if m.Players[0].Character.StateNo == 200 {
			sawAttack = true
		}
		if m.Players[1].Character.MoveType == "H" {
			sawHit = true
		}
		if sawAttack && sawHit && m.Players[1].Character.Ctrl && m.Players[1].Character.Life < m.Players[1].Character.MaxLife {
			if !sawContactCue || !audible || !sawSpark {
				t.Fatal("real KFM contact did not reach shared sound/effect playback", sawContactCue, audible, sawSpark)
			}
			heldGuardFlow(t, m)
			combatScalingFlow(t, m)
			runtimeFallbackFlow(t, m)
			animationCompatFlow(t, m)
			return
		}
	}
	t.Fatalf("logical walk/punch/hit/recovery incomplete: attack=%v hit=%v life=%d state=%d ctrl=%v", sawAttack, sawHit, m.Players[1].Character.Life, m.Players[1].Character.StateNo, m.Players[1].Character.Ctrl)
}

// Continue the real mixed-coordinate fight through held guard, posture changes,
// a blocked punch, release and jumping out. Native beta keeps Ctrl=1 throughout
// ordinary guard: it must not let the movement adapter restart walking/crouch.
func heldGuardFlow(t *testing.T, m *Match) {
	t.Helper()
	defender, attacker := m.Players[0].Character, m.Players[1].Character
	threat, err := cns.Parse(strings.NewReader(`[Statedef 9000]
type=S
physics=N
movetype=A
ctrl=0
anim=0
velset=0,0
[State 9000, stationary threat without a contact box]
type=HitDef
trigger1=Time=0
attr=S,NA
guard.dist=Const240p(180)
`))
	if err != nil {
		t.Fatal(err)
	}
	attacker.StateFile.States[9000] = threat.States[9000]
	keys := []string{"DirBack"}
	defender.InputSource = func(int, int) []string { return keys }
	attacker.InputSource = func(int, int) []string { return nil }
	for slot, player := range m.Players {
		c := player.Character
		c.X, c.Y = float64(-30+60*slot)*float64(c.LocalCoordinateWidth())/320, 0
		c.HitPauseTime, c.ShakeTime, c.HitTime = 0, 0, -1
		c.ChangeState(0, 1, -1)
	}
	attacker.ChangeState(9000, -1, -1)
	step := func(ticks int) {
		t.Helper()
		for tick := 0; tick < ticks; tick++ {
			if err := m.Update(); err != nil {
				t.Fatal("held guard lifecycle", err)
			}
		}
	}
	hold := func(state int) {
		t.Helper()
		step(8)
		previous := defender.Time
		for tick := 0; tick < 25; tick++ {
			step(1)
			if defender.StateNo != state || !defender.Ctrl || !defender.GetInGuardDist() || defender.Time != previous+1 {
				t.Fatalf("held guard restarted: state=%d time=%d previous=%d ctrl=%v distance=%v, want state %d", defender.StateNo, defender.Time, previous, defender.Ctrl, defender.GetInGuardDist(), state)
			}
			previous = defender.Time
		}
	}
	hold(130)
	keys = []string{"DirBack", "DirDown"}
	hold(131)
	keys = []string{"DirBack"}
	hold(130)

	// Contact uses the real punch, collision, guard shake/slide and recovery.
	life := defender.Life
	attacker.ChangeState(200, 0, -1)
	blocked, recovered := false, false
	for tick := 0; tick < 100; tick++ {
		step(1)
		blocked = blocked || defender.GetHitGuarded && defender.MoveType == "H"
		if blocked && defender.MoveType != "H" && defender.Ctrl {
			recovered = true
			break
		}
	}
	if !blocked || !recovered || defender.Life != life {
		t.Fatal("blocked punch did not restore control", blocked, recovered, defender.StateNo, defender.Life, life)
	}
	attacker.ChangeState(9000, -1, -1)
	hold(130)

	// Native cancels guard-end into forward walking immediately.
	keys = []string{"DirForward"}
	step(2)
	if defender.StateNo != 20 || !defender.Ctrl {
		t.Fatal("guard release prevented forward movement", defender.StateNo, defender.Time)
	}
	keys = []string{"DirBack"}
	hold(130)
	keys = []string{"DirBack", "DirUp"}
	step(24)
	if defender.StateNo != 50 || defender.StateType != "A" || defender.Y >= 0 {
		t.Fatal("up-back did not jump out of guard", defender.StateNo, defender.StateType, defender.Y)
	}
	keys = nil
	step(90)
	if defender.StateNo != 0 || !defender.Ctrl || defender.Y != 0 {
		t.Fatal("guard/jump lifecycle did not return to idle", defender.StateNo, defender.Time, defender.Y)
	}
	projectileGuardFlow(t, m)
}

// Continue guarding through independently moving projectiles and helpers while
// their owner remains idle, then block a real fireball, recover and retire them.
// Beta observations distinguish the projectile default90 from physical160.
func projectileGuardFlow(t *testing.T, m *Match) {
	t.Helper()
	var script strings.Builder
	script.WriteString(`[Statedef 9001]
type=S
movetype=I
physics=N
ctrl=0
anim=9990
velset=0,0
[State place]
type=PosSet
trigger1=1
x=Const240p(-100)
[State helper]
type=Helper
trigger1=Time=60
id=55
stateno=9003
postype=p1
pos=Const240p(120),0
helpertype=normal
`)
	for _, row := range []struct{ tick, id, action, lifetime int }{{0, 11, 9990, 45}, {100, 22, 9992, 110}} {
		fmt.Fprintf(&script, `[State projectile]
type=Projectile
trigger1=Time=%d
projid=%d
projanim=%d
projhitanim=9991
projremanim=9991
projremovetime=%d
projstagebound=1000
projedgebound=1000
offset=Const240p(40),0
velocity=Const240p(4),0
attr=S,SP
hitflag=MAF
guardflag=MA
damage=20,5
pausetime=0,0
guard.pausetime=0,0
ground.velocity=0,0
guard.velocity=0
guard.hittime=10
guard.slidetime=10
guard.ctrltime=10
sparkno=-1
guard.sparkno=-1
hitsound=-1
guardsound=-1
`, row.tick, row.id, row.action, row.lifetime)
	}
	for index, row := range []struct {
		tick       int
		expression string
	}{
		{10, "EnemyNear,InGuardDist"}, {25, "EnemyNear,InGuardDist"}, {50, "EnemyNear,InGuardDist"},
		{25, "NumProj"}, {50, "NumProj"}, {65, "EnemyNear,InGuardDist"},
		{85, "EnemyNear,InGuardDist"}, {65, "NumHelper(55)"}, {85, "NumHelper(55)"},
		{43, "EnemyNear,InGuardDist"}, {75, "EnemyNear,InGuardDist"}, {65, "MoveType=I"},
		{190, "EnemyNear,Life"}, {190, "ProjGuardedTime(22)>=0"}, {190, "ProjHitTime(22)>=0"},
		{190, "NumProj"}, {190, "EnemyNear,Ctrl"}, {190, "MoveType=I"},
	} {
		fmt.Fprintf(&script, "[State snapshot]\ntype=VarSet\ntrigger1=Time=%d\nv=%d\nvalue=%s\n", row.tick, 40+index, row.expression)
	}
	script.WriteString(`[Statedef 9002]
type=S
movetype=I
physics=N
ctrl=0
anim=9990
velset=0,0
[State place]
type=PosSet
trigger1=1
x=Const240p(100)
[State accept input]
type=CtrlSet
trigger1=Time=95
value=1
[Statedef 9003]
type=S
movetype=A
physics=N
ctrl=0
anim=9990
velset=Const240p(8),0
[State threat]
type=HitDef
trigger1=Time=0
attr=S,NA
guard.dist=Const240p(64)
[State retire]
type=DestroySelf
trigger1=Time=20
`)
	states, err := cns.Parse(strings.NewReader(script.String()))
	if err != nil {
		t.Fatal(err)
	}
	poses, err := air.Parse(strings.NewReader(`[Begin Action 9990]
0,0,0,0,-1
[Begin Action 9991]
0,0,0,0,5
[Begin Action 9992]
Clsn1Default: 1
Clsn1[0]=-6,-70,6,-40
0,0,0,0,-1
`))
	if err != nil {
		t.Fatal(err)
	}
	caster, receiver := m.Players[0].Character, m.Players[1].Character
	for slot, player := range m.Players {
		c := player.Character
		for number, state := range states.States {
			c.StateFile.States[number] = state
		}
		for number, action := range poses.Actions {
			c.AirFile.Actions[number] = action
		}
		c.Facing = 1 - 2*slot
		c.Y, c.VelX, c.VelY = 0, 0, 0
		c.HitPauseTime, c.ShakeTime, c.HitTime = 0, 0, -1
		c.SetLife(c.MaxLife)
		c.InputSource = func(int, int) []string { return nil }
		c.ChangeState(9001+slot, 0, -1)
	}
	receiver.InputSource = func(int, int) []string {
		if caster.Time >= 100 && caster.Time < 195 {
			return []string{"DirBack"}
		}
		return nil
	}
	for tick := 0; tick < 240; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("projectile/helper guard lifecycle", err)
		}
	}
	for index, expected := range []int{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 995, 1, 0, 0, 1, 1} {
		if got := caster.GetVar(40 + index); got != expected {
			t.Fatalf("native guard lifecycle observation %d: got %d want %d (receiver state/time/life %d/%d/%d)", index, got, expected, receiver.StateNo, receiver.Time, receiver.Life)
		}
	}
	if caster.MoveType != "I" || len(caster.Projectiles) != 0 || len(m.World.Helpers) != 0 || receiver.Life != 995 || receiver.GetInGuardDist() || receiver.StateNo != 0 || !receiver.Ctrl {
		t.Fatal("retired attack still held guard or prevented recovery", caster.MoveType, len(caster.Projectiles), len(m.World.Helpers), receiver.Life, receiver.StateNo, receiver.Ctrl)
	}
	caster.ChangeState(0, 1, -1)
}

// Continue the stock input/contact flow through one native-calibrated damage
// sequence, own-state recovery and a fresh round. Stats are dimensionless even
// with the existing 320p/1280p players. The beta observations below use attack150,
// defence200, AttackMulSet2, DefenceMulSet.5 and a brief p2defmul2 SuperPause.
func combatScalingFlow(t *testing.T, m *Match) {
	t.Helper()
	// Stock CMD rewrites var(1) every tick; keep instrumentation in unused slots.
	var script strings.Builder
	script.WriteString(`[Data]
attack=150
defence=200
[Statedef 700]
type=S
movetype=A
physics=N
ctrl=0
anim=9991
[State hold]
type=AssertSpecial
trigger1=1
flag=roundnotover
[State attack multiplier]
type=AttackMulSet
trigger1=Time=6
value=2
[State hit]
type=HitDef
trigger1=Time=0 || Time=10 || Time=50
attr=S,NA
hitflag=MAF
damage=100,20
pausetime=0,0
getpower=0,0
givepower=0,0
ground.type=High
ground.hittime=10000
ground.velocity=0,0
sparkno=-1
hitsound=-1
id=1234
[State brief super pause]
type=SuperPause
trigger1=Time=20
time=1
movetime=1
anim=-1
sound=-1
p2defmul=2
`)
	for _, row := range []struct {
		index, tick int
		expression  string
	}{
		{0, 3, "Target,Life"}, {1, 1, "Target,GetHitVar(damage)"},
		{2, 13, "Target,Life"}, {3, 11, "Target,GetHitVar(damage)"},
		{4, 15, "Target,Life"}, {5, 17, "Target,Life"}, {6, 19, "Target,Life"},
		{7, 48, "EnemyNear,Var(53)"}, {8, 48, "EnemyNear,Var(54)"}, {9, 48, "EnemyNear,Var(55)"},
		{10, 53, "Target,Life"}, {11, 51, "Target,GetHitVar(damage)"},
	} {
		fmt.Fprintf(&script, "[State snapshot]\ntype=VarSet\ntrigger1=Time=%d\nv=%d\nvalue=%s\n", row.tick, 40+row.index, row.expression)
	}
	for _, row := range []struct{ tick, value, absolute int }{{14, -100, 0}, {16, -100, 1}, {18, 100, 0}} {
		fmt.Fprintf(&script, "[State target life]\ntype=TargetLifeAdd\ntrigger1=Time=%d\nvalue=%d\nabsolute=%d\n", row.tick, row.value, row.absolute)
	}
	script.WriteString(`[Statedef 701]
type=S
movetype=I
physics=N
ctrl=0
anim=9990
[State wait]
type=Null
trigger1=1
[Statedef 5000]
type=S
movetype=H
physics=N
ctrl=0
anim=9990
[State hit defence]
type=DefenceMulSet
trigger1=Time=0
value=.5
[State self damage]
type=LifeAdd
trigger1=Time=25
value=-100
[State absolute self damage]
type=LifeAdd
trigger1=Time=27
value=-100
absolute=1
[State heal]
type=LifeAdd
trigger1=Time=29
value=100
[State recovered]
type=VarSet
trigger1=Time=33
v=59
value=1
[State recover]
type=SelfState
trigger1=Time=34
value=0
ctrl=1
[State life snapshot]
type=VarSet
trigger1=Time=26
v=53
value=Life
[State life snapshot]
type=VarSet
trigger1=Time=28
v=54
value=Life
[State life snapshot]
type=VarSet
trigger1=Time=30
v=55
value=Life
`)
	states, err := cns.Parse(strings.NewReader(script.String()))
	if err != nil {
		t.Fatal(err)
	}
	poses, err := air.Parse(strings.NewReader(`[Begin Action 9990]
Clsn2Default: 1
Clsn2[0]=-30,-100,30,0
0,0,0,0,-1
[Begin Action 9991]
Clsn1Default: 1
Clsn1[0]=-1000,-1000,1000,1000
0,0,0,0,-1
`))
	if err != nil {
		t.Fatal(err)
	}
	attacker, receiver := m.Players[0].Character, m.Players[1].Character
	for slot, player := range m.Players {
		c := player.Character
		c.StateFile.Data.Attack, c.StateFile.Data.Defence = states.Data.Attack, states.Data.Defence
		for _, key := range []string{"data.attack", "data.defence"} {
			c.StateFile.ConstantFields[key] = states.ConstantFields[key]
		}
		for number, state := range states.States {
			c.StateFile.States[number] = state
		}
		for number, action := range poses.Actions {
			c.AirFile.Actions[number] = action
		}
		c.X, c.Y, c.VelX, c.VelY = float64(-20+40*slot)*float64(c.LocalCoordinateWidth())/320, 0, 0, 0
		c.HitPauseTime, c.ShakeTime, c.HitTime = 0, 0, -1
		c.SetLife(c.MaxLife)
		c.Facing = 1 - 2*slot
		c.InputSource = func(int, int) []string { return nil }
		c.ChangeState(700+slot, 0, -1)
	}
	for tick := 0; tick < 100; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("scaled combat lifecycle", err)
		}
	}
	for index, expected := range []int{925, 75, 850, 75, 825, 725, 825, 812, 712, 812, 662, 150} {
		if got := attacker.GetVar(40 + index); got != expected {
			t.Fatalf("native damage sequence observation %d: got %d want %d (receiver life/state/time %d/%d/%d)", index, got, expected, receiver.Life, receiver.StateNo, receiver.Time)
		}
	}
	if receiver.StateNo != 0 || !receiver.Ctrl || receiver.Life != 637 {
		t.Fatal("scaled damage sequence did not recover", receiver.StateNo, receiver.Time, receiver.Life)
	}
	attacker.ChangeState(0, 1, -1)
	receiver.SetLife(0)
	receiver.ChangeState(5150, 0, -1)
	advanceUntil(t, m, PreOver, 2)
	advanceUntil(t, m, Over, 300)
	advanceUntil(t, m, PreIntro, 500)
	advanceUntil(t, m, Active, 600)
	for _, player := range m.Players {
		c := player.Character
		if c.Life != c.MaxLife || c.AttackMul != 1 || c.DefenceMul != 1 || c.GetDefenceDamageMultiplier() != .5 || c.StateFile.Data.Attack != 150 {
			t.Fatal("round reset lost base stats or retained temporary damage adjustments", c.Life, c.AttackMul, c.DefenceMul, c.GetDefenceDamageMultiplier())
		}
	}
}

// Continue the recovered fight through ordinary bottom-valued controller
// arguments, a same-tick state entry and SelfState back to grounded control.
// These twelve observations match a single native beta runtime sequence.
func runtimeFallbackFlow(t *testing.T, m *Match) {
	t.Helper()
	var script strings.Builder
	script.WriteString("[Statedef 9700]\ntype=S\nmovetype=I\nphysics=N\nctrl=0\nanim=0\n")
	operation := func(kind, params string) {
		fmt.Fprintf(&script, "[State operation]\ntype=%s\ntrigger1=Time=0\n%s\n", kind, params)
	}
	snapshot := func(index int, expression string) {
		operation("VarSet", fmt.Sprintf("v=%d\nvalue=%s", 40+index, expression))
	}
	operation("AssertSpecial", "flag=noko")
	operation("PosSet", "x=40\ny=-20")
	operation("PosSet", "x=1/0\ny=(var(58):=9)")
	snapshot(0, "Pos X")
	snapshot(1, "Pos Y")
	operation("PosAdd", "x=(var(58):=var(58)+1)\ny=1/0")
	snapshot(2, "Pos X")
	operation("VelSet", "x=2\ny=4")
	operation("VelMul", "x=1/0\ny=.5")
	snapshot(3, "Vel X")
	snapshot(4, "Vel Y")
	operation("LifeAdd", "value=1/0")
	snapshot(5, "Life")
	operation("LifeSet", "value=1/0")
	operation("LifeAdd", "value=100")
	snapshot(6, "Life")
	operation("PowerSet", "value=50")
	operation("PowerSet", "value=1/0")
	operation("PowerAdd", "value=100")
	operation("PowerAdd", "value=1/0")
	snapshot(7, "Power")
	operation("CtrlSet", "value=1/0")
	snapshot(8, "Ctrl")
	operation("PosSet", "x=11\ny=-30")
	operation("VelSet", "x=0\ny=0")
	operation("ChangeState", "value=9702\nctrl=1\nanim=0")
	script.WriteString("[Statedef 9702]\ntype=S\nmovetype=I\nphysics=N\nanim=0\nctrl=1/0\nvelset=1/0,2\n")
	snapshot(9, "Vel X")
	snapshot(10, "Vel Y")
	operation("LifeAdd", "value=-1000\nkill=1/0\nabsolute=1/0")
	snapshot(11, "Life")
	script.WriteString(`[State land]
type=PosSet
trigger1=Time=1
y=1/0
[State stop]
type=VelSet
trigger1=Time=1
x=0
y=0
[State recover]
type=SelfState
trigger1=Time=1
value=1/0
ctrl=1
[Statedef 9703]
[State fatal controller]
type=PosSet
trigger1=1
x=4
y=~(1/0)
[Statedef 9704]
velset=~(1/0),2
[State fatal entry]
type=Null
trigger1=1
[Statedef 9730]
[State fatal shield]
type=HitBy
trigger1=1
value=S,NA
time=~(1/0)
[Statedef 9731]
[State fatal override]
type=HitOverride
trigger1=1
attr=S,NA
stateno=0
time=~(1/0)
[Statedef 9732]
[State fatal sound]
type=PlaySnd
trigger1=1
value=0,0
volume=~(1/0)
[Statedef 9734]
[State fatal selector]
type=PosSet
trigger1=1
x=4
y=EnemyNear(~(1/0)),ID
[Statedef 9736]
[State fatal assignment index]
type=PosSet
trigger1=1
x=4
y=(var(~(1/0)):=(var(57):=1))
[Statedef 9737]
[State fatal interval bound]
type=PosSet
trigger1=1
x=4
y=(1=[~(1/0),(var(57):=1)])
[Statedef 9739]
[State fatal selector trigger]
type=PosSet
trigger1=EnemyNear(~(1/0)),ID
x=4
y=9
[Statedef 9733]
[State fatal palette]
type=RemapPal
trigger1=1
source=1,1
dest=1,~(1/0)
[Statedef 9722]
[State fatal admitted variable]
type=VarSet
trigger1=1
v=5
value=~(1/0)
`)
	states, err := cns.ParseNamed(strings.NewReader(script.String()), "runtime-fallback.cns")
	if err != nil {
		t.Fatal(err)
	}
	c := m.Players[0].Character
	for number, state := range states.States {
		c.StateFile.States[number] = state
	}
	c.ChangeState(9700, 0, -1)
	for tick := 0; tick < 8; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("runtime fallback lifecycle", err)
		}
	}
	for index, expected := range []int{0, 9, 10, 0, 2, 1000, 100, 100, 0, 0, 2, 1} {
		if got := c.GetVar(40 + index); got != expected {
			t.Fatalf("native runtime observation %d: got %d want %d", index, got, expected)
		}
	}
	if c.StateNo != 0 || !c.Ctrl || c.Y != 0 || c.VelX != 0 || c.VelY != 0 || c.Life != 1 || !c.IsAlive() || c.GetVar(58) != 10 {
		t.Fatal("runtime fallback did not preserve evaluation or recover", c.StateNo, c.Ctrl, c.Y, c.VelX, c.VelY, c.Life, c.IsAlive(), c.GetVar(58))
	}
	// Continue the same runtime recovery through invalid register/range
	// admission, valid writes, zero fallback and a second own-state return.
	var variables strings.Builder
	variables.WriteString("[Statedef 9720]\ntype=S\nmovetype=I\nphysics=N\nctrl=0\nanim=0\n")
	write := func(tick int, kind, params string) {
		fmt.Fprintf(&variables, "[State variable continuation]\ntype=%s\ntrigger1=Time=%d\n%s\n", kind, tick, params)
	}
	write(0, "VarRangeSet", "first=40\nlast=51\nvalue=0")
	write(0, "VarSet", "v=58\nvalue=60")
	write(0, "VarSet", "v=var(58)\nvalue=(var(40):=7)")
	write(1, "VarSet", "v=58\nvalue=-1")
	write(1, "VarAdd", "v=var(58)\nvalue=(var(41):=8)")
	write(2, "VarSet", "fv=var(58)+41\nvalue=(var(42):=9)")
	write(2, "VarSet", "v=var(58)\nvalue=~(1/0)")
	write(3, "VarSet", "v=(var(43):=5)\nvalue=(var(44):=11)")
	write(4, "VarSet", "v=45\nvalue=var(5)")
	write(5, "ChangeState", "value=9721\nctrl=1")
	variables.WriteString("[Statedef 9721]\ntype=S\nmovetype=I\nphysics=N\nctrl=1\nanim=0\n")
	write(0, "VarSet", "v=46\nvalue=Ctrl")
	write(1, "VarRangeSet", "first=-2\nlast=2\nvalue=(var(47):=13)")
	write(1, "VarRangeSet", "first=58\nlast=100\nvalue=19")
	write(1, "VarRangeSet", "first=-2\nlast=2\nfvalue=17")
	write(1, "VarRangeSet", "first=-2\nlast=2\nvalue=~(1/0)")
	write(2, "VarSet", "v=48\nvalue=var(58)")
	write(3, "VarRangeSet", "first=11\nlast=10\nvalue=(var(47):=33)")
	write(3, "VarRangeSet", "first=10\nlast=11\nvalue=(var(49):=var(49)+1)")
	write(3, "VarSet", "v=3\nvalue=9")
	write(3, "VarSet", "fv=3\nvalue=9")
	write(4, "VarRangeSet", "first=3\nlast=3\nvalue=1/0")
	write(4, "VarRangeSet", "first=3\nlast=3\nfvalue=1/0")
	write(5, "VarSet", "v=50\nvalue=var(3)")
	write(5, "VarSet", "v=51\nvalue=fvar(3)")
	write(6, "SelfState", "value=0\nctrl=1")
	registers, err := cns.ParseNamed(strings.NewReader(variables.String()), "variable-registers.cns")
	if err != nil {
		t.Fatal(err)
	}
	for number, state := range registers.States {
		c.StateFile.States[number] = state
	}
	c.ChangeState(9720, 0, -1)
	for tick := 0; tick < 16; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("variable admission/recovery", err)
		}
	}
	for index, expected := range []int{0, 0, 0, 5, 11, 11, 1, 0, -1, 1, 0, 0} {
		if c.GetVar(40+index) != expected {
			t.Fatal("native variable register observation", index, c.GetVar(40+index), expected)
		}
	}
	if c.StateNo != 0 || !c.Ctrl || c.GetVar(10) != 1 || c.GetVar(11) != 1 || c.GetFVar(3) != 0 {
		t.Fatal("variable writes did not complete and recover", c.StateNo, c.Ctrl)
	}
	scalarRuntimeRecoveryFlow(t, m)
	protectionMediaRecoveryFlow(t, m)
	// Genuine VM failures remain terminal and cannot partly apply a vector.
	for _, number := range []int{9703, 9704, 9722, 9730, 9731, 9732, 9733, 9734, 9736, 9737, 9739} {
		fatal := character.NewCharacter(c.StateFile, c.AirFile, c.CmdFile)
		fatal.X, fatal.VelX, fatal.VelY = 37, 4, 5
		var err error
		if number != 9704 {
			_, err = fatal.SM.ExecuteState(states.States[number], fatal)
		} else {
			err = fatal.SM.EnterState(states.States[number], fatal, engine.EntryOverrides{})
		}
		if err == nil || !strings.Contains(err.Error(), "bitwise not of bottom") || !strings.Contains(err.Error(), "runtime-fallback.cns") || fatal.X != 37 || fatal.VelX != 4 || fatal.VelY != 5 || fatal.GetVar(57) != 0 {
			t.Fatal("fatal VM error was hidden or partly applied", number, err, fatal.X, fatal.VelX, fatal.VelY)
		}
	}
}

// Continue through control/effect/stat fallbacks, a zero-damage contact and the
// minimum nonabsolute life adjustments, then recover both sides and retire art.
func scalarRuntimeRecoveryFlow(t *testing.T, m *Match) {
	t.Helper()
	c, other := m.Players[0].Character, m.Players[1].Character
	c.SetLife(1000)
	other.SetLife(1000)
	c.X, c.Y, other.X, other.Y = -70, 0, 70*float64(other.LocalCoordinateWidth())/320, 0
	c.RemoveExplod(-1)
	other.ChangeState(0, 0, -1)
	c.AirFile.Actions[9970] = &air.Action{ID: 9970, LoopStart: -1, Elements: []air.Element{{Group: 0, Image: 0, Time: -1, Clsn1: []air.Box{{X1: -500, Y1: -100, X2: 500, Y2: 0}}, Clsn2: []air.Box{{X1: -10, Y1: -100, X2: 10, Y2: 0}}}}}
	t.Cleanup(func() { delete(c.AirFile.Actions, 9970) })
	var script strings.Builder
	script.WriteString("[Statedef 9880]\ntype=S\nmovetype=A\nphysics=N\nctrl=0\nanim=9970\nvelset=2,0\n")
	for _, step := range []struct {
		kind   string
		tick   int
		params string
	}{
		{"VarRangeSet", 0, "first=40\nlast=51\nvalue=0"}, {"VarSet", 0, "v=58\nvalue=Pos X"},
		{"PlayerPush", 0, "value=1/0"}, {"PlayerPush", 1, "value=1/0"},
		{"AttackDist", 0, "value=200"}, {"AttackDist", 1, "value=200"}, {"AttackDist", 2, "value=1/0"}, {"AttackDist", 3, "value=1/0"},
		{"PosFreeze", 0, "value=1"}, {"PosFreeze", 1, "value=1/0"},
		{"AttackMulSet", 0, "value=2"}, {"AttackMulSet", 0, "value=1/0"},
		{"DefenceMulSet", 0, "value=2"}, {"DefenceMulSet", 0, "value=1/0"}, {"LifeAdd", 0, "value=-20"},
		{"HitAdd", 0, "value=3"}, {"HitAdd", 0, "value=1/0"}, {"VictoryQuote", 0, "value=1/0"},
		{"Explod", 0, "anim=9970\nid=0\nremovetime=-1"}, {"Explod", 0, "anim=9970\nid=1\nremovetime=-1"},
		{"RemoveExplod", 1, "id=1/0"},
		{"VarSet", 1, "v=40\nvalue=EnemyNear,InGuardDist"}, {"VarSet", 3, "v=41\nvalue=EnemyNear,InGuardDist"},
		{"VarSet", 2, "v=42\nvalue=Pos X-var(58)"}, {"VarSet", 0, "v=43\nvalue=Life"},
		{"VarSet", 2, "v=44\nvalue=NumExplod(0)"}, {"VarSet", 2, "v=45\nvalue=NumExplod(1)"},
		{"HitDef", 4, "attr=S,NA\nhitflag=MAF\ndamage=40,0\npausetime=0,0\nground.type=High\nground.hittime=1000\nground.velocity=0,0\nsparkno=-1\nguard.sparkno=-1\nhitsound=-1\nguardsound=-1\np2stateno=9881"},
		{"VarSet", 6, "v=46\nvalue=NumTarget"}, {"VarSet", 6, "v=47\nvalue=EnemyNear,Life"},
		{"TargetLifeAdd", 6, "value=-20"}, {"VarSet", 6, "v=49\nvalue=EnemyNear,Life"},
		{"LifeAdd", 6, "value=0"}, {"VarSet", 6, "v=50\nvalue=Life"},
		{"TargetLifeAdd", 6, "value=0"}, {"VarSet", 6, "v=51\nvalue=EnemyNear,Life"},
		{"ChangeState", 7, "value=9882\nctrl=1"},
	} {
		fmt.Fprintf(&script, "[State scalar continuation]\ntype=%s\ntrigger1=Time=%d\n%s\n", step.kind, step.tick, step.params)
	}
	script.WriteString("[Statedef 9881]\ntype=S\nmovetype=H\nphysics=N\nanim=0\nctrl=0\n[State zero defence]\ntype=DefenceMulSet\ntrigger1=Time=0\nvalue=0\n[State recover own state]\ntype=SelfState\ntrigger1=Time=10\nvalue=0\nctrl=1\n[Statedef 9882]\ntype=S\nmovetype=I\nphysics=N\nanim=0\nctrl=1\nvelset=0,0\n[State control]\ntype=VarSet\ntrigger1=Time=0\nv=48\nvalue=Ctrl\n[State retire art]\ntype=RemoveExplod\ntrigger1=Time=0\nid=1\n[State recover]\ntype=SelfState\ntrigger1=Time=1\nvalue=0\nctrl=1\n")
	states, err := cns.ParseNamed(strings.NewReader(script.String()), "scalar-runtime.cns")
	if err != nil {
		t.Fatal(err)
	}
	for number, state := range states.States {
		c.StateFile.States[number] = state
	}
	c.ChangeState(9880, 0, -1)
	for tick := 0; tick < 24; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("scalar/contact/recovery lifecycle", err)
		}
	}
	for index, expected := range []int{1, 0, 2, 999, 0, 1, 1, 1000, 1, 999, 999, 999} {
		if c.GetVar(40+index) != expected {
			t.Fatal("native scalar runtime observation", index, c.GetVar(40+index), expected)
		}
	}
	if c.StateNo != 0 || other.StateNo != 0 || !c.Ctrl || !other.Ctrl || len(c.Explods) != 0 || !c.IsAlive() || !other.IsAlive() {
		t.Fatal("scalar lifecycle did not recover/retire effects", c.StateNo, other.StateNo)
	}
}

// Continue the same fight through shield/override clearing, contact and recovery,
// with numeric sound/palette commands sharing the normal effect queues/banks.
func protectionMediaRecoveryFlow(t *testing.T, m *Match) {
	t.Helper()
	c, other := m.Players[0].Character, m.Players[1].Character
	for _, actor := range []*character.Character{c, other} {
		actor.StateFile.Data.Attack, actor.StateFile.Data.Defence = 100, 100
		actor.SetLife(1000)
		actor.SetAttackMul(1)
		actor.SetDefenceMul(1)
		actor.X, actor.Y, actor.VelX, actor.VelY = 0, 0, 0, 0
	}
	c.X, other.X = -70, 70*float64(other.LocalCoordinateWidth())/320
	var script strings.Builder
	state := func(number int, kind, move string, control int) {
		fmt.Fprintf(&script, "[Statedef %d]\ntype=%s\nmovetype=%s\nphysics=N\nctrl=%d\nanim=0\n", number, kind, move, control)
	}
	step := func(tick int, kind, params string) {
		fmt.Fprintf(&script, "[State protection/media continuation]\ntype=%s\ntrigger1=Time=%d\n%s\n", kind, tick, params)
	}
	state(700, "S", "A", 0)
	step(0, "ChangeAnim", "value=9970")
	step(0, "VarRangeSet", "first=40\nlast=51\nvalue=0")
	step(1, "HitDef", "attr=S,NA\ndamage=10,0\npausetime=0,0\nsparkno=-1\nguard.sparkno=-1\nhitsound=-1\nguardsound=-1\np1stateno=702\np2stateno=710\np2getp1state=1\nid=1")
	state(702, "S", "I", 0)
	step(0, "PlaySnd", "value=1/0,(var(48):=0)\nchannel=1/0\npan=1/0\nvolumescale=1/0\nfreqmul=1/0\nloop=1/0\nlowpriority=1/0\nvolume=(var(49):=9)")
	step(0, "SndPan", "channel=1/0\npan=(var(50):=1)")
	step(0, "StopSnd", "channel=1/0")
	step(0, "RemapPal", "source=1/0,(var(51):=1)\ndest=1,1/0")
	step(1, "VarSet", "v=40\nvalue=EnemyNear,Life")
	step(1, "VarSet", "v=41\nvalue=EnemyNear,StateNo")
	step(1, "VarSet", "v=42\nvalue=EnemyNear,StateType=A")
	step(10, "VarSet", "v=43\nvalue=EnemyNear,StateNo")
	step(10, "VarSet", "v=44\nvalue=EnemyNear,Ctrl")
	step(11, "SelfState", "value=0\nctrl=1")
	state(701, "S", "I", 0)
	step(0, "NotHitBy", "value=S,NA\ntime=20")
	step(0, "HitOverride", "attr=S,NA\nstateno=711\nslot=0\ntime=20\nforceair=1")
	step(1, "NotHitBy", "value=S,NA\ntime=1/0")
	step(1, "HitBy", "value2=S,NA\ntime=1/0")
	step(1, "HitOverride", "attr=S,NA\nstateno=1/0\nslot=1/0\ntime=1/0\nforceair=1/0")
	state(710, "S", "H", 0)
	step(3, "SelfState", "value=712\nctrl=1")
	state(711, "A", "H", 0)
	step(0, "Null", "")
	state(712, "S", "I", 1)
	step(12, "SelfState", "value=0\nctrl=1")
	states, err := cns.ParseNamed(strings.NewReader(script.String()), "protection-media.cns")
	if err != nil {
		t.Fatal(err)
	}
	for _, actor := range []*character.Character{c, other} {
		for number, state := range states.States {
			actor.StateFile.States[number] = state
		}
	}
	c.ChangeState(700, 0, -1)
	other.ChangeState(701, 0, -1)
	for tick := 0; tick < 32; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("protection/media/contact/recovery", err)
		}
	}
	for index, expected := range []int{990, 710, 0, 712, 1, 0, 0, 0, 0, 9, 1, 1} {
		if got := c.GetVar(40 + index); got != expected {
			t.Fatal("native protection/media observation", index, got, expected)
		}
	}
	if c.StateNo != 0 || other.StateNo != 0 || !c.Ctrl || !other.Ctrl || other.Life != 990 {
		t.Fatal("protection/media flow did not recover", c.StateNo, other.StateNo, other.Life)
	}
	// Continue through successive override reactions, explicit clearing, a
	// negative invalid timer and final own-state/control restoration.
	script.Reset()
	for _, actor := range []*character.Character{c, other} {
		actor.SetLife(1000)
	}
	state(700, "S", "A", 0)
	step(0, "ChangeAnim", "value=9970")
	step(0, "VarRangeSet", "first=40\nlast=51\nvalue=0")
	fmt.Fprint(&script, "[State successive hits]\ntype=HitDef\ntrigger1=Time=1 || Time=8 || Time=15 || Time=22\nattr=S,NA\ndamage=10,0\npausetime=0,0\nsparkno=-1\nguard.sparkno=-1\nhitsound=-1\nguardsound=-1\np2getp1state=0\n")
	for index, tick := range []int{5, 12, 19, 26} {
		step(tick, "VarSet", fmt.Sprintf("v=%d\nvalue=EnemyNear,Var(55)", 40+2*index))
		step(tick, "VarSet", fmt.Sprintf("v=%d\nvalue=EnemyNear,Life", 41+2*index))
	}
	for index, expr := range []string{"EnemyNear,Var(56)", "EnemyNear,Var(57)", "EnemyNear,Ctrl"} {
		step(28, "VarSet", fmt.Sprintf("v=%d\nvalue=%s", 48+index, expr))
	}
	step(30, "SelfState", "value=740\nctrl=1")
	state(701, "S", "I", 0)
	step(0, "VarRangeSet", "first=55\nlast=57\nvalue=0")
	step(0, "HitOverride", "attr=S,NA\nslot=99\nstateno=(var(56):=711)\ntime=20\nforceair=0")
	for _, number := range []int{711, 5000} {
		state(number, "S", "H", 0)
		step(0, "VarSet", "v=55\nvalue=StateNo")
		step(0, "VarAdd", "v=57\nvalue=1")
		step(1, "SelfState", "value=712\nctrl=1")
	}
	state(712, "S", "I", 1)
	for index, params := range []string{"slot=0\ntime=0", "slot=-99\ntime=20", "slot=0\ntime=-2"} {
		fmt.Fprintf(&script, "[State override continuation]\ntype=HitOverride\ntrigger1=Time=0 && var(57)=%d\nattr=S,NA\nstateno=711\n%s\n", index+1, params)
	}
	step(10, "SelfState", "value=740\nctrl=1")
	state(740, "S", "I", 1)
	step(1, "SelfState", "value=0\nctrl=1")
	ranges, err := cns.ParseNamed(strings.NewReader(script.String()), "override-recovery.cns")
	if err != nil {
		t.Fatal(err)
	}
	for _, actor := range []*character.Character{c, other} {
		for number, state := range ranges.States {
			actor.StateFile.States[number] = state
		}
	}
	c.ChangeState(700, 0, -1)
	other.ChangeState(701, 0, -1)
	for tick := 0; tick < 44; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("override range/reaction/clearing/recovery", err)
		}
	}
	for index, expected := range []int{711, 990, 5000, 980, 711, 970, 5000, 960, 711, 4, 1, 0} {
		if got := c.GetVar(40 + index); got != expected {
			t.Fatal("native override range observation", index, got, expected)
		}
	}
	if c.StateNo != 0 || other.StateNo != 0 || !c.Ctrl || !other.Ctrl || other.Life != 960 {
		t.Fatal("override sequence did not recover", c.StateNo, other.StateNo, other.Life)
	}
	for _, override := range other.HitOverrides {
		if override.Time != 0 {
			t.Fatal("override survived clearing", override)
		}
	}

}

// Continue through duplicate AIR definitions, animation changes with invalid
// selectors and numeric fallbacks, then return to ordinary stand/control.
func animationCompatFlow(t *testing.T, m *Match) {
	t.Helper()
	frames, err := air.Parse(strings.NewReader(`[Begin Action 0]
0,0,0,0,4
0,0,0,0,6
[Begin Action 9990]
0,0,0,0,-1
[Begin Action 900]
0,0,0,0,2
0,0,0,0,3
0,0,0,0,5
[Begin Action 900]
0,0,0,0,-1
`))
	if err != nil {
		t.Fatal(err)
	}
	var script strings.Builder
	script.WriteString("[Statedef 9800]\ntype=S\nmovetype=I\nphysics=N\nctrl=0\nanim=9990\n")
	for phase, row := range []struct{ value, element, first, second string }{
		{"900", "1", "AnimElemTime(2)", "AnimTime"},
		{"1/0", "1", "Anim", "AnimTime"},
		{"900", "1/0", "AnimElemNo(0)", "AnimTime"},
		{"900", "0", "AnimElemNo(0)", "AnimTime"},
		{"900", "99", "AnimElemNo(0)", "AnimTime"},
		{"900", "-1", "AnimElemNo(0)", "AnimTime"},
	} {
		fmt.Fprintf(&script, "[State change]\ntype=ChangeAnim\ntrigger1=Time=%d\nvalue=%s\nelem=%s\n", phase*3, row.value, row.element)
		for index, expression := range []string{row.first, row.second} {
			fmt.Fprintf(&script, "[State observe]\ntype=VarSet\ntrigger1=Time=%d\nv=%d\nvalue=%s\n", phase*3, 40+phase*2+index, expression)
		}
	}
	script.WriteString("[State recover]\ntype=SelfState\ntrigger1=Time=18\nvalue=0\nctrl=1\n")
	states, err := cns.Parse(strings.NewReader(script.String()))
	if err != nil {
		t.Fatal(err)
	}
	c := m.Players[0].Character
	for number, action := range frames.Actions {
		previous, exists := c.AirFile.Actions[number]
		t.Cleanup(func() {
			if exists {
				c.AirFile.Actions[number] = previous
			} else {
				delete(c.AirFile.Actions, number)
			}
		})
		c.AirFile.Actions[number] = action
	}
	c.StateFile.States[9800] = states.States[9800]
	for index := 40; index < 52; index++ {
		c.SetVar(index, 0)
	}
	c.ChangeState(9800, 0, -1)
	for tick := 0; tick < 24; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("animation compatibility lifecycle", err)
		}
	}
	for index, expected := range []int{-2, -10, 0, -10, 1, -10, 1, -10, 1, -10, 1, -10} {
		if got := c.GetVar(40 + index); got != expected {
			t.Fatalf("native animation observation %d: got %d want %d", index, got, expected)
		}
	}
	if c.StateNo != 0 || !c.Ctrl || c.Anim != 0 || c.Y != 0 {
		t.Fatal("animation changes did not recover to stand/control", c.StateNo, c.Ctrl, c.Anim, c.Y)
	}
	// Continue through duplicate entry settings and persistence, while repeated
	// triggers remain separate conditions. Preserve the first complete entry.
	var entry strings.Builder
	entry.WriteString(`[Statedef 9830]
type=S
type=A
movetype=I
movetype=A
physics=N
physics=S
ctrl=1
ctrl=0
anim=0
anim=9990
velset=2,-1
velset=9,-5
poweradd=100
poweradd=200
[State origin]
type=VarSet
trigger1=Time=0
v=58
value=Pos X
`)
	for _, observation := range []struct {
		slot       int
		expression string
	}{
		{40, "Ctrl"}, {41, "StateType=S"}, {42, "MoveType=I"}, {43, "Anim"}, {44, "Vel X"}, {45, "Vel Y"}, {46, "Power"},
	} {
		fmt.Fprintf(&entry, "[State entry observation]\ntype=VarSet\ntrigger1=Time=0\nv=%d\nvalue=%s\n", observation.slot, observation.expression)
	}
	entry.WriteString("[State first persistence]\ntype=VarAdd\ntrigger1=1\npersistent=0\npersistent=1\nv=47\nvalue=1\n[State repeated trigger conditions]\ntype=VarAdd\ntrigger1=Time=0\ntrigger1=Time>0\nv=48\nvalue=1\n")
	for _, observation := range []struct {
		slot       int
		expression string
	}{{49, "Pos X-var(58)"}, {50, "Pos Y"}, {51, "Time"}} {
		fmt.Fprintf(&entry, "[State motion observation]\ntype=VarSet\ntrigger1=Time=1\nv=%d\nvalue=%s\n", observation.slot, observation.expression)
	}
	entry.WriteString("[State land]\ntype=PosSet\ntrigger1=Time=3\ny=0\n[State stop]\ntype=VelSet\ntrigger1=Time=3\nx=0\ny=0\n[State recover]\ntype=SelfState\ntrigger1=Time=3\nvalue=0\nctrl=1\n")
	definitions, err := cns.ParseNamed(strings.NewReader(entry.String()), "state-header-precedence.cns")
	if err != nil {
		t.Fatal(err)
	}
	c.StateFile.States[9830] = definitions.States[9830]
	for index := 40; index < 52; index++ {
		c.SetVar(index, 0)
	}
	c.SetPower(0)
	c.ChangeState(9830, -1, -1)
	for tick := 0; tick < 8; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("state-header continuation", err)
		}
	}
	for index, expected := range []int{1, 1, 1, 0, 2, -1, 100, 1, 0, 2, -1, 1} {
		if c.GetVar(40+index) != expected {
			t.Fatal("native state-header observation", index, c.GetVar(40+index), expected)
		}
	}
	if c.StateNo != 0 || !c.Ctrl || c.Y != 0 || c.VelX != 0 || c.VelY != 0 {
		t.Fatal("state-header lifecycle did not recover", c.StateNo, c.Ctrl, c.Y)
	}
}

func TestStockMixedMatchJumpAndCNSLandingRecovery(t *testing.T) {
	m := stockMatch(t, 99)
	advanceUntil(t, m, Active, 600)
	frame := 0
	for _, p := range m.Players {
		p.Character.InputSource = func(int, int) []string {
			if frame < 5 || frame >= 6 && frame < 24 {
				return []string{"DirUp"}
			}
			return nil
		}
	}
	var airborne, landing [2]bool
	for ; frame < 180; frame++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
		for slot, p := range m.Players {
			c := p.Character
			if c.StateNo == 45 || c.AirJumpCount != 0 {
				t.Fatal("held low-height press became an air jump", slot+1, frame, c.StateNo, c.AirJumpCount)
			}
			airborne[slot] = airborne[slot] || c.Y < 0 && c.StateType == "A"
			landing[slot] = landing[slot] || c.StateNo == 52
		}
	}
	for slot, p := range m.Players {
		c := p.Character
		if !airborne[slot] || !landing[slot] || c.StateNo != 0 || !c.Ctrl || c.Y != 0 || c.VelY != 0 {
			t.Fatalf("player %d jump/landing incomplete: airborne=%v landing=%v state=%d ctrl=%v y=%v vy=%v", slot+1, airborne[slot], landing[slot], c.StateNo, c.Ctrl, c.Y, c.VelY)
		}
	}
	airJumpLandingFlow(t, m)
	bodyPushFlow(t, m)
}

// Continue the mixed-coordinate stock jump through a fresh air-jump command,
// exhausted budget, authored velocity, landing and a second complete flight.
func airJumpLandingFlow(t *testing.T, m *Match) {
	t.Helper()
	for flight := 0; flight < 2; flight++ {
		frame := 0
		for _, p := range m.Players {
			p.Character.InputSource = func(int, int) []string {
				switch {
				case frame < 5:
					return []string{"DirUp"}
				case frame == 16:
					return []string{"DirUp", "DirForward"}
				case frame == 30:
					return []string{"DirUp", "DirBack"}
				}
				return nil
			}
		}
		var entries [2]int
		var previous [2]int
		var launched [2]bool
		for ; frame < 180; frame++ {
			if err := m.Update(); err != nil {
				t.Fatal("stock air-jump lifecycle", err)
			}
			for slot, p := range m.Players {
				c := p.Character
				if c.StateNo == 45 && previous[slot] != 45 {
					entries[slot]++
				}
				if previous[slot] == 45 && c.StateNo == 50 {
					launched[slot] = c.VelX > 0 && c.VelY < 0
				}
				if c.AirJumpCount > 1 || entries[slot] > 1 {
					t.Fatal("stock per-flight air-jump budget exceeded", flight, slot+1, frame)
				}
				previous[slot] = c.StateNo
			}
		}
		for slot, p := range m.Players {
			c := p.Character
			if entries[slot] != 1 || !launched[slot] || c.StateNo != 0 || !c.Ctrl || c.Y != 0 || c.VelY != 0 || c.AirJumpCount != 0 {
				t.Fatal("stock air-jump/landing/reset incomplete", flight, slot+1, entries, launched, c.StateNo, c.Y, c.AirJumpCount)
			}
		}
	}
}

// Continue the jump/landing scenario through one beta body-push lifecycle:
// interior, wall, vertical separation, Width, air/ground, air/air, PosFreeze,
// release, Pause and reset.
func bodyPushFlow(t *testing.T, m *Match) {
	t.Helper()
	program := `[Statedef 5900]
[State 5900, enter]
type=ChangeState
trigger1=1
value=0
[Statedef 0]
type=S
movetype=I
physics=N
anim=9990
ctrl=0
velset=0,0
`
	for phase, tick := range []int{80, 170, 260, 350, 440, 530, 620, 710, 800, 890} {
		for side, expression := range []string{"Pos X", "EnemyNear, Pos X"} {
			program += fmt.Sprintf("[State 0, before movement %d]\ntype=VarSet\ntriggerall=TeamSide=1\ntrigger1=Time=%d\nfv=%d\nvalue=%s\n", phase*2+side, tick, phase*2+side, expression)
		}
	}
	program += `[State 0, push]
type=PlayerPush
trigger1=1
value=1
[State 0, fixed camera]
type=ScreenBound
trigger1=1
value=1
movecamera=0,0
[State 0, temporary widths]
type=Width
trigger1=Time>=270 && Time<360
player=Const240p(10),Const240p(20)
[State 0, airborne overlap]
type=StateTypeSet
trigger1=Time>=360 && Time<540 && (TeamSide=1 || Time>=450)
statetype=A
physics=N
[State 0, grounded freeze sequence]
type=StateTypeSet
trigger1=Time>=540
statetype=S
physics=N
[State 0, stored velocity]
type=VelSet
trigger1=Time>=540
x=Const240p(2)
y=0
[State 0, position freeze]
type=PosFreeze
trigger1=Time>=630 && Time<720 && TeamSide=1
trigger2=Time>=720 && Time<810 && TeamSide=2
trigger3=Time>=810 && Time<900
value=1
[State 0, placement]
type=PosSet
trigger1=1
x=Const240p(ifelse(Time>=90 && Time<180,ifelse(TeamSide=1,130,145),ifelse(TeamSide=1,0,20)))
y=Const240p(ifelse(Time>=180 && Time<270 && TeamSide=1,-100,ifelse(Time>=360 && Time<540 && (TeamSide=1 || Time>=450),-10,0)))
`
	for _, p := range m.Players {
		file, err := cns.ParseNamed(strings.NewReader(program), "connected-body-push.cns")
		if err != nil {
			t.Fatal(err)
		}
		file.SetLanguageVersion("1.1")
		old := p.Character.StateFile
		file.Data, file.Size, file.Movement, file.Velocity, file.ConstantFields = old.Data, old.Size, old.Movement, old.Velocity, old.ConstantFields
		units := float64(p.LocalCoord.Width) / 320
		box := air.Box{X1: int(-10 * units), Y1: int(-60 * units), X2: int(10 * units)}
		p.AIR = &air.Data{Actions: map[int]*air.Action{9990: {ID: 9990, LoopStart: -1, Elements: []air.Element{{Group: 0, Image: 0, Time: -1, Clsn2: []air.Box{box}}}}}}
		p.Character.StateFile, p.CMD = file, nil
	}
	m.Stage.Camera.StartX, m.Stage.Camera.StartY = 0, 0
	m.Stage.Camera.BoundLeft, m.Stage.Camera.BoundRight, m.Stage.Camera.BoundHigh, m.Stage.Camera.BoundLow = 0, 0, 0, 0
	m.Stage.Camera.VerticalFollow = 0
	m.Stage.StageInfo.AutoTurn = false
	m.Round++
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	advanceUntil(t, m, Active, 650)
	for _, p := range m.Players {
		p.Character.ChangeState(0, 0, -1) // Start the authored poses after fight placement.
	}
	step := func(ticks int) {
		t.Helper()
		for tick := 0; tick < ticks; tick++ {
			if err := m.Update(); err != nil {
				t.Fatal(err)
			}
		}
	}
	step(910)
	// Native wall observations retain the existing late-push/next-pass clamp:
	// do not redistribute the corner's half-step to change this measured order.
	want := []float64{-6, 26, 121.5, 145, 0, 20, -16, 36, -4, 24, -2, 22,
		-6, 26, -7, 25, -5, 27, -6, 26}
	// Redirected Pos X is converted into the observing root's coordinates.
	factor := 320 / float64(m.Players[0].Character.LocalCoordinateWidth())
	for index, expected := range want {
		if value := m.Players[0].Character.FloatVars[index] * factor; value != expected {
			t.Fatalf("native mixed-coordinate body push phase %d side %d: got %v want %v", index/2, index%2, value, expected)
		}
	}
	for side, p := range m.Players {
		c := p.Character
		if c.Time != 910 || c.VelX*320/float64(c.LocalCoordinateWidth()) != 2 || c.PosFrozen {
			t.Fatal("PosFreeze stopped CNS time, consumed velocity or failed to release", side, c.Time, c.VelX, c.PosFrozen)
		}
	}
	if err := m.Players[0].Character.PauseGame(engine.PauseParameters{Time: 2}); err != nil {
		t.Fatal(err)
	}
	step(1)
	positions := [2]float64{m.Players[0].Character.X, m.Players[1].Character.X}
	times := [2]int{m.Players[0].Character.Time, m.Players[1].Character.Time}
	step(2)
	for side, p := range m.Players {
		if p.Character.X != positions[side] || p.Character.Time != times[side] {
			t.Fatal("paused body push advanced an actor", side)
		}
	}
	m.Round++
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	advanceUntil(t, m, Active, 650)
	for _, p := range m.Players {
		p.Character.ChangeState(0, 0, -1)
	}
	step(85)
	for side, p := range m.Players {
		if p.Character.StateType != "S" || p.Character.Y != 0 || p.Character.VelX != 0 || p.Character.PosFrozen || p.Character.X*320/float64(p.Character.LocalCoordinateWidth()) != want[side] {
			t.Fatal("round reset retained air/Width/PosFreeze body-push state", side, p.Character.StateType, p.Character.X, p.Character.Y)
		}
	}
}

func TestPauseMatchClockBackgroundEffectsAndRoundReset(t *testing.T) {
	for _, pauseBackground := range []bool{false, true} {
		m := stockMatch(t, 99)
		advanceUntil(t, m, Active, 600)
		marker := &background.NormalElement{CommonElement: background.CommonElement{Enabled: true, Velocity: [2]float64{1, 0}}}
		m.Stage.Background.Elements = append(m.Stage.Background.Elements, marker)
		actions := &air.Data{Actions: map[int]*air.Action{9999: {ID: 9999, Elements: []air.Element{{Group: -1, Time: 100}}}}}
		if err := m.Effects.Queue.Push(effect.Request{Source: effect.Source{Sprites: m.Fight.EffectSprites, Animations: actions, Scale: 1}, Action: 9999, Facing: 1}); err != nil {
			t.Fatal(err)
		}
		if err := m.Effects.Flush(); err != nil {
			t.Fatal(err)
		}
		m.Timeline.BeginFrame()
		if err := m.Players[0].Character.PauseGame(engine.PauseParameters{Time: 3, PauseBackground: pauseBackground}); err != nil {
			t.Fatal(err)
		}
		m.Timeline.EndFrame()
		gameTime, phaseTime, timer := m.Timeline.GameTime, m.PhaseTime, m.timeTicks
		local := [2]int{m.Players[0].Character.Time, m.Players[1].Character.Time}
		for tick := 0; tick < 3; tick++ {
			if err := m.Update(); err != nil {
				t.Fatal(err)
			}
		}
		backgroundTicks := 3
		if pauseBackground {
			backgroundTicks = 0
		}
		if m.Timeline.GameTime != gameTime+3 || m.PhaseTime != phaseTime || m.timeTicks != timer || marker.Time != backgroundTicks || m.Effects.Active[0].Clock.Elapsed != 0 {
			t.Fatal("match pause failed to separate global/local/background/effect clocks", pauseBackground)
		}
		for slot, p := range m.Players {
			if p.Character.Time != local[slot] {
				t.Fatal("frozen match advanced a player")
			}
		}
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
		if m.PhaseTime != phaseTime+1 || m.timeTicks != timer+1 || m.Effects.Active[0].Clock.Elapsed != 1 {
			t.Fatal("match clocks failed to resume")
		}
		m.Timeline.BeginFrame()
		_ = m.Players[0].Character.PauseGame(engine.PauseParameters{Time: 3})
		m.Timeline.EndFrame()
		gameTime = m.Timeline.GameTime
		if err := m.resetRound(); err != nil {
			t.Fatal(err)
		}
		if m.Timeline.Paused() || m.Timeline.GameTime != gameTime || m.Players[0].Character.Timeline != m.Timeline {
			t.Fatal("round reset retained pause or detached the world clock")
		}
		if pauseBackground {
			pauseRuntimeRecoveryFlow(t, m)
		}
	}
}

// Continue the shared clock scenario through numeric fallback, argument side
// effects, zero-duration power updates, real pauses and own-state recovery.
func pauseRuntimeRecoveryFlow(t *testing.T, m *Match) {
	t.Helper()
	advanceUntil(t, m, Active, 600)
	var script strings.Builder
	script.WriteString("[Statedef 9750]\ntype=S\nmovetype=I\nphysics=N\nctrl=0\nanim=0\n")
	for _, step := range []struct {
		kind   string
		tick   int
		params string
	}{
		{"VarRangeSet", 0, "first=40\nlast=51\nvalue=0"},
		{"VarSet", 0, "v=58\nvalue=GameTime"}, {"PowerSet", 0, "value=0"},
		{"Pause", 0, "time=1/0\nmovetime=(var(40):=2)\nendcmdbuftime=(var(41):=3)\npausebg=(var(42):=0)"},
		{"VarSet", 1, "v=43\nvalue=GameTime-var(58)"},
		{"SuperPause", 1, "time=1/0\nmovetime=(var(44):=2)\nanim=-1\nsound=-1\npoweradd=(var(45):=100)\ndarken=1/0\nunhittable=1/0"},
		{"VarSet", 2, "v=46\nvalue=GameTime-var(58)"}, {"VarSet", 2, "v=47\nvalue=Power"},
		{"Pause", 2, "time=3\nmovetime=1\nendcmdbuftime=3"},
		{"VarSet", 3, "v=48\nvalue=GameTime-var(58)"}, {"VarSet", 4, "v=49\nvalue=GameTime-var(58)"},
		{"SuperPause", 4, "time=3\nmovetime=3\nanim=-1\nsound=-1\npoweradd=50\ndarken=0"},
		{"VarSet", 7, "v=50\nvalue=Power"}, {"VarSet", 7, "v=51\nvalue=GameTime-var(58)"},
		{"SelfState", 7, "value=0\nctrl=1"},
	} {
		fmt.Fprintf(&script, "[State pause continuation]\ntype=%s\ntrigger1=Time=%d\n%s\n", step.kind, step.tick, step.params)
	}
	script.WriteString("[Statedef 9751]\n[State fatal pause]\ntype=Pause\ntrigger1=1\ntime=1/0\nmovetime=~(1/0)\n[Statedef 9752]\n[State fatal superpause]\ntype=SuperPause\ntrigger1=1\ntime=0\npoweradd=100\np2defmul=~(1/0)\n")
	states, err := cns.ParseNamed(strings.NewReader(script.String()), "pause-runtime.cns")
	if err != nil {
		t.Fatal(err)
	}
	c := m.Players[0].Character
	for number, state := range states.States {
		c.StateFile.States[number] = state
	}
	c.ChangeState(9750, 0, -1)
	for tick := 0; tick < 16; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("pause numeric/recovery lifecycle", err)
		}
	}
	for index, expected := range []int{2, 3, 0, 1, 2, 100, 2, 100, 3, 6, 150, 9} {
		if c.GetVar(40+index) != expected {
			t.Fatal("native pause numeric observation", index, c.GetVar(40+index), expected)
		}
	}
	if c.StateNo != 0 || !c.Ctrl || m.Timeline.Paused() || c.Power != 150 {
		t.Fatal("pause lifecycle did not recover", c.StateNo, c.Power)
	}
	for _, number := range []int{9751, 9752} {
		fatal := character.NewCharacter(c.StateFile, c.AirFile, c.CmdFile)
		fatal.SetPower(31)
		_, err := fatal.SM.ExecuteState(states.States[number], fatal)
		if err == nil || !strings.Contains(err.Error(), "bitwise not of bottom") || !strings.Contains(err.Error(), "pause-runtime.cns") || fatal.Power != 31 || fatal.Timeline.Paused() {
			t.Fatal("pause VM failure was hidden or partly applied", number, err)
		}
	}
}

func TestStockSuperPauseConsumesPowerAndUsesCommonResources(t *testing.T) {
	m := stockMatch(t, 99)
	advanceUntil(t, m, Active, 600)
	c := m.Players[0].Character
	c.SetPower(1500)
	c.ChangeState(3000, 0, -1)
	m.SoundQueue.Drain()
	sawPause, sawEffect, sawSound := false, false, false
	frozenTime, pausedFrames := -1, 0
	for tick := 0; tick < 120; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
		for _, command := range m.SoundQueue.Drain() {
			if command.Operation == sound.Play && command.Bank == m.Fight.CommonSounds && command.Group == 20 && command.Item == 0 {
				sawSound = true
			}
		}
		for _, instance := range m.Effects.Active {
			if instance.Action == 100 && instance.Source.Sprites == m.Fight.EffectSprites && instance.Source.Animations == m.Fight.EffectAnimations {
				sawEffect = true
			}
		}
		if m.Timeline.SuperPaused() {
			pausedFrames++
			if !sawPause {
				sawPause, frozenTime = true, c.Time
				if c.Power != 500 {
					t.Fatal("stock SuperPause did not consume 1000 power", c.Power)
				}
			} else if c.Time != frozenTime {
				t.Fatal("stock super caller advanced without movetime")
			}
		} else if sawPause && tick > 0 {
			if pausedFrames != 30 || !sawEffect || !sawSound {
				t.Fatal("stock SuperPause clock/resources incomplete", pausedFrames, sawEffect, sawSound)
			}
			return
		}
	}
	t.Fatal("stock state 3000 did not execute and finish SuperPause")
}
