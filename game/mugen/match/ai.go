package match

import (
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"sort"
	"strings"
)

type aiBrain struct {
	random       *engine.RandomStream
	commands     [][][]string
	queue        [][]string
	nextDecision int
	frame        int
	inputs       []string
}

func (m *Match) brain(c *character.Character) *aiBrain {
	if m.aiBrains == nil {
		m.aiBrains = make(map[int32]*aiBrain)
	}
	if brain := m.aiBrains[c.EntityID]; brain != nil {
		return brain
	}
	b := &aiBrain{random: engine.NewRandomStream(m.World.RandomSeed() + int64(c.EntityID)*16807), frame: -1}
	if c.CmdFile != nil {
		var names []string
		for name := range c.CmdFile.Commands {
			names = append(names, name)
		}
		sort.Strings(names)
		for _, name := range names {
			for _, command := range c.CmdFile.Commands[name] {
				frames := character.CommandInputs(command.Sequence)
				button := false
				for _, frame := range frames {
					for _, key := range frame {
						button = button || (strings.HasPrefix(key, "Button") && key != "ButtonStart")
					}
				}
				if button && len(frames) > 0 {
					for index, frame := range frames {
						physical, reachable := c.CmdFile.PhysicalInputs(frame)
						if !reachable {
							frames = nil
							break
						}
						frames[index] = physical
					}
					if len(frames) > 0 {
						b.commands = append(b.commands, frames)
					}
				}
			}
		}
	}
	m.aiBrains[c.EntityID] = b
	return b
}

// BasicAIInput retains the adapter name but plans authored CMD sequences.
// CNS owns eligibility, cancels, power and resulting states. Difficulty affects
// reaction/defence; an independent stream leaves authored CNS Random untouched.
func (m *Match) BasicAIInput(playerID int) []string {
	if playerID < 1 || playerID > 4 || m.Phase != Active {
		return nil
	}
	p := m.ActivePlayers()[playerID-1]
	if p == nil || !p.Character.IsAlive() {
		return nil
	}
	c := p.Character
	b := m.brain(c)
	if b.frame == m.Tick {
		return b.inputs
	}
	b.frame, b.inputs = m.Tick, nil
	if len(b.queue) > 0 {
		b.inputs, b.queue = b.queue[0], b.queue[1:]
		return b.inputs
	}
	if !c.Ctrl && !(c.MoveType == "A" && c.MoveContact > 0) {
		return nil
	}
	var other *character.Character
	for _, enemy := range c.Opponents {
		if enemy.IsAlive() {
			other = enemy
			break
		}
	}
	if other == nil {
		return nil
	}
	level := max(1, min(8, c.AILevel))
	distance, _ := c.GetOpponentBodyDistance()
	unit := float64(c.LocalCoordinateWidth()) / 320
	if other.MoveType == "A" && distance < 90*unit && b.random.Next()%10 < int64(level) {
		b.inputs = []string{"DirBack"}
		if other.StateType == "C" {
			b.inputs = append(b.inputs, "DirDown")
		}
		return b.inputs
	}
	if m.Tick < b.nextDecision {
		if distance > 30*unit && c.Ctrl {
			b.inputs = []string{"DirForward"}
		}
		return b.inputs
	}
	b.nextDecision = m.Tick + max(4, 40-4*level)
	if distance > 65*unit && b.random.Next()%4 != 0 {
		b.inputs = []string{"DirForward"}
		if level >= 4 && b.random.Next()%4 == 0 {
			b.inputs = append(b.inputs, "DirUp")
		}
		return b.inputs
	}
	if len(b.commands) > 0 {
		b.queue = b.commands[int(b.random.Next()%int64(len(b.commands)))]
		b.inputs, b.queue = b.queue[0], b.queue[1:]
	} else {
		b.inputs, _ = c.CmdFile.PhysicalInputs([]string{"ButtonX"})
	}
	return b.inputs
}
