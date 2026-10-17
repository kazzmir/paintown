package main

import (
	"fmt"
	"image"
	"math"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/render"
	"github.com/kazzmir/paintown/game/mugen/sff"
	"github.com/kazzmir/paintown/game/mugen/sound"
)

type FightHUD struct {
	match         *match.Match
	engine        *Engine
	images        map[*sff.Sprite]*ebiten.Image
	clocks        map[string]*air.Player
	audioOwner    uint64
	played        map[string]bool
	levels        [2]int
	cue           string
	cueTime       int
	linearSampler render.Linear
	renderError   error
}

func NewFightHUD(game *match.Match, engine *Engine) *FightHUD {
	h := &FightHUD{match: game, engine: engine, images: map[*sff.Sprite]*ebiten.Image{}, clocks: map[string]*air.Player{}, audioOwner: game.SoundQueue.NewOwner(), played: map[string]bool{}}
	for slot, p := range game.Players {
		h.levels[slot] = p.Character.Power / 1000
	}
	return h
}

func (h *FightHUD) values(section, key string, fallback ...float64) []float64 {
	a := h.match.Fight.Attribute(section, key)
	if a == nil {
		return fallback
	}
	result := append([]float64(nil), fallback...)
	// Layout attributes are literal tuples, rather than CNS expressions.
	for index, part := range strings.Split(a.RawValue, ",") {
		var value float64
		if _, err := fmt.Sscanf(strings.TrimSpace(part), "%f", &value); err != nil {
			return fallback
		}
		if index < len(result) {
			result[index] = value
		} else {
			result = append(result, value)
		}
	}
	return result
}

func (h *FightHUD) text(section, key, fallback string) string {
	if a := h.match.Fight.Attribute(section, key); a != nil {
		return background.GetString(a.Value)
	}
	return fallback
}

func (h *FightHUD) Update(previous match.Phase, round int) error {
	if h.renderError != nil {
		return h.renderError
	}
	for _, clock := range h.clocks {
		clock.Update()
	}
	if previous != h.match.Phase || round != h.match.Round {
		for key, clock := range h.clocks {
			if strings.HasPrefix(key, "Round/") {
				clock.Reset(1)
			}
		}
	}
	if round != h.match.Round {
		clear(h.played)
		h.cue = ""
		for slot, p := range h.match.Players {
			h.levels[slot] = p.Character.Power / 1000
		}
	}
	var key, timeKey string
	switch h.match.Phase {
	case match.RoundCall:
		key, timeKey = fmt.Sprintf("round%d", h.match.Round), "round.sndtime"
	case match.FightCall:
		key, timeKey = "fight", "fight.sndtime"
	case match.PreOver, match.Over, match.Finished:
		key, timeKey = "KO", "KO.sndtime"
		if h.match.TimeOver {
			key = "TO"
		} else if h.match.Players[0].Character.Life <= 0 && h.match.Players[1].Character.Life <= 0 {
			key = "DKO"
		}
	}
	if key != h.cue {
		h.cue, h.cueTime = key, 0
	} else {
		h.cueTime++
	}
	if key != "" && !h.played[key] && h.cueTime >= h.match.Setting("Round", timeKey, 0) {
		h.played[key] = true
		if err := h.playSound("Round", key+".snd"); err != nil {
			return err
		}
	}
	for slot, p := range h.match.Players {
		level := p.Character.Power / 1000
		if level > h.levels[slot] {
			if err := h.playSound("Powerbar", fmt.Sprintf("level%d.snd", level)); err != nil {
				return err
			}
		}
		h.levels[slot] = level
	}
	return nil
}

func (h *FightHUD) playSound(section, key string) error {
	if h.match.Fight.Attribute(section, key) == nil {
		return nil
	}
	ids := h.values(section, key, -1, 0)
	group, item := int(ids[0]), int(ids[1])
	bank := h.match.Fight.Sounds
	if group < 0 || bank == nil || bank.Map[fmt.Sprintf("%d-%d", group, item)] == nil {
		return nil
	}
	return h.match.SoundQueue.Push(sound.Command{Operation: sound.Play, Owner: h.audioOwner, Bank: bank, Group: group, Item: item, Channel: -1, VolumeScale: 100, Frequency: 1})
}

func (h *FightHUD) sprite(screen *ebiten.Image, sprite *sff.Sprite, x, y, sx, sy float64) {
	if sprite == nil || sprite.Image == nil {
		return
	}
	img := h.images[sprite]
	if img == nil {
		img = ebiten.NewImageFromImage(sprite.Image)
		h.images[sprite] = img
	}
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Translate(-float64(sprite.XAxis), -float64(sprite.YAxis))
	op.GeoM.Scale(sx, sy)
	op.GeoM.Translate(x, y)
	if _, indexed := sprite.Image.(*image.Paletted); indexed {
		screen.DrawImage(img, op)
	} else if err := h.linearSampler.DrawImage(screen, img, op); err != nil {
		h.renderError = fmt.Errorf("fight HUD sprite %d,%d sampler: %w", sprite.GroupNumber, sprite.ImageNumber, err)
	}
}

func findSprite(archive *sff.SFF, group, item int) *sff.Sprite {
	if archive != nil {
		for i := range archive.Sprites {
			sprite := &archive.Sprites[i]
			if int(sprite.GroupNumber) == group && int(sprite.ImageNumber) == item {
				return sprite
			}
		}
	}
	return nil
}

func (h *FightHUD) component(screen *ebiten.Image, section, key string, x, y float64) {
	offset := h.values(section, key+".offset", 0, 0)
	scale := h.values(section, key+".scale", 1, 1)
	facing := h.values(section, key+".facing", 1)[0]
	x, y = x+offset[0], y+offset[1]
	var sprite *sff.Sprite
	if a := h.match.Fight.Attribute(section, key+".anim"); a != nil {
		id := int(background.GetFloat(a.Value))
		clockKey := section + "/" + key
		clock := h.clocks[clockKey]
		if clock == nil {
			action := h.match.Fight.Animations.Actions[id]
			if action == nil {
				return
			}
			clock = air.NewPlayer(action)
			h.clocks[clockKey] = clock
		}
		if clock.Error != nil {
			return
		}
		el := clock.Action.Elements[clock.CurrentIdx]
		x += float64(el.XOffset) * scale[0] * facing
		y += float64(el.YOffset) * scale[1]
		if el.FlipX {
			facing = -facing
		}
		if el.FlipY {
			scale[1] = -scale[1]
		}
		sprite = findSprite(h.match.Fight.Sprites, el.Group, el.Image)
	} else if h.match.Fight.Attribute(section, key+".spr") != nil {
		ids := h.values(section, key+".spr", 0, 0)
		sprite = findSprite(h.match.Fight.Sprites, int(ids[0]), int(ids[1]))
	}
	h.sprite(screen, sprite, x, y, scale[0]*facing, scale[1])
}

func (h *FightHUD) label(screen *ebiten.Image, section, key, text string, x, y float64) {
	info := h.values(section, key+".font", 0, 0, 0)
	index := int(info[0])
	if index < 1 || index >= len(h.match.Fight.Fonts) || h.match.Fight.Fonts[index] == nil {
		return
	}
	offset := h.values(section, key+".offset", 0, 0)
	align := font.AlignCenter
	if info[2] > 0 {
		align = font.AlignLeft
	} else if info[2] < 0 {
		align = font.AlignRight
	}
	h.match.Fight.Fonts[index].Draw(screen, text, int(x+offset[0]), int(y+offset[1]), int(info[1]), align)
}

func (h *FightHUD) bar(screen *ebiten.Image, section, prefix string, fraction float64) {
	pos := h.values(section, prefix+".pos", 0, 0)
	h.component(screen, section, prefix+".bg0", pos[0], pos[1])
	h.component(screen, section, prefix+".bg1", pos[0], pos[1])
	rangeX := h.values(section, prefix+".range.x", 0, 0)
	fraction = math.Max(0, math.Min(1, fraction))
	start, end := pos[0]+rangeX[0], pos[0]+rangeX[0]+(rangeX[1]-rangeX[0])*fraction
	if fraction > 0 {
		clip := image.Rect(int(math.Floor(math.Min(start, end))), 0, int(math.Ceil(math.Max(start, end)))+1, screen.Bounds().Dy()).Intersect(screen.Bounds())
		if !clip.Empty() {
			target := screen.SubImage(clip).(*ebiten.Image)
			h.component(target, section, prefix+".mid", pos[0], pos[1])
			h.component(target, section, prefix+".front", pos[0], pos[1])
		}
	}
}

func (h *FightHUD) Draw(screen *ebiten.Image) {
	m := h.match
	for slot, p := range m.Players {
		prefix := fmt.Sprintf("p%d", slot+1)
		c := p.Character
		h.bar(screen, "Lifebar", prefix, float64(c.Life)/float64(max(1, c.MaxLife)))
		h.bar(screen, "Powerbar", prefix, float64(c.Power)/float64(max(1, c.MaxPower)))
		pos := h.values("Powerbar", prefix+".pos", 0, 0)
		h.label(screen, "Powerbar", prefix+".counter", fmt.Sprint(c.Power/1000), pos[0], pos[1])
		pos = h.values("Face", prefix+".pos", 0, 0)
		h.component(screen, "Face", prefix+".bg", pos[0], pos[1])
		h.component(screen, "Face", prefix+".bg0", pos[0], pos[1])
		h.component(screen, "Face", prefix+".bg1", pos[0], pos[1])
		ids := h.values("Face", prefix+".face.spr", 9000, 0)
		offset := h.values("Face", prefix+".face.offset", 0, 0)
		scale := h.values("Face", prefix+".face.scale", 1, 1)
		facing := h.values("Face", prefix+".face.facing", 1)[0]
		portraitScale := float64(screen.Bounds().Dx()) / float64(p.LocalCoord.Width)
		p.DrawSprite(screen, c, int(ids[0]), int(ids[1]), pos[0]+offset[0], pos[1]+offset[1], scale[0]*facing*portraitScale, scale[1]*portraitScale)
		pos = h.values("Name", prefix+".pos", 0, 0)
		name := p.Definition.Info["displayname"]
		if name == "" {
			name = p.Definition.Info["name"]
		}
		h.label(screen, "Name", prefix+".name", name, pos[0], pos[1])
		pos = h.values("WinIcon", prefix+".pos", 0, 0)
		spacing := h.values("WinIcon", prefix+".iconoffset", 0, 0)
		for index := 0; index < m.Wins[slot]; index++ {
			h.component(screen, "WinIcon", prefix+".n", pos[0]+float64(index)*spacing[0], pos[1]+float64(index)*spacing[1])
		}
	}
	pos := h.values("Time", "pos", 0, 0)
	h.component(screen, "Time", "bg", pos[0], pos[1])
	timeText := fmt.Sprint(m.TimeLeft)
	if m.TimeLeft < 0 {
		timeText = "∞"
	}
	h.label(screen, "Time", "counter", timeText, pos[0], pos[1])
	pos = h.values("Round", "pos", 0, 0)
	switch m.Phase {
	case match.RoundCall:
		h.label(screen, "Round", "round.default", strings.ReplaceAll(h.text("Round", "round.default.text", "Round %i"), "%i", fmt.Sprint(m.Round)), pos[0], pos[1])
		h.component(screen, "Round", "round.default", pos[0], pos[1])
	case match.FightCall:
		h.component(screen, "Round", "fight", pos[0], pos[1])
	case match.PreOver:
		key := "KO"
		if m.TimeOver {
			key = "TO"
		} else if m.Winner < 0 {
			key = "DKO"
		}
		h.component(screen, "Round", key, pos[0], pos[1])
		h.label(screen, "Round", key, h.text("Round", key+".text", ""), pos[0], pos[1])
	case match.Over, match.Finished:
		key, text := "draw", h.text("Round", "draw.text", "Draw Game")
		if m.Winner >= 0 {
			key = "win"
			name := m.Players[m.Winner].Definition.Info["displayname"]
			text = strings.ReplaceAll(h.text("Round", "win.text", "%s Wins"), "%s", name)
		}
		h.label(screen, "Round", key, text, pos[0], pos[1])
		if m.Phase == match.Finished {
			info := h.engine.motif.SelectInfo.TitleFont
			if info[0] > 0 && info[0] < len(h.engine.motif.Fonts) && h.engine.motif.Fonts[info[0]] != nil {
				h.engine.motif.Fonts[info[0]].Draw(screen, "Start: rematch   Esc: menu", screen.Bounds().Dx()/2, screen.Bounds().Dy()-12, info[1], font.AlignCenter)
			}
		}
	}
}
