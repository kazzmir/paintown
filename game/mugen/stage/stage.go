package stage

import (
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

type Stage struct {
	Name    string
	Source  string
	DebugBG bool

	Camera struct {
		StartX                    float64
		StartY                    float64
		BoundLeft                 float64
		BoundRight                float64
		BoundHigh                 float64
		BoundLow                  float64
		VerticalFollow            float64
		FloorTension              float64
		Tension                   float64
		CutHigh, CutLow           float64
		OverdrawHigh, OverdrawLow float64
	}

	PlayerInfo struct {
		P1StartX   float64
		P1StartY   float64
		P1StartZ   float64
		P1Facing   int
		P2StartX   float64
		P2StartY   float64
		P2StartZ   float64
		P2Facing   int
		LeftBound  float64
		RightBound float64
		TopBound   float64
		BotBound   float64
	}

	Bound struct {
		ScreenLeft  float64
		ScreenRight float64
	}

	StageInfo struct {
		LocalCoord coord.Space
		ZOffset    float64
		AutoTurn   bool
		ResetBG    bool
	}

	Shadow struct {
		Intensity int
		Color     [3]int
		YScale    float64
		FadeRange [2]float64
	}

	Reflection struct {
		Intensity int
	}

	Background   background.Background
	SpriteData   *sff.SFF
	SpriteImages map[string]*background.SpriteImages
}

func (s *Stage) Update() {
	s.Background.Update()
}

func (s *Stage) Draw(screen *ebiten.Image, cameraX, cameraY float64) {
	if s.DebugBG {
		screen.Fill(color.RGBA{R: 255, B: 255, A: 255})
	}
	s.Background.DrawLayer(screen, 0, cameraX, cameraY, s.StageInfo.ZOffset, s.SpriteImages)
}

func (s *Stage) DrawForeground(screen *ebiten.Image, cameraX, cameraY float64) {
	s.Background.DrawLayer(screen, 1, cameraX, cameraY, s.StageInfo.ZOffset, s.SpriteImages)
}
