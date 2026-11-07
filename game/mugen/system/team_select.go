package main

import (
	"fmt"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
)

func (s *SelectState) initTeams() {
	s.teamReady, s.teamCursor = [2]bool{}, [2]int{}
	s.opponentTeamChosen = false
	s.teamCounts = [2]int{1, 1}
	s.teamModes = [2]match.TeamMode{match.Single, match.Single}
	s.teamMenu = strings.Contains(strings.ToLower(s.title), "team") || s.title == "Survival" || s.title == "Watch Mode"
	if s.title == "Survival" || s.title == "Team Arcade" || s.title == "Watch Mode" {
		// Survival has a scheduled opponent. Arcade/Watch ask for the
		// opposing mode after the first roster has been selected.
		s.teamReady[1] = true
	}
	if s.title == "Team Co-op" || s.title == "Survival Co-op" {
		// Each human chooses one member of the same simul team.
		s.teamMenu = false
		s.teamModes[0], s.teamCursor[0] = match.Simul, 1
	}
}

func (s *SelectState) updateTeamMenu() {
	modes := []match.TeamMode{match.Single, match.Simul, match.Turns}
	for side := 0; side < 2; side++ {
		if s.teamReady[side] {
			continue
		}
		if side == 1 && !s.showP2 && !s.teamReady[0] {
			continue
		}
		inputSlot := side + 1
		if !s.showP2 || s.title == "Team Co-op" || s.title == "Watch Mode" {
			inputSlot = 1
		}
		index := s.teamCursor[side]
		if input.GlobalManager.IsJustPressed(inputSlot, input.ActionUp) {
			index = (index + 2) % 3
		}
		if input.GlobalManager.IsJustPressed(inputSlot, input.ActionDown) {
			index = (index + 1) % 3
		}
		s.teamCursor[side] = index
		s.teamModes[side] = modes[index]
		switch modes[index] {
		case match.Single:
			s.teamCounts[side] = 1
		case match.Simul:
			s.teamCounts[side] = 2
		case match.Turns:
			if s.teamCounts[side] < 2 {
				s.teamCounts[side] = 2
			}
			if input.GlobalManager.IsJustPressed(inputSlot, input.ActionLeftUI) {
				s.teamCounts[side] = max(2, s.teamCounts[side]-1)
			}
			if input.GlobalManager.IsJustPressed(inputSlot, input.ActionRightUI) {
				s.teamCounts[side] = min(4, s.teamCounts[side]+1)
			}
		}
		if _, ok := s.confirm(inputSlot); ok {
			s.teamReady[side] = true
			break
		}
	}
	if s.teamReady[0] && s.teamReady[1] {
		s.teamMenu = false
		if s.p1Done {
			s.opponentTeamChosen = true
		}
	}
}

func (s *SelectState) pickMember(side int, ch *CharacterPortrait, palette int) bool {
	if ch == nil || ch.DefPath == "" {
		return false
	}
	selected := match.Selection{DefPath: ch.DefPath, Name: ch.Name, Palette: palette}
	if len(s.rosters[side]) > 0 && s.teamModes[side] == match.Simul {
		selected.AILevel = max(1, min(8, s.engine.cfg.Options.Difficulty))
	}
	s.rosters[side] = append(s.rosters[side], selected)
	return len(s.rosters[side]) >= s.teamCounts[side]
}

func (s *SelectState) selectedTeams(p1, p2 *CharacterPortrait) [2]match.TeamSelection {
	var teams [2]match.TeamSelection
	for side, ch := range []*CharacterPortrait{p1, p2} {
		teams[side] = match.TeamSelection{Mode: s.teamModes[side], Members: append([]match.Selection(nil), s.rosters[side]...)}
		for len(teams[side].Members) < s.teamCounts[side] {
			selected := match.Selection{DefPath: ch.DefPath, Name: ch.Name, Palette: s.palettes[side]}
			if side == 1 && !s.showP2 && s.title != "Training" {
				selected.AILevel = max(1, min(8, s.engine.cfg.Options.Difficulty))
			}
			teams[side].Members = append(teams[side].Members, selected)
		}
	}
	if s.title == "Team Co-op" || s.title == "Survival Co-op" {
		teams[0] = match.TeamSelection{Mode: match.Simul, Members: []match.Selection{teams[0].Members[0], teams[1].Members[0]}}
		for i := range teams[0].Members {
			teams[0].Members[i].AILevel = 0
		}
		mode, count := s.teamModes[1], s.teamCounts[1]
		if s.title == "Survival Co-op" {
			mode, count = match.Single, 1
		}
		teams[1] = match.TeamSelection{Mode: mode}
		for index := 0; index < count; index++ {
			ch := s.chars[(s.ticks+index)%len(s.chars)]
			if ch.DefPath == "" {
				ch = p2
			}
			teams[1].Members = append(teams[1].Members, match.Selection{DefPath: ch.DefPath, Name: ch.Name, Palette: index + 1, AILevel: max(1, min(8, s.engine.cfg.Options.Difficulty))})
		}
	}
	return teams
}

func (s *SelectState) drawTeamSelection(screen *ebiten.Image) {
	info := s.engine.motif.SelectInfo.TitleFont
	if info[0] < 1 || info[0] >= len(s.engine.motif.Fonts) || s.engine.motif.Fonts[info[0]] == nil {
		return
	}
	f := s.engine.motif.Fonts[info[0]]
	w, h := screen.Bounds().Dx(), screen.Bounds().Dy()
	for side := 0; side < 2; side++ {
		x := w * (side*2 + 1) / 4
		if s.teamMenu {
			title := fmt.Sprintf("P%d TEAM", side+1)
			if side == 1 && (!s.showP2 || s.title == "Team Co-op") {
				title = "OPPONENT TEAM"
			}
			f.Draw(screen, title, x, h/3, info[1], font.AlignCenter)
			for index, name := range []string{"Single", "Simul", "Turns"} {
				text := name
				if index == s.teamCursor[side] {
					text = "> " + name
				}
				if name == "Turns" {
					text += fmt.Sprintf(" (%d)", max(2, s.teamCounts[side]))
				}
				if s.teamReady[side] && index == s.teamCursor[side] {
					text += " OK"
				}
				f.Draw(screen, text, x, h/3+(index+1)*(f.Height+4), info[1], font.AlignCenter)
			}
		} else if s.teamCounts[side] > 1 {
			text := fmt.Sprintf("P%d: %s %d/%d", side+1, s.teamModes[side], min(len(s.rosters[side])+1, s.teamCounts[side]), s.teamCounts[side])
			f.Draw(screen, text, x, h-f.Height*2, info[1], font.AlignCenter)
		}
	}
	if s.teamMenu {
		f.Draw(screen, "Up/Down: mode   Left/Right: turns   Attack/Start: confirm", w/2, h-f.Height*2, info[1], font.AlignCenter)
	}
}
