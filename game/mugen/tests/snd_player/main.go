package main

import (
	"bytes"
	"fmt"
	"log"
	"os"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/audio"
	"github.com/hajimehoshi/ebiten/v2/audio/wav"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/hajimehoshi/ebiten/v2/inpututil"
	"github.com/kazzmir/paintown/game/mugen/snd"
)

const sampleRate = 44100 // Common typical sample rate for MUGEN wavs

type App struct {
	audioContext *audio.Context
	bank         *snd.SoundBank
	currentIndex int
	player       *audio.Player
	err          error
}

func NewApp() *App {
	// Initialize context
	ctx := audio.NewContext(sampleRate)

	path := "data-new/mugen/chars/kfm/kfm.snd"
	if _, err := os.Stat(path); os.IsNotExist(err) {
		// Fallback for when running inside the directory directly
		path = "../../../data-new/mugen/chars/kfm/kfm.snd"
	}
	f, err := os.Open(path)
	app := &App{
		audioContext: ctx,
	}

	if err != nil {
		app.err = err
		return app
	}
	defer f.Close()

	bank, err := snd.Parse(f)
	if err != nil {
		app.err = err
		return app
	}

	app.bank = bank
	return app
}

func (a *App) playCurrent() {
	if a.bank == nil || len(a.bank.Sounds) == 0 {
		return
	}

	sound := a.bank.Sounds[a.currentIndex]

	// Decode WAV with resampling to match the global audioContext
	d, err := wav.DecodeWithSampleRate(sampleRate, bytes.NewReader(sound.Data))
	if err != nil {
		a.err = fmt.Errorf("failed to decode sound %d-%d: %v", sound.Group, sound.Item, err)
		fmt.Println(a.err)
		return
	}

	a.err = nil // Clear error if successful Decode

	if a.player != nil {
		a.player.Close()
	}

	p, err := a.audioContext.NewPlayer(d)
	if err != nil {
		a.err = err
		return
	}

	a.player = p
	a.player.Play()
}

func (a *App) Update() error {
	// Handle Input
	if inpututil.IsKeyJustPressed(ebiten.KeySpace) {
		a.playCurrent()
	}

	if inpututil.IsKeyJustPressed(ebiten.KeyRight) {
		if a.bank != nil && len(a.bank.Sounds) > 0 {
			a.currentIndex = (a.currentIndex + 1) % len(a.bank.Sounds)
			a.playCurrent()
		}
	}

	if inpututil.IsKeyJustPressed(ebiten.KeyLeft) {
		if a.bank != nil && len(a.bank.Sounds) > 0 {
			a.currentIndex--
			if a.currentIndex < 0 {
				a.currentIndex = len(a.bank.Sounds) - 1
			}
			a.playCurrent()
		}
	}

	if inpututil.IsKeyJustPressed(ebiten.KeyEscape) {
		return ebiten.Termination
	}

	return nil
}

func (a *App) Draw(screen *ebiten.Image) {
	if a.err != nil {
		ebitenutil.DebugPrint(screen, fmt.Sprintf("Error: %v", a.err))
		return
	}

	if a.bank == nil || len(a.bank.Sounds) == 0 {
		ebitenutil.DebugPrint(screen, "No sounds loaded.")
		return
	}

	sound := a.bank.Sounds[a.currentIndex]

	status := "Paused"
	if a.player != nil && a.player.IsPlaying() {
		status = "Playing..."
	}

	msg := fmt.Sprintf("M.U.G.E.N SND Player\n\n"+
		"Sound %d/%d\n"+
		"Group: %d | Item: %d\n"+
		"Size: %d bytes\n\n"+
		"Status: %s\n\n"+
		"Controls:\n"+
		"  [SPACE] - Play current sound\n"+
		"  [LEFT]  - Previous sound\n"+
		"  [RIGHT] - Next sound\n"+
		"  [ESC]   - Quit\n",
		a.currentIndex+1, len(a.bank.Sounds),
		sound.Group, sound.Item, len(sound.Data),
		status)

	ebitenutil.DebugPrint(screen, msg)
}

func (a *App) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 320, 240
}

func main() {
	ebiten.SetWindowSize(640, 480)
	ebiten.SetWindowTitle("Paintown MUGEN SND Viewer")

	app := NewApp()

	if err := ebiten.RunGame(app); err != nil {
		log.Fatal(err)
	}
}
