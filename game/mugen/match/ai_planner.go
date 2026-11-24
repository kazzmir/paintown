package match

import (
	"math"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/combat"
)

type aiContext uint8

const (
	aiNeutral aiContext = iota
	aiNavigate
	aiLearnedAttack
	aiPredictiveGuard
	aiFallback
)

type aiWaypoint struct {
	x, y float64
	jump bool
}

type aiPlanner struct {
	context             aiContext
	age, cooldown       int
	enemy               *character.Character
	state, elapsed      int
	stateType           string
	known               *character.AIMove
	selected            *character.AIMove
	prepared            string
	attempted, guardLow bool
	path                []aiWaypoint
	plan                []character.CommandInput
	lastX, lastY        float64
}

func (p *aiPlanner) enter(context aiContext) {
	p.context, p.age = context, 0
	if context != aiLearnedAttack {
		p.selected, p.prepared, p.attempted = nil, "", false
		p.plan = nil
	}
	if context != aiNavigate {
		p.path = nil
	}
}

func aiPosition(c *character.Character, width int) (float64, float64) {
	scale := float64(width) / float64(c.LocalCoordinateWidth())
	return c.X * scale, c.Y * scale
}

func aiWorldBounds(b character.AIBounds, c *character.Character, width int) (float64, float64, float64, float64) {
	x, y := aiPosition(c, width)
	left, right := float64(b.Left), float64(b.Right)
	if c.Facing < 0 {
		left, right = -right, -left
	}
	return x + left, y + float64(b.Top), x + right, y + float64(b.Bottom)
}

// The target uses accumulated move bounds as a range estimate and saved common
// pose hurtboxes for stand/walk/crouch/air. Live bounds cover unseen poses.
func aiBody(c *character.Character, known *character.AIMove) character.AIBounds {
	if c.AIMoves != nil {
		for index, state := range [...]int{0, 20, 11, 50} {
			if c.StateNo == state && c.AIMoves.Poses[index].Valid {
				return c.AIMoves.Poses[index]
			}
		}
	}
	if known != nil && known.Attack.Valid {
		return known.Attack
	}
	return c.AIBodyBounds()
}

func aiOverlaps(attacker, target *character.Character, attack character.AIBounds, known *character.AIMove, width int) bool {
	body := aiBody(target, known)
	if !attack.Valid || !body.Valid {
		return false
	}
	a, b, c, d := aiWorldBounds(attack, attacker, width)
	x, y, z, w := aiWorldBounds(body, target, width)
	return character.Intersects(a, b, c, d, x, y, z, w)
}

func (b *aiBrain) observeOpponent(enemy *character.Character, level int) {
	p := &b.planner
	state, kind, elapsed, _ := enemy.AIAction()
	if enemy.MoveType == "H" || enemy.MoveType == "I" && enemy.Ctrl {
		p.known = nil
	}
	if p.enemy != enemy || p.state != state || p.stateType != kind || elapsed < p.elapsed {
		p.enemy, p.state, p.stateType, p.known = enemy, state, kind, nil
		if enemy.AIMoves != nil {
			known := enemy.AIMoves.Move(state, kind)
			if known != nil && int(b.random.Next()%12) < level+3 {
				p.known = known
			}
		}
	}
	p.elapsed = elapsed
}

// learnedInput owns a pass only while a real plan is executing. The existing
// weighted policy remains the fallback for unknown moves and ineligible plans.
func (b *aiBrain) learnedInput(c, enemy *character.Character, distance, level int, cheat bool) bool {
	p := &b.planner
	if c.AIMoves == nil || c.AIMoves.Commands != c.CmdFile || c.MoveType == "H" && p.context != aiPredictiveGuard {
		p.enter(aiFallback)
		return false
	}
	b.observeOpponent(enemy, level)
	if p.cooldown > 0 {
		p.cooldown--
	}
	if p.context == aiNeutral && enemy.AIStartHeld && c.Ctrl {
		p.makePath(c, enemy)
		p.enter(aiNavigate)
	}
	if p.context == aiLearnedAttack {
		owned := b.executeLearnedAttack(c, cheat)
		p.age++
		return owned
	}
	if p.known != nil && c.Ctrl {
		// Prediction can interrupt navigation/fallback. Native counter
		// selection then runs from that guard context, before its keys emit.
		if p.context != aiPredictiveGuard && p.cooldown == 0 && aiOverlaps(enemy, c, p.known.Attack, nil, c.AIMoves.Width) && (p.known.Startup.Time.Mean == 0 || p.elapsed <= int(p.known.Active[0].Time.Mean)) {
			p.cooldown = 5
			if level < 4 {
				p.cooldown += int(b.random.Next() % int64(4-level))
			}
			if int(b.random.Next()%20) <= level+11 {
				p.enter(aiPredictiveGuard)
			}
		}
		_, _, _, outcome := enemy.AIAction()
		remaining := int(p.known.Startup.Time.Mean) - p.elapsed
		beforeStartup := remaining > 0
		if !beforeStartup && p.known.Control[outcome].Time.Valid {
			remaining = int(p.known.Control[outcome].Time.Mean) - p.elapsed
		}
		chance := level * 5
		if beforeStartup {
			chance = 2 + p.elapsed/4
			if p.elapsed < 3 {
				chance += 3
			}
			chance -= 6 / max(1, remaining)
		} else {
			if distance > 200 {
				chance /= 2
			}
			if distance > 150 {
				chance /= 2
			}
			for _, edge := range []int{100, 75, 50} {
				if distance < edge {
					chance += 10
				}
			}
		}
		if p.context == aiPredictiveGuard && remaining > 0 && int(b.random.Next()%100) < chance {
			if selected := b.counterMove(c, enemy, remaining, beforeStartup, cheat); selected != nil {
				p.enter(aiLearnedAttack)
				p.selected = selected
				return b.executeLearnedAttack(c, cheat)
			}
		}
	}
	if p.context == aiPredictiveGuard {
		if p.known == nil && c.GuardThreat() == nil {
			p.enter(aiNeutral)
			b.keys[3] = false
			return false
		}
		flags := ""
		if p.known != nil {
			flags = p.known.GuardFlag
		}
		if threat := c.GuardThreat(); threat != nil {
			flags = threat.CurrentHitDef.GuardFlag
		}
		flags = strings.ReplaceAll(strings.ToUpper(flags), "M", "HL")
		low, high := strings.Contains(flags, "L"), strings.Contains(flags, "H")
		if low != high {
			p.guardLow = low
		} else if p.age == 0 {
			p.guardLow = c.StateType == "C"
		}
		b.keys[0], b.keys[1], b.keys[2], b.keys[3] = false, false, p.guardLow, true
		p.age++
		return true
	}
	if p.context == aiNavigate {
		if b.navigate(c) {
			p.age++
			return true
		}
		p.enter(aiNeutral)
	}
	if p.context != aiFallback {
		p.enter(aiFallback)
	}
	p.age++
	return false
}

func (b *aiBrain) counterMove(c, enemy *character.Character, remaining int, beforeStartup, cheat bool) *character.AIMove {
	type candidate struct {
		move  *character.AIMove
		plans [][]character.CommandInput
	}
	var candidates []candidate
	for _, move := range c.AIMoves.Moves(c.StateType) {
		if len(candidates) == 49 {
			break
		}
		index := move.CommandIndex
		if index < 0 || index >= len(b.commandNames) || b.commandNames[index] == "" {
			continue
		}
		startup := int(move.Startup.Time.Mean)
		var plans [][]character.CommandInput
		if !cheat {
			for _, plan := range b.commandPlans[index] {
				budget := remaining - startup
				// The final step releases transient keys after admission. It is
				// not part of the time needed to complete the physical command.
				for _, step := range plan[:len(plan)-1] {
					if step.Time >= budget {
						budget = 0
						break
					}
					budget -= step.Time
				}
				if budget > 0 {
					plans = append(plans, plan)
				}
			}
			if len(plans) == 0 {
				continue
			}
		}
		if startup >= remaining || !aiOverlaps(c, enemy, move.Attack, b.planner.known, c.AIMoves.Width) {
			continue
		}
		if move.ReversalAttribute != "" {
			mask, _ := combat.ParseAttributes(move.ReversalAttribute)
			incoming, _ := combat.ParseAttributes(b.planner.known.Attribute)
			if !beforeStartup || int(move.Active[0].Time.Mean) < remaining || !mask.Matches(incoming) {
				continue
			}
		}
		if beforeStartup && b.planner.known.ReversalAttribute != "" {
			mask, _ := combat.ParseAttributes(b.planner.known.ReversalAttribute)
			attack, _ := combat.ParseAttributes(move.Attribute)
			if mask.Matches(attack) {
				continue
			}
		}
		candidates = append(candidates, candidate{move, plans})
	}
	if len(candidates) == 0 {
		return nil
	}
	selected := candidates[int(b.random.Next()%int64(len(candidates)))]
	if len(selected.plans) > 0 {
		b.planner.plan = selected.plans[int(b.random.Next()%int64(len(selected.plans)))]
	}
	return selected.move
}

func (b *aiBrain) executeLearnedAttack(c *character.Character, cheat bool) bool {
	p := &b.planner
	move := p.selected
	if move == nil || c.MoveType == "H" {
		p.enter(aiNeutral)
		return false
	}
	if p.attempted {
		if c.MoveType == "I" && c.Ctrl && p.age > 0 {
			p.enter(aiNeutral)
			return false
		}
		return true
	}
	if p.prepared == "" {
		for _, context := range move.Contexts {
			if p.prepared == "" || context.StateType == c.StateType {
				p.prepared = context.StateType
			}
			if p.prepared == c.StateType {
				break
			}
		}
	}
	if c.StateType != p.prepared && p.age <= 4 {
		b.keys[0], b.keys[2] = p.prepared == "A", p.prepared == "C"
		return true
	}
	p.attempted = true
	if cheat {
		b.admitted = b.commandNames[move.CommandIndex : move.CommandIndex+1]
	} else if len(p.plan) > 0 {
		b.queue = p.plan
		b.planActive = true
		b.nextInput()
	}
	return true
}

func (p *aiPlanner) makePath(c, enemy *character.Character) {
	width := c.AIMoves.Width
	x, y := aiPosition(c, width)
	goalX, goalY := aiPosition(enemy, width)
	goalX -= 40 * float64(c.Facing)
	p.lastX, p.lastY = x, y
	p.path = []aiWaypoint{{goalX, goalY, goalY != 0}}
	if goalY == 0 || c.StateFile == nil || len(c.StateFile.Velocity.JumpNeu) < 2 {
		return
	}
	scale := float64(width) / float64(c.LocalCoordinateWidth())
	velocity := c.StateFile.Velocity.JumpFwd
	if (goalX-x)*float64(c.Facing) < 0 {
		velocity = c.StateFile.Velocity.JumpBack
	}
	gravity := c.StateFile.Movement.YAccel
	if gravity <= 0 {
		return
	}
	reach := math.Abs(2 * c.StateFile.Velocity.JumpNeu[1] * velocity / gravity * scale)
	if math.Abs(goalX-x) > reach/2 {
		launch := goalX - math.Copysign(reach/2, goalX-x)
		p.path = append([]aiWaypoint{{launch, 0, false}}, p.path...)
	}
}

func (b *aiBrain) navigate(c *character.Character) bool {
	p := &b.planner
	if len(p.path) == 0 || c.MoveType == "H" {
		return false
	}
	goal := p.path[0]
	x, y := aiPosition(c, c.AIMoves.Width)
	crossed := (goal.x-p.lastX)*(goal.x-x) <= 0 || math.Abs(goal.x-x) < 1
	// Keep the waypoint's original axis, not the preceding frame. A moving
	// jump finishes on horizontal passage; a vertical jump uses its height.
	// Requiring both crossings in one pass can make an actor jump forever.
	yReached := !goal.jump || c.VelX != 0 || (goal.y-p.lastY)*(goal.y-y) <= 0
	if crossed && yReached {
		p.path = p.path[1:]
		p.lastX, p.lastY = x, y
		b.releaseDirections()
		return len(p.path) > 0
	}
	direction := (goal.x - x) * float64(c.Facing)
	b.keys[0], b.keys[1], b.keys[2], b.keys[3] = goal.jump && c.StateType != "A", direction > 0, false, direction < 0
	return true
}
