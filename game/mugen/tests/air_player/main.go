package main

import (
	"fmt"
	"image/color"
	"log"
	"os"
	"sort"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

type App struct {
	sprites         map[string]*ebiten.Image
	spriteAxes      map[string][2]float64
	actions         *air.Data
	cachedActionIDs []int

	currentActionIdx int
	currentFrameIdx  int
	tickCounter      int

	err error
}

func getPath(relPath string) string {
	if _, err := os.Stat(relPath); err == nil {
		return relPath
	}
	return "../../../" + relPath
}

func NewApp() *App {
	app := &App{
		sprites:    make(map[string]*ebiten.Image),
		spriteAxes: make(map[string][2]float64),
	}

	// 1. Try to load an external palette (ACT)
	var initialPalette color.Palette
	if actFile, err := os.Open(getPath("data-new/mugen/chars/kfm/kfm6.act")); err == nil {
		if pal, err := sff.ReadPaletteACT(actFile, true); err == nil {
			initialPalette = pal
		}
		actFile.Close()
	}

	// 2. Load SFF (Sprites)
	sffFile, err := os.Open(getPath("data-new/mugen/chars/kfm/kfm.sff"))
	if err != nil {
		app.err = fmt.Errorf("failed to open sff: %v", err)
		return app
	}
	defer sffFile.Close()

	sffData, err := sff.ParseWithPalette(sffFile, initialPalette)
	if err != nil {
		app.err = fmt.Errorf("failed to parse sff: %v", err)
		return app
	}

	// Cache images
	for _, spr := range sffData.Sprites {
		if spr.Image != nil {
			key := fmt.Sprintf("%d-%d", spr.GroupNumber, spr.ImageNumber)
			app.sprites[key] = ebiten.NewImageFromImage(spr.Image)
			app.spriteAxes[key] = [2]float64{float64(spr.XAxis), float64(spr.YAxis)}
		}
	}

	// 2. Load AIR (Animations)
	airFile, err := os.Open(getPath("data-new/mugen/chars/kfm/kfm.air"))
	if err != nil {
		app.err = fmt.Errorf("failed to open air: %v", err)
		return app
	}
	defer airFile.Close()

	airData, err := air.Parse(airFile)
	if err != nil {
		app.err = fmt.Errorf("failed to parse air: %v", err)
		return app
	}
	app.actions = airData

	// Sort action IDs for consistent cycling
	for id := range airData.Actions {
		app.cachedActionIDs = append(app.cachedActionIDs, id)
	}
	sort.Ints(app.cachedActionIDs)

	return app
}

func (a *App) Update() error {
	input.GlobalManager.Update()
	if input.GlobalManager.IsJustPressed(1, input.ActionEscape) {
		return ebiten.Termination
	}

	if a.err != nil {
		fmt.Println("App Error:", a.err)
		a.err = nil // Print once
	}

	if len(a.cachedActionIDs) == 0 {
		return nil
	}

	// Cycle Actions
	if input.GlobalManager.IsJustPressed(1, input.ActionRightUI) {
		a.currentActionIdx = (a.currentActionIdx + 1) % len(a.cachedActionIDs)
		a.currentFrameIdx = 0
		a.tickCounter = 0
	} else if input.GlobalManager.IsJustPressed(1, input.ActionLeftUI) {
		a.currentActionIdx--
		if a.currentActionIdx < 0 {
			a.currentActionIdx = len(a.cachedActionIDs) - 1
		}
		a.currentFrameIdx = 0
		a.tickCounter = 0
	}

	// Advance Animation Frame
	actionID := a.cachedActionIDs[a.currentActionIdx]
	action := a.actions.Actions[actionID]

	if len(action.Elements) > 0 {
		elem := action.Elements[a.currentFrameIdx]

		// Instant skip handling for elements without loaded sprites
		key := fmt.Sprintf("%d-%d", elem.Group, elem.Image)
		_, hasSprite := a.sprites[key]

		if !hasSprite {
			// Fast forward past broken unlinked sprites
			a.currentFrameIdx++
			if a.currentFrameIdx >= len(action.Elements) {
				a.currentFrameIdx = 0
			}
			a.tickCounter = 0
			return nil
		}

		// If Time == -1, the frame holds indefinitely
		if elem.Time != -1 {
			a.tickCounter++
			if a.tickCounter >= elem.Time && elem.Time > 0 {
				// Move to next frame
				a.tickCounter = 0
				a.currentFrameIdx++

				// Loop animation back to frame 0
				if a.currentFrameIdx >= len(action.Elements) {
					a.currentFrameIdx = 0
				}
			} else if elem.Time == 0 {
				// Instant skip frame
				a.currentFrameIdx++
				if a.currentFrameIdx >= len(action.Elements) {
					a.currentFrameIdx = 0
				}
			}
		}
	}

	return nil
}

// Skipping cross lines without image/color
func (a *App) Draw(screen *ebiten.Image) {
	if a.err != nil {
		ebitenutil.DebugPrint(screen, fmt.Sprintf("Error: %v", a.err))
		return
	}
	if len(a.cachedActionIDs) == 0 {
		ebitenutil.DebugPrint(screen, "No actions loaded.")
		return
	}

	actionID := a.cachedActionIDs[a.currentActionIdx]
	action := a.actions.Actions[actionID]

	baseX, baseY := float64(screen.Bounds().Dx()/2), float64(screen.Bounds().Dy()*3/4)

	// Draw anchor point (a small red cross), wait I can't easily without image/color
	// I'll skip it and just transform.

	if len(action.Elements) > 0 {
		elem := action.Elements[a.currentFrameIdx]
		key := fmt.Sprintf("%d-%d", elem.Group, elem.Image)

		if img, ok := a.sprites[key]; ok {
			op := &ebiten.DrawImageOptions{}

			axis := a.spriteAxes[key]

			// M.U.G.E.N sprites have an internal pivot (AxisX, AxisY) corresponding to the Floor/Center point.
			// Ebiten defaults Origin to Top-Left of the image (0,0).
			// So, to put the sprite's bottom-center "feet" onto (0,0), we translate negatively by the axis.
			op.GeoM.Translate(-axis[0], -axis[1])

			// Apply the .air frame's specific coordinate displacements
			op.GeoM.Translate(float64(elem.XOffset), float64(elem.YOffset))

			// Handle M.U.G.E.N Flip Flags ("H" for horizontal, "V" for vertical)
			if strings.Contains(elem.Flags, "H") {
				op.GeoM.Scale(-1, 1)
			}
			if strings.Contains(elem.Flags, "V") {
				op.GeoM.Scale(1, -1)
			}

			// Finally, translate the logical M.U.G.E.N (0,0) combat origin to the physical Ebiten screen anchor point.
			op.GeoM.Translate(baseX, baseY)

			// Render sprite
			screen.DrawImage(img, op)
		}
	}

	msg := fmt.Sprintf("M.U.G.E.N AIR Player\n\n"+
		"Action ID: %d (%d / %d)\n"+
		"Total Frames: %d\n"+
		"Current Frame: %d (Hold: %d/%d ticks)\n"+
		"Loaded Sprites: %d\n\n"+
		"Controls:\n"+
		"  [LEFT]  - Previous Action\n"+
		"  [RIGHT] - Next Action\n"+
		"  [ESC]   - Quit\n",
		actionID, a.currentActionIdx+1, len(a.cachedActionIDs),
		len(action.Elements),
		a.currentFrameIdx, a.tickCounter, action.Elements[a.currentFrameIdx].Time, len(a.sprites))

	ebitenutil.DebugPrint(screen, msg)
}

func (a *App) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 480, 360
}

func main() {
	ebiten.SetWindowSize(960, 720)
	ebiten.SetWindowTitle("Paintown MUGEN AIR Viewer")

	// Load configuration
	cfg, err := config.LoadConfig("data-new/mugen")
	if err != nil {
		fmt.Printf("Warning: failed to load mugen.cfg, using defaults: %v\n", err)
		cfg = &config.MugenConfig{}
	}
	input.GlobalManager = input.NewInputManager(cfg)

	app := NewApp()

	if err := ebiten.RunGame(app); err != nil {
		log.Fatal(err)
	}
}
