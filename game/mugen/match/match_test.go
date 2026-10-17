package match

import (
	"bytes"
	"path/filepath"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
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
	m, err := New(Request{Players: [2]Selection{{DefPath: filepath.Join(root, "chars/kfm/kfm.def"), Palette: 1}, {DefPath: filepath.Join(root, "chars/kfm720/kfm720.def"), Palette: 2}}, StagePath: "stages/kfm.def", Mode: "versus"}, Options{RootDir: root, FightPath: "data/fight.def", GameSpace: coord.Space{Width: 1280, Height: 720}, RoundTime: time, LifePercent: 100, Input: func(int, int) []string { return nil }})
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
		if !p.Character.Ctrl || p.Character.RoundState != 2 || p.Character.PlayerID != slot+1 {
			t.Fatal("control/identity", slot, p.Character)
		}
	}
	m.Players[1].Character.SetLife(0)
	advanceUntil(t, m, PreOver, 2)
	if m.Winner != 0 || m.Wins != [2]int{1, 0} || m.TimeOver {
		t.Fatal("KO result", m.Winner, m.Wins, m.TimeOver)
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
	advanceUntil(t, m, Finished, 500)
	if m.Wins != [2]int{2, 0} || !m.Players[0].Character.IsMatchOver {
		t.Fatal("match result", m.Wins)
	}
}

func TestStockMatchTimeOverUsesLifeFraction(t *testing.T) {
	m := stockMatch(t, 1)
	advanceUntil(t, m, Active, 600)
	m.Players[0].Character.MaxLife = 2000
	m.Players[0].Character.Life = 1000
	m.Players[1].Character.Life = 750
	advanceUntil(t, m, PreOver, 61)
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
			return
		}
	}
	t.Fatalf("logical walk/punch/hit/recovery incomplete: attack=%v hit=%v life=%d state=%d ctrl=%v", sawAttack, sawHit, m.Players[1].Character.Life, m.Players[1].Character.StateNo, m.Players[1].Character.Ctrl)
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
