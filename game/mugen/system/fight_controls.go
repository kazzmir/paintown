package main

import (
	"fmt"
	"math"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
)

// Runtime pause holds the whole fight, independently of CNS Pause/SuperPause.
// Input is still sampled every update; frame stepping advances exactly one pass.
func (s *FightState) holdForUserPause() bool {
	im := input.GlobalManager
	if im == nil {
		return false
	}
	if im.KeyJustPressed(ebiten.KeyPause) {
		s.userPaused = !s.userPaused
		return s.userPaused
	}
	if !s.userPaused {
		return false
	}
	if s.training != nil {
		if s.updateTrainingMenu() {
			s.userPaused = false
			return false
		}
	} else if s.engine.cfg.Input.AnyKeyUnpauses && !im.KeyJustPressed(ebiten.KeyScrollLock) && im.AnyKeyJustPressed() {
		s.userPaused = false
		return false
	}
	return !im.KeyJustPressed(ebiten.KeyScrollLock)
}

type trainingControls struct {
	repeat                                                 menuRepeat
	cursor, control, guard, stance, distance, jam, aiLevel int
	minimized                                              bool
	utility                                                utilityMenu
	frame, cooldown                                        int
	actor                                                  *character.Character
	mask                                                   uint16
	raw                                                    []string
	rawFacing                                              int
}

var trainingChoices = [][]string{
	{"Cooperative", "AI", "Manual"}, {"None", "Auto"},
	{"Stand", "Crouch", "Jump", "W Jump"}, {"Any", "Close", "Medium", "Far"},
	{"None", "A", "B", "C", "X", "Y", "Z", "Start"},
}

func (c *trainingControls) rows() []utilityRow {
	rows := []utilityRow{{"Dummy control", trainingChoices[0][c.control]}}
	if c.control == 1 {
		return append(rows, utilityRow{"AI Level", fmt.Sprint(c.aiLevel)})
	}
	if c.control == 2 {
		return rows
	}
	labels := []string{"Guard mode", "Dummy mode", "Distance", "Button jam"}
	for i, v := range []int{c.guard, c.stance, c.distance, c.jam} {
		rows = append(rows, utilityRow{labels[i], trainingChoices[i+1][v]})
	}
	return rows
}
func (s *FightState) updateTrainingMenu() bool {
	im, c := input.GlobalManager, s.training
	direction := c.repeat.directions(im, s.engine.ticksPerSecond())
	for action := input.ActionA; action <= input.ActionZ; action++ {
		if im.IsJustPressed(1, action) {
			return true
		}
	}
	if im.KeyJustPressed(ebiten.KeyM) || im.IsJustPressed(1, input.ActionStart) {
		c.minimized = !c.minimized
	}
	if c.minimized {
		return false
	}
	count := len(c.rows())
	if direction[0] {
		c.cursor = (c.cursor + count - 1) % count
	}
	if direction[1] {
		c.cursor = (c.cursor + 1) % count
	}
	change := 0
	if direction[2] {
		change--
	}
	if direction[3] {
		change++
	}
	if change != 0 {
		if c.cursor == 0 {
			c.control = (c.control + change + 3) % 3
		} else if c.control == 1 {
			c.aiLevel = (c.aiLevel-1+change+8)%8 + 1
		} else if c.control == 0 {
			values := []*int{&c.control, &c.guard, &c.stance, &c.distance, &c.jam}
			v := values[c.cursor]
			*v = (*v + change + len(trainingChoices[c.cursor])) % len(trainingChoices[c.cursor])
		}
		c.cursor = min(c.cursor, len(c.rows())-1)
		c.mask, c.cooldown, c.frame = 0, 0, -1
		level := 0
		if c.control == 1 {
			level = c.aiLevel
		}
		if s.match.Players[1].Character.AILevel != level {
			s.match.SetAILevel(2, level)
		}
	}
	return false
}

const (
	trainingUp uint16 = 1 << iota
	trainingBack
	trainingDown
	trainingForward
)

var trainingInputs = []string{"DirUp", "DirBack", "DirDown", "DirForward", "ButtonA", "ButtonB", "ButtonC", "ButtonX", "ButtonY", "ButtonZ", "ButtonStart"}

func (s *FightState) trainingInput(facing int) []string {
	c, game := s.training, s.match
	if c.control == 2 {
		return character.GetRawInputs(2, facing)
	}
	if c.control == 1 {
		return game.BasicAIInput(2)
	}
	if game.Phase != match.Active {
		return nil
	}
	dummy := game.Players[1].Character
	if c.actor != dummy {
		c.actor = dummy
		c.mask, c.cooldown, c.frame = 0, 0, -1
	}
	if c.frame != game.Tick {
		c.frame, c.rawFacing = game.Tick, dummy.Facing
		if c.cooldown > 0 {
			c.cooldown--
		} else {
			s.updateCooperativeDummy(dummy)
		}
		c.raw = c.raw[:0]
		for i, name := range trainingInputs {
			if c.mask&(1<<uint(i)) != 0 {
				c.raw = append(c.raw, name)
			}
		}
	}
	if facing == c.rawFacing {
		return c.raw
	}
	// Helpers recognize the same physical snapshot in their own heading.
	raw := append([]string(nil), c.raw...)
	for i, key := range raw {
		if key == "DirForward" {
			raw[i] = "DirBack"
		} else if key == "DirBack" {
			raw[i] = "DirForward"
		}
	}
	return raw
}

func (s *FightState) updateCooperativeDummy(dummy *character.Character) {
	c := s.training
	oldDown := c.mask&trainingDown != 0
	c.mask &^= trainingUp | trainingBack | trainingDown | trainingForward
	threat := dummy.GetInGuardDist()
	if c.guard == 1 && threat {
		crouch := c.stance == 1
		// Guard flags distinguish lows and overheads, independently of the
		// attacker's standing/crouching state. Preserve authored dual-height choice.
		flags := s.trainingGuardFlags(dummy)
		if strings.Contains(flags, "L") && !strings.Contains(flags, "H") {
			crouch = true
		}
		if strings.Contains(flags, "H") && !strings.Contains(flags, "L") {
			crouch = false
		}
		c.mask |= trainingBack
		if crouch && dummy.StateType != "A" {
			c.mask |= trainingDown
		}
		if oldDown != (c.mask&trainingDown != 0) {
			c.cooldown = 5
		}
		return
	}
	if c.distance != 0 && dummy.Ctrl && len(dummy.Opponents) > 0 {
		x, _ := dummy.PositionOf(dummy.Opponents[0])
		distance := math.Abs(x - dummy.X)
		unit := float64(dummy.LocalCoordinateWidth()) / 320
		low, high := []float64{0, 0, 100, 280}[c.distance]*unit, []float64{0, 50, 150, 0}[c.distance]*unit
		if high > 0 && distance > high {
			c.mask |= trainingForward
			return
		}
		// None must not accidentally guard merely because spacing walks back.
		if low > 0 && distance < low && !threat {
			c.mask |= trainingBack
			return
		}
	}
	switch c.stance {
	case 1:
		c.mask |= trainingDown
	case 2:
		if dummy.Y >= 0 {
			c.mask |= trainingUp
		}
	case 3:
		if dummy.Y >= 0 || dummy.VelY >= 0 {
			c.mask |= trainingUp
		}
	}
	if c.jam != 0 {
		// Native jam toggles its held button, then leaves five passes unchanged:
		// six pressed passes followed by six released passes, not isolated taps.
		c.mask ^= 1 << uint(c.jam+3)
		c.cooldown = 5
	}
}
func (s *FightState) trainingGuardFlags(dummy *character.Character) string {
	flags := ""
	check := func(enemy *character.Character) {
		if enemy == nil || enemy.MoveType != "A" || !enemy.HitDefActive {
			return
		}
		x, _ := dummy.PositionOf(enemy)
		distance := (dummy.X - x) * float64(enemy.Facing)
		if distance >= 0 && distance < enemy.GuardDistance*dummy.ScaleFrom(enemy) {
			value := strings.ToUpper(enemy.CurrentHitDef.GuardFlag)
			if strings.Contains(value, "M") {
				value += "HL"
			}
			flags += value
		}
	}
	for _, enemy := range dummy.Opponents {
		check(enemy)
		if enemy != nil {
			for _, projectile := range enemy.Projectiles {
				check(projectile)
			}
		}
	}
	for _, helper := range s.match.World.Helpers {
		if helper.TeamSide != dummy.TeamSide {
			check(helper)
		}
	}
	return flags
}
func (s *FightState) drawTrainingMenu(screen *ebiten.Image) {
	c := s.training
	if !s.userPaused || c == nil || c.minimized {
		return
	}
	c.utility.draw(s.engine, screen, c.rows(), c.cursor, "Training Menu", 30)
}
