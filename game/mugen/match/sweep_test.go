package match

import (
	"fmt"
	"testing"
)

func TestStockMixedMatchSweepCompletesRecovery(t *testing.T) {
	for attacker := 0; attacker < 2; attacker++ {
		t.Run(fmt.Sprint(attacker+1), func(t *testing.T) {
			m := stockMatch(t, 99)
			advanceUntil(t, m, Active, 600)
			frame := 0
			for slot, p := range m.Players {
				c := p.Character
				c.X = float64(-20+40*slot) * float64(c.LocalCoordinateWidth()) / 320
				c.InputSource = func(int, int) []string { return nil }
				if slot == attacker {
					c.InputSource = func(int, int) []string {
						if frame == 15 {
							return []string{"DirDown", "ButtonB"}
						}
						if frame < 50 {
							return []string{"DirDown"}
						}
						return nil
					}
				}
			}
			victim := m.Players[1-attacker].Character
			seen := map[int]bool{}
			firstHit, sawAttack := -1, false
			for ; frame < 240; frame++ {
				if err := m.Update(); err != nil {
					t.Fatal(err)
				}
				sawAttack = sawAttack || m.Players[attacker].Character.StateNo == 440
				if victim.MoveType == "H" && firstHit == -1 {
					firstHit = victim.StateNo
				}
				if firstHit != -1 {
					seen[victim.StateNo] = true
				}
				if firstHit != -1 && victim.StateNo == 0 && victim.Ctrl {
					if !sawAttack || firstHit != 5070 || !seen[5071] || !seen[5110] || !seen[5120] || victim.Life != victim.MaxLife-72 {
						t.Fatal("incomplete stock sweep path", sawAttack, firstHit, seen, victim.Life)
					}
					return
				}
			}
			t.Fatalf("sweep failed to recover: attack=%v first=%d seen=%v state=%d time=%d life=%d", sawAttack, firstHit, seen, victim.StateNo, victim.Time, victim.Life)
		})
	}
}
