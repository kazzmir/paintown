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
	"github.com/kazzmir/paintown/game/mugen/stage"
)

type Game struct {
	player *character.Player
	stage  *stage.Stage
	camX   float64
	camY   float64

	windowWidth  int
	windowHeight int
}

func (g *Game) Update() error {
	input.GlobalManager.Update()

	if input.GlobalManager.IsJustPressed(1, input.ActionEscape) {
		fmt.Println("\nEscape pressed, terminating test.")
		return ebiten.Termination
	}

	// Update stage (animations)
	if g.stage != nil {
		g.stage.Update()
	}

	// Update player
	g.player.Update()

	// Rudimentary camera follow
	g.camX = g.player.Character.X

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
	// Clear screen
	screen.Fill(color.Black)

	if g.stage != nil {
		g.stage.Draw(screen, g.camX, g.camY)
	}

	// Draw player (Y is relative to stage floor)
	screenX := 160 + g.player.Character.X - g.camX
	screenY := 0.0
	if g.stage != nil {
		screenY = g.stage.StageInfo.ZOffset
	}
	screenY = screenY - g.player.Character.Y - g.camY

	g.player.DrawScaled(screen, screenX, screenY, 1.0, 1.0)

	if g.stage != nil {
		g.stage.DrawForeground(screen, g.camX, g.camY)
	}

	msg := fmt.Sprintf("State: %d\nTime: %d\nAnim: %d\nAnimTime: %d\nPos: (%.2f, %.2f)\nCam: (%.1f, %.1f)",
		g.player.Character.GetStateNo(),
		g.player.Character.GetTime(),
		g.player.Character.GetAnim(),
		g.player.Character.GetAnimTime(),
		g.player.Character.X,
		g.player.Character.Y,
		g.camX, g.camY)
	ebitenutil.DebugPrint(screen, msg)
}

func (g *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 320, 240
}

func main() {
	fmt.Println("Starting MUGEN Character Test...")

	dataDirFlag := flag.String("data-dir", "data-new/mugen", "Path to MUGEN data directory")
	stageFlag := flag.String("stage", "stages/kfm.def", "Path to stage definition (relative to data dir)")
	flag.Parse()

	root := *dataDirFlag
	if filepath.Base(root) == "data" {
		root = filepath.Dir(root)
	}
	fmt.Printf("MUGEN Root identified as: %s\n", root)

	cfg, err := config.LoadConfig(root)
	if err != nil {
		fmt.Printf("Warning: failed to load mugen.cfg, using defaults: %v\n", err)
		cfg = &config.MugenConfig{}
	}
	input.GlobalManager = input.NewInputManager(cfg)

	// Load Stage
	s, err := stage.LoadStage(*stageFlag, root)
	if err != nil {
		fmt.Printf("Warning: failed to load stage %s: %v\n", *stageFlag, err)
	} else {
		fmt.Printf("Stage '%s' loaded successfully.\n", s.Name)
	}

	// Load KFM
	baseDir := filepath.Join(root, "chars/kfm")
	p, err := character.LoadPlayer(baseDir, "kfm.def")
	if err != nil {
		log.Fatalf("failed to load player from %s: %v", baseDir, err)
	}
	fmt.Println("Player loaded successfully.")

	// Initial position
	p.Character.X = 0
	p.Character.Y = 0

	p.Character.ChangeState(0, -1, -1)

	width := cfg.Video.Width
	if width == 0 {
		width = 640
	}
	height := cfg.Video.Height
	if height == 0 {
		height = 480
	}

	gameInst := &Game{
		player:       p,
		stage:        s,
		camX:         0,
		camY:         0,
		windowWidth:  width,
		windowHeight: height,
	}

	ebiten.SetWindowSize(width, height)
	ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)
	ebiten.SetWindowTitle("MUGEN Character Stage Test")
	if err := ebiten.RunGame(gameInst); err != nil {
		log.Fatal(err)
	}
}
