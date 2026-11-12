package character

// Beta's liedown counter is distinct from fall.recovertime (air recovery).
func (c *Character) resetLiedownTime() {
	c.liedownRemaining = c.liedownDuration()
	c.liedownInitialized = true
}

func (c *Character) liedownDuration() int32 {
	if c.StateFile != nil {
		return int32(c.StateFile.Data.LiedownTime)
	}
	return 60
}

func (c *Character) getLiedownTime() int32 {
	if !c.liedownInitialized {
		return c.liedownDuration()
	}
	return c.liedownRemaining
}

func (c *Character) advanceLiedownTime() {
	if !c.liedownInitialized || c.MoveType != "H" {
		c.resetLiedownTime()
	}
	if c.StateNo == 5110 && c.liedownRemaining > 0 {
		c.liedownRemaining--
	}
}

// Physical input transitions, including release/directions/start, shorten the
// remaining liedown counter. Holding an input does not repeatedly shorten it.
func (c *Character) updateRecoveryInput(raw []string) {
	var mask uint16
	for _, key := range raw {
		bit := -1
		switch key {
		case "ButtonA":
			bit = 0
		case "ButtonB":
			bit = 1
		case "ButtonC":
			bit = 2
		case "ButtonX":
			bit = 3
		case "ButtonY":
			bit = 4
		case "ButtonZ":
			bit = 5
		case "ButtonStart":
			bit = 6
		case "DirUp":
			bit = 7
		case "DirDown":
			bit = 8
		case "DirForward":
			bit = 9
			if c.Facing < 0 {
				bit = 10
			}
		case "DirBack":
			bit = 10
			if c.Facing < 0 {
				bit = 9
			}
		}
		if bit >= 0 {
			mask |= 1 << bit
		}
	}
	changed := mask ^ c.recoveryInputMask
	c.recoveryInputMask = mask
	if c.StateNo != 5110 {
		return
	}
	if !c.liedownInitialized {
		c.resetLiedownTime()
	}
	for ; changed != 0; changed &= changed - 1 {
		if c.liedownRemaining > 0 {
			c.liedownRemaining = int32(int64(c.liedownRemaining) * 3 / 4)
		}
	}
}
