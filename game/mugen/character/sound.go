package character

import (
	"fmt"
	"github.com/kazzmir/paintown/game/mugen/snd"
	"github.com/kazzmir/paintown/game/mugen/sound"
)

func (c *Character) BindSound(queue *sound.Queue, own, common *snd.SoundBank) {
	if queue == nil {
		queue = &sound.Queue{}
	}
	c.SoundQueue = queue
	c.AudioOwner = queue.NewOwner()
	c.OwnSounds, c.CommonSounds = own, common
}

func (c *Character) soundPosition(offset int, absolute bool) float32 {
	position := float32(offset)
	if !absolute {
		position = float32(c.X-c.CameraX) + float32(c.Facing)*position
	}
	return position * 320 / float32(c.LocalCoordinateWidth())
}

func (c *Character) emitSound(command sound.Command) error {
	if c.SoundQueue == nil {
		c.BindSound(nil, c.OwnSounds, c.CommonSounds)
	}
	command.Owner = c.AudioOwner
	return c.SoundQueue.Push(command)
}

func (c *Character) PlaySound(request sound.PlayParameters) error {
	bank := c.OwnSounds
	if request.Common {
		bank = c.CommonSounds
	}
	if bank == nil || bank.Map[fmt.Sprintf("%d-%d", request.Group, request.Item)] == nil {
		c.EvaluationWarning(fmt.Sprintf("PlaySnd sound %d,%d is unavailable", request.Group, request.Item))
		return nil
	}
	width := c.GameCoord.Width
	if width <= 0 {
		width = 320
	}
	// PlaySnd uses common-game pixels for its offset; SndPan first converts
	// its offset from the character's local coordinates.
	pan := float32(request.Pan) * 320 / float32(width)
	if !request.Absolute {
		pan += c.soundPosition(0, false)
	}
	return c.emitSound(sound.Command{Operation: sound.Play, Bank: bank, Group: request.Group, Item: request.Item, Channel: request.Channel,
		VolumeScale: request.VolumeScale, Frequency: request.Frequency, Loop: request.Loop, LowPriority: request.LowPriority,
		Pan: pan})
}

func (c *Character) PanSound(channel, offset int, absolute bool) error {
	return c.emitSound(sound.Command{Operation: sound.Pan, Channel: channel, Pan: c.soundPosition(offset, absolute)})
}

func (c *Character) StopSound(channel int) error {
	operation := sound.Stop
	if channel == -1 {
		operation = sound.StopAll
	}
	return c.emitSound(sound.Command{Operation: operation, Channel: channel})
}
