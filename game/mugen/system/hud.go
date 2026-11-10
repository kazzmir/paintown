package main

import (
	"fmt"
	"image"
	"math"
	"sort"
	"strconv"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/character"
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
	backgrounds   map[string][]string
	messages      map[string]*hudMessage
	audioOwner    uint64
	played        map[string]bool
	levels        [2]int
	pendingSounds map[string]hudSound
	combos        [2]hudCombo
	comboSources  map[*character.Character]uint64
	linearSampler render.Linear
	indexSampler  render.Nearest
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
		if strings.TrimSpace(part) == "" {
			continue // optional tuple components retain their own defaults
		}
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

func (h *FightHUD) Update(_ match.Phase, round int) error {
	if h.renderError != nil {
		return h.renderError
	}
	if round != h.match.Round {
		clear(h.played)
		clear(h.pendingSounds)
		h.combos = [2]hudCombo{}
		h.comboSources = nil
		for slot, p := range h.match.Players {
			h.levels[slot] = p.Character.Power / 1000
		}
	}
	h.updateMessages(round)
	h.updateCombos()
	if h.match.SimulationSkipped {
		return nil
	}
	for key, clock := range h.clocks {
		if !strings.HasPrefix(key, "Round/") {
			clock.Update()
		}
	}
	if err := h.updateAnnouncementSounds(); err != nil {
		return err
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

func (h *FightHUD) sprite(screen *ebiten.Image, sprite *sff.Sprite, x, y, sx, sy float64, blend string, alphaSrc, alphaDst int, angle ...float64) {
	if sprite == nil || sprite.Image == nil {
		return
	}
	img := h.images[sprite]
	if img == nil {
		img = ebiten.NewImageFromImage(sff.UploadPixels(sprite.Image))
		h.images[sprite] = img
	}
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Translate(-float64(sprite.XAxis), -float64(sprite.YAxis))
	op.GeoM.Scale(sx, sy)
	if len(angle) > 0 {
		op.GeoM.Rotate(-angle[0] * math.Pi / 180)
	}
	op.GeoM.Translate(x, y)
	destination := render.ApplyBlend(op, blend, alphaSrc, alphaDst)
	transform := h.match.Timeline.AllPalette.Transform()
	if transform.Enabled || destination >= 0 {
		style := render.DrawStyle{DestinationFactor: destination, Palette: transform}
		var err error
		if _, indexed := sprite.Image.(*image.Paletted); indexed {
			err = h.indexSampler.DrawStyledImage(screen, img, op, style)
		} else {
			err = h.linearSampler.DrawStyledImage(screen, img, op, style)
		}
		if err != nil {
			h.renderError = fmt.Errorf("fight HUD palette: %w", err)
		}
	} else if _, indexed := sprite.Image.(*image.Paletted); indexed {
		screen.DrawImage(img, op)
	} else if err := h.linearSampler.DrawImage(screen, img, op, destination); err != nil {
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
	var blend string
	var alphaSrc, alphaDst int
	var angle float64
	if h.match.Fight.Attribute(section, key+".anim") != nil {
		id := int(h.values(section, key+".anim", 0)[0])
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
		visual := clock.Visual()
		el := visual.Element
		if el == nil {
			return
		}
		blend, alphaSrc, alphaDst = el.Blend, visual.AlphaSrc, visual.AlphaDst
		x += visual.X * scale[0] * facing
		y += visual.Y * scale[1]
		scale[0] *= visual.ScaleX
		scale[1] *= visual.ScaleY
		angle = visual.Angle * facing
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
	h.sprite(screen, sprite, x, y, scale[0]*facing, scale[1], blend, alphaSrc, alphaDst, angle)
}

func (h *FightHUD) label(screen *ebiten.Image, section, key, text string, x, y float64) {
	info := h.values(section, key+".font", 0, 0, 0, 256, 256, 256)
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
	if err := h.match.Fight.Fonts[index].DrawWithColor(screen, text, int(x+offset[0]), int(y+offset[1]), int(info[1]), align, [3]float32{float32(info[3]), float32(info[4]), float32(info[5])}, h.match.Timeline.AllPalette.Transform()); err != nil {
		h.renderError = err
	}
}

// Numbered backgrounds may be sparse and extend beyond bg1. Cache authored
// component names once per HUD, preserving numeric order without scanning an
// arbitrary maximum on every draw. Attribute lookup keeps first-field policy.
func (h *FightHUD) numberedBackgrounds(screen *ebiten.Image, section, prefix string, x, y float64) {
	if h.backgrounds == nil {
		h.backgrounds = map[string][]string{}
	}
	cacheKey := section + "/" + prefix
	keys, ready := h.backgrounds[cacheKey]
	if !ready {
		seen := map[int]bool{}
		start := strings.ToLower(prefix) + ".bg"
		for _, group := range h.match.Fight.AST.Sections {
			if !strings.EqualFold(strings.TrimSpace(group.Name), section) {
				continue
			}
			for _, attribute := range group.Attributes {
				name := strings.ToLower(attribute.ID.String())
				if !strings.HasPrefix(name, start) {
					continue
				}
				number, field, found := strings.Cut(strings.TrimPrefix(name, start), ".")
				if !found || field != "anim" && field != "spr" {
					continue
				}
				if index, err := strconv.Atoi(number); err == nil && index >= 0 {
					seen[index] = true
				}
			}
		}
		indices := make([]int, 0, len(seen))
		for index := range seen {
			indices = append(indices, index)
		}
		sort.Ints(indices)
		for _, index := range indices {
			keys = append(keys, fmt.Sprintf("%s.bg%d", prefix, index))
		}
		h.backgrounds[cacheKey] = keys
	}
	for _, key := range keys {
		h.component(screen, section, key, x, y)
	}
}

func (h *FightHUD) bar(screen *ebiten.Image, section, prefix string, fraction float64) {
	pos := h.values(section, prefix+".pos", 0, 0)
	h.numberedBackgrounds(screen, section, prefix, pos[0], pos[1])
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

func (h *FightHUD) teamSection(side int, base string) string {
	mode := h.match.Teams[side].Mode
	if mode == match.Simul {
		return "Simul " + base
	}
	if mode == match.Turns {
		return "Turns " + base
	}
	return base
}

func (h *FightHUD) portrait(screen *ebiten.Image, player *character.Player, section, key string, x, y float64) {
	ids := h.values(section, key+".spr", 9000, 0)
	offset := h.values(section, key+".offset", 0, 0)
	scale := h.values(section, key+".scale", 1, 1)
	facing := h.values(section, key+".facing", 1)[0]
	ratio := float64(screen.Bounds().Dx()) / float64(player.LocalCoord.Width)
	player.DrawSprite(screen, player.Character, int(ids[0]), int(ids[1]), x+offset[0], y+offset[1], scale[0]*facing*ratio, scale[1]*ratio)
}

func (h *FightHUD) drawBars(screen *ebiten.Image) {
	m := h.match
	for slot, p := range m.ActivePlayers() {
		if p == nil {
			continue
		}
		side := slot % 2
		prefix := fmt.Sprintf("p%d", slot+1)
		section := h.teamSection(side, "Lifebar")
		if m.Fight.Attribute(section, prefix+".pos") == nil {
			section = "Lifebar"
		}
		h.bar(screen, section, prefix, float64(p.Character.Life)/float64(max(1, p.Character.MaxLife)))
		if slot < 2 {
			h.bar(screen, "Powerbar", prefix, float64(p.Character.GetPower())/float64(max(1, p.Character.MaxPower)))
			pos := h.values("Powerbar", prefix+".pos", 0, 0)
			h.label(screen, "Powerbar", prefix+".counter", fmt.Sprint(p.Character.GetPower()/1000), pos[0], pos[1])
		}
		section = h.teamSection(side, "Face")
		if m.Fight.Attribute(section, prefix+".pos") == nil {
			section = "Face"
		}
		pos := h.values(section, prefix+".pos", 0, 0)
		h.component(screen, section, prefix+".bg", pos[0], pos[1])
		h.numberedBackgrounds(screen, section, prefix, pos[0], pos[1])
		h.portrait(screen, p, section, prefix+".face", pos[0], pos[1])
		section = h.teamSection(side, "Name")
		if m.Fight.Attribute(section, prefix+".pos") == nil {
			section = "Name"
		}
		pos = h.values(section, prefix+".pos", 0, 0)
		name := p.Definition.Info["displayname"]
		if name == "" {
			name = p.Character.Name
		}
		h.label(screen, section, prefix+".name", name, pos[0], pos[1])
		if slot < 2 {
			pos = h.values("WinIcon", prefix+".pos", 0, 0)
			spacing := h.values("WinIcon", prefix+".iconoffset", 0, 0)
			for index := 0; index < m.Wins[side]; index++ {
				h.component(screen, "WinIcon", prefix+".n", pos[0]+float64(index)*spacing[0], pos[1]+float64(index)*spacing[1])
			}
		}
	}
	for side, team := range m.Teams {
		if team.Mode != match.Turns {
			continue
		}
		prefix := fmt.Sprintf("p%d.teammate", side+1)
		pos := h.values("Turns Face", prefix+".pos", 0, 0)
		spacing := h.values("Turns Face", prefix+".spacing", 0, 0)
		display := 0
		for index, player := range team.Members {
			if index == team.Active {
				continue
			}
			x, y := pos[0]+float64(display)*spacing[0], pos[1]+float64(display)*spacing[1]
			h.numberedBackgrounds(screen, "Turns Face", prefix, x, y)
			h.portrait(screen, player, "Turns Face", prefix+".face", x, y)
			if index < team.Active {
				h.component(screen, "Turns Face", prefix+".ko", x, y)
			}
			display++
		}
	}
	pos := h.values("Time", "pos", 0, 0)
	h.component(screen, "Time", "bg", pos[0], pos[1])
	timeText := fmt.Sprint(m.TimeLeft)
	if m.TimeLeft < 0 {
		timeText = "∞"
	}
	h.label(screen, "Time", "counter", timeText, pos[0], pos[1])
}

func (h *FightHUD) Draw(screen *ebiten.Image) {
	m := h.match
	if !m.Timeline.NoBarDisplay && (h.engine == nil || !h.engine.hideFightBars && h.engine.demoShowsBars()) {
		h.drawBars(screen)
		h.drawCombos(screen)
	}
	pos := h.values("Round", "pos", 0, 0)
	for _, group := range []string{"round", "round.default", "fight", "finish", "result"} {
		message := h.messages[group]
		if message == nil {
			continue
		}
		fallback := ""
		if message.key == "round.default" {
			fallback = "Round %i"
		} else if message.key == "win" {
			fallback = "%s Wins"
		} else if message.key == "draw" {
			fallback = "Draw Game"
		}
		text := strings.ReplaceAll(h.text("Round", message.key+".text", fallback), "%i", fmt.Sprint(m.Round))
		if message.key == "win" && m.Winner >= 0 {
			name := m.Players[m.Winner].Character.Name
			if definition := m.Players[m.Winner].Definition; definition != nil && definition.Info["displayname"] != "" {
				name = definition.Info["displayname"]
			}
			text = strings.ReplaceAll(text, "%s", name)
		}
		h.drawMessage(screen, message, text, pos[0], pos[1])
	}
	if m.Phase == match.Finished && h.engine != nil {
		info := h.engine.motif.SelectInfo.TitleFont
		if info[0] > 0 && info[0] < len(h.engine.motif.Fonts) && h.engine.motif.Fonts[info[0]] != nil {
			h.engine.motif.Fonts[info[0]].Draw(screen, "Start: rematch   Esc: menu", screen.Bounds().Dx()/2, screen.Bounds().Dy()-12, info[1], font.AlignCenter)
		}
	}
}
