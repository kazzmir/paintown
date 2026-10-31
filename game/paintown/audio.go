package paintown

import (
    "sync"
    "bufio"
    "os"
    "io"
    "time"

    "github.com/kazzmir/paintown/game/data"

    audiolib "github.com/hajimehoshi/ebiten/v2/audio"
    "github.com/hajimehoshi/ebiten/v2/audio/wav"
)

type audioVolumeTarget interface {
    SetVolume(float64)
}

type volumeControl struct {
    sync.Mutex
    volume int
    playing map[audioVolumeTarget]struct{}
}

func newVolumeControl() *volumeControl {
    return &volumeControl{volume:100, playing:make(map[audioVolumeTarget]struct{})}
}

func (control *volumeControl) get() int {
    control.Lock()
    defer control.Unlock()
    return control.volume
}

func (control *volumeControl) set(volume int) {
    control.Lock()
    defer control.Unlock()
    control.volume = max(0,min(100,volume))
    for player := range control.playing {
        player.SetVolume(float64(control.volume)/100)
    }
}

func (control *volumeControl) track(player audioVolumeTarget) {
    control.Lock()
    defer control.Unlock()
    player.SetVolume(float64(control.volume)/100)
    control.playing[player] = struct{}{}
}

func (control *volumeControl) untrack(player audioVolumeTarget) {
    control.Lock()
    defer control.Unlock()
    delete(control.playing,player)
}

var soundSettings = newVolumeControl()
var musicSettings = newVolumeControl()

func SoundVolume() int { return soundSettings.get() }
func MusicVolume() int { return musicSettings.get() }

// Volume settings are independent and shared across menus for this session.
func SetSoundVolume(volume int) { soundSettings.set(volume) }
func SetMusicVolume(volume int) { musicSettings.set(volume) }

func trackSoundVolume(player audioVolumeTarget) { soundSettings.track(player) }
func untrackSoundVolume(player audioVolumeTarget) { soundSettings.untrack(player) }

type AudioManager struct {
    sounds map[string]*sync.Pool
    context *audiolib.Context
}

func MakeAudioManager(context *audiolib.Context) *AudioManager {
    return &AudioManager{
        context: context,
        sounds: make(map[string]*sync.Pool),
    }
}

func (audio *AudioManager) LoadSound(path string) (*audiolib.Player, func(), error) {
    pool, exists := audio.sounds[path]
    if !exists {
        fullPath := data.DataPath(path)

        pcmBytes, err := (func () ([]byte, error) {
            file, err := os.Open(fullPath)
            if err != nil {
                return nil, err
            }

            defer file.Close()
            wavStream, err := wav.DecodeWithSampleRate(audio.context.SampleRate(), bufio.NewReader(file))
            if err != nil {
                return nil, err
            }

            return io.ReadAll(wavStream)
        })()

        if err != nil {
            return nil, func(){}, err
        }

        pool = &sync.Pool{
            New: func() any {
                return audio.context.NewPlayerFromBytes(pcmBytes)
            },
        }
        audio.sounds[path] = pool
    }

    player := pool.Get().(*audiolib.Player)
    player.Rewind()
    trackSoundVolume(player)

    putBack := func() {
        untrackSoundVolume(player)
        pool.Put(player)
    }

    return player, putBack, nil
}

func (audio *AudioManager) PlaySound(path string) error {
    if path != "" && SoundVolume() > 0 {
        sound, putBack, err := audio.LoadSound(path)
        if err != nil {
            return err
        } else {
            sound.Play()
            go func() {
                for sound.IsPlaying() {
                    time.Sleep(100 * time.Millisecond)
                }

                putBack()
            }()
        }
    }

    return nil
}
