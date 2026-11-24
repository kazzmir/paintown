package character

import "github.com/kazzmir/paintown/game/mugen/cmd"

// AIMemory keeps completed estimates across matches without writing generated
// binary caches into a content pack. Live actors always get independent banks.
// A source revision or game-width change selects a fresh compatible profile.
type AIMemory struct {
	entries map[aiMemoryKey]*AIMoveLibrary
	order   []aiMemoryKey
}

type aiMemoryKey struct {
	definition string
	revision   [32]byte
	width      int
}

func (l *AIMoveLibrary) clone(commands *cmd.Data) *AIMoveLibrary {
	copy := NewAIMoveLibrary(commands, l.Width)
	copy.Poses = l.Poses
	for _, bank := range [][]*AIMove{l.groundOrder, l.airOrder} {
		for _, move := range bank {
			next := *move
			next.Contexts = append([]AIEntryContext(nil), move.Contexts...)
			copy.complete(next)
		}
	}
	return copy
}

func (m *AIMemory) Bind(definition string, revision [32]byte, initial *AIMoveLibrary) *AIMoveLibrary {
	if m.entries == nil {
		m.entries = make(map[aiMemoryKey]*AIMoveLibrary)
	}
	key := aiMemoryKey{definition, revision, initial.Width}
	stored := m.entries[key]
	if stored == nil {
		// Completed data is small and bounded independently from sprite/audio
		// resources. Avoid retaining every character ever visited in a session.
		if len(m.order) == 64 {
			delete(m.entries, m.order[0])
			m.order = m.order[1:]
		}
		stored = initial.clone(initial.Commands)
		m.entries[key] = stored
		m.order = append(m.order, key)
	}
	live := stored.clone(initial.Commands)
	live.onComplete = func(next AIMove) {
		stored.complete(next)
		stored.Poses = live.Poses
	}
	return live
}
