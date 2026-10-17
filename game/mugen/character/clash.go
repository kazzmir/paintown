package character

import "github.com/kazzmir/paintown/game/mugen/cns"

func priorityOf(hit cns.HitDef) (int, string) {
	priority, class := hit.Priority, hit.PriorityClass
	if class == "" {
		class = "Hit"
		// Keep zero-valued programmatic definitions at the ordinary default.
		// Parsed explicit priority zero retains its nonempty class.
		if priority == 0 {
			priority = 4
		}
	}
	return priority, class
}

func winsClash(hit, other cns.HitDef) bool {
	priority, class := priorityOf(hit)
	otherPriority, otherClass := priorityOf(other)
	if priority != otherPriority {
		return priority > otherPriority
	}
	return class == "Hit" && otherClass != "Dodge"
}

// Resolve reciprocal eligibility before consuming a one-target definition.
// A no-hit tie leaves both definitions armed and does not mark their guards.
func resolveClashes(raw []contact) []contact {
	type pair struct{ attacker, defender *Character }
	reciprocal := make(map[pair]cns.HitDef, len(raw))
	for _, event := range raw {
		reciprocal[pair{event.attacker, event.defender}] = event.hit
	}
	used := map[*Character]bool{}
	var contacts []contact
	for _, event := range raw {
		if other, ok := reciprocal[pair{event.defender, event.attacker}]; ok && !winsClash(event.hit, other) {
			continue
		}
		if event.hit.HitOnce && used[event.attacker] {
			continue
		}
		used[event.attacker] = true
		contacts = append(contacts, event)
	}
	return contacts
}
