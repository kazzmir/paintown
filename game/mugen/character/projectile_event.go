package character

import (
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// The beta keeps one latest projectile contact per owner, across state changes.
// The owner's active clock ages it; projectile retirement does not erase it.
type projectileEvent struct {
	id, age int
	kind    string
}

func (p *Character) recordProjectileEvent(kind string) {
	if p.projectile != nil {
		p.rootPlayer().lastProjectileEvent = &projectileEvent{id: p.projectile.parameters.ID, kind: kind}
	}
}

func (c *Character) projectileEventTime(kind string, id int) int {
	e := c.lastProjectileEvent
	if e == nil || id > 0 && e.id != id || kind == "contact" && e.kind == "cancel" || kind != "contact" && e.kind != kind {
		return -1
	}
	return e.age
}

func (c *Character) lookupProjectileEvent(name string, args []evaluator.Value) (evaluator.Value, bool) {
	kind := ""
	switch name {
	case "projhittime":
		kind = "hit"
	case "projguardedtime":
		kind = "guard"
	case "projcontacttime":
		kind = "contact"
	case "projcanceltime":
		kind = "cancel"
	default:
		return evaluator.Value{}, false
	}
	if len(args) != 1 {
		c.EvaluationWarning(name + " requires one ID")
		return evaluator.Value{}, true
	}
	id, err := evaluator.CoerceInt(args[0], c, name+" ID")
	if err != nil {
		return evaluator.Value{}, true
	}
	return evaluator.Int(int32(c.projectileEventTime(kind, int(id)))), true
}

func (c *Character) lookupLegacyProjectile(name, operator string, args []evaluator.Value, extra string) (evaluator.Value, bool) {
	kind, suffix := "", ""
	switch {
	case strings.HasPrefix(name, "projhit"):
		kind, suffix = "hit", strings.TrimPrefix(name, "projhit")
	case strings.HasPrefix(name, "projguarded"):
		kind, suffix = "guard", strings.TrimPrefix(name, "projguarded")
	case strings.HasPrefix(name, "projcontact"):
		kind, suffix = "contact", strings.TrimPrefix(name, "projcontact")
	default:
		return evaluator.Value{}, false
	}
	id := 0
	if suffix != "" {
		n, err := strconv.Atoi(suffix)
		if err != nil {
			return evaluator.Value{}, false
		}
		id = n
	}
	if len(args) < 1 || len(args) > 2 {
		return evaluator.Value{}, true
	}
	age := c.projectileEventTime(kind, id)
	matched := age == 0
	if len(args) == 2 {
		value := int(args[1].Number())
		switch extra {
		case "", "=", "==":
			matched = age >= 0 && age == value
		case "!=":
			matched = age >= 0 && age != value
		case "<":
			matched = age >= 0 && age < value
		case "<=":
			matched = age >= 0 && age <= value
		case ">":
			matched = age >= 0 && age > value
		case ">=":
			matched = age >= 0 && age >= value
		}
	}
	if !args[0].Bool() {
		matched = !matched
	}
	// The beta accepts the main != spelling with the same behavior as =.
	return evaluator.Bool(matched), true
}
