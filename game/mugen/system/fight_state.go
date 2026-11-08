package main

import (
	"fmt"
	"image/color"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/combat"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/render"
)

type FightState struct {
	engine                             *Engine
	match                              *match.Match
	hud                                *FightHUD
	stageCanvas, gameCanvas, hudCanvas *ebiten.Image
	environmentColorImage              *ebiten.Image
	viewportBackgroundImage            *ebiten.Image
	effectImages                       map[effectImageKey]effectImage
	effectUsed                         map[effectImageKey]bool
	effectWarnings                     map[string]bool
	linearSampler                      render.Linear
	indexSampler                       render.Nearest
	renderError                        error
	debugSelection                     int
	debugFont                          *font.Font
}

func NewFightState(engine *Engine, request match.Request) (*FightState, error) {
	// Resolve packed selections once at the presentation boundary. Match,
	// round reset, helpers and storyboards then use ordinary immutable files.
	resolve := func(selection *match.Selection) error {
		if selection.DefPath == "" {
			return nil
		}
		path, err := engine.characterPacks.Resolve(selection.DefPath, engine.dataDir, filepath.Join(engine.dataDir, "chars"))
		if err != nil {
			return fmt.Errorf("character %q: %w", selection.DefPath, err)
		}
		selection.DefPath = path
		return nil
	}
	for side := range request.Players {
		if err := resolve(&request.Players[side]); err != nil {
			return nil, err
		}
		// A request copy must not rewrite a campaign/rematch template's slice.
		request.Teams[side].Members = append([]match.Selection(nil), request.Teams[side].Members...)
		for member := range request.Teams[side].Members {
			if err := resolve(&request.Teams[side].Members[member]); err != nil {
				return nil, err
			}
		}
	}
	if err := engine.ensureSound(); err != nil {
		return nil, err
	}
	seed := int64(0)
	if request.RandomState == nil {
		seed = engine.nextMatchSeed()
	}
	game, err := match.New(request, match.Options{RootDir: engine.dataDir, MotifDir: filepath.Dir(engine.motif.Source), FightPath: engine.motif.Files.Fight,
		RosterInput: character.GetRawInputs,
		GameTime:    engine.gameTime, TicksPerSecond: int32(engine.ticksPerSecond()),
		TeamLoseOnKO: engine.cfg.Options.TeamLoseOnKO, Team1Vs2Life: engine.cfg.Options.Team1Vs2Life,
		GameSpace: coord.Space{Width: engine.cfg.Config.GameWidth, Height: engine.cfg.Config.GameHeight}, RoundTime: engine.cfg.Options.Time, LifePercent: engine.cfg.Options.Life, SoundQueue: engine.sound.queue, MotifSpace: engine.motif.Info.LocalCoord, ExplodMax: max(8, engine.cfg.Config.ExplodMax), SuperTargetDefenceMul: engine.cfg.Rules.SuperTargetDefenceMul, StageCrop: !engine.cfg.Video.StageFit, HelperMax: max(4, min(56, engine.cfg.Config.HelperMax)),
		PlayerProjectileMax: max(5, engine.cfg.Config.PlayerProjectileMax), AfterImageMax: max(1, engine.cfg.Config.AfterImageMax), RandomSeed: seed, PowerRules: &combat.PowerRules{Attack: engine.cfg.Rules.DefaultAttackLifeToPowerMul, GetHit: engine.cfg.Rules.DefaultGetHitLifeToPowerMul}})
	if err != nil {
		return nil, err
	}
	fmt.Printf("Match RNG seed: %d\n", game.World.RandomSeed())
	s := &FightState{engine: engine, match: game, debugSelection: -1}
	if engine.cfg.Debug.Debug {
		s.debugSelection = 0
	}
	for slot, p := range game.Actors() {
		p.Character.InputSource = func(playerID, facing int) []string {
			selected, _ := game.SelectionForPlayer(playerID)
			if selected.AILevel > 0 {
				return game.BasicAIInput(playerID)
			}
			if playerID == 3 && (game.Request.Mode == "Team Co-op" || game.Request.Mode == "Survival Co-op") {
				playerID = 2
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
	s.updateDebugSelection()
	if input.GlobalManager.IsJustPressed(1, input.ActionEscape) {
		if err := s.match.ClearSounds(); err != nil {
			return s, err
		}
		s.match.Effects.Clear()
		return NewTitleState(s.engine), nil
	}
	if s.match.Phase == match.Finished && s.engine.modeRun != nil && s.match.Request.Campaign == s.engine.modeRun.id {
		return s.engine.modeRun.result(s)
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
	s.beginEffectDraw()
	defer s.endEffectDraw()
	stageSpace, gameSpace := m.Stage.StageInfo.LocalCoord, m.Options.GameSpace
	motifSpace := s.engine.motif.Info.LocalCoord
	if s.stageCanvas == nil {
		s.stageCanvas = ebiten.NewImage(gameSpace.Width, gameSpace.Height)
		s.gameCanvas = ebiten.NewImage(gameSpace.Width, gameSpace.Height)
		s.hudCanvas = ebiten.NewImage(motifSpace.Width, motifSpace.Height)
	}
	toStage, _ := gameSpace.ScaleTo(stageSpace)
	stageToGame, _ := stageSpace.ScaleTo(gameSpace)
	cameraX, cameraY := m.CameraX*toStage, (m.CameraY-m.Viewport.CropY)*toStage
	projection := background.Projection{Space: stageSpace, Transform: coord.Transform{
		ScaleX: stageToGame, ScaleY: stageToGame, Y: -m.Viewport.CropY,
	}}
	m.Stage.Background.CameraZoom = m.Viewport.ZoomScale()
	allPalette := m.Timeline.AllPalette.Transform()
	backgroundPalette := palette.Combine(m.Timeline.BackgroundPalette.Transform(), allPalette)
	environment := m.Timeline.EnvironmentColor
	environmentActive := environment.Time != 0
	tint := m.Timeline.SceneTint()
	s.stageCanvas.Fill(allPalette.ClearColor(color.Black))
	if !environmentActive && !m.Timeline.NoBackground {
		if err := m.Stage.DrawWithPaletteTint(s.stageCanvas, cameraX, cameraY, backgroundPalette, allPalette, tint, projection); err != nil {
			s.renderError = err
			return
		}
	}
	s.gameCanvas.Fill(allPalette.ClearColor(color.Black))
	// Backgrounds rasterize directly at game resolution, retaining fractional
	// camera projection without an intermediate stage-pixel rounding step.
	backgroundOptions := &ebiten.DrawImageOptions{Filter: ebiten.FilterNearest}
	if environmentActive {
		// Negative durations suppress the stage but draw no color quad.
		s.gameCanvas.Fill(color.Black)
	} else {
		s.gameCanvas.DrawImage(s.stageCanvas, backgroundOptions)
	}
	drawEnvironment := func() {
		if s.environmentColorImage == nil {
			s.environmentColorImage = ebiten.NewImage(1, 1)
		}
		s.environmentColorImage.Fill(environment.Color)
		options := &ebiten.DrawImageOptions{}
		options.GeoM.Scale(float64(gameSpace.Width), float64(gameSpace.Height))
		if err := s.linearSampler.DrawStyledImage(s.gameCanvas, s.environmentColorImage, options, render.DrawStyle{DestinationFactor: -1, Palette: allPalette}); err != nil {
			s.renderError = fmt.Errorf("EnvColor sampler: %w", err)
		}
	}
	if !m.Timeline.GlobalNoShadow {
		if m.Stage.Reflection.Intensity > 0 {
			reflection := render.Shadow{Reflection: true, Intensity: m.Stage.Reflection.Intensity, YScale: 1, FloorY: m.Stage.GroundLevel()*stageToGame - m.CameraY}
			character.DrawPlayerShadows(s.gameCanvas, m.Actors(), gameSpace, m.CameraX, reflection)
			s.drawEffectShadows(s.gameCanvas, reflection)
		}
		shadow := render.Shadow{Intensity: m.Stage.Shadow.Intensity, YScale: m.Stage.Shadow.YScale, XShear: m.Stage.Shadow.XShear, FadeRange: m.Stage.Shadow.FadeRange, FloorY: m.Stage.GroundLevel()*stageToGame - m.CameraY}
		character.DrawPlayerShadows(s.gameCanvas, m.Actors(), gameSpace, m.CameraX, shadow)
		s.drawEffectShadows(s.gameCanvas, shadow)
	}
	if environment.Time > 0 && environment.Under {
		drawEnvironment()
	}
	// Render actors/effects in the common game surface so HD sprites are not
	// reduced to a low-resolution stage raster before the final viewport fit.
	s.drawEffects(s.gameCanvas, 0)
	character.DrawPlayersWithSprites(s.gameCanvas, m.Actors(), gameSpace, m.CameraX, m.Stage.GroundLevel()*stageToGame-m.CameraY, s.ownedEffectLayers(s.gameCanvas))
	frontEnvironment := environment.Time > 0 && !environment.Under
	if !frontEnvironment {
		s.drawEffects(s.gameCanvas, 1)
	}
	// Stage foreground still draws for negative durations. A positive front
	// quad covers it; an under quad leaves it visible above the actors.
	s.stageCanvas.Clear()
	if !m.Timeline.NoForeground {
		if err := m.Stage.DrawForegroundWithPaletteTint(s.stageCanvas, cameraX, cameraY, backgroundPalette, tint, projection); err != nil {
			s.renderError = err
			return
		}
	}
	s.gameCanvas.DrawImage(s.stageCanvas, backgroundOptions)
	if frontEnvironment {
		drawEnvironment()
		s.drawEffects(s.gameCanvas, 1)
	}
	s.drawEffects(s.gameCanvas, 2)
	s.hudCanvas.Clear()
	// Consume HUD drawing even while covered, keeping lazy animation clocks
	// and resource initialization independent of EnvColor visibility.
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
		// The beta applies AllPalFX again after scene blending/clamping.
		if err := s.linearSampler.DrawStyledImage(screen, surface, options, render.DrawStyle{DestinationFactor: -1, Palette: allPalette}); err != nil {
			s.renderError = fmt.Errorf("fight viewport sampler: %w", err)
		}
	}
	world, err := coord.Project(gameSpace, target, s.engine.cfg.Video.KeepAspect)
	shake := m.Timeline.ShakeOffset()
	if shake != 0 && (environment.Time > 0 || allPalette.Enabled) {
		if s.viewportBackgroundImage == nil {
			s.viewportBackgroundImage = ebiten.NewImage(1, 1)
		}
		if environment.Time > 0 {
			// The fixed EnvColor backdrop uses the same first GPU pass and
			// framebuffer clamp as its quad in the moving world surface.
			s.viewportBackgroundImage.Clear()
			if err := s.linearSampler.DrawStyledImage(s.viewportBackgroundImage, s.environmentColorImage, &ebiten.DrawImageOptions{}, render.DrawStyle{DestinationFactor: -1, Palette: allPalette}); err != nil {
				s.renderError = fmt.Errorf("EnvColor viewport backdrop: %w", err)
				return
			}
		} else {
			// Ordinary shake exposes raw black; AllPalFX applies its final
			// viewport pass to those gaps as well as to the moving content.
			s.viewportBackgroundImage.Fill(color.Black)
		}
		backdrop := world
		backdrop.ScaleX *= float64(gameSpace.Width)
		backdrop.ScaleY *= float64(gameSpace.Height)
		drawFitted(s.viewportBackgroundImage, backdrop, err)
	}
	world.Y += shake * float64(gameSpace.Width) / 320 * world.ScaleY
	drawFitted(s.gameCanvas, world, err)
	hud, err := coord.HUDProject(gameSpace, motifSpace, target, s.engine.cfg.Video.KeepAspect)
	// Native positive EnvColor covers the HUD with either under flag.
	if environment.Time <= 0 {
		drawFitted(s.hudCanvas, hud, err)
	}
	if err := s.drawDebugClipboard(screen); err != nil {
		s.renderError = err
	}
}
