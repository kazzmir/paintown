package main

import (
	"flag"
	"fmt"
	"image/color"
	"log"
	"os"
	"path/filepath"
	"sort"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/hajimehoshi/ebiten/v2/inpututil"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/stage"
)

type AppState int

const (
	StateMenu AppState = iota
	StateViewer
)

type Game struct {
	state AppState

	// Menu state
	dataDir       string
	stages        []string
	selectedIndex int

	// Viewer state
	currentStage *stage.Stage
	camX         float64
	camY         float64

	windowWidth  int
	windowHeight int
}

func (g *Game) Update() error {
	// Global escape check
	if inpututil.IsKeyJustPressed(ebiten.KeyEscape) {
		if g.state == StateViewer {
			g.state = StateMenu
			g.currentStage = nil
			return nil
		}
		return ebiten.Termination
	}

	switch g.state {
	case StateMenu:
		g.updateMenu()
	case StateViewer:
		g.updateViewer()
	}

	return nil
}

func (g *Game) updateMenu() {
	if inpututil.IsKeyJustPressed(ebiten.KeyDown) {
		g.selectedIndex = (g.selectedIndex + 1) % len(g.stages)
	}
	if inpututil.IsKeyJustPressed(ebiten.KeyUp) {
		g.selectedIndex = (g.selectedIndex - 1 + len(g.stages)) % len(g.stages)
	}
	if inpututil.IsKeyJustPressed(ebiten.KeyEnter) {
		g.loadSelectedStage()
	}
}

func (g *Game) loadSelectedStage() {
	sName := g.stages[g.selectedIndex]
	s, err := stage.LoadStage(filepath.Join("stages", sName), g.dataDir)
	if err != nil {
		log.Printf("Error loading stage %s: %v", sName, err)
		return
	}
	g.currentStage = s
	g.camX = s.Camera.StartX
	g.camY = s.Camera.StartY
	g.state = StateViewer
}

func (g *Game) updateViewer() {
	if g.currentStage == nil {
		return
	}

	g.currentStage.Update()

	speed := 2.0
	if ebiten.IsKeyPressed(ebiten.KeyShift) {
		speed = 10.0
	}

	if ebiten.IsKeyPressed(ebiten.KeyArrowLeft) {
		g.camX -= speed
	}
	if ebiten.IsKeyPressed(ebiten.KeyArrowRight) {
		g.camX += speed
	}
	if ebiten.IsKeyPressed(ebiten.KeyArrowUp) {
		g.camY -= speed
	}
	if ebiten.IsKeyPressed(ebiten.KeyArrowDown) {
		g.camY += speed
	}

	// Constrain camera
	if g.camX < g.currentStage.Camera.BoundLeft {
		g.camX = g.currentStage.Camera.BoundLeft
	}
	if g.camX > g.currentStage.Camera.BoundRight {
		g.camX = g.currentStage.Camera.BoundRight
	}
	if g.camY < g.currentStage.Camera.BoundHigh {
		g.camY = g.currentStage.Camera.BoundHigh
	}
	if g.camY > g.currentStage.Camera.BoundLow {
		g.camY = g.currentStage.Camera.BoundLow
	}
}

func (g *Game) Draw(screen *ebiten.Image) {
	screen.Fill(color.Black)

	switch g.state {
	case StateMenu:
		g.drawMenu(screen)
	case StateViewer:
		g.drawViewer(screen)
	}
}

func (g *Game) drawMenu(screen *ebiten.Image) {
	ebitenutil.DebugPrint(screen, "MUGEN Independent Stage Test\n\nSelect a stage:\n(Use Arrows to navigate, Enter to select, Esc to quit)\n\n")

	y := 80
	for i, s := range g.stages {
		prefix := "  "
		if i == g.selectedIndex {
			prefix = "> "
		}
		ebitenutil.DebugPrintAt(screen, prefix+s, 20, y)
		y += 15
	}
}

func (g *Game) drawViewer(screen *ebiten.Image) {
	if g.currentStage == nil {
		return
	}

	g.currentStage.Draw(screen, g.camX, g.camY)
	g.currentStage.DrawForeground(screen, g.camX, g.camY)

	msg := fmt.Sprintf("Stage: %s\nCam: (%.1f, %.1f)\nBounds: L:%.1f R:%.1f H:%.1f L:%.1f\nEsc to Menu",
		g.currentStage.Name, g.camX, g.camY,
		g.currentStage.Camera.BoundLeft, g.currentStage.Camera.BoundRight,
		g.currentStage.Camera.BoundHigh, g.currentStage.Camera.BoundLow)
	ebitenutil.DebugPrint(screen, msg)
}

func (g *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 320, 240
}

func main() {
	dataDirFlag := flag.String("data-dir", "data-new/mugen", "Path to MUGEN data directory")
	flag.Parse()

	dataDir := *dataDirFlag
	// Normalize dataDir
	if filepath.Base(dataDir) == "data" {
		dataDir = filepath.Dir(dataDir)
	}

	// Scan stages
	stagesDir := filepath.Join(dataDir, "stages")
	entries, err := os.ReadDir(stagesDir)
	if err != nil {
		log.Fatalf("Failed to read stages directory: %v", err)
	}

	var stages []string
	for _, entry := range entries {
		if !entry.IsDir() && strings.HasSuffix(strings.ToLower(entry.Name()), ".def") {
			stages = append(stages, entry.Name())
		}
	}
	sort.Strings(stages)

	if len(stages) == 0 {
		log.Fatalf("No .def files found in %s", stagesDir)
	}

	// Set up input and config
	cfg, _ := config.LoadConfig(dataDir)
	input.GlobalManager = input.NewInputManager(cfg)

	width := 640
	height := 480
	if cfg.Video.Width != 0 {
		width = cfg.Video.Width
		height = cfg.Video.Height
	}

	game := &Game{
		state:        StateMenu,
		dataDir:      dataDir,
		stages:       stages,
		windowWidth:  width,
		windowHeight: height,
	}

	ebiten.SetWindowSize(width, height)
	ebiten.SetWindowTitle("MUGEN Stage Test")
	ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)
	if err := ebiten.RunGame(game); err != nil {
		log.Fatal(err)
	}
}
