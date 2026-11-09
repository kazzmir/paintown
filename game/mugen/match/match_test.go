package match

import (
	"bytes"
	"fmt"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
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
}

func TestStockMixedMatchJumpAndCNSLandingRecovery(t *testing.T) {
	m := stockMatch(t, 99)
	advanceUntil(t, m, Active, 600)
	frame := 0
	for _, p := range m.Players {
		p.Character.InputSource = func(int, int) []string {
			if frame < 5 {
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
	bodyPushFlow(t, m)
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
