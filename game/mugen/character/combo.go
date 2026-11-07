package character

import "github.com/kazzmir/paintown/game/mugen/cns"

// Combo presentation belongs to the attacking root, including helper and
// projectile contacts. Attack HitCount and received GetHitVar(hitcount) keep
// their independent semantics.
func (c *Character) creditCombo(target *Character, hit cns.HitDef, continuing bool) {
	owner := c.rootPlayer()
	if owner.comboTarget != target || !continuing {
		owner.ComboCount = 0
	}
	owner.comboTarget = target
	count := 1
	if hit.NumHits != nil {
		count = *hit.NumHits
	}
	owner.HitAdd(count)
}

func (w *World) finishCombos() {
	for _, root := range w.Roots {
		if target := root.comboTarget; target != nil && (target.destroyed || target.MoveType != "H" || target.GetHitGuarded) {
			root.comboTarget = nil
			root.ComboCount = 0
			// Do not emit a new display event: the HUD retains the last total
			// for its configured displaytime after the defender recovers.
		}
	}
}
