package character

import (
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

func (c *Character) requestHitShake(shake cns.HitShake) {
	if c.Timeline == nil || shake.Time <= 0 {
		return
	}
	c.Timeline.RequestShake(engine.ShakeParameters{
		Time: shake.Time, Amplitude: float32(shake.Amplitude) * 320 / float32(c.LocalCoordinateWidth()),
		Frequency: float32(shake.Frequency), Phase: float32(shake.Phase),
	})
}
