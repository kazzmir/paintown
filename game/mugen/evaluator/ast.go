package evaluator

import (
	"fmt"
)

// Expression is the base interface for all AST nodes.
type Expression interface {
	String() string
}

// Number represents a numeric literal.
type Number struct {
	Value   float64
	IsFloat bool
	Raw     string
}

func (n *Number) String() string {
	if n.Raw != "" {
		return n.Raw
	}
	return fmt.Sprintf("%g", n.Value)
}

// Group retains subexpression boundaries, including interval placement rules.
type Group struct{ Expression Expression }

func (g *Group) String() string { return "(" + g.Expression.String() + ")" }

// Redirect changes the environment of one trigger, not the rest of a formula.
type Redirect struct {
	Selector Expression
	Trigger  Expression
}

func (r *Redirect) String() string { return r.Selector.String() + ", " + r.Trigger.String() }

// Legacy is an old-style nullary trigger clause. Its literal arguments are not
// ordinary expressions; e.g. AnimElem = 5 + 4 means (AnimElem = 5) + 4.
type Legacy struct {
	Name          string
	Operator      string
	Arguments     []Expression
	ExtraOperator string
}

func (l *Legacy) String() string {
	s := l.Name + " " + l.Operator + " " + l.Arguments[0].String()
	for i, arg := range l.Arguments[1:] {
		s += ", "
		if i == 0 && l.ExtraOperator != "" {
			s += l.ExtraOperator + " "
		}
		s += arg.String()
	}
	return s
}

// StringExpr represents a string literal.
type StringExpr struct {
	Value string
}

func (s *StringExpr) String() string {
	return fmt.Sprintf("\"%s\"", s.Value)
}

// Identifier represents a trigger name or variable (e.g., "time", "vel x").
type Identifier struct {
	Name string
}

func (i *Identifier) String() string {
	return i.Name
}

// Unary represents a unary expression (e.g., "-5", "!time").
type Unary struct {
	Operator   string
	Expression Expression
}

func (u *Unary) String() string {
	return fmt.Sprintf("(%s%s)", u.Operator, u.Expression.String())
}

// Binary represents a binary expression (e.g., "time = 0", "pos x > 10").
type Binary struct {
	Left     Expression
	Operator string
	Right    Expression
}

func (b *Binary) String() string {
	return fmt.Sprintf("(%s %s %s)", b.Left.String(), b.Operator, b.Right.String())
}

// Call represents a trigger call with arguments (e.g., "var(0)", "const(velocity.run.fwd.x)").
type Call struct {
	Name      string
	Arguments []Expression
}

func (c *Call) String() string {
	args := ""
	for i, arg := range c.Arguments {
		if i > 0 {
			args += ", "
		}
		args += arg.String()
	}
	return fmt.Sprintf("%s(%s)", c.Name, args)
}

// Interval represents a range in MUGEN, e.g. [a, b] or (a, b]
type Interval struct {
	Left     Expression
	Right    Expression
	LeftInc  bool
	RightInc bool
}

func (i *Interval) String() string {
	l := "("
	if i.LeftInc {
		l = "["
	}
	r := ")"
	if i.RightInc {
		r = "]"
	}
	return fmt.Sprintf("%s%s, %s%s", l, i.Left.String(), i.Right.String(), r)
}
