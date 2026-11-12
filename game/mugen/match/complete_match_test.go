package match

import (
	"fmt"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/sound"
)

// These scenarios use the production stock CNS, command buffers, collision,
// damage and round lifecycle. They never set life, force states or reposition.
func TestStockInputDrivenCompleteMatches(t *testing.T) {
	for _, scenario := range []struct {
		name   string
		time   int
		winner func(int) int
		want   [2]int
		rounds int
	}{
		{"p1", 99, func(int) int { return 0 }, [2]int{2, 0}, 2},
		{"p2", 99, func(int) int { return 1 }, [2]int{0, 2}, 2},
		{"alternating", 99, func(round int) int {
			if round == 2 {
				return 1
			}
			return 0
		}, [2]int{2, 1}, 3},
		{"time-draw", 0, func(int) int { return -1 }, [2]int{2, 2}, 3},
	} {
		t.Run(scenario.name, func(t *testing.T) {
			m := stockMatch(t, scenario.time)
			bot := func(slot, facing int) []string {
				attacker := scenario.winner(m.Round)
				if attacker < 0 || slot != attacker+1 || m.Phase != Active {
					return nil
				}
				c := m.Players[attacker].Character
				distance, _ := c.GetOpponentBodyDistance()
				if distance > 16*float64(c.LocalCoordinateWidth())/320 {
					return []string{"DirForward"}
				}
				if m.Tick%40 < 2 {
					return []string{"ButtonX"}
				}
				return nil
			}
			m.Options.Input = bot
			for _, p := range m.Players {
				p.Character.InputSource = bot
			}
			identities := [2]int32{m.Players[0].Character.EntityID, m.Players[1].Character.EntityID}
			hits := map[int]bool{}
			rounds := map[int]bool{}
			cues := map[int]bool{}
			oldRound := m.Round
			committed := map[int]bool{}
			maxPreOver := 0
			for i := 0; i < 20000 && m.Phase != Finished; i++ {
				round := m.Round
				if err := m.Update(); err != nil {
					t.Fatal(err)
				}
				if m.Phase == PreOver {
					maxPreOver = max(maxPreOver, m.PhaseTime)
					if m.PhaseTime >= 900 {
						t.Fatal("stock KO reached the readiness fallback", m.Round)
					}
				}
				if m.Phase == Over && !committed[m.Round] {
					committed[m.Round] = true
					attacker := scenario.winner(m.Round)
					if attacker >= 0 {
						if m.TimeOver || m.Winner != attacker || m.Players[1-attacker].Character.Life != 0 {
							t.Fatal("round did not end in the intended actual KO", m.Round, m.Winner, m.TimeOver)
						}
					} else if !m.TimeOver || m.Winner != -1 {
						t.Fatal("round did not end in a time draw", m.Round)
					}
				}
				for _, command := range m.SoundQueue.Drain() {
					if command.Operation == sound.Play && command.Bank == m.Fight.CommonSounds && command.Group == 5 {
						cues[round] = true
					}
				}
				for _, p := range m.Players {
					if p.Character.Life < p.Character.MaxLife {
						hits[round] = true
					}
				}
				rounds[m.Round] = true
				if m.Round != oldRound {
					for slot, p := range m.Players {
						c := p.Character
						if c.EntityID != identities[slot] || c.Life != c.MaxLife || c.RoundsExisted != m.Round-1 || c.RoundWon || c.RoundLost || c.RoundDraw || c.IsMatchOver {
							t.Fatal("invalid fresh round", slot, m.Round, c.Life)
						}
					}
					oldRound = m.Round
				}
			}
			if m.Phase != Finished || m.Wins != scenario.want || m.Round != scenario.rounds || len(rounds) != scenario.rounds {
				t.Fatalf("incomplete match: phase=%v round=%d wins=%v tick=%d states=%d/%d life=%d/%d", m.Phase, m.Round, m.Wins, m.Tick, m.Players[0].Character.StateNo, m.Players[1].Character.StateNo, m.Players[0].Character.Life, m.Players[1].Character.Life)
			}
			for round := 1; round <= scenario.rounds; round++ {
				if scenario.winner(round) >= 0 && (!hits[round] || !cues[round]) {
					t.Fatal("round lacked actual damage/contact sound", round, hits, cues)
				}
			}
			t.Logf("rounds=%d wins=%v tick=%d actual-damage-rounds=%v max-pre-over=%d", m.Round, m.Wins, m.Tick, hits, maxPreOver)
			// Reusing request/options creates a fresh match and cannot retain a result,
			// root object, clock, sound queue, helper registry or effect scene.
			if scenario.name == "p1" {
				for rematch := 0; rematch < 3; rematch++ {
					next, err := New(m.Request, m.Options)
					if err != nil {
						t.Fatal(err)
					}
					if next.World == m.World || next.Effects == m.Effects || next.Round != 1 || next.Wins != [2]int{} || next.Draws != 0 || next.Timeline.GameTime != 0 {
						t.Fatal("rematch retained result or resource owner")
					}
					m = next
					for i := 0; i < 20000 && m.Phase != Finished; i++ {
						if err = m.Update(); err != nil {
							t.Fatal(err)
						}
						m.SoundQueue.Drain()
					}
					if m.Phase != Finished || m.Wins != scenario.want {
						t.Fatal("sustained rematch failed", fmt.Sprint(rematch), m.Phase, m.Wins)
					}
				}
			}
		})
	}
}
