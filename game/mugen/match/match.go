// Package match owns the production one-on-one match lifecycle. Presentation
// adapters and scripted tests advance the same instance one tick at a time.
package match

import (
	"fmt"
	"math"
	"path/filepath"
	"time"

	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/combat"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/fight"
	"github.com/kazzmir/paintown/game/mugen/sound"
	"github.com/kazzmir/paintown/game/mugen/stage"
)

type Selection struct {
	DefPath string
	Name    string
	Palette int
	AILevel int
}

type Request struct {
	Players   [2]Selection
	StagePath string
	Mode      string
	Music     string
	Teams     [2]TeamSelection
	MatchNo   int
	Campaign  uint64
}

type Options struct {
	RootDir, MotifDir, FightPath string
	GameSpace                    coord.Space
	RoundTime, LifePercent       int
	TeamLoseOnKO                 bool
	Team1Vs2Life                 int
	Input                        func(playerID, facing int) []string
	SoundQueue                   *sound.Queue
	MotifSpace                   coord.Space
	MaxEffects                   int
	SuperTargetDefenceMul        float64
	StageCrop                    bool // false preserves the default StageFit=1
	HelperMax                    int
	PowerRules                   *combat.PowerRules
	RandomSeed                   int64 // zero chooses a fresh session-derived seed
	PlayerProjectileMax          int   // zero uses the default pool of 32
	AfterImageMax                int   // zero uses the default global pool of 16
	ExplodMax                    int   // zero uses MaxEffects, then the default global pool of 256
}

type Phase int

const (
	PreIntro Phase = iota
	Intro
	RoundCall
	FightCall
	Active
	PreOver
	Over
	Finished
)

type Match struct {
	Request                 Request
	Options                 Options
	Players                 [2]*character.Player
	Teams                   [2]Team
	Stage                   *stage.Stage
	Fight                   *fight.Definition
	Phase                   Phase
	Announcements           AnnouncementTimes
	SimulationSkipped       bool // KO skips and the reset initialization tick
	PhaseTime, Tick, Round  int
	Wins                    [2]int
	Draws                   int // draw rounds, including draws converted to wins
	Winner                  int // 0/1, or -1 for a draw
	TimeOver                bool
	TimeLeft                int     // counts; -1 means unlimited
	CameraX, CameraY        float64 // common game-space units
	SoundQueue              *sound.Queue
	Effects                 *effect.Scene
	Viewport                *coord.Viewport
	Timeline                *engine.Timeline
	World                   *character.World
	commonEffects           effect.Source
	timeTicks               int
	matchDone               bool
	resultCommitted         bool
	roundEndDeadline        int
	cameraGoalX             float64
	cameraLeft, cameraRight float32
	introClearTicks         int
	winPoseStarted          bool
	initializingRound       bool
	pendingRoundOver        bool
	slowdown                engine.RoundSlowdown
	aiBrains                map[int32]*aiBrain
	elapsedFightTicks       int
	startingLife            [2]float32 // mean fraction before this round's CNS
}

func New(request Request, options Options) (*Match, error) {
	var err error
	request, err = normalizeTeams(request)
	if err != nil {
		return nil, err
	}
	if request.Mode == "Training" {
		options.RoundTime = -1
	}

	if _, err := options.GameSpace.ScaleTo(options.GameSpace); err != nil {
		return nil, err
	}
	if options.RoundTime < -1 || options.LifePercent <= 0 {
		return nil, fmt.Errorf("invalid match time/life options")
	}
	m := &Match{Request: request, Options: options, Round: 1, Winner: -1, Timeline: &engine.Timeline{}}
	helperMax := options.HelperMax
	if helperMax == 0 {
		helperMax = 56
	}
	m.World = character.NewWorld(helperMax)
	limit := options.ExplodMax
	if limit == 0 {
		limit = options.MaxEffects
	}
	if limit == 0 {
		limit = 256
	}
	options.ExplodMax = max(8, limit)
	m.World.ExplodMax = options.ExplodMax
	if options.AfterImageMax != 0 {
		m.World.AfterImageMax = max(1, options.AfterImageMax)
	}
	if options.PlayerProjectileMax != 0 {
		m.World.ProjectileMax = max(5, options.PlayerProjectileMax)
	}
	seed := options.RandomSeed
	if seed == 0 {
		seed = engine.NewRandomStream(time.Now().Unix()).Next()
	}
	m.World.SeedRandom(seed)
	m.World.AfterEntityUpdate = m.updateHorizontalCamera
	if options.SuperTargetDefenceMul <= 0 {
		options.SuperTargetDefenceMul = 1.5
		m.Options = options
	}
	m.SoundQueue = options.SoundQueue
	if m.SoundQueue == nil {
		m.SoundQueue = &sound.Queue{}
	}
	m.Stage, err = stage.LoadStage(request.StagePath, options.RootDir)
	if err != nil {
		return nil, fmt.Errorf("match stage: %w", err)
	}
	m.Viewport, err = coord.StageViewport(options.GameSpace, m.Stage.StageInfo.LocalCoord, !options.StageCrop, m.Stage.Camera.CutHigh, m.Stage.Camera.CutLow)
	if err != nil {
		return nil, err
	}
	options.GameSpace = m.Viewport.Space
	m.Options = options
	stageToGame, _ := m.Stage.StageInfo.LocalCoord.ScaleTo(options.GameSpace)
	m.Viewport.GroundY = m.Stage.GroundLevel() * stageToGame
	m.Fight, err = fight.Load(options.FightPath, options.RootDir)
	if err != nil {
		return nil, fmt.Errorf("match fight screen: %w", err)
	}
	m.Effects = effect.NewScene(options.ExplodMax)
	m.World.UnownedExplodCount = m.Effects.UnownedCount
	m.Effects.Queue.Admit = m.World.CanCreateExplod
	motifWidth := options.MotifSpace.Width
	if motifWidth <= 0 {
		motifWidth = 320
	}
	fxScale := 1.0
	if attribute := m.Fight.Attribute("FightFx", "scale"); attribute != nil {
		fxScale = background.GetFloat(attribute.Value)
	}
	m.commonEffects = effect.Source{Sprites: m.Fight.EffectSprites, Animations: m.Fight.EffectAnimations, Scale: fxScale * float64(options.GameSpace.Width) / float64(motifWidth)}
	m.World.HomeTeam = homeTeam(request.Mode)
	m.World.MatchNo = max(1, request.MatchNo)
	m.World.Training = request.Mode == "Training"
	for side, selection := range request.Teams {
		team := &m.Teams[side]
		team.Mode, team.entered = selection.Mode, make(map[*character.Player]bool)
		team.next = selection.StartMember
		m.World.TeamModes[side] = string(selection.Mode)
		for index, selected := range selection.Members {
			if selected.Palette < 1 || selected.Palette > 12 || selected.AILevel < 0 || selected.AILevel > 8 {
				return nil, fmt.Errorf("team %d member %d: invalid palette/AI level", side+1, index+1)
			}
			player, err := character.LoadPlayerWithOptions(filepath.Dir(selected.DefPath), filepath.Base(selected.DefPath), character.LoadOptions{RootDir: options.RootDir, MotifDir: options.MotifDir})
			if err != nil {
				return nil, fmt.Errorf("team %d member %d: %w", side+1, index+1, err)
			}
			player.BindPalette(player.Character, selected.Palette)
			team.Members = append(team.Members, player)
		}
		m.Players[side] = team.Members[team.next]
	}

	if err := m.resetRound(); err != nil {
		return nil, err
	}
	return m, nil
}

func (m *Match) Setting(section, key string, fallback int) int {
	if attribute := m.Fight.Attribute(section, key); attribute != nil {
		return int(background.GetFloat(attribute.Value))
	}
	return fallback
}

func (m *Match) resetRound() error {
	m.aiBrains = nil
	for side := range m.Teams {
		team := &m.Teams[side]
		team.Active = team.next
		m.Players[side] = team.Members[team.Active]
	}
	m.World.StageInfo = map[string]string{"info.name": m.Stage.Name, "info.displayname": m.Stage.DisplayName, "info.author": m.Stage.Author, "info.authorname": m.Stage.Author}
	m.World.StageNoAutoTurn = !m.Stage.StageInfo.AutoTurn
	m.Timeline.ClearPauses()
	m.Timeline.ResetRoundEffectClock()
	m.Timeline.ClearShake()
	m.Effects.Clear()
	// A fresh round retires its voices/cache before new entry controllers emit
	// commands. Complete native round/custom-state lifecycle remains to verify.
	if err := m.ClearSounds(); err != nil {
		return err
	}
	m.Phase, m.PhaseTime, m.Winner, m.TimeOver, m.timeTicks = PreIntro, 0, -1, false, 0
	m.winPoseStarted = false
	m.resultCommitted = false
	m.initializingRound = m.Round > 1
	m.pendingRoundOver = false
	m.resetAnnouncements()
	m.slowdown.Clear()
	m.TimeLeft = m.Options.RoundTime
	m.elapsedFightTicks = 0
	s := m.Stage
	if m.Round > 1 {
		s.ResetBackground()
	}
	stageScale, _ := s.StageInfo.LocalCoord.ScaleTo(m.Options.GameSpace)
	m.Viewport.GroundY = s.GroundLevel() * stageScale
	m.CameraX, m.CameraY = s.Camera.StartX*stageScale, s.Camera.StartY*stageScale+m.Viewport.CropY
	m.cameraGoalX = m.CameraX
	half := float64(m.Options.GameSpace.Width) / 2
	m.World.ProjectileStageBounds = [2]float64{s.Camera.BoundLeft*stageScale - half, s.Camera.BoundRight*stageScale + half}
	m.World.ProjectileStageBoundsKnown = true
	m.Viewport.Zoom = s.Camera.StartZoom
	m.cameraLeft, m.cameraRight = float32(m.CameraX-half/m.Viewport.Zoom), float32(m.CameraX+half/m.Viewport.Zoom)
	m.Viewport.CameraX, m.Viewport.CameraY = m.CameraX, m.CameraY
	m.Viewport.CameraX = m.publishedCameraX()
	for slot, p := range m.ActivePlayers() {
		if p == nil {
			continue
		}
		side := slot % 2
		previous := p.Character
		c := character.NewCharacter(previous.StateFile, p.AIR, p.CMD)
		c.Name, c.AuthorName = previous.Name, previous.AuthorName
		if p.Definition != nil {
			c.Name, c.AuthorName = p.Definition.Info["name"], p.Definition.Info["author"]
		}
		c.BindSound(m.SoundQueue, p.SND, m.Fight.CommonSounds)
		c.LocalCoord = p.LocalCoord
		c.GameCoord = m.Options.GameSpace
		c.Viewport = m.Viewport
		c.BindTimeline(m.Timeline)
		c.SuperTargetDefenceMul = m.Options.SuperTargetDefenceMul
		c.PowerRules = m.Options.PowerRules

		c.BindEffects(&m.Effects.Queue, m.commonEffects)
		c.InputSlot, p.InputSlot = slot+1, slot+1
		selected, _ := m.SelectionForPlayer(slot + 1)
		p.BindPalette(c, selected.Palette)
		c.AILevel = selected.AILevel
		c.TeamSide = side + 1
		c.RoundNo, c.RoundsExisted, c.RoundState = m.Round, 0, 0
		if m.Round > 1 && m.Teams[side].entered[p] {
			c.RoundsExisted = previous.RoundsExisted + 1
		}
		c.Ctrl = false
		c.MaxLife = c.MaxLife * m.Options.LifePercent / 100
		c.MaxLife = m.rosterLife(side, c.MaxLife)
		if m.Teams[side].Mode == Single && m.Teams[1-side].Mode == Simul && m.Options.Team1Vs2Life > 0 {
			c.MaxLife = c.MaxLife * m.Options.Team1Vs2Life / 100
		}
		c.Life = c.MaxLife
		if m.Round > 1 && m.hasTurns() && m.Teams[side].entered[p] && previous.IsAlive() {
			c.Life = min(c.MaxLife, previous.Life)
		}
		m.Teams[side].entered[p] = true
		c.Commands = character.NewCommandBuffer(120)
		p.Commands = c.Commands
		c.InputSource = m.Options.Input
		if m.Round > 1 {
			c.Power = previous.Power
			for index := c.StateFile.Data.IntPersistIndex; index >= 0 && index < len(c.IntVars); index++ {
				c.IntVars[index] = previous.IntVars[index]
			}
			for index := c.StateFile.Data.FloatPersistIndex; index >= 0 && index < len(c.FloatVars); index++ {
				c.FloatVars[index] = previous.FloatVars[index]
			}
		}
		x, y, z, facing := s.PlayerInfo.P1StartX, s.PlayerInfo.P1StartY, s.PlayerInfo.P1StartZ, s.PlayerInfo.P1Facing
		if side == 1 {
			x, y, z, facing = s.PlayerInfo.P2StartX, s.PlayerInfo.P2StartY, s.PlayerInfo.P2StartZ, s.PlayerInfo.P2Facing
		}
		if facing == 0 {
			facing = 1
			if side == 1 {
				facing = -1
			}
		}
		scale, _ := s.StageInfo.LocalCoord.ScaleTo(p.LocalCoord)
		if slot > 1 {
			x -= 25 * float64(facing)
		}
		c.X, c.Y, c.Z, c.Facing = x*scale, y*scale, z*scale, facing
		c.StartX, c.StartY, c.StartZ, c.StartFacing = c.X, c.Y, c.Z, c.Facing
		c.LeftBound, c.RightBound = s.PlayerInfo.LeftBound*scale, s.PlayerInfo.RightBound*scale
		c.ScreenLeftMargin, c.ScreenRightMargin = s.Bound.ScreenLeft*scale, s.Bound.ScreenRight*scale
		c.TopBound, c.BotBound = s.PlayerInfo.TopBound*scale, s.PlayerInfo.BotBound*scale
		p.Character = c
	}
	for side, team := range m.Teams {
		if team.Mode != Simul {
			continue
		}
		maximum, power := 0, 0
		for _, p := range m.SidePlayers(side) {
			maximum = max(maximum, p.Character.MaxPower)
			power = max(power, p.Character.Power)
		}
		for _, p := range m.SidePlayers(side) {
			p.Character.MaxPower = maximum
			p.Character.Power = min(maximum, power)
		}
	}
	if err := m.World.ResetRoots(m.activeRoots()...); err != nil {
		return err
	}
	for _, p := range m.Actors() {
		p.Character.ChangeState(5900, -1, -1)
		if p.Character.RuntimeError != nil {
			return p.Character.RuntimeError
		}
	}
	for side := range m.Teams {
		m.startingLife[side] = 0
		players := m.SidePlayers(side)
		for _, p := range players {
			c := p.Character
			m.startingLife[side] = float32(float64(m.startingLife[side]) + float64(c.Life)/float64(max(1, c.MaxLife)))
		}
		m.startingLife[side] /= float32(len(players))
	}
	return nil
}

func (m *Match) ClearSounds() error {
	return m.SoundQueue.Push(sound.Command{Operation: sound.ClearScene})
}

func (m *Match) enter(phase Phase) {
	m.Phase, m.PhaseTime = phase, 0
	if phase == Intro {
		m.introClearTicks = 0
	}
}
func (m *Match) roundState(value int) {
	m.Timeline.ResetRoundEffectClock()
	for _, p := range m.Actors() {
		if value == 2 && p.Character.RoundState != 2 {
			// Native restores root X/Y on the first fight-call transition,
			// including a moving owner during Pause. Animation and CNS clocks
			// remain intact; velocity/facing/Z resets are not inferred here.
			p.Character.X, p.Character.Y = p.Character.StartX, p.Character.StartY
		}
		p.Character.RoundState = value
	}
}

func (m *Match) Update() error {
	m.SimulationSkipped = false
	if m.Phase == Finished {
		m.SimulationSkipped = true
		return nil
	}
	if m.initializingRound {
		m.SimulationSkipped = true
		m.initializingRound = false
		m.Timeline.SkipSimulationFrame()
		m.Tick++
		return nil
	}
	if !m.slowdown.BeginFrame() {
		m.SimulationSkipped = true
		m.Timeline.SkipSimulationFrame()
		m.Tick++
		return nil
	}
	paused := m.Timeline.Paused()
	m.advanceAnnouncements(paused)
	if m.pendingRoundOver {
		m.pendingRoundOver = false
		m.commitRoundResult()
		m.roundState(4)
		m.enter(Over)
		m.Announcements.Win = 0
		m.roundEndDeadline = m.Setting("Round", "over.time", 210)
		m.slowdown.Clear()
		for _, player := range m.Actors() {
			player.Character.HitDefActive = false
		}
	}
	if err := m.startWinPose(); err != nil {
		return err
	}
	if !paused || m.Phase == PreOver {
		m.slowdown.Advance()
	}
	if err := m.Effects.AdvanceWithPause(paused, m.Timeline.SuperPaused()); err != nil {
		return fmt.Errorf("fight effects: %w", err)
	}
	if !m.Timeline.BackgroundFrozen() {
		m.Stage.Update()
	}
	stageScale, _ := m.Stage.StageInfo.LocalCoord.ScaleTo(m.Options.GameSpace)
	m.Viewport.GroundY = m.Stage.GroundLevel() * stageScale
	// The initial KO pass counts toward the native damage grace.
	damageDisabled := m.Phase == Over || (m.Phase == PreOver && m.PhaseTime+1 >= m.Setting("Round", "over.hittime", 10))
	for _, p := range m.Actors() {
		c := p.Character
		c.RoundDamageDisabled = damageDisabled
		scale, _ := m.Options.GameSpace.ScaleTo(p.LocalCoord)
		c.CameraX = m.CameraX * scale
		c.ScreenWidth = p.LocalCoord.Width
		if m.Phase != Active && m.Phase != PreOver {
			c.Ctrl = false
		}
	}
	if err := character.TickCharacters(m.activeRoots()); err != nil {
		return fmt.Errorf("match tick %d: %w", m.Tick, err)
	}
	if err := m.Effects.Flush(); err != nil {
		return fmt.Errorf("fight effects: %w", err)
	}
	m.updateCamera()
	if m.World.Training {
		for _, p := range m.Actors() {
			p.Character.SetPower(p.Character.MaxPower)
			if p.Character.MoveType != "H" {
				p.Character.SetLife(p.Character.MaxLife)
			}
		}
	}
	m.Tick++
	// A moving owner can self-KO while the other root remains frozen. Native
	// enters RoundState 3 immediately even though ordinary phase clocks stop.
	if paused && m.Phase == Active && !m.Timeline.NoKO && m.hasKO() {
		m.finishRound()
		return nil
	}
	if paused && m.Phase != PreOver && m.Phase != Over {
		return nil
	}
	m.PhaseTime++
	if m.Phase == FightCall || m.Phase == Active {
		// Recovery uses elapsed fight passes even under TimerFreeze or an
		// unlimited timer. Pause and skipped simulation passes do not count.
		m.elapsedFightTicks++
	}
	switch m.Phase {
	case PreIntro:
		if m.PhaseTime >= m.Setting("Round", "start.waittime", 30) {
			m.roundState(1)
			m.enter(Intro)
		}
	case Intro:
		intro := false
		for _, p := range m.Actors() {
			intro = intro || p.Character.Specials["intro"]
		}
		if intro {
			m.introClearTicks = 0
		} else {
			m.introClearTicks++
			if m.Announcements.Round == announcementInactive {
				// Native presents Round on the next pass after intro release,
				// before the remaining intro/control countdown has completed.
				m.Announcements.Round = -1
			}
		}
		if m.introClearTicks >= 10 {
			m.enter(RoundCall)
		}
	case RoundCall:
		// Display durations and round.time affect presentation only. The beta
		// retains this countdown even with a one-frame or 80-frame component.
		if m.PhaseTime >= 60 {
			m.roundState(2)
			m.enter(FightCall)
			m.Announcements.Fight = -1
			if m.Setting("Round", "ctrl.time", 30) <= 0 {
				m.startActive()
			}
		}
	case FightCall:
		if m.PhaseTime >= m.Setting("Round", "ctrl.time", 30) {
			m.startActive()
		}
	case Active:
		frozen := false
		for _, p := range m.Actors() {
			frozen = frozen || p.Character.Specials["timerfreeze"]
		}
		if m.TimeLeft >= 0 && !frozen {
			m.timeTicks++
			if m.timeTicks >= max(1, m.Setting("Time", "framespercount", 60)) {
				m.timeTicks = 0
				m.TimeLeft--
			}
		}
		if (!m.Timeline.NoKO && m.hasKO()) || (m.Options.RoundTime >= 0 && m.TimeLeft < 0) {
			m.finishRound()
		}
	case PreOver:
		if !m.TimeOver || m.PhaseTime == 1 {
			// A second KO during the grace interval can replace a provisional
			// win with a draw before RoundState 4 commits either tally.
			m.publishRoundResult()
		}
		if m.PhaseTime >= m.Setting("Round", "over.waittime", 45) && (m.roundActorsReady() || m.PhaseTime >= 900) {
			m.pendingRoundOver = true
		}
	case Over:
		held := false
		for _, p := range m.Actors() {
			held = held || p.Character.Specials["roundnotover"]
		}
		// The beta keeps a rolling 19-pass deadline while a win pose is
		// asserted, rather than simply testing the flag at over.time.
		if held {
			m.roundEndDeadline = m.PhaseTime + 19
		}
		if m.PhaseTime >= m.roundEndDeadline {
			if m.matchDone {
				m.enter(Finished)
			} else {
				m.Round++
				return m.resetRound()
			}
		}
	}
	return nil
}

func (m *Match) startActive() {
	m.enter(Active)
	m.timeTicks = 2
	for _, player := range m.Actors() {
		player.Character.Ctrl = true
	}
}

func (m *Match) roundActorsReady() bool {
	for _, player := range m.Actors() {
		c := player.Character
		ready := c.IsAlive() && c.Ctrl && c.StateType == "S" && c.MoveType == "I"
		ready = ready || (c.StateNo == 5150 && c.Time > 0)
		if !ready {
			return false
		}
	}
	return true
}

func (m *Match) hasKO() bool {
	return m.teamKO(0) || m.teamKO(1)
}

func (m *Match) finishRound() {
	m.Announcements.Finish = 0
	slow := true
	for _, p := range m.Actors() {
		slow = slow && !p.Character.Specials["nokoslow"]
	}
	m.TimeOver = !m.hasKO()
	if m.TimeOver {
		m.TimeLeft = 0
	} else if slow {
		m.slowdown.Start(m.Setting("Round", "slow.time", 60))
	}
	if !m.TimeOver {
		m.publishRoundResult()
	}
	m.roundState(3)
	m.enter(PreOver)
}

func (m *Match) publishRoundResult() {
	a, am := m.teamLife(0)
	b, bm := m.teamLife(1)
	left, right := a*bm, b*am
	if m.teamKO(0) && m.teamKO(1) {
		left, right = 0, 0
	} else if m.teamKO(0) != m.teamKO(1) {
		if m.teamKO(0) {
			left, right = 0, 1
		} else {
			left, right = 1, 0
		}
	}
	m.Winner = -1
	if left > right {
		m.Winner = 0
	} else if right > left {
		m.Winner = 1
	}
	for _, p := range m.Actors() {
		side := p.Character.TeamSide - 1
		p.Character.RoundWon = m.Winner == side
		p.Character.RoundLost = m.Winner >= 0 && m.Winner != side
		p.Character.RoundDraw = m.Winner < 0
		p.Character.RoundTimeOver = m.TimeOver
	}
}

// RoundState 4 commits the result; Win/Lose may already have reported it during
// the KO wait. Native Quick-VS is intentionally a different, unlimited mode.
func (m *Match) commitRoundResult() {
	if m.resultCommitted {
		return
	}
	m.resultCommitted = true
	if m.Winner >= 0 {
		m.Wins[m.Winner]++
	} else {
		m.Draws++
		limit := m.Setting("Round", "match.maxdrawgames", 1)
		if m.Draws > limit {
			m.Wins[0]++
			m.Wins[1]++
		}
	}
	target := max(1, m.Setting("Round", "match.wins", 2))
	if m.isSurvival() {
		// Each defeated opponent advances the streak. Ordinary match.wins
		// must not grant a fresh full-health round against that opponent.
		target = 1
	}
	m.matchDone = m.Wins[0] >= target || m.Wins[1] >= target
	m.commitTeamResult()
	for _, p := range m.Actors() {
		p.Character.IsMatchOver = m.matchDone
	}
}

func (m *Match) updateCamera() {
	m.updateHorizontalCamera()
	scale, _ := m.Stage.StageInfo.LocalCoord.ScaleTo(m.Options.GameSpace)
	y := 0.0
	participants := false
	horizontalParticipants := false
	for _, actor := range m.cameraActors() {
		horizontalParticipants = horizontalParticipants || actor.CameraMovementEnabled(0)
		if !actor.CameraMovementEnabled(1) {
			continue
		}
		participants = true
		factor := float64(m.Options.GameSpace.Width) / float64(actor.LocalCoordinateWidth())
		y = math.Min(y, actor.Y*factor)
	}
	if participants && horizontalParticipants {
		y = math.Min(0, y+m.Stage.Camera.FloorTension*scale) * m.Stage.Camera.VerticalFollow
		m.CameraY = math.Max(m.Stage.Camera.BoundHigh*scale, math.Min(m.Stage.Camera.BoundLow*scale, y)) + m.Viewport.CropY
	}
	m.Viewport.CameraX, m.Viewport.CameraY = m.publishedCameraX(), m.CameraY
}
