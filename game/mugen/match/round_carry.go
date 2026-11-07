package match

import "github.com/kazzmir/paintown/game/mugen/character"

// RoundCarry retains a survivor's round-persistent values while the campaign
// loads another opponent. Transient combat/animation/helper state starts fresh.
type RoundCarry struct {
	Life, Power, RoundsExisted int
	IntVars                    [60]int
	FloatVars                  [40]float64
}

func SnapshotRound(c *character.Character) *RoundCarry {
	return &RoundCarry{Life: c.Life, Power: c.GetPower(), RoundsExisted: c.RoundsExisted, IntVars: c.IntVars, FloatVars: c.FloatVars}
}

func copyPersistentVariables(c *character.Character, integers [60]int, floats [40]float64) {
	for index := c.StateFile.Data.IntPersistIndex; index >= 0 && index < len(c.IntVars); index++ {
		c.IntVars[index] = integers[index]
	}
	for index := c.StateFile.Data.FloatPersistIndex; index >= 0 && index < len(c.FloatVars); index++ {
		c.FloatVars[index] = floats[index]
	}
}

func (m *Match) initialCarry(slot int) *RoundCarry {
	if m.Round != max(1, m.Request.RoundStart) {
		return nil
	}
	side, member := slot%2, m.Teams[slot%2].Active
	if slot > 1 {
		member = 1
	}
	if member < len(m.Request.Carry[side]) {
		return m.Request.Carry[side][member]
	}
	return nil
}
