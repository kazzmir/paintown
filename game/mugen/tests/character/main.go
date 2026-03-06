package main

import (
	"fmt"
	"image/color"
	"log"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/character"
)

type Game struct {
	player  *character.Player
	camX    float64
	camY    float64
	punched bool
}

func (g *Game) Update() error {
	// Exit on ESC
	if ebiten.IsKeyPressed(ebiten.KeyEscape) {
		return ebiten.Termination
	}

	var activeCommands []string
	if ebiten.IsKeyPressed(ebiten.KeyX) {
		activeCommands = append(activeCommands, "x")
	}
	if ebiten.IsKeyPressed(ebiten.KeyA) {
		activeCommands = append(activeCommands, "a")
	}
	if len(activeCommands) > 0 {
		g.player.Commands.Add(activeCommands)
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

	// Draw player directly to the screen
	g.player.Draw(screen, g.camX, g.camY)

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
	return 640, 480
}

func main() {
	fmt.Println("Starting MUGEN Character Test...")
	// Load KFM
	baseDir := "data-new/mugen/chars/kfm/"
	p, err := character.LoadPlayer(baseDir, "kfm.def")
	if err != nil {
		log.Fatalf("failed to load player: %v", err)
	}
	fmt.Println("Player loaded successfully.")

	// Initial position
	p.Character.X = 320
	p.Character.Y = 400

	// Force initial state to Stand
	p.Character.ChangeState(0, -1, -1)
	fmt.Printf("Initial State: %d, Anim: %d\n", p.Character.GetStateNo(), p.Character.GetAnim())

	g := &Game{
		player: p,
		camX:   0,
		camY:   0,
	}

	ebiten.SetWindowSize(640, 480)
	ebiten.SetWindowTitle("MUGEN Character SM Test")
	fmt.Println("Running Ebiten game loop...")
	if err := ebiten.RunGame(g); err != nil {
		log.Fatal(err)
	}
}
