package character

import (
	"math"
	"slices"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cmd"
)

// AIStatistic retains the target's slowly updated estimate, observed limits and
// absolute deviation. A completed repeat contributes five percent to the mean.
type AIStatistic struct {
	Mean, Deviation float32
	Min, Max        int
	Valid           bool
}

func (s *AIStatistic) set(n int) {
	*s = AIStatistic{Mean: float32(n), Min: n, Max: n, Valid: true}
}

func (s *AIStatistic) merge(next AIStatistic) {
	if !next.Valid {
		return
	}
	if !s.Valid {
		*s = next
		return
	}
	difference := math.Abs(float64(int(next.Mean - s.Mean)))
	s.Mean = float32(.95*float64(s.Mean) + .05*float64(next.Mean))
	s.Deviation = float32(.95*float64(s.Deviation) + .05*difference)
	s.Min, s.Max = min(s.Min, next.Min), max(s.Max, next.Max)
}

type AIPhase struct{ Time, X, Y AIStatistic }

func (p *AIPhase) set(t, x, y int) { p.Time.set(t); p.X.set(x); p.Y.set(y) }
func (p *AIPhase) merge(next AIPhase) {
	p.Time.merge(next.Time)
	p.X.merge(next.X)
	p.Y.merge(next.Y)
}

// AIBounds is a union in game pixels, relative to a move's initial axis/facing.
type AIBounds struct {
	Left, Top, Right, Bottom float32
	Valid                    bool
}

func (b *AIBounds) include(left, top, right, bottom float64) {
	if left > right {
		left, right = right, left
	}
	if top > bottom {
		top, bottom = bottom, top
	}
	if !b.Valid {
		*b = AIBounds{float32(int(left)), float32(int(top)), float32(int(right)), float32(int(bottom)), true}
		return
	}
	b.Left, b.Top = min(b.Left, float32(int(left))), min(b.Top, float32(int(top)))
	b.Right, b.Bottom = max(b.Right, float32(int(right))), max(b.Bottom, float32(int(bottom)))
}

func (b *AIBounds) merge(next AIBounds) {
	if !next.Valid {
		return
	}
	if !b.Valid {
		*b = next
		return
	}
	b.Left = float32(.95*float64(b.Left) + .05*float64(next.Left))
	b.Top = float32(.95*float64(b.Top) + .05*float64(next.Top))
	b.Right = float32(.95*float64(b.Right) + .05*float64(next.Right))
	b.Bottom = float32(.95*float64(b.Bottom) + .05*float64(next.Bottom))
}

type AIEntryContext struct {
	StateNo   int
	StateType string
	MoveType  string
	Ctrl      bool
}

// Outcome groups are whiff, guard and hit, not the fighter's standing/air pose.
type AIMove struct {
	StateNo, CommandIndex int
	StateType             string
	Contexts              []AIEntryContext
	Startup               AIPhase
	Active, Control, End  [3]AIPhase
	Attack, Body          AIBounds
	Damage                AIStatistic
	HitFlag, GuardFlag    string
	Attribute             string
}

// AIMoveLibrary belongs to one roster entry, surviving fresh round actors.
// Input/planner state and an unfinished trial belong to the actor instead.
type AIMoveLibrary struct {
	Commands              *cmd.Data
	Width                 int
	Ground                map[int]*AIMove
	Air                   map[int]*AIMove
	Poses                 [4]AIBounds // common stand, walk, crouch and air hurtboxes
	indices               map[string]int
	groundOrder, airOrder []*AIMove
}

func NewAIMoveLibrary(commands *cmd.Data, width int) *AIMoveLibrary {
	l := &AIMoveLibrary{Commands: commands, Width: max(1, width), Ground: make(map[int]*AIMove), Air: make(map[int]*AIMove), indices: make(map[string]int)}
	if commands != nil {
		for index, name := range commands.IndexedNames() {
			if name != "" {
				l.indices[name] = index
			}
		}
	}
	return l
}

func (l *AIMoveLibrary) Move(state int, stateType string) *AIMove {
	if l == nil {
		return nil
	}
	if stateType == "A" {
		return l.Air[state]
	}
	return l.Ground[state]
}

func (l *AIMoveLibrary) Moves(stateType string) []*AIMove {
	if l == nil {
		return nil
	}
	if stateType == "A" {
		return l.airOrder
	}
	return l.groundOrder
}

func (l *AIMoveLibrary) complete(next AIMove) {
	bank := l.Ground
	if next.StateType == "A" {
		bank = l.Air
	} else if next.StateType != "S" && next.StateType != "C" {
		return
	}
	if previous := bank[next.StateNo]; previous != nil {
		for _, context := range next.Contexts {
			known := slices.ContainsFunc(previous.Contexts, func(old AIEntryContext) bool { return old.StateNo == context.StateNo })
			if !known && len(previous.Contexts) < 32 {
				previous.Contexts = append(previous.Contexts, context)
			}
		}
		previous.CommandIndex = next.CommandIndex
		previous.Startup.merge(next.Startup)
		previous.Attack.merge(next.Attack)
		previous.Body.merge(next.Body)
		previous.Damage.merge(next.Damage)
		for outcome := range next.Active {
			previous.Active[outcome].merge(next.Active[outcome])
			previous.Control[outcome].merge(next.Control[outcome])
			previous.End[outcome].merge(next.End[outcome])
		}
		if previous.Attribute != next.Attribute {
			previous.Attribute = ""
		}
		if previous.GuardFlag != next.GuardFlag {
			previous.GuardFlag = ""
		}
		return
	}
	// Native banks are bounded. Refuse additional observations, not characters
	// or their state execution, when the learning space has been exhausted.
	limit := 80
	if next.StateType == "A" {
		limit = 40
	}
	if len(bank) < limit {
		copy := next
		copy.Contexts = append([]AIEntryContext(nil), next.Contexts...)
		bank[next.StateNo] = &copy
		if next.StateType == "A" {
			l.airOrder = append(l.airOrder, &copy)
		} else {
			l.groundOrder = append(l.groundOrder, &copy)
		}
	}
}

type aiMoveTrial struct {
	move           AIMove
	ticks, outcome int
	x, y           int
	facing         int
	active         bool
	lastDefinition int
	damage         int
	projectile     *Character
}

func (c *Character) BeginControllerEvaluation() {
	if c.AIMoves != nil && c.AIMoves.Commands != c.CmdFile {
		c.AIMoves = NewAIMoveLibrary(c.CmdFile, c.AIMoves.Width)
		c.aiTrial = nil
	}
	c.aiCommandReads = c.aiCommandReads[:0]
	c.aiCapturing = c.AIMoves != nil && c.Parent == nil && c.RoundState == 2 && !c.IsPaused() && c.currentStateOwner() == c
}
func (c *Character) EndControllerEvaluation() { c.aiCapturing = false }

// RecordAICommand is called only by the evaluator's successful command query;
// diagnostic reads, input matching and other controllers cannot invent a cause.
func (c *Character) RecordAICommand(name string) {
	if !c.aiCapturing || len(c.aiCommandReads) >= 9 || c.AIMoves.Commands != c.CmdFile {
		return
	}
	if index, found := c.AIMoves.indices[name]; found {
		c.aiCommandReads = append(c.aiCommandReads, index)
	}
}

func (c *Character) beginAIMove(context AIEntryContext, command int) {
	c.aiTrial = &aiMoveTrial{move: AIMove{StateNo: c.StateNo, StateType: c.StateType, CommandIndex: command, Contexts: []AIEntryContext{context}}, x: int(c.X * c.aiScale()), y: int(c.Y * c.aiScale()), facing: c.Facing}
}

func (c *Character) aiScale() float64 {
	if c.AIMoves == nil {
		return 1
	}
	return float64(c.AIMoves.Width) / float64(c.LocalCoordinateWidth())
}

func (c *Character) aiBoxes(boxes []air.Box, x, y float64, facing int, destination *AIBounds) {
	sx, sy := c.spriteScale()
	scale := c.aiScale()
	for _, box := range boxes {
		left, top, right, bottom := BoxToWorld(box, c.X, c.Y, c.Facing, sx, sy)
		destination.include((left*scale-x)*float64(facing), top*scale-y, (right*scale-x)*float64(facing), bottom*scale-y)
	}
}

// Observe after movement/body push, using the completed pose saved before AIR
// advances. Clock only real actor passes: hitpause and shared Pause add no time.
func (c *Character) observeAIMove() {
	if c.AIMoves == nil || c.Parent != nil || c.DebugDisabled || c.worldFrozen {
		return
	}
	if c.currentStateOwner() != c || c.MoveType == "H" || c.RoundState != 2 {
		c.aiTrial = nil
		return
	}
	frame := c.contactFrame
	if frame == nil {
		frame = c.CurrentElement()
	}
	if !c.pausedThisTick && frame != nil {
		for pose, state := range [...]int{0, 20, 11, 50} {
			if c.StateNo == state {
				c.aiBoxes(frame.Clsn2, c.X*c.aiScale(), c.Y*c.aiScale(), c.Facing, &c.AIMoves.Poses[pose])
			}
		}
	}
	t := c.aiTrial
	if t == nil {
		return
	}
	x := int((c.X*c.aiScale() - float64(t.x)) * float64(t.facing))
	y := int(c.Y*c.aiScale() - float64(t.y))
	if c.MoveGuarded > 0 {
		t.outcome = 1
	} else if c.MoveHit > 0 {
		t.outcome = 2
	}
	if t.active && c.Ctrl && !t.move.Control[t.outcome].Time.Valid {
		t.move.Control[t.outcome].set(t.ticks, x, y)
	}
	if c.MoveType == "I" && c.Ctrl {
		if t.active {
			t.move.End[t.outcome].set(t.ticks, x, y)
			t.move.Damage.set(t.damage)
			c.AIMoves.complete(t.move)
		}
		c.aiTrial = nil
		return
	}
	if c.pausedThisTick {
		return
	}
	if frame != nil {
		c.aiBoxes(frame.Clsn1, float64(t.x), float64(t.y), t.facing, &t.move.Attack)
		c.aiBoxes(frame.Clsn2, float64(t.x), float64(t.y), t.facing, &t.move.Body)
	}
	active := c.HitDefActive && frame != nil && len(frame.Clsn1) > 0
	projectile := t.projectile != nil && !t.projectile.destroyed && !t.projectile.projectile.removing
	if active || projectile {
		if !t.active {
			t.move.Startup.set(t.ticks, x, y)
			t.active = true
		}
		t.move.Active[t.outcome].set(t.ticks, x, y)
		if c.HitDefActive && c.aiHitDefinition != t.lastDefinition {
			t.lastDefinition = c.aiHitDefinition
			if len(c.CurrentHitDef.Damage) > 0 {
				t.damage += c.CurrentHitDef.Damage[0]
			}
		}
		if active {
			t.move.HitFlag, t.move.GuardFlag, t.move.Attribute = c.CurrentHitDef.HitFlag, c.CurrentHitDef.GuardFlag, c.CurrentHitDef.Attr
		}
	}
	if t.active && c.Ctrl && !t.move.Control[t.outcome].Time.Valid {
		t.move.Control[t.outcome].set(t.ticks, x, y)
	}
	t.ticks++
}
