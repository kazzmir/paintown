package character

import "github.com/kazzmir/paintown/game/mugen/air"

func projectileCancellation(a, b *Character) bool {
	if a.destroyed || b.destroyed || a.projectile == nil || b.projectile == nil || a.projectile.removing || b.projectile.removing || a.TeamSide == b.TeamSide {
		return false
	}
	if a.worldFrozen && b.worldFrozen {
		return false
	}
	first, second := a.CurrentElement(), b.CurrentElement()
	if first == nil || second == nil {
		return false
	}
	ax, ay := a.spriteScale()
	bx, by := b.spriteScale()
	x, y := a.PositionOf(b)
	scale := a.ScaleFrom(b)
	box := func(v air.Box, x, y float64, facing int, sx, sy float64) [4]float64 {
		left, top, right, bottom := BoxToWorld(v, x, y, facing, sx, sy)
		return [4]float64{left, top, right, bottom}
	}
	// Native cancellation uses Clsn2 against Clsn2, independently of the
	// attack definition, remaining hits, and HitDef's priority/class.
	for _, left := range first.Clsn2 {
		aa := box(left, a.X, a.Y, a.Facing, ax, ay)
		for _, right := range second.Clsn2 {
			bb := box(right, x, y, b.Facing, bx*scale, by*scale)
			if Intersects(aa[0], aa[1], aa[2], aa[3], bb[0], bb[1], bb[2], bb[3]) {
				return true
			}
		}
	}
	return false
}

func cancelProjectiles(players []*Character) {
	var projectiles []*Character
	for _, owner := range players {
		projectiles = append(projectiles, owner.Projectiles...)
	}
	used := map[*Character]bool{}
	for i, a := range projectiles {
		if used[a] {
			continue
		}
		for _, b := range projectiles[i+1:] {
			if used[b] || !projectileCancellation(a, b) {
				continue
			}
			// Each projectile can participate in only one cancellation pair
			// this pass, including a surviving higher-priority projectile.
			used[a], used[b] = true, true
			left, right := a.projectile.priority, b.projectile.priority
			if left <= right {
				a.removeProjectile("cancel")
			} else {
				a.projectile.priority--
			}
			if right <= left {
				b.removeProjectile("cancel")
			} else {
				b.projectile.priority--
			}
			break
		}
	}
}
