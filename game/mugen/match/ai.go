package match

import (
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"math"
	"strings"
)

type aiBrain struct {
	random       *engine.RandomStream
	commands     [][]character.CommandInput
	commandPlans map[int][][]character.CommandInput
	commandNames []string
	admitted     []string
	queue        []character.CommandInput
	held         []string
	remaining    int
	planActive   bool
	frame        int
	inputs       []string
	keys         [11]bool // persistent physical virtual keys; directions precede buttons
	source       *cmd.Data
	level        int
	cheat        bool
	facing       int // heading of this tick's cached root-relative snapshot
	planner      aiPlanner
}

func (m *Match) brain(c *character.Character) *aiBrain {
	if m.aiBrains == nil {
		m.aiBrains = make(map[int32]*aiBrain)
	}
	if brain := m.aiBrains[c.EntityID]; brain != nil && brain.source == c.CmdFile && brain.level == c.AILevel && brain.cheat == m.Options.AICheat {
		return brain
	}
	if m.aiRandom == nil {
		m.aiRandom = make(map[int]*engine.RandomStream)
	}
	random := m.aiRandom[c.InputSlot]
	if random == nil {
		random = engine.NewRandomStream(m.World.RandomSeed() + int64(c.InputSlot)*16807)
		m.aiRandom[c.InputSlot] = random
	}
	b := &aiBrain{random: random, frame: -1, source: c.CmdFile, level: c.AILevel, cheat: m.Options.AICheat, commandPlans: make(map[int][][]character.CommandInput)}
	if c.CmdFile != nil {
		names := c.CmdFile.IndexedNames()
		b.commandNames = names
		for index, name := range names {
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
						b.commandPlans[index] = append(b.commandPlans[index], steps)
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
	clear(b.keys[:])
	for index, key := range aiKeys {
		for _, held := range b.held {
			b.keys[index] = b.keys[index] || key == held
		}
	}
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

// BasicAIInput retains the adapter name. One decision per input tick updates
// persistent physical keys and separate declared-command admission. CNS owns
// eligibility, cancels, power and resulting states. The independent stream keeps
// AI sampling from consuming authored CNS Random draws.
func (m *Match) BasicAIInput(playerID int) []string {
	if playerID < 1 || playerID > 4 || m.Phase != Active {
		return nil
	}
	p := m.ActivePlayers()[playerID-1]
	if p == nil || p.Character == nil {
		return nil
	}
	c := p.Character
	if c.AILevel <= 0 || c.DebugDisabled || !c.IsAlive() {
		delete(m.aiBrains, c.EntityID)
		return nil
	}
	b := m.brain(c)
	if b.frame == m.Tick {
		return b.inputs
	}
	b.frame, b.inputs, b.facing = m.Tick, nil, c.Facing
	b.admitted = nil
	other, distance := m.aiOpponent(c)
	if other == nil {
		clear(b.keys[:])
		b.queue, b.held, b.remaining, b.planActive = nil, nil, 0, false
		b.planner = aiPlanner{}
		return nil
	}
	level := max(1, min(8, c.AILevel))
	if b.planner.context == aiLearnedAttack && c.MoveType == "H" {
		b.queue, b.held, b.remaining, b.planActive = nil, nil, 0, false
		b.planner.enter(aiFallback)
	}
	first, second := 4+int(b.random.Next()%6), 4+int(b.random.Next()%6)
	if b.random.Next()%100 == 0 {
		first = 10 // Start is an occasional native choice, not an attack button alias.
	}
	if first == second || b.random.Next()%8 != 0 {
		second = -1
	}
	if b.remaining == 0 && len(b.queue) == 0 && !b.planActive && b.learnedInput(c, other, distance, level, m.Options.AICheat) {
		b.recovery(c, m.Options.GameSpace.Width)
		b.inputs = b.snapshot()
		return b.inputs
	}
	if b.planner.context != aiLearnedAttack && b.defence(c, other, distance, level) {
		b.planner.enter(aiFallback)
		b.inputs = b.snapshot()
		return b.inputs
	}
	if !c.GetInGuardDist() {
		b.keys[3] = false
	}
	if b.remaining > 0 || len(b.queue) > 0 {
		b.inputs = b.nextInput()
		return b.inputs
	}
	if b.planActive {
		b.planActive = false
		clear(b.keys[:])
		b.inputs = b.snapshot()
		return b.inputs
	}
	if b.random.Next()%100 < 58 {
		b.movement(distance)
		b.attack(c, distance, level, first, second, m.Options.AICheat)
	}
	b.recovery(c, m.Options.GameSpace.Width)
	b.inputs = b.snapshot()
	return b.inputs
}

// BasicAIInputForFacing presents the root's one physical snapshot in a
// key-controlled helper's heading without sampling again or changing the cache.
func (m *Match) BasicAIInputForFacing(playerID, facing int) []string {
	keys := m.BasicAIInput(playerID)
	if len(keys) == 0 {
		return keys
	}
	p := m.ActivePlayers()[playerID-1]
	if facing == m.brain(p.Character).facing {
		return keys
	}
	keys = append([]string(nil), keys...)
	for index, key := range keys {
		switch key {
		case "DirForward":
			keys[index] = "DirBack"
		case "DirBack":
			keys[index] = "DirForward"
		}
	}
	return keys
}

// BasicAICommands shares one decision with this tick's physical snapshot. A
// key-controlled helper sees its root's labels with its own command buffer;
// ordinary human input and helpers without key control never use this path.
func (m *Match) BasicAICommands(playerID int) []string {
	if !m.Options.AICheat || playerID < 1 || playerID > 4 || m.Phase != Active {
		return nil
	}
	p := m.ActivePlayers()[playerID-1]
	if p == nil || p.Character == nil || !p.Character.IsAlive() || p.Character.DebugDisabled || p.Character.AILevel <= 0 {
		return nil
	}
	m.BasicAIInput(playerID)
	return m.brain(p.Character).admitted
}
