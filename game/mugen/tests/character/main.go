package main

import (
	"flag"
	"fmt"
	"image/color"
	"log"
	"path/filepath"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/stage"
)

type Game struct {
	p1    *character.Player
	p2    *character.Player
	stage *stage.Stage
	camX  float64
	camY  float64

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

	// Update players with camera/screen info for boundaries
	g.p1.Character.CameraX = g.camX
	g.p1.Character.ScreenWidth = 320 // Logical width
	g.p2.Character.CameraX = g.camX
	g.p2.Character.ScreenWidth = 320

	g.p1.Update()
	g.p2.Update()

	// Rudimentary camera follow (center between players)
	g.camX = (g.p1.Character.X + g.p2.Character.X) / 2
	g.camY = (g.p1.Character.Y + g.p2.Character.Y) / 2

	// Clamp camera to stage bounds
	if g.stage != nil {
		if g.camX < g.stage.Camera.BoundLeft {
			g.camX = g.stage.Camera.BoundLeft
		}
		if g.camX > g.stage.Camera.BoundRight {
			g.camX = g.stage.Camera.BoundRight
		}
		if g.camY < g.stage.Camera.BoundHigh {
			g.camY = g.stage.Camera.BoundHigh
		}
		if g.camY > g.stage.Camera.BoundLow {
			g.camY = g.stage.Camera.BoundLow
		}
	}

	// Get raw inputs from the latest history for display
	raw := ""
	if len(g.p1.Character.Commands.History) > 0 {
		raw = strings.Join(g.p1.Character.Commands.History[len(g.p1.Character.Commands.History)-1], ",")
	}

	fmt.Printf("\rP1 State: %d | P2 State: %d | P1 Life: %d | P2 Life: %d | Cmd: %-20s | Raw: %s",
		g.p1.Character.GetStateNo(),
		g.p2.Character.GetStateNo(),
		g.p1.Character.Life,
		g.p2.Character.Life,
		g.p1.Character.LastCommandMatched,
		raw)

	return nil
}

func (g *Game) Draw(screen *ebiten.Image) {
	// Clear screen
	screen.Fill(color.Black)

	if g.stage != nil {
		g.stage.Draw(screen, g.camX, g.camY)
	}

	drawPlayer := func(p *character.Player) {
		screenX := 160 + p.Character.X - g.camX
		screenY := 0.0
		if g.stage != nil {
			screenY = g.stage.StageInfo.ZOffset
		}
		screenY = screenY + p.Character.Y - g.camY
		p.DrawScaled(screen, screenX, screenY, 1.0, 1.0)
	}

	drawPlayer(g.p1)
	drawPlayer(g.p2)

	// Debug visuals (draw boxes last so they are on top)
	drawDebug := func(p *character.Player) {
		screenY := 0.0
		if g.stage != nil {
			screenY = g.stage.StageInfo.ZOffset
		}
		screenY = screenY + p.Character.Y - g.camY

		el := p.Character.CurrentElement()
		if el != nil {
			sX, sY := p.Character.StateFile.Size.XScale, p.Character.StateFile.Size.YScale
			if sX == 0 {
				sX = 1
			}
			if sY == 0 {
				sY = 1
			}

			// Draw Clsn2 (Hurtboxes) in Green
			for _, b := range el.Clsn2 {
				x1, y1, x2, y2 := character.BoxToWorld(b, p.Character.X, p.Character.Y, p.Character.Facing, sX, sY)
				// y1-p.Character.Y is the local Y offset of the box from the character axis
				ebitenutil.DrawRect(screen, 160+x1-g.camX, screenY+(y1-p.Character.Y), x2-x1, y2-y1, color.RGBA{0, 255, 0, 180})
			}
			// Draw Clsn1 (Hitboxes) in Red
			for _, b := range el.Clsn1 {
				x1, y1, x2, y2 := character.BoxToWorld(b, p.Character.X, p.Character.Y, p.Character.Facing, sX, sY)
				ebitenutil.DrawRect(screen, 160+x1-g.camX, screenY+(y1-p.Character.Y), x2-x1, y2-y1, color.RGBA{255, 0, 0, 220})
			}
		}
	}

	drawDebug(g.p1)
	drawDebug(g.p2)

	if g.stage != nil {
		g.stage.DrawForeground(screen, g.camX, g.camY)
	}

	msg := fmt.Sprintf("P1 State: %d Life: %d\nP2 State: %d Life: %d\nCam: (%.1f, %.1f)",
		g.p1.Character.GetStateNo(), g.p1.Character.Life,
		g.p2.Character.GetStateNo(), g.p2.Character.Life,
		g.camX, g.camY)
	ebitenutil.DebugPrint(screen, msg)
}

func (g *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 320, 240
}

func main() {
	fmt.Println("Starting MUGEN Character Combat Test...")

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

	// Load KFM for both players
	baseDir := filepath.Join(root, "chars/kfm")
	p1, err := character.LoadPlayer(baseDir, "kfm.def")
	if err != nil {
		log.Fatalf("failed to load player 1 from %s: %v", baseDir, err)
	}
	p2, err := character.LoadPlayer(baseDir, "kfm.def")
	if err != nil {
		log.Fatalf("failed to load player 2 from %s: %v", baseDir, err)
	}
	fmt.Println("Players loaded successfully.")

	// Initial positions from stage
	p1.Character.StartX = s.PlayerInfo.P1StartX
	p1.Character.StartY = s.PlayerInfo.P1StartY
	p1.Character.StartZ = s.PlayerInfo.P1StartZ
	p1.Character.StartFacing = s.PlayerInfo.P1Facing

	p1.Character.X = p1.Character.StartX
	p1.Character.Y = p1.Character.StartY
	p1.Character.Z = p1.Character.StartZ
	p1.Character.Facing = p1.Character.StartFacing
	p1.Character.PlayerID = 1
	p1.Character.Target = []*character.Character{p2.Character}

	// Apply boundaries (stage-defined)
	p1.Character.LeftBound = s.PlayerInfo.LeftBound
	p1.Character.RightBound = s.PlayerInfo.RightBound
	p1.Character.TopBound = s.PlayerInfo.TopBound
	p1.Character.BotBound = s.PlayerInfo.BotBound

	p2.Character.StartX = s.PlayerInfo.P2StartX
	p2.Character.StartY = s.PlayerInfo.P2StartY
	p2.Character.StartZ = s.PlayerInfo.P2StartZ
	p2.Character.StartFacing = s.PlayerInfo.P2Facing
	// Default P2 to face left (-1) toward P1 if stage doesn't specify
	if p2.Character.StartFacing == 0 {
		p2.Character.StartFacing = -1
	}

	p2.Character.X = p2.Character.StartX
	p2.Character.Y = p2.Character.StartY
	p2.Character.Z = p2.Character.StartZ
	p2.Character.Facing = p2.Character.StartFacing
	p2.Character.PlayerID = 2
	p2.Character.Target = []*character.Character{p1.Character}

	p2.Character.LeftBound = s.PlayerInfo.LeftBound
	p2.Character.RightBound = s.PlayerInfo.RightBound
	p2.Character.TopBound = s.PlayerInfo.TopBound
	p2.Character.BotBound = s.PlayerInfo.BotBound

	p1.Character.ChangeState(0, -1, -1)
	p2.Character.ChangeState(0, -1, -1)

	width := cfg.Video.Width
	if width == 0 {
		width = 640
	}
	height := cfg.Video.Height
	if height == 0 {
		height = 480
	}

	gameInst := &Game{
		p1:           p1,
		p2:           p2,
		stage:        s,
		camX:         0,
		camY:         0,
		windowWidth:  width,
		windowHeight: height,
	}

	ebiten.SetWindowSize(width, height)
	ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)
	ebiten.SetWindowTitle("MUGEN Character Combat Test")
	if err := ebiten.RunGame(gameInst); err != nil {
		log.Fatal(err)
	}
}
