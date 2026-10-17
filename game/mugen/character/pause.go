package character

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/engine"
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
