package main

import (
	"fmt"
	"time"

	"github.com/hajimehoshi/ebiten/v2/audio"
	"github.com/kazzmir/paintown/game/mugen/config"
	"github.com/kazzmir/paintown/game/mugen/sound"
)

type engineSound struct {
	queue     *sound.Queue
	mixer     *sound.Mixer
	player    *audio.Player
	menuOwner uint64
	warnings  map[string]bool
}

func (e *Engine) soundConfig() config.SoundConfig {
	if e.cfg == nil {
		return config.DefaultSoundConfig()
	}
	return e.cfg.Sound
}

// CPU sound initialization is also used by injected production-flow tests.
// Only the application entry point creates a real streaming device player.
func (e *Engine) ensureSound() error {
	if e.sound != nil {
		return nil
	}
	cfg := e.soundConfig()
	if err := cfg.Validate(); err != nil {
		return fmt.Errorf("sound: %w", err)
	}
	mixer, err := sound.NewMixer(sound.Options{SampleRate: cfg.SampleRate, MaxVoices: cfg.WavChannels, Volume: cfg.EffectsVolume(), Stereo: cfg.StereoEffects, Reverse: cfg.ReverseStereo, PanningWidth: float32(cfg.PanningWidth)})
	if err != nil {
		return err
	}
	queue := &sound.Queue{}
	e.sound = &engineSound{queue: queue, mixer: mixer, menuOwner: queue.NewOwner(), warnings: map[string]bool{}}
	return nil
}

func (e *Engine) startSoundDevice() error {
	if err := e.ensureSound(); err != nil {
		return err
	}
	if !e.soundConfig().Sound || e.sound.player != nil {
		return nil
	}
	cfg := e.soundConfig()
	context := audio.CurrentContext()
	if context == nil {
		context = audio.NewContext(cfg.SampleRate)
	} else if context.SampleRate() != cfg.SampleRate {
		return fmt.Errorf("sound device sample rate %d differs from configured %d", context.SampleRate(), cfg.SampleRate)
	}
	player, err := context.NewPlayer(e.sound.mixer)
	if err != nil {
		return fmt.Errorf("sound device: %w", err)
	}
	player.SetBufferSize(time.Second * time.Duration(1<<cfg.BufferSize) / time.Duration(cfg.SampleRate))
	e.sound.player = player
	player.Play()
	return nil
}

func (e *Engine) soundWarning(err error) {
	if err == nil {
		return
	}
	message := err.Error()
	if e.sound != nil {
		if e.sound.warnings[message] || len(e.sound.warnings) >= 128 {
			return
		}
		e.sound.warnings[message] = true
	}
	fmt.Printf("Sound warning: %s\n", message)
}

func (e *Engine) PlaySnd(group, item int) {
	if err := e.ensureSound(); err != nil {
		e.soundWarning(err)
		return
	}
	if e.motif == nil || e.motif.SoundBank == nil || e.motif.SoundBank.Map[fmt.Sprintf("%d-%d", group, item)] == nil {
		return
	}
	e.soundWarning(e.sound.queue.Push(sound.Command{Operation: sound.Play, Owner: e.sound.menuOwner, Bank: e.motif.SoundBank, Group: group, Item: item, Channel: -1, VolumeScale: 100, Frequency: 1}))
}

func (e *Engine) flushSound() error {
	if err := e.ensureSound(); err != nil {
		return err
	}
	if err := e.sound.mixer.SetVolume(e.soundConfig().EffectsVolume()); err != nil {
		return err
	}
	for _, command := range e.sound.queue.Drain() {
		if !e.soundConfig().Sound && command.Operation == sound.Play {
			continue
		}
		e.soundWarning(e.sound.mixer.Apply(command))
	}
	return nil
}

func (e *Engine) closeSound() error {
	if e.sound == nil {
		return nil
	}
	var err error
	if e.sound.player != nil {
		err = e.sound.player.Close()
	}
	if closeErr := e.sound.mixer.Close(); err == nil {
		err = closeErr
	}
	e.sound = nil
	return err
}
