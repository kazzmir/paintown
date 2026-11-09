package main

import (
	"path/filepath"

	"github.com/kazzmir/paintown/game/mugen/resource"
	"github.com/kazzmir/paintown/game/mugen/sound"
)

func (e *Engine) playMusic(spec sound.MusicSpec, source string, restart bool) {
	if spec.File != "" {
		path, err := resource.Resolve(spec.File, filepath.Dir(source), e.dataDir, filepath.Join(e.dataDir, "data"), filepath.Join(e.dataDir, "sound"))
		if err != nil {
			e.sound.mixer.Music().Stop()
			e.soundWarning(err)
			return
		}
		spec.File = path
	}
	e.soundWarning(e.sound.mixer.Music().Play(spec, restart))
}

// Music follows active states, not their constructors: a future title or versus
// screen may be constructed while a storyboard still owns playback.
func (e *Engine) syncMusic() error {
	if err := e.ensureSound(); err != nil {
		return err
	}
	bus := e.sound.mixer.Music()
	entry := e.sound.musicState != e.currentState
	if entry {
		e.sound.musicState, e.sound.musicScene = e.currentState, nil
		if e.motif != nil {
			key := ""
			switch e.currentState.(type) {
			case *TitleState:
				key = "title"
			case *SelectState:
				key = "select"
			case *VersusState:
				key = "vs"
			case *WinScreenState:
				key = "victory"
			case *ContinueScreenState:
				key = "continue"
			case *GameOverScreenState:
				key = "gameover"
			}
			if key != "" && e.demo == nil {
				track := e.motif.Music[key]
				e.playMusic(track, e.motif.Source, false)
			}
		}
		if fight, ok := e.currentState.(*FightState); ok && fight.match != nil {
			if e.demo != nil && e.themeNumber("Demo Mode", "fight.playbgm", 0) == 0 {
				if e.themeNumber("Demo Mode", "fight.stopbgm", 1) != 0 {
					bus.Stop()
				}
			} else {
				track := fight.match.Stage.Music
				if fight.match.Request.Music != "" {
					track.File = fight.match.Request.Music
				}
				e.playMusic(track, fight.match.Stage.Source, true)
			}
		}
		if e.currentState == nil {
			bus.Stop()
		}
	}
	if story, ok := e.currentState.(*StoryboardState); ok {
		if !story.loaded {
			story.load()
		}
		if story.loadError != nil {
			return story.loadError
		}
		if entry {
			bus.Stop()
		}
		if scene := story.player.CurrentScene(); scene != e.sound.musicScene {
			e.sound.musicScene = scene
			if scene != nil && scene.MusicSet {
				track := sound.DefaultMusic(scene.MusicLoop)
				track.File = scene.MusicFile
				e.playMusic(track, story.storyboard, true)
			}
		}
	}
	paused := false
	if fight, ok := e.currentState.(*FightState); ok && fight.match != nil {
		paused = fight.match.Timeline.NoMusic
	}
	bus.SetPlayback(e.soundConfig().MusicVolume(), paused)
	_, _, _, err := bus.Status()
	e.soundWarning(err)
	return nil
}
