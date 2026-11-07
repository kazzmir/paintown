package main

import (
	"fmt"
	"os"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

type modeRun struct {
	engine                *Engine
	id                    uint64
	template              match.Request
	opponents             []*CharacterPortrait
	stages                []string
	random                *engine.RandomStream
	index                 int
	survival              bool
	ending                string
	lastFight             *FightState
	carryLife, carryPower [2]int
}

func (s *SelectState) prepareMode(request match.Request) (match.Request, error) {
	if s.title == "Watch Mode" {
		for side := range request.Teams {
			for member := range request.Teams[side].Members {
				request.Teams[side].Members[member].AILevel = max(1, min(8, s.engine.cfg.Options.Difficulty))
			}
		}
		return request, nil
	}
	arcade := s.title == "Arcade Mode" || s.title == "Team Arcade" || s.title == "Team Co-op"
	survival := s.title == "Survival" || s.title == "Survival Co-op"
	if !arcade && !survival {
		return request, nil
	}
	s.engine.nextCampaign++
	seed := s.engine.pendingMatchSeed
	if seed == 0 {
		seed = int64(s.ticks) + 1
	}
	r := &modeRun{engine: s.engine, id: s.engine.nextCampaign, template: request, stages: s.stages, survival: survival, random: engine.NewRandomStream(seed)}
	var available []*CharacterPortrait
	for _, ch := range s.chars {
		if ch.DefPath != "" {
			available = append(available, ch)
		}
		if ch.DefPath == request.Teams[0].Members[0].DefPath {
			r.ending = ch.EndingStoryboard
		}
	}
	if len(available) == 0 {
		return request, fmt.Errorf("mode has no opponents")
	}
	if survival {
		r.opponents = available
	} else {
		file, err := os.Open(s.selectDefPath)
		if err != nil {
			return request, err
		}
		definition, err := parsers.ParseSelectDef(file)
		file.Close()
		if err != nil {
			return request, err
		}
		limits := definition.ArcadeMaxMatches
		if s.title != "Arcade Mode" {
			limits = definition.TeamMaxMatches
		}
		for order, count := range limits {
			if count <= 0 {
				continue
			}
			var pool []*CharacterPortrait
			for _, ch := range available {
				if max(1, ch.Entry.Order) == order+1 {
					pool = append(pool, ch)
				}
			}
			for i := len(pool) - 1; i > 0; i-- {
				j := int(r.random.Next() % int64(i+1))
				pool[i], pool[j] = pool[j], pool[i]
			}
			r.opponents = append(r.opponents, pool[:min(count, len(pool))]...)
		}
		if len(r.opponents) == 0 {
			r.opponents = available
		}
	}
	s.engine.modeRun = r
	return r.request(), nil
}

func rampDifficulty(base, won int, ramp config.AIRamp) int {
	if len(ramp.Start) < 2 || len(ramp.End) < 2 {
		return max(1, min(8, base))
	}
	offset := ramp.Start[1]
	if won >= ramp.End[0] {
		offset = ramp.End[1]
	} else if won > ramp.Start[0] && ramp.End[0] > ramp.Start[0] {
		offset += (ramp.End[1] - ramp.Start[1]) * (won - ramp.Start[0]) / (ramp.End[0] - ramp.Start[0])
	}
	return max(1, min(8, base+offset))
}

func (r *modeRun) request() match.Request {
	request := r.template
	request.Campaign, request.MatchNo = r.id, r.index+1
	leader := r.opponents[r.index%len(r.opponents)]
	stage := leader.Entry.Stage
	if stage == "" || strings.EqualFold(stage, "random") {
		stage = r.stages[int(r.random.Next()%int64(len(r.stages)))]
	}
	request.StagePath, request.Music = stage, leader.Entry.Music
	ramp := r.engine.cfg.Arcade.ArcadeRamp
	if r.survival {
		ramp = r.engine.cfg.Arcade.SurvivalRamp
	} else if r.template.Mode != "Arcade Mode" {
		ramp = r.engine.cfg.Arcade.TeamRamp
	}
	level := rampDifficulty(r.engine.cfg.Options.Difficulty, r.index, ramp)
	request.Teams[1].Members = append([]match.Selection(nil), r.template.Teams[1].Members...)
	for index := range request.Teams[1].Members {
		ch := leader
		if index > 0 {
			ch = r.opponents[(r.index+index)%len(r.opponents)]
		}
		palette := 1
		if r.engine.cfg.Arcade.AIRandomColor {
			palette = int(r.random.Next()%12) + 1
		}
		request.Teams[1].Members[index] = match.Selection{DefPath: ch.DefPath, Name: ch.Name, Palette: palette, AILevel: level}
	}
	request.Players[0], request.Players[1] = request.Teams[0].Members[0], request.Teams[1].Members[0]
	return request
}

func (r *modeRun) restore(f *FightState) {
	if !r.survival || f.match.Request.Campaign != r.id {
		return
	}
	for member, p := range f.match.SidePlayers(0) {
		if member >= len(r.carryLife) {
			break
		}
		if r.carryLife[member] > 0 {
			p.Character.SetLife(min(p.Character.MaxLife, r.carryLife[member]))
			p.Character.SetPower(r.carryPower[member])
		}
	}
}

func (r *modeRun) result(f *FightState) (State, error) {
	r.lastFight = f
	if err := f.match.ClearSounds(); err != nil {
		return f, err
	}
	if f.match.Winner != 0 {
		if r.survival {
			return &GameOverScreenState{engine: r.engine, score: r.index, survival: true}, nil
		}
		if r.engine.themeNumber("Continue Screen", "enabled", 1) == 0 {
			return NewGameOverScreenState(r.engine), nil
		}
		return &ContinueScreenState{engine: r.engine, retry: f.match.Request}, nil
	}
	if r.survival {
		for index, p := range f.match.SidePlayers(0) {
			if index < len(r.carryLife) {
				r.carryLife[index], r.carryPower[index] = p.Character.Life, p.Character.GetPower()
			}
		}
		r.index++
		return NewVersusState(r.engine, r.request()), nil
	}
	r.index++
	next := func() (State, error) {
		if r.index < len(r.opponents) {
			return NewVersusState(r.engine, r.request()), nil
		}
		credits, err := r.engine.configuredStory("End Credits", NewTitleState(r.engine))
		if err != nil {
			return nil, err
		}
		if r.ending != "" {
			return &StoryboardState{engine: r.engine, storyboard: r.ending, nextState: func() State { return credits }}, nil
		}
		ending, err := r.engine.configuredStory("Default Ending", credits)
		if err != nil {
			return nil, err
		}
		if r.engine.themeNumber("Win Screen", "enabled", 1) != 0 {
			return &completionScreen{engine: r.engine, fight: f, next: ending}, nil
		}
		return ending, nil
	}
	if r.index < len(r.opponents) && r.engine.themeNumber("Victory Screen", "enabled", 1) != 0 {
		return &WinScreenState{engine: r.engine, winner: f.match.Players[0], next: next}, nil
	}
	return next()
}
