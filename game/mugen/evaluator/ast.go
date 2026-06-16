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
	Value float64
}

func (n *Number) String() string {
	return fmt.Sprintf("%g", n.Value)
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
