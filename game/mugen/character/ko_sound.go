package character

import (
	"github.com/kazzmir/paintown/game/mugen/sound"
)

// Root KO voices require committed death in the shared post-damage phase.
// NoKO can retain Life 0 without a KO voice or echo. Contact independently
// interrupts channel 0; a protected direct LifeSet does not interrupt it.
// Pause/SuperPause and helper KO ownership remain to verify.
func (c *Character) updateKOSound(suppressed bool) error {
	if c.Parent != nil {
		return nil
	}
	if c.IsAlive() {
		c.koSoundStarted, c.koSoundAge = false, 0
		return nil
	}
	volume := float32(0)
	if !c.koSoundStarted {
		c.koSoundStarted, c.koSoundAge = true, 1
		if err := c.StopSound(0); err != nil {
			return err
		}
		volume = 100
	} else {
		c.koSoundAge++
		if c.StateFile != nil && c.StateFile.Data.KOEcho != 0 {
			switch c.koSoundAge {
			case 20:
				volume = 50
			case 50:
				volume = 25
			}
		}
	}
	if volume == 0 || suppressed {
		return nil
	}
	bank := c.OwnSounds
	if bank == nil || bank.Map["11-0"] == nil {
		c.EvaluationWarning("KO sound 11,0 is unavailable")
		return nil
	}
	return c.emitSound(sound.Command{Operation: sound.Play, Bank: bank, Group: 11, Item: 0, Channel: -1, VolumeScale: volume, Frequency: 1, Pan: c.soundPosition(0, false) + 160})
}
