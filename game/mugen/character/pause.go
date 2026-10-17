package character

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/sound"
)

func (c *Character) BindTimeline(timeline *engine.Timeline) {
	c.Timeline = timeline
	for _, child := range c.Helpers {
		child.BindTimeline(timeline)
	}
	for _, child := range c.Projectiles {
		child.BindTimeline(timeline)
	}
}

func (c *Character) PauseGame(parameters engine.PauseParameters) error {
	if c.Timeline == nil || c.pauseActor == nil {
		return fmt.Errorf("Pause timeline/actor is unavailable")
	}
	c.Timeline.RequestPause(c.pauseActor, parameters, false)
	return nil
}

func (c *Character) SuperPauseGame(parameters engine.SuperPauseParameters) error {
	if c.Timeline == nil || c.pauseActor == nil {
		return fmt.Errorf("SuperPause timeline/actor is unavailable")
	}
	c.Timeline.RequestSuperPause(c.pauseActor, parameters)
	c.AddPower(parameters.PowerAdd)
	multiplier := float64(parameters.TargetDefenceMul)
	if multiplier <= 0 {
		multiplier = c.SuperTargetDefenceMul
	}
	if multiplier > 0 {
		for target, generation := range c.hitTargets {
			if target.MoveType != "H" || target.hitGeneration != generation {
				delete(c.hitTargets, target)
				continue
			}
			target.superDefenceMul *= multiplier
		}
	}
	if parameters.SoundGroup >= 0 {
		bank := c.CommonSounds
		if parameters.OwnSound {
			bank = c.OwnSounds
		}
		if bank == nil || bank.Map[fmt.Sprintf("%d-%d", parameters.SoundGroup, parameters.SoundItem)] == nil {
			c.EvaluationWarning(fmt.Sprintf("SuperPause sound %d,%d is unavailable", parameters.SoundGroup, parameters.SoundItem))
		} else if err := c.emitSound(sound.Command{Operation: sound.Play, Bank: bank, Group: parameters.SoundGroup, Item: parameters.SoundItem,
			Channel: -1, VolumeScale: 100, Frequency: 1, Pan: c.soundPosition(0, false) + 160}); err != nil {
			return err
		}
	}
	if parameters.Animation < 0 {
		return nil
	}
	scale := float64(c.GameCoord.Width) / float64(c.LocalCoordinateWidth())
	if scale <= 0 {
		scale = 1
	}
	source := c.CommonEffects
	if parameters.OwnAnimation {
		source = effect.Source{Animations: c.AirFile, Palette: c.Palettes, Scale: scale}
		if c.Palettes != nil {
			source.Sprites = c.Palettes.Archive
		}
	}
	if source.Sprites == nil || source.Animations == nil || source.Animations.Actions[parameters.Animation] == nil {
		c.EvaluationWarning(fmt.Sprintf("SuperPause action %d is unavailable", parameters.Animation))
		return nil
	}
	if c.EffectQueue == nil {
		c.BindEffects(nil, c.CommonEffects)
	}
	return c.EffectQueue.Push(effect.Request{Source: source, Action: parameters.Animation, Facing: c.Facing,
		X: (c.X + float64(c.Facing)*parameters.Position[0]) * scale, Y: (c.Y + parameters.Position[1]) * scale, IgnorePause: true, IgnoreSuperPause: true})
}

func (c *Character) bufferPausedCommands() {
	if c.Timeline == nil || !c.Timeline.BufferCommands(c.pauseActor) {
		return
	}
	c.updateCommands(true)
	if c.pauseCommands == nil {
		c.pauseCommands = map[string]bool{}
	}
	for _, name := range c.ActiveCommands {
		c.pauseCommands[name] = true
	}
}
