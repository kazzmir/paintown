package main

import (
	"fmt"
	"image/color"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/render"
)

type FightState struct {
	engine                             *Engine
	match                              *match.Match
	hud                                *FightHUD
	stageCanvas, gameCanvas, hudCanvas *ebiten.Image
	effectImages                       map[effectImageKey]effectImage
	effectWarnings                     map[string]bool
	linearSampler                      render.Linear
	renderError                        error
}

func NewFightState(engine *Engine, request match.Request) (*FightState, error) {
	if err := engine.ensureSound(); err != nil {
		return nil, err
	}
	game, err := match.New(request, match.Options{RootDir: engine.dataDir, MotifDir: filepath.Dir(engine.motif.Source), FightPath: engine.motif.Files.Fight,
		GameSpace: coord.Space{Width: engine.cfg.Config.GameWidth, Height: engine.cfg.Config.GameHeight}, RoundTime: engine.cfg.Options.Time, LifePercent: engine.cfg.Options.Life, SoundQueue: engine.sound.queue, MotifSpace: engine.motif.Info.LocalCoord, MaxEffects: engine.cfg.Config.SysExplodMax, SuperTargetDefenceMul: engine.cfg.Rules.SuperTargetDefenceMul, StageCrop: !engine.cfg.Video.StageFit, HelperMax: max(4, min(56, engine.cfg.Config.HelperMax))})
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
	if s.renderError != nil {
		return s, s.renderError
	}
	if input.GlobalManager.IsJustPressed(1, input.ActionEscape) {
		if err := s.match.ClearSounds(); err != nil {
			return s, err
		}
		s.match.Effects.Clear()
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
	if err := s.hud.Update(previousPhase, previousRound); err != nil {
		return s, err
	}
	return s, nil
}

func (s *FightState) Draw(screen *ebiten.Image) {
	m := s.match
	stageSpace, gameSpace := m.Stage.StageInfo.LocalCoord, m.Options.GameSpace
	motifSpace := s.engine.motif.Info.LocalCoord
	if s.stageCanvas == nil {
		s.stageCanvas = ebiten.NewImage(stageSpace.Width, stageSpace.Height)
		s.gameCanvas = ebiten.NewImage(gameSpace.Width, gameSpace.Height)
		s.hudCanvas = ebiten.NewImage(motifSpace.Width, motifSpace.Height)
	}
	toStage, _ := gameSpace.ScaleTo(stageSpace)
	stageToGame, _ := stageSpace.ScaleTo(gameSpace)
	cameraX, cameraY := m.CameraX*toStage, (m.CameraY-m.Viewport.CropY)*toStage
	s.stageCanvas.Fill(color.Black)
	m.Stage.Draw(s.stageCanvas, cameraX, cameraY)
	s.gameCanvas.Fill(color.Black)
	backgroundOptions := &ebiten.DrawImageOptions{Filter: ebiten.FilterLinear}
	tint := m.Timeline.SceneTint()
	backgroundOptions.ColorScale.Scale(tint[0], tint[1], tint[2], 1)
	backgroundOptions.GeoM.Scale(stageToGame, stageToGame)
	backgroundOptions.GeoM.Translate(0, -m.Viewport.CropY)
	s.gameCanvas.DrawImage(s.stageCanvas, backgroundOptions)
	// Render actors/effects in the common game surface so HD sprites are not
	// reduced to a low-resolution stage raster before the final viewport fit.
	s.drawEffects(s.gameCanvas, 0)
	character.DrawPlayers(s.gameCanvas, m.Players[:], gameSpace, m.CameraX, m.Stage.StageInfo.ZOffset*stageToGame-m.CameraY)
	s.drawEffects(s.gameCanvas, 1)
	s.stageCanvas.Clear()
	m.Stage.DrawForeground(s.stageCanvas, cameraX, cameraY)
	s.gameCanvas.DrawImage(s.stageCanvas, backgroundOptions)
	s.drawEffects(s.gameCanvas, 2)
	s.hudCanvas.Clear()
	s.hud.Draw(s.hudCanvas)
	screen.Fill(color.Black)
	target := coord.Space{Width: screen.Bounds().Dx(), Height: screen.Bounds().Dy()}
	drawFitted := func(surface *ebiten.Image, transform coord.Transform, err error) {
		if err != nil {
			return
		}
		options := &ebiten.DrawImageOptions{}
		if surface == s.hudCanvas {
			options.ColorScale.Scale(tint[0], tint[1], tint[2], 1)
		}
		options.GeoM.Scale(transform.ScaleX, transform.ScaleY)
		options.GeoM.Translate(transform.X, transform.Y)
		if err := s.linearSampler.Draw(screen, surface, options.GeoM, options.ColorScale); err != nil {
			s.renderError = fmt.Errorf("fight viewport sampler: %w", err)
		}
	}
	world, err := coord.Project(gameSpace, target, s.engine.cfg.Video.KeepAspect)
	drawFitted(s.gameCanvas, world, err)
	hud, err := coord.HUDProject(gameSpace, motifSpace, target, s.engine.cfg.Video.KeepAspect)
	drawFitted(s.hudCanvas, hud, err)
}
