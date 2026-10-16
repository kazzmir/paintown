// Package exprfixture runs deterministic language fixtures through the same
// evaluator used by the game. It does not emulate world-bound combat triggers.
package exprfixture

import (
	"fmt"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type Expected struct {
	Kind  string  `json:"kind"`
	Value float64 `json:"value"`
}
type Step struct {
	Expression string      `json:"expr"`
	Want       Expected    `json:"want"`
	Vars       map[int]int `json:"vars,omitempty"`
}
type Fixture struct {
	Name  string `json:"name"`
	Time  int    `json:"time,omitempty"`
	Steps []Step `json:"steps"`
}
type Observation struct {
	Fixture    string      `json:"fixture"`
	Step       int         `json:"step"`
	Expression string      `json:"expr"`
	Kind       string      `json:"kind"`
	Value      float64     `json:"value"`
	Warnings   []string    `json:"warnings,omitempty"`
	Vars       map[int]int `json:"vars,omitempty"`
}

func Run(f Fixture) ([]Observation, error) {
	if f.Name == "" || len(f.Steps) == 0 {
		return nil, fmt.Errorf("fixture needs a name and steps")
	}
	env := &context{time: f.Time}
	var observations []Observation
	for i, step := range f.Steps {
		expr, err := evaluator.Parse(step.Expression)
		if err != nil {
			return observations, fmt.Errorf("%s step %d: %w", f.Name, i, err)
		}
		env.warnings = nil
		value := evaluator.EvaluateValue(expr, env)
		kind := map[evaluator.Kind]string{evaluator.IntKind: "int", evaluator.FloatKind: "float", evaluator.BottomKind: "bottom", evaluator.TextKind: "text"}[value.Kind()]
		obs := Observation{Fixture: f.Name, Step: i, Expression: step.Expression, Kind: kind, Value: value.Number(), Warnings: env.warnings}
		for index, n := range env.vars {
			if n != 0 {
				if obs.Vars == nil {
					obs.Vars = make(map[int]int)
				}
				obs.Vars[index] = n
			}
		}
		observations = append(observations, obs)
		if kind != step.Want.Kind || value.Number() != step.Want.Value {
			return observations, fmt.Errorf("%s step %d: got %s %v, want %s %v", f.Name, i, kind, value, step.Want.Kind, step.Want.Value)
		}
		for index, want := range step.Vars {
			if index < 0 || index >= len(env.vars) {
				return observations, fmt.Errorf("%s step %d: expected variable index %d is invalid", f.Name, i, index)
			}
			if env.vars[index] != want {
				return observations, fmt.Errorf("%s step %d: var(%d)=%d, want %d", f.Name, i, index, env.vars[index], want)
			}
		}
	}
	return observations, nil
}

type context struct {
	time     int
	vars     [60]int
	fvars    [40]float64
	warnings []string
}

func (c *context) GetTime() int                  { return c.time }
func (c *context) GetAnimTime() int              { return 0 }
func (c *context) GetPos() (float64, float64)    { return 0, 0 }
func (c *context) GetVel() (float64, float64)    { return 0, 0 }
func (c *context) GetVar(i int) int              { return c.vars[i] }
func (c *context) SetVar(i, v int)               { c.vars[i] = v }
func (c *context) GetFVar(i int) float64         { return c.fvars[i] }
func (c *context) SetFVar(i int, v float64)      { c.fvars[i] = v }
func (c *context) GetSysVar(int) int             { return 0 }
func (c *context) GetSysFVar(int) float64        { return 0 }
func (c *context) GetStateNo() int               { return 0 }
func (c *context) GetCtrl() bool                 { return true }
func (c *context) GetStateType() string          { return "S" }
func (c *context) GetMoveType() string           { return "I" }
func (c *context) GetPhysics() string            { return "N" }
func (c *context) GetAnim() int                  { return 0 }
func (c *context) GetAnimElem() int              { return 1 }
func (c *context) GetFacing() int                { return 1 }
func (c *context) Command(string) bool           { return false }
func (c *context) GetCNSVelocity() *cns.Velocity { return nil }
func (c *context) GetCNSMovement() *cns.Movement { return nil }
func (c *context) EvaluationWarning(s string)    { c.warnings = append(c.warnings, s) }
