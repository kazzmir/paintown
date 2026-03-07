package main

import (
	"flag"
	"fmt"
	"image/color"
	"log"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
)

type Game struct {
	player *character.Player
	camX   float64
	camY   float64

	windowWidth  int
	windowHeight int
}

func (g *Game) Update() error {
	// Update Input Manager (poll gamepads, etc.)
	input.GlobalManager.Update()

	if input.GlobalManager.IsJustPressed(1, input.ActionEscape) {
		fmt.Println("\nEscape pressed, terminating test.")
		return ebiten.Termination
	}
	// Update player (ticks state machine and command buffer)
	g.player.Update()

	// Consolidated console output (single line updated with \r)
	fmt.Printf("\rTick: %d | State: %d | Pos: (%.1f, %.1f) | Anim: %d (Time: %d) | Key: %-10s | Action: %-15s | Cmd: %-30s | Change: %-15s",
		g.player.Character.GetTime(),
		g.player.Character.GetStateNo(),
		g.player.Character.X,
		g.player.Character.Y,
		g.player.Character.GetAnim(),
		g.player.Character.GetAnimTime(),
		input.GlobalManager.LastKeyEvent,
		input.GlobalManager.LastActionEvent,
		g.player.Character.LastCommandMatched,
		g.player.Character.LastStateChange)

	return nil
}

func (g *Game) Draw(screen *ebiten.Image) {
	// Clear screen to light gray
	screen.Fill(color.RGBA{200, 200, 200, 255})

	g.player.DrawScaled(screen, g.camX, g.camY, 1.5, 1.5)

	msg := fmt.Sprintf("State: %d\nTime: %d\nAnim: %d\nAnimTime: %d\nPos: (%.2f, %.2f)\nStateType: %s\nPhysics: %s\nAction: %s",
		g.player.Character.GetStateNo(),
		g.player.Character.GetTime(),
		g.player.Character.GetAnim(),
		g.player.Character.GetAnimTime(),
		g.player.Character.X,
		g.player.Character.Y,
		g.player.Character.GetStateType(),
		g.player.Character.GetPhysics(),
		input.GlobalManager.LastActionEvent)
	ebitenutil.DebugPrint(screen, msg)
}

func (g *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 320, 240 // Logical MUGEN resolution
}

func main() {
	fmt.Println("Starting MUGEN Character Test...")

	dataDirFlag := flag.String("data-dir", "data-new/mugen", "Path to MUGEN data directory")
	flag.Parse()

	// Robust root determination
	root := *dataDirFlag
	if filepath.Base(root) == "data" {
		root = filepath.Dir(root)
	}
	fmt.Printf("MUGEN Root identified as: %s\n", root)

	// Load configuration
	cfg, err := config.LoadConfig(root)
	if err != nil {
		fmt.Printf("Warning: failed to load mugen.cfg, using defaults: %v\n", err)
		cfg = &config.MugenConfig{}
	}
	input.GlobalManager = input.NewInputManager(cfg)

	// Load KFM
	baseDir := filepath.Join(root, "chars/kfm")
	p, err := character.LoadPlayer(baseDir, "kfm.def")
	if err != nil {
		log.Fatalf("failed to load player from %s: %v", baseDir, err)
	}
	fmt.Println("Player loaded successfully.")

	// Initial position (center of logical 320x240 screen)
	p.Character.X = 160
	p.Character.Y = 0

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
		camY:         -200,
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
