package main

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

// Direct modes use the same request/team/input path as selection. Partners are
// CPU-owned, except P3 in co-op, which uses the second human input adapter.
func quickMatchRequest(raw, mode, stage, partner3, partner4 string, cfg *config.MugenConfig) (match.Request, error) {
	paths := parsers.SplitList(raw)
	if len(paths) != 2 {
		return match.Request{}, fmt.Errorf("--quick-match requires two character entries")
	}
	request := match.Request{Mode: "Versus Mode", StagePath: strings.TrimSpace(stage)}
	switch strings.ToLower(strings.TrimSpace(mode)) {
	case "", "versus", "vs":
	case "watch":
		request.Mode = "Watch Mode"
	case "training":
		request.Mode = "Training"
	case "coop", "co-op":
		if strings.TrimSpace(partner3) == "" {
			return request, fmt.Errorf("direct co-op requires --p3 for the second human")
		}
		request.Mode = "Team Co-op"
	default:
		return request, fmt.Errorf("unknown direct mode %q (versus, watch, training, coop)", mode)
	}
	if request.Mode == "Training" && (partner3 != "" || partner4 != "") {
		return request, fmt.Errorf("direct Training uses one fighter per side; omit --p3/--p4")
	}
	if request.StagePath == "" {
		request.StagePath = cfg.Debug.StartStage
	}
	if request.StagePath == "" {
		request.StagePath = "stages/kfm.def"
	}
	level := max(1, min(8, cfg.Options.Difficulty))
	for slot, path := range paths {
		path = strings.Trim(strings.TrimSpace(path), "\"")
		if path == "" {
			return request, fmt.Errorf("--quick-match player %d is blank", slot+1)
		}
		request.Players[slot] = match.Selection{DefPath: path, Palette: slot + 1}
		if request.Mode == "Watch Mode" || request.Mode == "Team Co-op" && slot == 1 {
			request.Players[slot].AILevel = level
		}
	}
	for side, partner := range []string{partner3, partner4} {
		partner = strings.Trim(strings.TrimSpace(partner), "\"")
		if partner == "" {
			continue
		}
		selected := match.Selection{DefPath: partner, Palette: side + 3, AILevel: level}
		if request.Mode == "Team Co-op" && side == 0 {
			selected.AILevel = 0
		}
		request.Teams[side] = match.TeamSelection{Mode: match.Simul, Members: []match.Selection{request.Players[side], selected}}
	}
	return request, nil
}
