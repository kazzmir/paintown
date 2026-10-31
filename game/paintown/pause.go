package paintown

import (
	"errors"
	"fmt"
	"image"
	"image/color"
	"log"
	"slices"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/vector"
	"github.com/kazzmir/paintown/game/data"
	"github.com/kazzmir/paintown/game/lib/coroutine"
)

var ErrReturnToMainMenu = errors.New("return to main menu")

type PauseInput struct{ Up, Down, Confirm, Back bool }
type PauseAction int

const (
	PauseStay PauseAction = iota
	PauseContinue
	PauseQuitToMain
)

// This controller only updates menu state; world simulation never advances
// while its coroutine is active.
type PauseMenu struct {
	Root, Current *Menu
	Player        *PlayerState
	stack         []*Menu
	preview       *movePreview
	previewLayer  *ebiten.Image
}

func NewPauseMenu(player *PlayerState) (*PauseMenu, error) {
	root, err := LoadMenu("menu/in-game.txt", nil)
	if err != nil {
		return nil, err
	}
	for index := range root.Options {
		switch root.Options[index].Kind {
		case "return":
			root.Options[index].Name = "Quit to Main Menu"
		case "menu":
			root.Options[index].Name = "Game Options"
		}
	}
	return &PauseMenu{Root: root, Current: root, Player: player}, nil
}

func (pause *PauseMenu) reset() {
	pause.Current = pause.Root
	pause.stack = nil
	pause.preview = nil
	pause.Root.Selected = 0
	pause.Root.Notice = ""
	pause.Root.opacity, pause.Root.scrollOffset = 0, 0
}

func (pause *PauseMenu) Update(input PauseInput, system System) PauseAction {
	defer pause.updateMovePreview()
	menu := pause.Current
	menu.Update()
	play := func(path string) {
		if err := system.PlaySound(path); err != nil {
			log.Printf("Pause menu sound: %v", err)
		}
	}
	if input.Down && menu.Move(1) {
		play(menu.SelectSound)
	}
	if input.Up && menu.Move(-1) {
		play(menu.SelectSound)
	}
	if input.Back {
		play(menu.BackSound)
		if len(pause.stack) == 0 {
			return PauseContinue
		}
		pause.Current = pause.stack[len(pause.stack)-1]
		pause.stack = pause.stack[:len(pause.stack)-1]
		return PauseStay
	}
	if !input.Confirm || len(menu.Options) == 0 {
		return PauseStay
	}
	option := menu.Options[menu.Selected]
	play(menu.OKSound)
	switch option.Kind {
	case "continue":
		return PauseContinue
	case "return":
		return PauseQuitToMain
	case "move-list":
		pause.stack = append(pause.stack, menu)
		pause.Current = makeMoveList(pause.Player, menu)
	case "menu", "gibs":
		next, err := menu.OpenSubmenu(option)
		if err != nil {
			log.Printf("Pause submenu: %v", err)
			menu.Notice = "Could not open this menu"
		} else {
			pause.stack = append(pause.stack, menu)
			pause.Current = next
		}
	case "set-gibs":
		menu.ApplySetting(option)
	case "move", "notice":
		// Move entries are informational, not game commands.
	case "fullscreen":
		fullscreen := !ebiten.IsFullscreen()
		ebiten.SetFullscreen(fullscreen)
		menu.Notice = fmt.Sprintf("Fullscreen: %v", fullscreen)
	default:
		menu.Notice = option.Name + " is not implemented yet"
	}
	return PauseStay
}

func makeMoveList(player *PlayerState, parent *Menu) *Menu {
	menu := *parent
	menu.Name, menu.MenuInfo = "Move List", player.Name+" moves (relative to facing)\nEscape: Back"
	menu.Options = nil
	menu.Selected, menu.ticks = 0, 0
	menu.Notice = ""
	menu.opacity, menu.scrollOffset = 0, 0
	menu.Animations = nil
	menu.Bounds = image.Rect(30, 120, 300, 420)
	menu.FontSize, menu.FontWidth = 18, 18
	if parent.font != nil {
		face := *parent.font
		face.Size = 18
		menu.font = &face
	}
	menu.InfoX, menu.InfoY, menu.MenuInfoX, menu.MenuInfoY = 320, 65, 320, 452
	menu.Scroll, menu.Wrap, menu.DistanceFade = true, true, true
	for name, animation := range player.Animations {
		if len(animation.Keys) == 0 {
			continue
		}
		var keys []string
		for _, key := range animation.Keys {
			switch key {
			case InputKeyAttack1:
				keys = append(keys, "A")
			case InputKeyAttack2:
				keys = append(keys, "S")
			case InputKeyJump:
				keys = append(keys, "Space")
			case InputKeyForward:
				keys = append(keys, "Forward")
			case InputKeyBack:
				keys = append(keys, "Back")
			case InputKeyDown:
				keys = append(keys, "Down")
			}
		}
		status := "On the ground"
		switch animation.Status {
		case "grab":
			status = "While grabbing"
		case "jump":
			status = "While jumping"
		}
		if animation.Sequence != "" {
			status += "; after " + strings.ReplaceAll(animation.Sequence, "-", " ")
		}
		menu.Options = append(menu.Options, MenuOption{
			Kind: "move", Name: strings.ReplaceAll(name, "-", " "), Info: strings.Join(keys, " > ") + "\n" + status,
			moveAnimation: animation,
		})
	}
	slices.SortFunc(menu.Options, func(a, b MenuOption) int { return strings.Compare(a.Name, b.Name) })
	if len(menu.Options) == 0 {
		menu.Options = []MenuOption{{Kind: "notice", Name: "No moves available", Info: "This character has no configured move inputs."}}
	}
	return &menu
}

func readPauseInput() PauseInput {
	input := ReadMenuInput()
	return PauseInput{Up: input.Up, Down: input.Down, Confirm: input.Confirm, Back: input.Back || PauseRequested()}
}

func RunPauseMenu(pause *PauseMenu, yield coroutine.YieldFunc, setDraw func(data.DrawFunc) data.DrawFunc, system System) error {
	return runPauseMenu(pause, yield, setDraw, system, readPauseInput)
}

func runPauseMenu(pause *PauseMenu, yield coroutine.YieldFunc, setDraw func(data.DrawFunc) data.DrawFunc, system System, readInput func() PauseInput) error {
	pause.reset()
	var background data.DrawFunc
	background = setDraw(func(screen *ebiten.Image) {
		if background != nil {
			background(screen)
		}
		vector.FillRect(screen, 0, 0, data.ScreenWidth, data.ScreenHeight, color.NRGBA{A: 120}, false)
		pause.Draw(screen)
	})
	defer setDraw(background)
	for {
		// Consume the opening key and yield every paused frame, allowing only
		// menu updates and rendering (not players, enemies, or projectiles).
		if err := yield(); err != nil {
			return err
		}
		switch pause.Update(readInput(), system) {
		case PauseContinue:
			// Do not leak Space/S or other menu inputs into gameplay.
			return yield()
		case PauseQuitToMain:
			return ErrReturnToMainMenu
		}
	}
}
