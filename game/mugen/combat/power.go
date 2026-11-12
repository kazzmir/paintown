package combat

// PowerRules supplies the global default HitDef transfer multipliers. A caller
// must distinguish an absent rule set from explicitly configured zero factors.
type PowerRules struct {
	Attack, GetHit float64
}

func DefaultPowerRules() PowerRules {
	return PowerRules{Attack: .7, GetHit: .6}
}
