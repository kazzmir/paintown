package storyboard

import (
	"fmt"
	"image"
	"image/color"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

// FadeTool handles fade-in and fade-out transitions.
type FadeTool struct {
	FadeInTime   int
	FadeOutTime  int
	FadeInColor  color.RGBA
	FadeOutColor color.RGBA
	Ticker       int
	State        int // 0=fadein, 1=normal, 2=fadeout, 3=done
}

const (
	FadeStateIn   = 0
	FadeStateNorm = 1
	FadeStateOut  = 2
	FadeStateDone = 3
)

func (f *FadeTool) Update() {
	f.Ticker++
	switch f.State {
	case FadeStateIn:
		if f.Ticker >= f.FadeInTime {
			f.State = FadeStateNorm
			f.Ticker = 0
		}
	case FadeStateOut:
		if f.Ticker >= f.FadeOutTime {
			f.State = FadeStateDone
		}
	}
}

func (f *FadeTool) StartFadeOut() {
	if f.State == FadeStateNorm {
		f.State = FadeStateOut
		f.Ticker = 0
	}
}

func (f *FadeTool) Draw(screen *ebiten.Image) {
	var alpha float64
	var c color.RGBA
	switch f.State {
	case FadeStateIn:
		if f.FadeInTime > 0 {
			alpha = 1.0 - float64(f.Ticker)/float64(f.FadeInTime)
		}
		c = f.FadeInColor
	case FadeStateOut:
		if f.FadeOutTime > 0 {
			alpha = float64(f.Ticker) / float64(f.FadeOutTime)
		}
		c = f.FadeOutColor
	default:
		return
	}
	if alpha <= 0 {
		return
	}

	w, h := screen.Bounds().Dx(), screen.Bounds().Dy()
	overlay := ebiten.NewImage(w, h)
	overlay.Fill(color.RGBA{R: c.R, G: c.G, B: c.B, A: uint8(alpha * 255)})
	op := &ebiten.DrawImageOptions{}
	op.Blend = ebiten.BlendSourceOver
	screen.DrawImage(overlay, op)
}

// Layer is an animated element displayed at a specific time in a scene.
type Layer struct {
	Offset    [2]float64
	StartTime int
	Player    *air.Player
	Enabled   bool

	// Sprite data for drawing
	Sprites    map[string]*background.SpriteImages
	RawSprites map[string]*sff.Sprite
	Screen     [2]float64 // [screenW, screenH]
}

func (l *Layer) Update(ticker int) {
	if ticker >= l.StartTime && !l.Enabled {
		l.Enabled = true
	}
	if l.Enabled && l.Player != nil {
		l.Player.Update()
	}
}

func (l *Layer) Draw(screen *ebiten.Image, defaultX, defaultY float64) {
	if !l.Enabled || l.Player == nil {
		return
	}
	el := l.Player.CurrentElement()
	if el == nil {
		return
	}
	key := fmt.Sprintf("%d-%d", el.Group, el.Image)
	si, ok := l.Sprites[key]
	if !ok {
		return
	}
	img := si.Masked
	if img == nil {
		return
	}

	pivotX, pivotY := 0, 0
	if l.RawSprites != nil {
		if spr, ok := l.RawSprites[key]; ok {
			pivotX = int(spr.XAxis)
			pivotY = int(spr.YAxis)
		}
	}

	x := defaultX + l.Offset[0] + float64(el.XOffset) - float64(pivotX)
	y := defaultY + l.Offset[1] + float64(el.YOffset) - float64(pivotY)

	op := &ebiten.DrawImageOptions{}
	op.GeoM.Translate(x, y)
	op.Filter = ebiten.FilterNearest

	// Optional additive blend based on action
	if el.Flags != "" {
		f := strings.ToLower(el.Flags)
		if strings.Contains(f, "a") {
			op.Blend = ebiten.BlendLighter
		}
	}

	screen.DrawImage(img, op)
}

// Scene is a single scene in a storyboard.
type Scene struct {
	EndTime         int
	Fader           FadeTool
	Background      *background.Background
	Layers          []*Layer
	DefaultPosition [2]float64
	MusicFile       string
	MusicLoop       bool
	ClearColor      color.RGBA
	ClearColorSet   bool

	ticker int

	// Sprite images keyed "group-image"
	SpriteImages map[string]*background.SpriteImages
	RawSprites   map[string]*sff.Sprite
}

func (s *Scene) Reset() {
	s.ticker = 0
	s.Fader.State = FadeStateIn
	s.Fader.Ticker = 0
	for _, l := range s.Layers {
		l.Enabled = false
		if l.Player != nil {
			l.Player.CurrentIdx = 0
			l.Player.TickCounter = 0
		}
	}
}

// Update advances the scene by one tick. Returns true when the scene is finished.
func (s *Scene) Update() bool {
	if s.Background != nil {
		s.Background.Update()
	}
	for _, l := range s.Layers {
		l.Update(s.ticker)
	}
	s.Fader.Update()

	// Trigger fade-out at the right time
	if s.ticker == s.EndTime-s.Fader.FadeOutTime {
		s.Fader.StartFadeOut()
	}

	s.ticker++
	return s.ticker >= s.EndTime
}

// Draw renders the scene to the screen.
func (s *Scene) Draw(screen *ebiten.Image) {
	if s.ClearColorSet {
		screen.Fill(s.ClearColor)
	}

	// Build sprite map for background rendering
	if s.Background != nil {
		spriteMap := s.buildSpriteMap(screen)
		for _, el := range s.Background.Elements {
			if el.GetLayerNo() == 0 {
				el.Draw(screen, 0, 0, spriteMap)
			}
		}
	}

	// Draw layers
	for _, l := range s.Layers {
		l.Draw(screen, s.DefaultPosition[0], s.DefaultPosition[1])
	}

	// Foreground
	if s.Background != nil {
		spriteMap := s.buildSpriteMap(screen)
		for _, el := range s.Background.Elements {
			if el.GetLayerNo() == 1 {
				el.Draw(screen, 0, 0, spriteMap)
			}
		}
	}

	s.Fader.Draw(screen)
}

func (s *Scene) buildSpriteMap(screen *ebiten.Image) map[string]*background.SpriteImages {
	return s.SpriteImages
}

// Storyboard holds all scenes and sprite data for a cutscene.
type Storyboard struct {
	StartScene int
	Scenes     []*Scene

	// Raw sprites from the SFF used by this storyboard
	Sprites    []*sff.Sprite
	SpriteMap  map[string]*background.SpriteImages
	RawSprites map[string]*sff.Sprite
}

// NewStoryboard creates a Storyboard from parsed data.
// sprites is the SFF sprite list, images is a pre-built ebiten image map.
func NewStoryboard(sprites []*sff.Sprite, images map[string]*ebiten.Image) *Storyboard {
	sb := &Storyboard{
		Sprites:    sprites,
		SpriteMap:  make(map[string]*background.SpriteImages),
		RawSprites: make(map[string]*sff.Sprite),
	}
	for key, img := range images {
		sb.SpriteMap[key] = &background.SpriteImages{
			Masked: img,
			Opaque: img,
		}
	}
	for _, sp := range sprites {
		key := fmt.Sprintf("%d-%d", sp.GroupNumber, sp.ImageNumber)
		sb.RawSprites[key] = sp
	}
	return sb
}

// Run executes the storyboard, advancing through scenes.
// Returns true if it should transition to the next screen, false if interrupted.
func (sb *Storyboard) Run(screen *ebiten.Image, input InputState) bool {
	return false // will be called frame by frame externally
}

// InputState allows the storyboard to check if the player wants to skip.
type InputState struct {
	Skip bool
}

// IsSkipRequested returns true if input requests a skip.
func (s InputState) IsSkipRequested() bool { return s.Skip }

// Player manages playback of a storyboard frame-by-frame.
type Player struct {
	sb         *Storyboard
	currentIdx int
	done       bool
}

func NewPlayer(sb *Storyboard) *Player {
	p := &Player{sb: sb, currentIdx: sb.StartScene}
	if len(sb.Scenes) > 0 {
		sb.Scenes[p.currentIdx].Reset()
	}
	return p
}

// Update advances the storyboard by one tick. Returns true when done.
func (p *Player) Update(input InputState) bool {
	if p.done || len(p.sb.Scenes) == 0 {
		return true
	}

	if input.IsSkipRequested() {
		p.currentIdx++
		if p.currentIdx >= len(p.sb.Scenes) {
			p.done = true
			return true
		}
		p.sb.Scenes[p.currentIdx].Reset()
		return false
	}

	scene := p.sb.Scenes[p.currentIdx]
	finished := scene.Update()
	if finished {
		p.currentIdx++
		if p.currentIdx >= len(p.sb.Scenes) {
			p.done = true
			return true
		}
		p.sb.Scenes[p.currentIdx].Reset()
	}
	return false
}

// Draw renders the current scene.
func (p *Player) Draw(screen *ebiten.Image) {
	if p.done || len(p.sb.Scenes) == 0 {
		return
	}
	// Fill with black as default
	screen.Fill(color.RGBA{0, 0, 0, 255})
	p.sb.Scenes[p.currentIdx].Draw(screen)
}

// SubImage helper
func SubImage(img *ebiten.Image, r image.Rectangle) *ebiten.Image {
	return img.SubImage(r).(*ebiten.Image)
}
