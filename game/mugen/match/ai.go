package match

import (
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"math"
	"sort"
	"strings"
)

type aiBrain struct {
	random       *engine.RandomStream
	commands     [][]character.CommandInput
	commandNames []string
	admitted     []string
	queue        []character.CommandInput
	held         []string
	remaining    int
	nextDecision int
	frame        int
	inputs       []string
	keys         [11]bool // persistent physical virtual keys; directions precede buttons
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
		b.commandNames = names
		for _, name := range names {
			for _, command := range c.CmdFile.Commands[name] {
				steps := character.CommandInputs(command.Sequence)
				button := false
				for _, step := range steps {
					for _, key := range step.Keys {
						button = button || (strings.HasPrefix(key, "Button") && key != "ButtonStart")
					}
				}
				if button && len(steps) > 0 {
					for index, step := range steps {
						physical, reachable := c.CmdFile.PhysicalInputs(step.Keys)
						if !reachable {
							steps = nil
							break
						}
						steps[index].Keys = physical
					}
					if len(steps) > 0 {
						b.commands = append(b.commands, steps)
					}
				}
			}
		}
	}
	m.aiBrains[c.EntityID] = b
	return b
}

func (b *aiBrain) nextInput() []string {
	if b.remaining == 0 {
		step := b.queue[0]
		b.queue = b.queue[1:]
		b.held, b.remaining = step.Keys, step.Time
	}
	b.remaining--
	return b.held
}

var aiKeys = [...]string{"DirUp", "DirForward", "DirDown", "DirBack", "ButtonA", "ButtonB", "ButtonC", "ButtonX", "ButtonY", "ButtonZ", "ButtonStart"}

func (b *aiBrain) snapshot() []string {
	var keys []string
	for index, held := range b.keys {
		if held {
			keys = append(keys, aiKeys[index])
		}
	}
	return keys
}

func (b *aiBrain) releaseDirections() {
	clear(b.keys[:4])
}

// Native fallback movement changes held directions occasionally, rather than
// generating a fresh forward press on every pass. These weights include neutral,
// retreat, crouch and diagonal jumps. Inputs remain relative to command facing.
func (b *aiBrain) movement(distance int) {
	if b.random.Next()%1000 < 62 && distance > 125 && !b.keys[1] {
		b.releaseDirections()
		b.keys[0] = b.random.Next()%1000 < 150
		b.keys[1] = b.random.Next()%1000 < 750
	}
	if b.random.Next()%1000 < 62 {
		b.releaseDirections()
		switch choice := b.random.Next() % 19; {
		case choice == 0:
			b.keys[0], b.keys[3] = true, true
		case choice == 1:
			b.keys[0] = true
		case choice <= 4:
			b.keys[0], b.keys[1] = true, true
		case choice <= 10:
			b.keys[1] = true
		case choice <= 14:
			b.keys[3] = true
		case choice == 15:
			b.keys[2], b.keys[3] = true, true
		case choice <= 17:
			b.keys[2] = true
		default:
			b.keys[2], b.keys[1] = true, true
		}
	}
}

// Distances in the target input policy use game-space axes, not body edges or
// the selected fighter's private CNS units. Peer positions may use another profile.
func (m *Match) aiOpponent(c *character.Character) (*character.Character, int) {
	var selected *character.Character
	nearest := math.Inf(1)
	for _, enemy := range c.Opponents {
		if enemy == nil || enemy.DebugDisabled || !enemy.IsAlive() {
			continue
		}
		x, _ := c.PositionOf(enemy)
		distance := math.Abs(c.X - x)
		if distance < nearest {
			selected, nearest = enemy, distance
		}
	}
	width := m.Options.GameSpace.Width
	if width <= 0 {
		width = 320
	}
	if selected == nil {
		return nil, 0
	}
	return selected, int(nearest * float64(width) / float64(c.LocalCoordinateWidth()))
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
	b.admitted = nil
	if b.remaining > 0 || len(b.queue) > 0 {
		b.inputs = b.nextInput()
		return b.inputs
	}
	if !c.Ctrl && !(c.MoveType == "A" && c.MoveContact > 0) {
		return nil
	}
	other, distance := m.aiOpponent(c)
	if other == nil {
		return nil
	}
	level := max(1, min(8, c.AILevel))
	if other.MoveType == "A" && distance < 90 && b.random.Next()%10 < int64(level) {
		b.inputs = []string{"DirBack"}
		if other.StateType == "C" {
			b.inputs = append(b.inputs, "DirDown")
		}
		return b.inputs
	}
	if m.Tick < b.nextDecision {
		b.inputs = b.snapshot()
		return b.inputs
	}
	b.nextDecision = m.Tick + max(4, 40-4*level)
	if b.random.Next()%100 < 58 {
		b.movement(distance)
	}
	b.inputs = b.snapshot()
	if m.Options.AICheat && len(b.commandNames) > 0 {
		// Native AI can admit a declared label without simulating its motion
		// or charge. Do not forge press/release history or bypass CNS gates.
		index := int(b.random.Next() % int64(len(b.commandNames)))
		b.admitted = b.commandNames[index : index+1]
	} else if len(b.commands) > 0 {
		b.queue = b.commands[int(b.random.Next()%int64(len(b.commands)))]
		b.inputs = b.nextInput()
	} else {
		b.inputs, _ = c.CmdFile.PhysicalInputs([]string{"ButtonX"})
	}
	return b.inputs
}

// BasicAICommands shares one decision with this tick's physical snapshot. A
// key-controlled helper sees its root's labels with its own command buffer;
// ordinary human input and helpers without key control never use this path.
func (m *Match) BasicAICommands(playerID int) []string {
	if !m.Options.AICheat || playerID < 1 || playerID > 4 || m.Phase != Active {
		return nil
	}
	p := m.ActivePlayers()[playerID-1]
	if p == nil || !p.Character.IsAlive() || p.Character.AILevel <= 0 {
		return nil
	}
	m.BasicAIInput(playerID)
	return m.brain(p.Character).admitted
}
