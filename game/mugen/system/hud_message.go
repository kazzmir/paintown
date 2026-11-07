package main

import (
	"fmt"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/match"
)

// A timed message waits on the round's simulation clock. Once visible, its
// animation and display lifetime advance on real frames, including CNS pause
// and skipped KO simulation frames. Static HUD components use their own clocks.
type hudMessage struct {
	key             string
	phase           match.Phase
	delay, duration int
	age             int
	started         bool
	clock           *air.Player
}

func (h *FightHUD) newMessage(group, key string, phase match.Phase, delay int) {
	message := &hudMessage{key: key, phase: phase, delay: max(0, delay), duration: 1}
	if attribute := h.match.Fight.Attribute("Round", key+".anim"); attribute != nil {
		id := int(background.GetFloat(attribute.Value))
		if action := h.match.Fight.Animations.Actions[id]; action != nil {
			message.clock = air.NewPlayer(action)
			message.duration = 0
			for _, element := range action.Elements {
				if element.Time == -1 {
					message.duration = -1
					break
				}
				message.duration += element.Time
			}
		}
	}
	if h.match.Fight.Attribute("Round", key+".displaytime") != nil {
		message.duration = int(h.values("Round", key+".displaytime", 1)[0])
	}
	h.messages[group] = message
	delete(h.clocks, "Round/"+key)
}

func (h *FightHUD) updateMessages(previous match.Phase, previousRound int) {
	m := h.match
	if h.messages == nil || previousRound != m.Round {
		h.messages = map[string]*hudMessage{}
		for key := range h.clocks {
			if strings.HasPrefix(key, "Round/") {
				delete(h.clocks, key)
			}
		}
	}
	for _, message := range h.messages {
		if message.started {
			message.age++
			if message.clock != nil {
				message.clock.Update()
			}
		}
	}
	if !m.SimulationSkipped {
		switch m.Phase {
		case match.RoundCall:
			if previous != m.Phase || h.messages["round"] == nil {
				key := fmt.Sprintf("round%d", m.Round)
				if m.Fight.Attribute("Round", key+".anim") == nil && m.Fight.Attribute("Round", key+".spr") == nil && m.Fight.Attribute("Round", key+".font") == nil {
					key = "round.default"
				}
				h.newMessage("round", key, m.Phase, m.Setting("Round", "round.time", 0))
			}
		case match.FightCall:
			if previous != m.Phase || h.messages["fight"] == nil {
				newDelay := m.Setting("Round", "fight.time", 0)
				h.newMessage("fight", "fight", m.Phase, newDelay)
			}
		case match.PreOver:
			key := "KO"
			if m.TimeOver {
				key = "TO"
			} else if m.Winner < 0 {
				key = "DKO"
			}
			if message := h.messages["finish"]; previous != m.Phase || message == nil || message.key != key {
				// The measured beta gate is eleven post-KO simulation passes
				// for over.hittime0/3/10/20; damage grace is independent.
				h.newMessage("finish", key, m.Phase, 11+m.Setting("Round", "KO.time", 0))
			}
		}
		for _, message := range h.messages {
			if !message.started && message.phase == m.Phase && m.PhaseTime >= message.delay {
				message.started = true
				if message.clock != nil {
					h.clocks["Round/"+message.key] = message.clock
				}
			}
		}
	}
}

func (h *FightHUD) drawMessage(screen *ebiten.Image, message *hudMessage, text string, x, y float64) {
	if message == nil || !message.started || (message.duration >= 0 && message.age >= message.duration) {
		return
	}
	h.component(screen, "Round", message.key, x, y)
	h.label(screen, "Round", message.key, text, x, y)
}
