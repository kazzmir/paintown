package character

// IsAlive reports the retained death state. Restoring positive life does not
// revive a player once the beta has committed its death. Before the first
// update, programmatic callers can still finish setting initial life.
func (c *Character) IsAlive() bool {
	if !c.lifeStateInitialized {
		return c.Life > 0
	}
	return !c.knockedOut
}

func (c *Character) initializeLifeState() {
	if !c.lifeStateInitialized {
		c.knockedOut = c.Life <= 0
		c.lifeStateInitialized = true
	}
}

func (c *Character) noKOThisPass() bool {
	if c.Parent == nil && c.World != nil && c.World.Training {
		return true
	}
	if c.Timeline != nil {
		return c.Timeline.NoKO
	}
	return c.Specials["noko"]
}

// Each entity completes its own life decision after CNS and pending damage.
// An earlier assertion can protect a later entity in this pass. Frozen entities
// retain their death state, and a later assertion cannot undo an earlier death.
func (c *Character) completeLifeState() {
	if !c.worldFrozen && c.Life <= 0 && !c.noKOThisPass() {
		c.knockedOut = true
	}
}
