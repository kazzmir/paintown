package main

import (
	"fmt"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
)

type announcementClock int

const (
	roundAnnouncement announcementClock = iota
	fightAnnouncement
	finishAnnouncement
	winAnnouncement
)

// Start delays follow simulation time. Once visible, animation and expiry
// advance on real frames, including CNS pauses and skipped KO frames.
type hudMessage struct {
	key             string
	timer           announcementClock
	delay, duration int
	age             int
	started         bool
	clock           *air.Player
}

type hudSound struct {
	key   string
	timer announcementClock
	delay int
}

func (h *FightHUD) announcementTime(timer announcementClock) int {
	times := h.match.Announcements
	switch timer {
	case roundAnnouncement:
		return times.Round
	case fightAnnouncement:
		return times.Fight
	case finishAnnouncement:
		return times.Finish
	default:
		return times.Win
	}
}

func (h *FightHUD) newMessage(group, key string, timer announcementClock, delay int) {
	message := &hudMessage{key: key, timer: timer, delay: max(0, delay), duration: 1}
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

func (h *FightHUD) finishKey() string {
	if h.match.TimeOver {
		return "TO"
	}
	if h.match.Winner < 0 {
		return "DKO"
	}
	return "KO"
}

func (h *FightHUD) updateMessages(previousRound int) {
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
	if m.SimulationSkipped {
		return
	}
	if m.Announcements.Round >= 0 && h.messages["round.default"] == nil {
		delay := m.Setting("Round", "round.time", 0)
		// Both components exist independently. The default draws above the
		// numbered component and can outlive it (or overlap Fight/control).
		h.newMessage("round", fmt.Sprintf("round%d", m.Round), roundAnnouncement, delay)
		h.newMessage("round.default", "round.default", roundAnnouncement, delay)
	}
	if m.Announcements.Fight >= 0 && h.messages["fight"] == nil {
		h.newMessage("fight", "fight", fightAnnouncement, m.Setting("Round", "fight.time", 0))
	}
	if m.Announcements.Finish >= 0 {
		key := h.finishKey()
		if message := h.messages["finish"]; message == nil || message.key != key {
			// The measured beta gate is eleven post-KO simulation passes;
			// damage grace and the visible component's lifetime are separate.
			h.newMessage("finish", key, finishAnnouncement, 11+m.Setting("Round", "KO.time", 0))
		}
	}
	if m.Announcements.Win >= 0 && h.messages["result"] == nil {
		key := "win"
		if m.Winner < 0 {
			key = "draw"
		}
		h.newMessage("result", key, winAnnouncement, m.Setting("Round", "win.time", 60))
	}
	for _, message := range h.messages {
		if !message.started && h.announcementTime(message.timer) >= message.delay {
			message.started = true
			if message.clock != nil {
				h.clocks["Round/"+message.key] = message.clock
			}
		}
	}
}

func (h *FightHUD) updateAnnouncementSounds() error {
	if h.pendingSounds == nil {
		h.pendingSounds = map[string]hudSound{}
	}
	if h.match.Announcements.Round >= 0 {
		key := fmt.Sprintf("round%d", h.match.Round)
		h.pendingSounds["round"] = hudSound{key, roundAnnouncement, max(0, h.match.Setting("Round", "round.sndtime", 0))}
	}
	if h.match.Announcements.Fight >= 0 {
		h.pendingSounds["fight"] = hudSound{"fight", fightAnnouncement, max(0, h.match.Setting("Round", "fight.sndtime", 0))}
	}
	if h.match.Announcements.Finish >= 0 {
		h.pendingSounds["finish"] = hudSound{h.finishKey(), finishAnnouncement, max(0, h.match.Setting("Round", "KO.sndtime", 0))}
	}
	for _, group := range []string{"round", "fight", "finish"} {
		cue, present := h.pendingSounds[group]
		if present && !h.played[cue.key] && h.announcementTime(cue.timer) >= cue.delay {
			h.played[cue.key] = true
			if err := h.playSound("Round", cue.key+".snd"); err != nil {
				return err
			}
		}
	}
	return nil
}

func (h *FightHUD) drawMessage(screen *ebiten.Image, message *hudMessage, text string, x, y float64) {
	if !message.visible() {
		return
	}
	h.component(screen, "Round", message.key, x, y)
	h.label(screen, "Round", message.key, text, x, y)
}

func (message *hudMessage) visible() bool {
	return message != nil && message.started && (message.duration < 0 || message.age < message.duration)
}
