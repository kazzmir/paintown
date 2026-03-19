package paintown

import (
    "sync"
    "bufio"
    "os"
    "io"

    "github.com/kazzmir/paintown/game/data"

    audiolib "github.com/hajimehoshi/ebiten/v2/audio"
    "github.com/hajimehoshi/ebiten/v2/audio/wav"
)

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

func (audio *AudioManager) LoadSound(path string) (*audiolib.Player, error) {
    pool, exists := audio.sounds[path]
    if !exists {
        fullPath := data.DataPath(path)

        pcmBytes, err := (func () ([]byte, error) {
            file, err := os.Open(fullPath)
            if err != nil {
                return nil, err
            }

            wavStream, err := wav.DecodeWithSampleRate(audio.context.SampleRate(), bufio.NewReader(file))
            if err != nil {
                return nil, err
            }

            return io.ReadAll(wavStream)
        })()

        if err != nil {
            return nil, err
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
    return player, nil
}

func (audio *AudioManager) PlaySound(path string) error {
    if path != "" {
        sound, err := audio.LoadSound(path)
        if err != nil {
            return err
        } else {
            sound.Play()
        }
    }

    return nil
}
