// Package match owns the production one-on-one match lifecycle. Presentation
// adapters and scripted tests advance the same instance one tick at a time.
package match

import (
	"fmt"
	"math"
	"path/filepath"

	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/fight"
	"github.com/kazzmir/paintown/game/mugen/palette"
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
}

type Options struct {
	RootDir, MotifDir, FightPath string
	GameSpace                    coord.Space
	RoundTime, LifePercent       int
	Input                        func(playerID, facing int) []string
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
	Request                Request
	Options                Options
	Players                [2]*character.Player
	Stage                  *stage.Stage
	Fight                  *fight.Definition
	Phase                  Phase
	PhaseTime, Tick, Round int
	Wins                   [2]int
	Winner                 int // 0/1, or -1 for a draw
	TimeOver               bool
	TimeLeft               int     // counts; -1 means unlimited
	CameraX, CameraY       float64 // common game-space units
	timeTicks              int
	matchDone              bool
}

func New(request Request, options Options) (*Match, error) {
	if _, err := options.GameSpace.ScaleTo(options.GameSpace); err != nil {
		return nil, err
	}
	if options.RoundTime < -1 || options.LifePercent <= 0 {
		return nil, fmt.Errorf("invalid match time/life options")
	}
	m := &Match{Request: request, Options: options, Round: 1, Winner: -1}
	var err error
	m.Stage, err = stage.LoadStage(request.StagePath, options.RootDir)
	if err != nil {
		return nil, fmt.Errorf("match stage: %w", err)
	}
	m.Fight, err = fight.Load(options.FightPath, options.RootDir)
	if err != nil {
		return nil, fmt.Errorf("match fight screen: %w", err)
	}
	for slot, selected := range request.Players {
		if selected.Palette < 1 || selected.Palette > 12 || selected.AILevel < 0 || selected.AILevel > 8 {
			return nil, fmt.Errorf("match player %d: invalid palette/AI level", slot+1)
		}
		m.Players[slot], err = character.LoadPlayerWithOptions(filepath.Dir(selected.DefPath), filepath.Base(selected.DefPath), character.LoadOptions{RootDir: options.RootDir, MotifDir: options.MotifDir})
		if err != nil {
			return nil, fmt.Errorf("match player %d: %w", slot+1, err)
		}
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
	m.Phase, m.PhaseTime, m.Winner, m.TimeOver, m.timeTicks = PreIntro, 0, -1, false, 0
	m.TimeLeft = m.Options.RoundTime
	s := m.Stage
	stageScale, _ := s.StageInfo.LocalCoord.ScaleTo(m.Options.GameSpace)
	m.CameraX, m.CameraY = s.Camera.StartX*stageScale, s.Camera.StartY*stageScale
	for slot, p := range m.Players {
		previous := p.Character
		c := character.NewCharacter(previous.StateFile, p.AIR, p.CMD)
		c.LocalCoord = p.LocalCoord
		c.GameCoord = m.Options.GameSpace
		c.Palettes = palette.New(p.SFF)
		c.PlayerID, p.PlayerID = slot+1, slot+1
		c.PaletteNo, c.AILevel = m.Request.Players[slot].Palette, m.Request.Players[slot].AILevel
		c.RoundNo, c.RoundsExisted, c.RoundState = m.Round, m.Round-1, 0
		c.Ctrl = false
		c.MaxLife = c.MaxLife * m.Options.LifePercent / 100
		c.Life = c.MaxLife
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
		if slot == 1 {
			x, y, z, facing = s.PlayerInfo.P2StartX, s.PlayerInfo.P2StartY, s.PlayerInfo.P2StartZ, s.PlayerInfo.P2Facing
		}
		if facing == 0 {
			facing = 1
			if slot == 1 {
				facing = -1
			}
		}
		scale, _ := s.StageInfo.LocalCoord.ScaleTo(p.LocalCoord)
		c.X, c.Y, c.Z, c.Facing = x*scale, y*scale, z*scale, facing
		c.StartX, c.StartY, c.StartZ, c.StartFacing = c.X, c.Y, c.Z, c.Facing
		c.LeftBound, c.RightBound = s.PlayerInfo.LeftBound*scale, s.PlayerInfo.RightBound*scale
		c.TopBound, c.BotBound = s.PlayerInfo.TopBound*scale, s.PlayerInfo.BotBound*scale
		p.Character = c
	}
	for slot, p := range m.Players {
		p.Character.Target = []*character.Character{m.Players[1-slot].Character}
		p.Character.ChangeState(5900, -1, -1)
		if p.Character.RuntimeError != nil {
			return p.Character.RuntimeError
		}
	}
	return nil
}

func (m *Match) enter(phase Phase) { m.Phase, m.PhaseTime = phase, 0 }
func (m *Match) roundState(value int) {
	for _, p := range m.Players {
		p.Character.RoundState = value
	}
}

func (m *Match) Update() error {
	if m.Phase == Finished {
		return nil
	}
	m.Stage.Update()
	for _, p := range m.Players {
		c := p.Character
		scale, _ := m.Options.GameSpace.ScaleTo(p.LocalCoord)
		c.CameraX = m.CameraX * scale
		c.ScreenWidth = p.LocalCoord.Width
		if m.Phase != Active && m.Phase != PreOver {
			c.Ctrl = false
		}
		if err := p.Update(); err != nil {
			return fmt.Errorf("match tick %d: %w", m.Tick, err)
		}
	}
	m.updateCamera()
	m.Tick++
	m.PhaseTime++
	switch m.Phase {
	case PreIntro:
		if m.PhaseTime >= m.Setting("Round", "start.waittime", 30) {
			m.roundState(1)
			m.enter(Intro)
		}
	case Intro:
		intro := false
		for _, p := range m.Players {
			intro = intro || p.Character.Specials["intro"]
		}
		if !intro {
			m.roundState(2)
			m.enter(RoundCall)
		}
	case RoundCall:
		if m.PhaseTime >= m.Setting("Round", "round.default.displaytime", 60) {
			m.enter(FightCall)
		}
	case FightCall:
		if m.PhaseTime >= m.Setting("Round", "ctrl.time", 30) {
			m.enter(Active)
			for _, p := range m.Players {
				p.Character.Ctrl = true
			}
		}
	case Active:
		frozen := false
		for _, p := range m.Players {
			frozen = frozen || p.Character.Specials["timerfreeze"]
		}
		if m.TimeLeft >= 0 && !frozen {
			m.timeTicks++
			if m.timeTicks >= max(1, m.Setting("Time", "framespercount", 60)) {
				m.timeTicks = 0
				m.TimeLeft = max(0, m.TimeLeft-1)
			}
		}
		if m.Players[0].Character.Life <= 0 || m.Players[1].Character.Life <= 0 || m.TimeLeft == 0 {
			m.finishRound()
		}
	case PreOver:
		if m.PhaseTime >= m.Setting("Round", "over.waittime", 45) {
			for _, p := range m.Players {
				p.Character.Ctrl = false
				p.Character.HitDefActive = false
			}
		}
		if m.PhaseTime >= m.Setting("Round", "over.waittime", 45)+m.Setting("Round", "over.wintime", 45) {
			m.roundState(4)
			for slot, p := range m.Players {
				state := 170
				if m.Winner < 0 {
					state = 175
				} else if slot == m.Winner {
					state = 180
				} else if p.Character.Life <= 0 {
					continue
				}
				p.Character.ChangeState(state, 0, -1)
				if p.Character.RuntimeError != nil {
					return p.Character.RuntimeError
				}
			}
			m.enter(Over)
		}
	case Over:
		if m.PhaseTime >= m.Setting("Round", "over.time", 210) {
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

func (m *Match) finishRound() {
	a, b := m.Players[0].Character, m.Players[1].Character
	m.TimeOver = a.Life > 0 && b.Life > 0
	left, right := int64(a.Life)*int64(b.MaxLife), int64(b.Life)*int64(a.MaxLife)
	m.Winner = -1
	if left > right {
		m.Winner = 0
	} else if right > left {
		m.Winner = 1
	}
	if m.Winner >= 0 {
		m.Wins[m.Winner]++
	}
	m.matchDone = m.Wins[0] >= max(1, m.Setting("Round", "match.wins", 2)) || m.Wins[1] >= max(1, m.Setting("Round", "match.wins", 2))
	for _, p := range m.Players {
		p.Character.IsMatchOver = m.matchDone
	}
	m.roundState(3)
	m.enter(PreOver)
}

func (m *Match) updateCamera() {
	scale, _ := m.Stage.StageInfo.LocalCoord.ScaleTo(m.Options.GameSpace)
	x, y := 0.0, 0.0
	for _, p := range m.Players {
		factor, _ := p.LocalCoord.ScaleTo(m.Options.GameSpace)
		x += p.Character.X * factor / 2
		y = math.Min(y, p.Character.Y*factor)
	}
	m.CameraX = math.Max(m.Stage.Camera.BoundLeft*scale, math.Min(m.Stage.Camera.BoundRight*scale, x))
	y = math.Min(0, y+m.Stage.Camera.FloorTension*scale) * m.Stage.Camera.VerticalFollow
	m.CameraY = math.Max(m.Stage.Camera.BoundHigh*scale, math.Min(m.Stage.Camera.BoundLow*scale, y))
}
