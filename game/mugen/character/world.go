package character

import (
	"fmt"
	"math"
	"sort"

	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

// World owns entity identity and root relationships. TickCharacters schedules
// roots/helpers; projectiles still use legacy child adapters.
type World struct {
	Registry           *engine.Registry[*Character]
	Roots              []*Character
	Helpers            []*Character
	HelperMax          int
	orphanEffectOwners []*Character

	// AfterEntityUpdate observes completed root/helper CNS and physics. Match
	// uses this for camera easing; state-only worlds leave it nil.
	AfterEntityUpdate func()
	random            *engine.RandomStream
}

func NewWorld(helperMax int) *World {
	helperMax = max(4, min(56, helperMax))
	return &World{Registry: engine.NewRegistry[*Character](int32(helperMax)), HelperMax: helperMax, random: engine.NewRandomStream(1)}
}

// SeedRandom starts a new match stream. ResetRoots deliberately preserves it.
func (w *World) SeedRandom(seed int64) { w.random.Seed(seed) }

func (w *World) RandomSeed() int64 { return w.random.InitialSeed() }

func (w *World) ResetRoots(players ...*Character) error {
	for _, owner := range w.orphanEffectOwners {
		for _, e := range owner.Explods {
			if e.Visual != nil {
				e.Visual.Active = false
			}
		}
	}
	w.orphanEffectOwners = nil
	for _, helper := range w.Helpers {
		helper.destroyed = true
		w.Registry.Remove(helper.EntityID)
	}
	w.Helpers = nil
	old := w.Roots
	w.Roots = nil
	for index, c := range players {
		if c == nil {
			return fmt.Errorf("cannot reset nil root")
		}
		if index >= len(old) {
			if err := w.AddRoots(c); err != nil {
				return err
			}
			continue
		}
		previous := old[index]
		previous.destroyed = true
		if err := w.Registry.Replace(previous.EntityID, c); err != nil {
			return err
		}
		c.World, c.EntityID, c.TeamSide = w, previous.EntityID, previous.TeamSide
		w.Roots = append(w.Roots, c)
	}
	for _, c := range w.Roots {
		c.Opponents = w.enemies(c, false)
	}
	return nil
}

func (w *World) AddRoots(players ...*Character) error {
	for _, c := range players {
		if c == nil {
			return fmt.Errorf("cannot register nil root")
		}
		if c.Parent != nil {
			continue
		}
		if c.World == w {
			continue
		}
		if c.World != nil {
			return fmt.Errorf("root already belongs to another world")
		}
		id, err := w.Registry.Allocate(c)
		if err != nil {
			return err
		}
		c.World, c.EntityID = w, id
		if c.TeamSide == 0 {
			c.TeamSide = len(w.Roots)%2 + 1
		}
		w.Roots = append(w.Roots, c)
	}
	for _, c := range w.Roots {
		c.Opponents = w.enemies(c, false)
	}
	return nil
}

func (c *Character) ensureWorld() error {
	if c.World != nil {
		return nil
	}
	root := c.rootPlayer()
	if c.isProjectile {
		if err := root.ensureWorld(); err != nil {
			return err
		}
		c.World = root.World
		return nil
	}
	if root.World == nil {
		world := NewWorld(56)
		players := append([]*Character{root}, root.Opponents...)
		if err := world.AddRoots(players...); err != nil {
			return err
		}
	}
	if c != root && c.World == nil {
		return root.World.addHelper(c)
	}
	return nil
}

func (c *Character) rootPlayer() *Character {
	if c.Root != nil {
		return c.Root
	}
	return c
}

func (w *World) addHelper(c *Character) error {
	if c.World == w {
		return nil
	}
	if c.World != nil {
		return fmt.Errorf("helper already belongs to another world")
	}
	id, err := w.Registry.Allocate(c)
	if err != nil {
		return err
	}
	c.World, c.EntityID, c.TeamSide = w, id, c.rootPlayer().TeamSide
	c.Opponents = w.enemies(c, false)
	w.Helpers = append(w.Helpers, c)
	return nil
}

func (w *World) enemies(c *Character, nearest bool) []*Character {
	var result []*Character
	for _, root := range w.Roots {
		if root.TeamSide != c.TeamSide {
			result = append(result, root)
		}
	}
	if nearest {
		sort.SliceStable(result, func(i, j int) bool {
			x, _ := c.PositionOf(result[i])
			y, _ := c.PositionOf(result[j])
			return math.Abs(x-c.X) < math.Abs(y-c.X)
		})
	}
	return result
}

// Combat candidates include helpers without changing root-only Enemy/NumEnemy
// queries. Enemy roots precede friendly roots, then helpers follow creation.
func (w *World) contactCandidates(c *Character, team string) []*Character {
	eligible := func(other *Character) bool {
		if other == c || other.destroyed {
			return false
		}
		switch team {
		case "B":
			return true
		case "F":
			return other.TeamSide == c.TeamSide
		default:
			return other.TeamSide != c.TeamSide
		}
	}
	var result []*Character
	for _, root := range w.Roots {
		if eligible(root) {
			result = append(result, root)
		}
	}
	// Roots precede helpers; among roots, opponents precede friends even
	// when the friendly root has the lower world ID.
	sort.SliceStable(result, func(i, j int) bool {
		return result[i].TeamSide != c.TeamSide && result[j].TeamSide == c.TeamSide
	})
	for _, helper := range w.Helpers {
		if eligible(helper) {
			// Both normal/player helpers can attack after birth, but their
			// first incoming contact is two world ticks after creation.
			if helper.helperBirthKnown && c.Timeline != nil && c.Timeline.GameTime-helper.helperBirthTick < 2 {
				continue
			}
			result = append(result, helper)
		}
	}
	return result
}

func (c *Character) helperFamily(id int, filter bool) []*Character {
	var result []*Character
	if c.World == nil || c.destroyed {
		return result
	}
	root := c.rootPlayer()
	for _, helper := range c.World.Helpers {
		if helper.destroyed || helper.rootPlayer() != root || (filter && id > 0 && helper.HelperID != id) {
			continue
		}
		result = append(result, helper)
	}
	return result
}

func (c *Character) lookupEntity(name string) (evaluator.Value, bool) {
	switch name {
	case "id", "teamside", "ishelper", "numhelper", "numtarget", "numexplod", "numenemy", "numpartner", "parentdist x", "parentdist y", "rootdist x", "rootdist y":
	default:
		return evaluator.Value{}, false
	}
	if err := c.ensureWorld(); err != nil {
		c.EvaluationWarning(err.Error())
		return evaluator.Value{}, true
	}
	switch name {
	case "id":
		if c.isProjectile {
			return evaluator.Value{}, true
		}
		return evaluator.Int(c.EntityID), true
	case "teamside":
		return evaluator.Int(int32(c.TeamSide)), true
	case "ishelper", "numhelper", "numtarget", "numexplod":
		return c.LookupFunction(name, nil)
	case "numenemy":
		return evaluator.Int(int32(len(c.World.enemies(c, false)))), true
	case "numpartner":
		count := 0
		for _, root := range c.World.Roots {
			if root != c.rootPlayer() && root.TeamSide == c.TeamSide {
				count++
			}
		}
		return evaluator.Int(int32(count)), true
	default:
		if c.Parent == nil {
			return evaluator.Value{}, true
		}
		other := c.Parent
		if name == "rootdist x" || name == "rootdist y" {
			other = c.rootPlayer()
		}
		if other.destroyed {
			return evaluator.Value{}, true
		}
		x, y := c.PositionOf(other)
		if name == "parentdist x" || name == "rootdist x" {
			return evaluator.Float(float32((x - c.X) * float64(c.Facing))), true
		}
		return evaluator.Float(float32(y - c.Y)), true
	}
}

type hitTarget struct {
	generation uint64
	id         int
}

func (c *Character) acquireTarget(target *Character, id int) {
	c.acquiredTargets(-1, false)
	if c.hitTargets == nil {
		c.hitTargets = map[*Character]hitTarget{}
	}
	if _, ok := c.hitTargets[target]; !ok {
		c.targetOrder = append(c.targetOrder, target)
	}
	c.hitTargets[target] = hitTarget{target.hitGeneration, id}
}

func (c *Character) acquiredTargets(id int, filter bool) []*Character {
	var result []*Character
	var active []*Character
	for _, target := range c.targetOrder {
		record, ok := c.hitTargets[target]
		if !ok || target.destroyed || target.MoveType != "H" || record.generation != target.hitGeneration {
			delete(c.hitTargets, target)
			continue
		}
		active = append(active, target)
		if filter && id >= 0 && record.id != id {
			continue
		}
		result = append(result, target)
	}
	c.targetOrder = active
	return result
}

func (c *Character) Redirect(name string, id evaluator.Value, hasID bool) evaluator.EvaluatorEnvironment {
	if err := c.ensureWorld(); err != nil {
		c.EvaluationWarning(err.Error())
		return nil
	}
	n := int(id.Number())
	var candidates []*Character
	switch name {
	case "parent":
		if c.Parent != nil {
			return c.redirectEnvironment(c.Parent)
		}
	case "root":
		if c.Parent != nil && !c.rootPlayer().destroyed {
			return c.redirectEnvironment(c.rootPlayer())
		}
	case "helper":
		candidates = c.helperFamily(n, hasID)
	case "target":
		candidates = c.acquiredTargets(n, hasID)
	case "enemy", "enemynear":
		candidates = c.World.enemies(c, name == "enemynear")
		if n < 0 {
			n = 0
		}
		if n < len(candidates) {
			return c.redirectEnvironment(candidates[n])
		}
		return nil
	case "partner":
		for _, root := range c.World.Roots {
			if root != c.rootPlayer() && root.TeamSide == c.TeamSide {
				return c.redirectEnvironment(root)
			}
		}
	case "playerid":
		if who, ok := c.World.Registry.Lookup(int32(n)); ok && !who.destroyed {
			return c.redirectEnvironment(who)
		}
	}
	if len(candidates) > 0 {
		return c.redirectEnvironment(candidates[0])
	}
	return nil
}

func (c *Character) LookupFunction(name string, args []evaluator.Value) (evaluator.Value, bool) {
	switch name {
	case "ishelper", "numhelper", "numtarget", "numexplod", "playeridexist":
	default:
		return evaluator.Value{}, false
	}
	if len(args) > 1 || (name == "playeridexist" && len(args) != 1) {
		c.EvaluationWarning(name + ": invalid argument count")
		return evaluator.Value{}, true
	}
	id := 0
	if len(args) == 1 {
		n, err := evaluator.CoerceInt(args[0], c, name+" ID")
		if err != nil {
			c.EvaluationWarning(err.Error())
			return evaluator.Value{}, true
		}
		id = int(n)
	}
	if err := c.ensureWorld(); err != nil {
		c.EvaluationWarning(err.Error())
		return evaluator.Value{}, true
	}
	switch name {
	case "ishelper":
		return evaluator.Bool(c.Parent != nil && !c.isProjectile && (len(args) == 0 || c.HelperID == id)), true
	case "numhelper":
		return evaluator.Int(int32(len(c.helperFamily(id, len(args) > 0)))), true
	case "numtarget":
		return evaluator.Int(int32(len(c.acquiredTargets(id, len(args) > 0)))), true
	case "numexplod":
		return evaluator.Int(int32(c.numExplods(id, len(args) > 0))), true
	default:
		who, ok := c.World.Registry.Lookup(int32(id))
		return evaluator.Bool(ok && !who.destroyed), true
	}
}
