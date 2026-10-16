// Package sound contains display-independent sound commands and PCM playback.
package sound

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/snd"
)

type Operation uint8

const (
	Play Operation = iota
	Pan
	Stop
	StopAll
	StopOwner
)

// PlayParameters retain controller values until the character resolves its
// bank and camera-relative position. This beta uses common-game pixels for
// PlaySnd offsets; SndPan's separate command takes character-local pixels.
type PlayParameters struct {
	Common                 bool
	Group, Item, Channel   int
	VolumeScale, Frequency float32
	Loop, LowPriority      bool
	Pan                    int
	Absolute               bool
}

// Command uses a scene-local audio owner, independent of the input/player slot.
// Pan is measured from screen center in 240p units, sampled at controller time.
type Command struct {
	Operation              Operation
	Owner                  uint64
	Bank                   *snd.SoundBank
	Group, Item, Channel   int
	VolumeScale, Frequency float32
	Pan                    float32
	Loop, LowPriority      bool
}

// Queue belongs to the simulation goroutine. Presentation drains its ordered
// commands; the mixer separately synchronizes access from the audio thread.
type Queue struct {
	commands  []Command
	nextOwner uint64
}

func (q *Queue) NewOwner() uint64 {
	q.nextOwner++
	return q.nextOwner
}

func (q *Queue) Push(command Command) error {
	if len(q.commands) >= 16384 {
		return fmt.Errorf("undrained sound command queue exceeds 16384 entries")
	}
	q.commands = append(q.commands, command)
	return nil
}

func (q *Queue) Drain() []Command {
	commands := q.commands
	q.commands = nil
	return commands
}
