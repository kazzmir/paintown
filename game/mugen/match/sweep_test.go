package match

import (
	"fmt"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/cns"
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
					legacyBounceRecoveryFlow(t, m, victim)
					return
				}
			}
			t.Fatalf("sweep failed to recover: attack=%v first=%d seen=%v state=%d time=%d life=%d", sawAttack, firstHit, seen, victim.StateNo, victim.Time, victim.Life)
		})
	}
}

// Continue the knockdown through the modern common states with a legacy
// character's omitted 1.0 constants, including the reported state5101 PosSet.
func legacyBounceRecoveryFlow(t *testing.T, m *Match, victim *character.Character) {
	t.Helper()
	for _, player := range m.Players {
		player.Character.InputSource = func(int, int) []string { return nil }
	}
	for key := range victim.StateFile.ConstantFields {
		if strings.HasPrefix(key, "movement.down.") || strings.HasPrefix(key, "movement.air.gethit.") || strings.HasSuffix(key, "friction.threshold") || strings.HasPrefix(key, "velocity.air.gethit.") {
			delete(victim.StateFile.ConstantFields, key)
		}
	}
	if value, ok := victim.LookupConstant("movement.down.bounce.offset.y"); !ok || value.Number() != 20*float64(victim.LocalCoordinateWidth())/320 {
		t.Fatal("omitted bounce offset lost character units", value, ok)
	}
	victim.Y, victim.VelY, victim.ShakeTime, victim.HitPauseTime = 0, 0, 0, 0
	victim.GetHit = cns.ParseHitDef(map[string]string{"fall": "1", "fall.yvelocity": "-4.5", "fall.recover": "0"})
	victim.HitFall = true
	victim.ChangeState(5101, -1, -1)
	seen := map[int]bool{}
	bounced := false
	for tick := 0; tick < 240; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal("legacy bounce/recovery", err)
		}
		seen[victim.StateNo] = true
		bounced = bounced || victim.StateNo == 5101 && victim.VelY < 0
		if victim.StateNo == 0 && victim.Ctrl {
			if !bounced || !seen[5110] || !seen[5120] || victim.Y != 0 {
				t.Fatal("incomplete legacy bounce/recovery", bounced, seen, victim.Y)
			}
			return
		}
	}
	t.Fatal("legacy bounce never recovered", seen, victim.StateNo, victim.Time, victim.Y)
}
