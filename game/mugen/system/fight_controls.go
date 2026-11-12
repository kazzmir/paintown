package main

import (
	"fmt"
	"image"
	"image/color"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
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
	cursor, control, guard, stance, distance, jam int
	minimized                                     bool
	panel                                         *ebiten.Image
}

var trainingChoices = [][]string{
	{"Cooperative", "AI", "Manual"},
	{"None", "Auto"},
	{"Stand", "Crouch", "Jump", "W Jump"},
	{"Any", "Close", "Medium", "Far"},
	{"None", "A", "B", "C", "X", "Y", "Z", "Start"},
}

func (s *FightState) updateTrainingMenu() bool {
	im, controls := input.GlobalManager, s.training
	for action := input.ActionA; action <= input.ActionZ; action++ {
		if im.IsJustPressed(1, action) {
			return true
		}
	}
	if im.KeyJustPressed(ebiten.KeyM) {
		controls.minimized = !controls.minimized
	}
	if controls.minimized {
		return false
	}
	if im.IsJustPressed(1, input.ActionUp) {
		controls.cursor = (controls.cursor + 4) % 5
	}
	if im.IsJustPressed(1, input.ActionDown) {
		controls.cursor = (controls.cursor + 1) % 5
	}
	change := 0
	if im.IsJustPressed(1, input.ActionLeftUI) {
		change--
	}
	if im.IsJustPressed(1, input.ActionRightUI) {
		change++
	}
	if change != 0 {
		values := []*int{&controls.control, &controls.guard, &controls.stance, &controls.distance, &controls.jam}
		value := values[controls.cursor]
		*value = (*value + change + len(trainingChoices[controls.cursor])) % len(trainingChoices[controls.cursor])
		// AILevel must reach authored CNS as well as the input adapter. Other
		// modes keep the dummy human-owned, including its helpers on creation.
		level := 0
		if controls.control == 1 {
			level = max(1, min(8, s.engine.cfg.Options.Difficulty))
		}
		s.match.SetAILevel(2, level)
	}
	return false
}

func (s *FightState) trainingInput(facing int) []string {
	controls, game := s.training, s.match
	if controls.control == 2 {
		return character.GetRawInputs(2, facing)
	}
	if controls.control == 1 {
		return game.BasicAIInput(2)
	}
	if game.Phase != match.Active {
		return nil
	}
	dummy := game.Players[1].Character
	var raw []string
	switch controls.stance {
	case 1:
		raw = append(raw, "DirDown")
	case 2:
		if dummy.StateType != "A" {
			raw = append(raw, "DirUp")
		}
	case 3:
		// Releasing between attempts also exercises authored air-jump budgets.
		if game.Tick%4 == 0 {
			raw = append(raw, "DirUp")
		}
	}
	guarding := controls.guard == 1 && dummy.GetInGuardDist()
	if guarding {
		raw = append(raw, "DirBack")
		if game.Players[0].Character.StateType == "C" && controls.stance != 1 {
			raw = append(raw, "DirDown")
		}
	} else if controls.distance != 0 && dummy.Ctrl {
		// Functional spacing policy in character units. Exact native dummy/AI
		// decisions remain part of the separately tracked fidelity work.
		target := []float64{0, 0, 100, 200}[controls.distance]
		distance, _ := dummy.GetOpponentBodyDistance()
		scale := float64(dummy.LocalCoordinateWidth()) / 320
		if distance > (target+4)*scale {
			raw = append(raw, "DirForward")
		}
		if distance < (target-4)*scale {
			raw = append(raw, "DirBack")
		}
	}
	if controls.jam != 0 && game.Tick%8 == 0 {
		raw = append(raw, []string{"", "ButtonA", "ButtonB", "ButtonC", "ButtonX", "ButtonY", "ButtonZ", "ButtonStart"}[controls.jam])
	}
	return raw
}

func (s *FightState) drawTrainingMenu(screen *ebiten.Image) {
	controls := s.training
	if !s.userPaused || controls == nil {
		return
	}
	if controls.panel == nil {
		controls.panel = ebiten.NewImage(280, 150)
	}
	panel := controls.panel
	if controls.minimized {
		panel = panel.SubImage(image.Rect(0, 0, 280, 42)).(*ebiten.Image)
	}
	panel.Fill(color.RGBA{0, 0, 0, 210})
	text := "TRAINING - PAUSED"
	if controls.minimized {
		text += "\nM: show menu | Pause: resume"
	} else {
		labels := []string{"Dummy control", "Guard mode", "Dummy mode", "Distance", "Button jam"}
		values := []int{controls.control, controls.guard, controls.stance, controls.distance, controls.jam}
		var lines []string
		for index, label := range labels {
			marker := " "
			if controls.cursor == index {
				marker = ">"
			}
			lines = append(lines, fmt.Sprintf("%s %s: %s", marker, label, trainingChoices[index][values[index]]))
		}
		text += "\n" + strings.Join(lines, "\n") + "\nArrows: change | M: minimize\nAttack/Pause: resume | Scroll: step"
	}
	ebitenutil.DebugPrintAt(panel, text, 5, 4)
	options := &ebiten.DrawImageOptions{}
	scale := float64(screen.Bounds().Dy()) / 240
	options.GeoM.Scale(scale, scale)
	options.GeoM.Translate(float64(screen.Bounds().Min.X)+8*scale, float64(screen.Bounds().Min.Y)+8*scale)
	screen.DrawImage(panel, options)
}
