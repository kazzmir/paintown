package main

import (
	"fmt"
	"image/color"
	"log"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
)

type Game struct {
	player  *character.Player
	camX    float64
	camY    float64
	punched bool

	windowWidth  int
	windowHeight int
}

func (g *Game) Update() error {
	// Update Input Manager (poll gamepads, etc.)
	input.GlobalManager.Update()

	// Exit on action Escape
	if input.GlobalManager.IsPressed(1, input.ActionEscape) {
		return ebiten.Termination
	}

	if g.player.Character.GetTime() == 100 && !g.punched {
		g.punched = true
		g.player.Character.ChangeState(200, -1, -1)
	}

	if g.player.Character.GetTime() > 120 && g.player.Character.GetStateNo() == 0 && g.punched {
		return ebiten.Termination
	}

	// Update player (ticks state machine and command buffer)
	g.player.Update()

	return nil
}

func (g *Game) Draw(screen *ebiten.Image) {
	// Clear screen to light gray
	screen.Fill(color.RGBA{200, 200, 200, 255})

	// The player.Draw normally handles its own coordinates, but we want 2x scale
	// Let's modify how we call it or apply a global transformation.
	// For now, let's just use a simple approach:
	g.player.DrawScaled(screen, g.camX, g.camY, 1.5, 1.5)

	// Debug info
	msg := fmt.Sprintf("State: %d\nTime: %d\nAnim: %d\nAnimTime: %d\nPos: (%.2f, %.2f)\nCam: (%.2f, %.2f)\nStateType: %s\nPhysics: %s\nMoveType: %s",
		g.player.Character.GetStateNo(),
		g.player.Character.GetTime(),
		g.player.Character.GetAnim(),
		g.player.Character.GetAnimTime(),
		g.player.Character.X,
		g.player.Character.Y,
		g.camX, g.camY,
		g.player.Character.GetStateType(),
		g.player.Character.GetPhysics(),
		g.player.Character.GetMoveType())
	ebitenutil.DebugPrint(screen, msg)
}

func (g *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 320, 240 // Logical MUGEN resolution
}

func main() {
	fmt.Println("Starting MUGEN Character Test...")

	// Load configuration
	cfg, err := config.LoadConfig("data-new/mugen")
	if err != nil {
		fmt.Printf("Warning: failed to load mugen.cfg, using defaults: %v\n", err)
		cfg = &config.MugenConfig{}
	}
	input.GlobalManager = input.NewInputManager(cfg)

	// Load KFM
	baseDir := "data-new/mugen/chars/kfm/"
	p, err := character.LoadPlayer(baseDir, "kfm.def")
	if err != nil {
		log.Fatalf("failed to load player: %v", err)
	}
	fmt.Println("Player loaded successfully.")

	// Initial position (center of logical 320x240 screen)
	p.Character.X = 160
	p.Character.Y = 200

	// Force initial state to Stand
	p.Character.ChangeState(0, -1, -1)
	fmt.Printf("Initial State: %d, Anim: %d\n", p.Character.GetStateNo(), p.Character.GetAnim())

	width := cfg.Video.Width
	if width == 0 {
		width = 640
	}
	height := cfg.Video.Height
	if height == 0 {
		height = 480
	}

	g := &Game{
		player:       p,
		camX:         0,
		camY:         0,
		windowWidth:  width,
		windowHeight: height,
	}

	ebiten.SetWindowSize(width, height)
	ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)
	ebiten.SetWindowTitle("MUGEN Character SM Test")
	fmt.Println("Running Ebiten game loop...")
	if err := ebiten.RunGame(g); err != nil {
		log.Fatal(err)
	}
}
