package evaluator

import (
	"fmt"
	"math"
)

// Kind models the reference numeric types. Text is used internally by the
// trigger interfaces for symbolic/string parameters and is not a numeric value.
type Kind uint8

const (
	BottomKind Kind = iota
	IntKind
	FloatKind
	TextKind
)

type Value struct {
	kind    Kind
	integer int32
	real    float32
	text    string
}

func Int(n int32) Value { return Value{kind: IntKind, integer: n} }
func Float(n float32) Value {
	if math.IsNaN(float64(n)) || math.IsInf(float64(n), 0) {
		return Value{}
	}
	return Value{kind: FloatKind, real: n}
}
func Text(s string) Value { return Value{kind: TextKind, text: s} }
func Bool(b bool) Value {
	if b {
		return Int(1)
	}
	return Int(0)
}
func (v Value) Kind() Kind     { return v.kind }
func (v Value) IsBottom() bool { return v.kind == BottomKind }
func (v Value) IsNumber() bool { return v.kind == IntKind || v.kind == FloatKind }
func (v Value) Bool() bool     { return v.IsNumber() && v.Number() != 0 }
func (v Value) Number() float64 {
	if v.kind == IntKind {
		return float64(v.integer)
	}
	if v.kind == FloatKind {
		return float64(v.real)
	}
	return 0
}
func (v Value) Text() (string, bool) { return v.text, v.kind == TextKind }
func (v Value) Int32() (int32, bool) { return v.integer, v.kind == IntKind }
func (v Value) String() string {
	switch v.kind {
	case IntKind:
		return fmt.Sprintf("%d", v.integer)
	case FloatKind:
		return fmt.Sprintf("%g", v.real)
	case TextKind:
		return fmt.Sprintf("%q", v.text)
	default:
		return "bottom"
	}
}

func (v Value) truncate() (int32, bool) {
	if !v.IsNumber() {
		return 0, false
	}
	n := math.Trunc(v.Number())
	if n < math.MinInt32 || n > math.MaxInt32 {
		return 0, false
	}
	return int32(n), true
}

// CoerceInt implements the documented warning-and-truncation rule for integer
// trigger/controller arguments. Operators that require integer operands (%, &,
// |, ^, ~) deliberately do not call this conversion.
func CoerceInt(v Value, env EvaluatorEnvironment, context string) (int32, error) {
	n, ok := v.truncate()
	if !ok {
		return 0, fmt.Errorf("%s: cannot convert %v to int32", context, v)
	}
	if v.kind == FloatKind {
		if sink, ok := env.(WarningEnvironment); ok {
			sink.EvaluationWarning(fmt.Sprintf("%s: float %v truncated to int %d", context, v, n))
		}
	}
	return n, nil
}
