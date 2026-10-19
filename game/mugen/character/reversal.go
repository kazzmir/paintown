package character

import (
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/combat"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

type reversalState struct {
	mask combat.Attributes
	hit  cns.HitDef
}

func reversalCollision(reverser, attacker *Character) bool {
	if reverser.reversal == nil || attacker.isProjectile || !attacker.HitDefActive || attacker.MoveType != "A" || attacker.destroyed || reverser.destroyed || reverser.Life <= 0 || reverser.TeamSide == attacker.TeamSide || reverser.worldFrozen && attacker.worldFrozen {
		return false
	}
	// Unlike HitBy, reversal masks match the authored attack posture.
	attr, err := combat.ParseAttributes(attacker.CurrentHitDef.Attr)
	if err != nil || !reverser.reversal.mask.Matches(attr) {
		return false
	}
	if attacker.CurrentHitDef.HitOnce && len(attacker.HitGuard) > 0 {
		return false
	}
	a, b := reverser.contactFrame, attacker.contactFrame
	if !reverser.contactFrameReady {
		a = reverser.CurrentElement()
	}
	if !attacker.contactFrameReady {
		b = attacker.CurrentElement()
	}
	if a == nil || b == nil {
		return false
	}
	ax, ay := reverser.spriteScale()
	bx, by := attacker.spriteScale()
	x, y := reverser.PositionOf(attacker)
	scale := reverser.ScaleFrom(attacker)
	for _, left := range a.Clsn1 {
		x1, y1, x2, y2 := BoxToWorld(left, reverser.X, reverser.Y, reverser.Facing, ax, ay)
		for _, right := range b.Clsn1 {
			u1, v1, u2, v2 := BoxToWorld(right, x, y, attacker.Facing, bx*scale, by*scale)
			if Intersects(x1, y1, x2, y2, u1, v1, u2, v2) {
				return true
			}
		}
	}
	return false
}

func reverseAttacks(players []*Character) error {
	// The beta reverses character HitDefs, not Projectile controllers.
	attackers := players
	for _, reverser := range players {
		for _, attacker := range attackers {
			if !reversalCollision(reverser, attacker) {
				continue
			}
			hit := reverser.reversal.hit
			reverser.reversal = nil
			attacker.HitDefActive = false
			target := attacker
			reverser.MoveHit, reverser.MoveContact, reverser.MoveGuarded = 1, 1, 0
			target.MoveReversed = 1
			n := 1
			if hit.NumHits != nil {
				n = *hit.NumHits
			}
			reverser.HitCount += n
			reverser.UniqHitCount += n
			if len(hit.GetPower) > 0 {
				reverser.AddPower(hit.GetPower[0])
			}
			if len(hit.PauseTime) > 0 {
				reverser.HitPauseTime = hit.PauseTime[0]
			}
			if len(hit.PauseTime) > 1 {
				target.HitPauseTime = hit.PauseTime[1]
			}
			if hit.P1StateNo != nil && *hit.P1StateNo >= 0 {
				reverser.changeStateWithEntry(*hit.P1StateNo, engine.EntryOverrides{}, true)
			}
			if hit.P2StateNo != nil && *hit.P2StateNo >= 0 {
				if hit.P2GetP1State {
					target.stateOwner = reverser.currentStateOwner()
				}
				target.changeStateWithEntry(*hit.P2StateNo, engine.EntryOverrides{}, true)
			}
			reverser.acquireTarget(target, hit.ID)
			record := reverser.hitTargets[target]
			record.reversal = true
			reverser.hitTargets[target] = record
			// Reversal sparkxy is added to the reversed attack's spark offset.
			hit.SparkXY = append([]int(nil), hit.SparkXY...)
			for len(hit.SparkXY) < 2 {
				hit.SparkXY = append(hit.SparkXY, 0)
			}
			for i, v := range attacker.CurrentHitDef.SparkXY {
				if i < 2 {
					hit.SparkXY[i] += int(float32(v) * float32(reverser.ScaleFrom(attacker)))
				}
			}
			if err := reverser.contactEffectFrom(attacker, reverser, hit, false); err != nil {
				return err
			}
			if err := target.contactSound(reverser, hit, false); err != nil {
				return err
			}
			break
		}
	}
	return nil
}
