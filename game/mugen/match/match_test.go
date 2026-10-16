package match

import (
	"path/filepath"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/palette"
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
	sawAttack, sawHit := false, false
	for tick := 0; tick < 600; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
		if m.Players[0].Character.StateNo == 200 {
			sawAttack = true
		}
		if m.Players[1].Character.MoveType == "H" {
			sawHit = true
		}
		if sawAttack && sawHit && m.Players[1].Character.Ctrl && m.Players[1].Character.Life < m.Players[1].Character.MaxLife {
			return
		}
	}
	t.Fatalf("logical walk/punch/hit/recovery incomplete: attack=%v hit=%v life=%d state=%d ctrl=%v", sawAttack, sawHit, m.Players[1].Character.Life, m.Players[1].Character.StateNo, m.Players[1].Character.Ctrl)
}
