package main

import (
	"fmt"
	"image/color"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/input"
)

var inputConfigActions = []input.Action{input.ActionJump, input.ActionCrouch, input.ActionLeft, input.ActionRight, input.ActionA, input.ActionB, input.ActionC, input.ActionX, input.ActionY, input.ActionZ, input.ActionStart, input.ActionEscape}

// Session edits use the same immutable binding setters as YAML. No user file is
// rewritten. Raw UI navigation remains available while a player is disabled or
// while the action used to open this screen is being rebound.
type InputConfigState struct {
	engine                        *Engine
	back                          State
	player, cursor                int
	waiting, appendBinding, armed bool
	canvas                        *ebiten.Image
}

func NewInputConfigState(engine *Engine, back State) *InputConfigState {
	return &InputConfigState{engine: engine, back: back, player: 1}
}

func (s *InputConfigState) writeKeyboard(keys ...ebiten.Key) {
	im := input.GlobalManager
	action := inputConfigActions[s.cursor-1]
	if s.appendBinding {
		keys = append(im.KeyboardBindings(s.player, action), keys...)
	}
	if s.player == 1 {
		im.P1Keyboard.SetBindings(action.String(), keys...)
		s.engine.cfg.Input.Player1Keyboard = im.P1Keyboard
	} else {
		im.P2Keyboard.SetBindings(action.String(), keys...)
		s.engine.cfg.Input.Player2Keyboard = im.P2Keyboard
	}
}

func (s *InputConfigState) Update() (State, error) {
	im := input.GlobalManager
	if s.engine.motif.OptionBG != nil {
		s.engine.motif.OptionBG.Update()
	}
	if im.KeyJustPressed(ebiten.KeyEscape) || im.IsJustPressed(1, input.ActionEscape) {
		if s.waiting {
			s.waiting = false
			return s, nil
		}
		return s.back, nil
	}
	if s.waiting {
		if !s.armed {
			if !im.AnyKeyJustPressed() {
				held := false
				for key := ebiten.Key(0); key <= ebiten.KeyMax; key++ {
					held = held || im.KeyPressed(key)
				}
				s.armed = !held
			}
			return s, nil
		}
		for key := ebiten.Key(0); key <= ebiten.KeyMax; key++ {
			if im.KeyJustPressed(key) {
				s.writeKeyboard(key)
				s.waiting = false
				break
			}
		}
		return s, nil
	}
	if im.KeyJustPressed(ebiten.KeyTab) {
		s.player = 3 - s.player
	}
	if im.KeyJustPressed(ebiten.KeyArrowUp) || im.IsJustPressed(1, input.ActionUp) {
		s.cursor = (s.cursor + 12) % 13
	}
	if im.KeyJustPressed(ebiten.KeyArrowDown) || im.IsJustPressed(1, input.ActionDown) {
		s.cursor = (s.cursor + 1) % 13
	}
	confirm := im.KeyJustPressed(ebiten.KeyEnter) || im.KeyJustPressed(ebiten.KeyNumpadEnter) || im.IsJustPressed(1, input.ActionEnter)
	if s.cursor == 0 {
		if confirm || im.KeyJustPressed(ebiten.KeyArrowLeft) || im.KeyJustPressed(ebiten.KeyArrowRight) || im.IsJustPressed(1, input.ActionLeftUI) || im.IsJustPressed(1, input.ActionRightUI) {
			im.KeyboardDisabled[s.player-1] = !im.KeyboardDisabled[s.player-1]
			s.engine.cfg.Input.KeyboardDisabled = im.KeyboardDisabled
		}
	} else if im.KeyJustPressed(ebiten.KeyDelete) {
		s.appendBinding = false
		s.writeKeyboard()
	} else if confirm || im.KeyJustPressed(ebiten.KeyInsert) {
		s.waiting, s.armed = true, false
		s.appendBinding = im.KeyJustPressed(ebiten.KeyInsert)
	}
	return s, nil
}

func (s *InputConfigState) Draw(screen *ebiten.Image) {
	if s.canvas == nil {
		s.canvas = ebiten.NewImage(400, 280)
	}
	s.canvas.Fill(color.RGBA{12, 18, 32, 255})
	im := input.GlobalManager
	ebitenutil.DebugPrintAt(s.canvas, fmt.Sprintf("PLAYER %d KEYBOARD  |  Tab: other player", s.player), 8, 4)
	enabled := "Enabled"
	if im.KeyboardDisabled[s.player-1] {
		enabled = "Disabled"
	}
	rows := []string{"Keyboard: " + enabled}
	for _, action := range inputConfigActions {
		var names []string
		for _, key := range im.KeyboardBindings(s.player, action) {
			names = append(names, key.String())
		}
		value := strings.Join(names, ", ")
		if len(names) == 0 {
			value = "disabled"
		}
		rows = append(rows, fmt.Sprintf("%-7s %s", action.String(), value))
	}
	for index, row := range rows {
		marker := "  "
		if index == s.cursor {
			marker = "> "
		}
		ebitenutil.DebugPrintAt(s.canvas, marker+row, 8, 32+index*16)
	}
	hint := "Enter replace | Insert add | Delete disable"
	if s.waiting {
		hint = "Release controls, then press a key. Esc cancels"
	}
	ebitenutil.DebugPrintAt(s.canvas, hint+"\nSession changes only | Esc: back", 8, 244)
	drawUtilityCanvas(screen, s.canvas)
}

func drawUtilityCanvas(screen, canvas *ebiten.Image) {
	screen.Fill(color.RGBA{4, 8, 16, 255})
	scale := min(float64(screen.Bounds().Dx())/float64(canvas.Bounds().Dx()), float64(screen.Bounds().Dy())/float64(canvas.Bounds().Dy()))
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Scale(scale, scale)
	op.GeoM.Translate(float64(screen.Bounds().Min.X)+(float64(screen.Bounds().Dx())-float64(canvas.Bounds().Dx())*scale)/2, float64(screen.Bounds().Min.Y)+(float64(screen.Bounds().Dy())-float64(canvas.Bounds().Dy())*scale)/2)
	screen.DrawImage(canvas, op)
}
