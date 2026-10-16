package main

import (
	"image/color"
	"math"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
)

type FightState struct {
	engine                             *Engine
	match                              *match.Match
	hud                                *FightHUD
	stageCanvas, gameCanvas, hudCanvas *ebiten.Image
}

func NewFightState(engine *Engine, request match.Request) (*FightState, error) {
	game, err := match.New(request, match.Options{RootDir: engine.dataDir, MotifDir: filepath.Dir(engine.motif.Source), FightPath: engine.motif.Files.Fight,
		GameSpace: coord.Space{Width: engine.cfg.Config.GameWidth, Height: engine.cfg.Config.GameHeight}, RoundTime: engine.cfg.Options.Time, LifePercent: engine.cfg.Options.Life})
	if err != nil {
		return nil, err
	}
	s := &FightState{engine: engine, match: game}
	for slot, p := range game.Players {
		p.Character.InputSource = func(playerID, facing int) []string {
			if game.Request.Players[playerID-1].AILevel > 0 {
				return game.BasicAIInput(playerID)
			}
			return character.GetRawInputs(playerID, facing)
		}
		// Keep the adapter on reset characters as well.
		if slot == 0 {
			game.Options.Input = p.Character.InputSource
		}
	}
	s.hud = NewFightHUD(game, engine)
	return s, nil
}

func (s *FightState) Update() (State, error) {
	if input.GlobalManager.IsJustPressed(1, input.ActionEscape) {
		return NewTitleState(s.engine), nil
	}
	if s.match.Phase == match.Finished && input.GlobalManager.IsJustPressed(1, input.ActionEnter) {
		return NewFightState(s.engine, s.match.Request)
	}
	previousPhase := s.match.Phase
	previousRound := s.match.Round
	if err := s.match.Update(); err != nil {
		return s, err
	}
	s.hud.Update(previousPhase, previousRound)
	return s, nil
}

func (s *FightState) Draw(screen *ebiten.Image) {
	m := s.match
	stageSpace := m.Stage.StageInfo.LocalCoord
	gameSpace := m.Options.GameSpace
	if s.stageCanvas == nil {
		s.stageCanvas = ebiten.NewImage(stageSpace.Width, stageSpace.Height)
		s.gameCanvas = ebiten.NewImage(gameSpace.Width, gameSpace.Height)
		s.hudCanvas = ebiten.NewImage(s.engine.motif.Info.LocalCoord.Width, s.engine.motif.Info.LocalCoord.Height)
	}
	s.stageCanvas.Fill(color.Black)
	toStage, _ := gameSpace.ScaleTo(stageSpace)
	cameraX, cameraY := m.CameraX*toStage, m.CameraY*toStage
	m.Stage.Draw(s.stageCanvas, cameraX, cameraY)
	for _, p := range m.Players {
		scale, _ := p.LocalCoord.ScaleTo(stageSpace)
		c := p.Character
		p.DrawScaled(s.stageCanvas, float64(stageSpace.Width)/2+c.X*scale-cameraX, m.Stage.StageInfo.ZOffset+c.Y*scale-cameraY, scale, scale)
	}
	m.Stage.DrawForeground(s.stageCanvas, cameraX, cameraY)
	s.gameCanvas.Fill(color.Black)
	scale, _ := stageSpace.ScaleTo(gameSpace)
	// Cut suggestions distribute the aspect-ratio difference. Stock KFM's
	// 35/25 split maps its floor to y=660 in the supplied 1280x720 profile.
	difference := float64(stageSpace.Height) - float64(gameSpace.Height)/scale
	top := difference / 2
	if difference > 0 && m.Stage.Camera.CutHigh+m.Stage.Camera.CutLow > 0 {
		top = difference * m.Stage.Camera.CutHigh / (m.Stage.Camera.CutHigh + m.Stage.Camera.CutLow)
	} else if difference < 0 && m.Stage.Camera.OverdrawHigh+m.Stage.Camera.OverdrawLow > 0 {
		top = difference * m.Stage.Camera.OverdrawHigh / (m.Stage.Camera.OverdrawHigh + m.Stage.Camera.OverdrawLow)
	}
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Scale(scale, scale)
	op.GeoM.Translate(0, -top*scale)
	s.gameCanvas.DrawImage(s.stageCanvas, op)
	s.hudCanvas.Clear()
	s.hud.Draw(s.hudCanvas)
	hudScale := float64(gameSpace.Width) / float64(s.hudCanvas.Bounds().Dx())
	op = &ebiten.DrawImageOptions{}
	op.GeoM.Scale(hudScale, hudScale)
	s.gameCanvas.DrawImage(s.hudCanvas, op)
	screen.Fill(color.Black)
	fit := math.Min(float64(screen.Bounds().Dx())/float64(gameSpace.Width), float64(screen.Bounds().Dy())/float64(gameSpace.Height))
	op = &ebiten.DrawImageOptions{}
	op.GeoM.Scale(fit, fit)
	op.GeoM.Translate((float64(screen.Bounds().Dx())-float64(gameSpace.Width)*fit)/2, (float64(screen.Bounds().Dy())-float64(gameSpace.Height)*fit)/2)
	screen.DrawImage(s.gameCanvas, op)
}
